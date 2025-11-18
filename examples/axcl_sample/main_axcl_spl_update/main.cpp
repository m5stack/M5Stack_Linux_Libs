#include <stdio.h>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include "axcl.h"
#include "cmdline.h"
#include "md5.h"

#define STORAGE_TYPE_NOR 2
#define SPL_SIZE (0x40000)
#define log(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

extern "C" axclError axclrtControlExecuteShellCmd(const char *cmd, const char *const args[], size_t argc, const char **output, int32_t timeout);

static bool check_spl(const std::string &spl);
static int get_storage_type(uint32_t device);
static bool update_spl_nor_flash(uint32_t device, const std::string &spl);

int main(int argc, char *argv[]) {
    cmdline::parser a;
    a.add<std::string>("spl", 'i', "path of spl_AX650_card_signed.bin", true);
    a.add<uint32_t>("device", 'd', "device index", true, 0, cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.parse_check(argc, argv);
    const std::string spl = a.get<std::string>("spl");
    const uint32_t device = a.get<uint32_t>("device");

    if (!check_spl(spl)) {
        return 1;
    }

    printf("Are you sure to update spl? (y/n): ");
    char c;
    std::cin >> c;
    if (c != 'y' && c != 'Y') {
        return 0;
    }

    axclError ret;
    if (ret = axclInit(NULL); ret != AXCL_SUCC) {
        log("axclInit failed: 0x%x", (uint32_t)ret);
        return 1;
    }

    axclrtDeviceList device_list;
    if (ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        log("no device is connected");
        axclFinalize();
        return 1;
    }

    if (device >= device_list.num) {
        log("device index %d is out of connected device num %d", device, device_list.num);
        axclFinalize();
        return 1;
    }

    const int32_t device_id = device_list.devices[device];
    if (ret = axclrtSetDevice(device_id); AXCL_SUCC != ret) {
        log("active device, ret = 0x%x", (uint32_t)ret);
        axclFinalize();
        return 1;
    }

    auto storage = get_storage_type(device_id);
    if (storage != STORAGE_TYPE_NOR) {
        log("device is not nor flash, storage type = %d", storage);
        axclrtResetDevice(device_id);
        axclFinalize();
        return 1;
    }

    update_spl_nor_flash(device_id, spl);

    axclrtResetDevice(device_id);
    axclFinalize();
    return 0;
}

static bool check_spl(const std::string &spl) {
    if (!std::filesystem::exists(spl)) {
        log("%s not exist", spl.c_str());
        return false;
    }

    if (!std::filesystem::exists(spl)) {
        log("%s not exist", spl.c_str());
        return false;
    }

    const uint64_t file_size = std::filesystem::file_size(spl);
    if (file_size != SPL_SIZE) {
        log("spl image size %lu is not equal to %d", file_size, SPL_SIZE);
        return false;
    }

    char head[8];
    std::ifstream ifs(spl, std::ios::binary);
    ifs.read(head, 8);
    ifs.close();

    if (head[4] != 0x22 || head[5] != 0x33 || head[6] != 0x54 || head[7] != 0x55) {
        log("invalid spl image data: magic check failed");
        return false;
    }

    return true;
}

static int get_storage_type(uint32_t device) {
    const char *output = nullptr;
    if (axclError ret = axclrtControlExecuteShellCmd("ax_lookat 0x4200010", NULL, 0, &output, 1000); AXCL_SUCC != ret) {
        log("get device storage type failed, ret = 0x%x", (uint32_t)ret);
        return -1;
    }

    if (output) {
        uint32_t value;
        if (sscanf(output, "%*[^:]:  %x", &value) == 1) {
            return (int)(value & 0x3);
        }
    }

    return -2;
}

static bool update_spl_nor_flash(uint32_t device, const std::string &spl) {
    axclError ret;
    char *data = new char[SPL_SIZE];
    std::ifstream ifs(spl, std::ios::binary);
    ifs.read(data, SPL_SIZE);
    ifs.close();

    char hex[36] = {0};
    uint8_t md5[16];
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char *)data, SPL_SIZE);
    MD5Final(md5, &ctx);
    for (int i = 0; i < 16; i++) {
        sprintf(&hex[i * 2], "%02x", md5[i]);
    }
    delete[] data;

    log("transferring %s to device ...", spl.c_str());
    if (ret = axclrtTransferFile(spl.c_str(), "/opt/data/spl.bin", FILE_TRANSFER_FROM_HOST_TO_DEVICE); AXCL_SUCC != ret) {
        log("transfer spl file to device failed, ret = 0x%x", (uint32_t)ret);
        return false;
    }

    const char *output = nullptr;
    if (ret = axclrtControlExecuteShellCmd("md5sum /opt/data/spl.bin | awk '{print $1}'", NULL, 0, &output, 1000); AXCL_SUCC != ret) {
        log("get md5sum of transferred spl file failed, ret = 0x%x", (uint32_t)ret);
        return false;
    } else {
        if (output) {
            std::string str(output);
            str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); }), str.end());
            if (0 != strcmp(str.c_str(), hex)) {
                log("check md5sum of transferred spl file failed, expected = %s1, actual = %s1", hex, str.c_str());
                return false;
            }
        } else {
            log("get md5sum of transferred spl file failed, output is null");
            return false;
        }
    }

    // erase nor flash
    log("erasing flash ...");
    constexpr int32_t erase_timeout = 120000;
    if (ret = axclrtControlExecuteShellCmd("flash_eraseall /dev/mtd0", NULL, 0, &output, erase_timeout); AXCL_SUCC != ret) {
        log("erase nor flash failed, ret = 0x%x", (uint32_t)ret);
        return false;
    } else {
        if (output) {
            if (!strstr(output, "100% complete")) {
                log("erase nor flash failed, output: %s", output);
                return false;
            }
        } else {
            log("erase nor flash failed, output is null");
            return false;
        }
    }

    // write spl to nor flash
    log("updating spl ...");
    constexpr int32_t write_timeout = 10000;
    if (ret = axclrtControlExecuteShellCmd("dd if=/opt/data/spl.bin of=/dev/mtd0", NULL, 0, NULL, write_timeout); AXCL_SUCC != ret) {
        log("write spl to nor flash failed, ret = 0x%x", (uint32_t)ret);
        return false;
    }

    // sync
    log("syncing ...");
    constexpr int32_t sync_timeout = 10000;
    if (ret = axclrtControlExecuteShellCmd("sync", NULL, 0, NULL, sync_timeout); AXCL_SUCC != ret) {
        log("sync failed, ret = 0x%x", (uint32_t)ret);
        return false;
    }

    log("update spl success");
    return true;
}