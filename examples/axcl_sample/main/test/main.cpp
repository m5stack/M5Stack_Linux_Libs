/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "../utils/logger.h"
#include "axcl.h"
#include "cmdline.h"

static int sample_sys_alloc_free();
static int sample_sys_alloc_cache_free();
static int sample_sys_memory();
static int sample_sys_commom_pool();
static int sample_sys_private_pool();
static int sample_sys_link();

int main(int argc, char *argv[]) {
    cmdline::parser a;
    a.add<uint32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0, cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");
    a.parse_check(argc, argv);
    const uint32_t device_index = a.get<uint32_t>("device");
    const std::string json = a.get<std::string>("json");

    axclError ret;
    SAMPLE_LOG_I("json: %s", json.c_str());
    ret = axclInit(json.c_str());
    if (AXCL_SUCC != ret) {
        return 1;
    }

    axclrtDeviceList device_list;
    if (axclError ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        SAMPLE_LOG_E("no device is connected");
        axclFinalize();
        return ret;
    }

    if (device_index >= device_list.num) {
        SAMPLE_LOG_E("device index %d is out of connected device num %d", device_index, device_list.num);
        axclFinalize();
        return 1;
    }

    const int32_t device_id = device_list.devices[device_index];
    SAMPLE_LOG_I("device index: %d, bus number: %d", device_index, device_id);

    ret = axclrtSetDevice(device_id);
    if (AXCL_SUCC != ret) {
        axclFinalize();
        return 1;
    }

    sample_sys_alloc_free();
    sample_sys_alloc_cache_free();
    sample_sys_memory();
    sample_sys_commom_pool();
    sample_sys_private_pool();
    sample_sys_link();

    axclrtResetDevice(device_id);
    axclFinalize();
    return 0;
}

static int sample_sys_alloc_free() {
    AX_S32 ret, i;
    AX_U64 PhyAddr[1024*8]= { 0 };
    AX_VOID *pVirAddr[1024*8] = { 0 };
    AX_U32 BlockSize = 1 * 1024 * 1024;
    AX_U32 align = 0x1000;
    AX_S8 blockname[20];
    AX_U32 BlockCount = 1024*8;

    SAMPLE_LOG_I("sys_alloc_free begin");

    for (i = 0; i < BlockCount; i++) {
        sprintf((char *)blockname, "noncache_block_%d", i);
        ret = AXCL_SYS_MemAlloc(&PhyAddr[i], (AX_VOID **)&pVirAddr[i], BlockSize, align, blockname);
        if (AXCL_SUCC != ret) {
            if (BlockCount > 7000){
                pVirAddr[i] = NULL;
                PhyAddr[i] = 0;
                break;
            }
            else
            {
                SAMPLE_LOG_E("AXCL_SYS_MemAlloc failed");
                return -1;
            }
        }
        SAMPLE_LOG_I("alloc PhyAddr= 0x%llx,pVirAddr=%p", PhyAddr[i], pVirAddr[i]);
    }

    for (i = 0; ((i < BlockCount) && (PhyAddr[i] != 0)); i++) {
        ret = axclrtMemset((void*)PhyAddr[i], 0x89, BlockSize);
        if (AXCL_SUCC != ret) {
            SAMPLE_LOG_E("axclrtMemset failed");
            return -1;
        }
        SAMPLE_LOG_I("axclrtMemset PhyAddr= 0x%llx,pVirAddr=%p", PhyAddr[i], pVirAddr[i]);
    }


    for (i = 0; ((i < BlockCount) && (PhyAddr[i] != 0)); i++) {
        ret = AXCL_SYS_MemFree(PhyAddr[i], (AX_VOID *)pVirAddr[i]);
        if (AXCL_SUCC != ret) {
            SAMPLE_LOG_E("AXCL_SYS_MemFree failed");
            return -1;
        }
        SAMPLE_LOG_I("free PhyAddr= 0x%llx,pVirAddr=%p", PhyAddr[i], pVirAddr[i]);
    }

    SAMPLE_LOG_I("sys_alloc_free end success");
    return ret;
}

static int sample_sys_memory() {
    axclError ret;
    constexpr uint32_t size = 64 * 1024 * 1024;

    void *host_mem[2] = {NULL, NULL};
    void *dev_mem[2] = {NULL, NULL};

    SAMPLE_LOG_I("alloc host and device memory, size: 0x%x", size);
    for (int i = 0; i < 2; ++i) {
        if (ret = axclrtMallocHost(&host_mem[i], size); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("alloc host memory[%d] failed, ret = 0x%x", i, ret);
            goto __END__ERROR;
        }

        if (ret = axclrtMalloc(&dev_mem[i], size, AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("alloc device memory[%d] failed, ret = 0x%x", i, ret);
            goto __END__ERROR;
        }

        SAMPLE_LOG_I("memory [%d]: host %p, device %p", i, host_mem[i], dev_mem[i]);
    }

    ::memset(host_mem[0], 0xA8, size);
    ::memset(host_mem[1], 0x00, size);

    SAMPLE_LOG_I("memcpy from host memory[0] %p to device memory[0] %p", host_mem[0], dev_mem[0]);
    if (ret = axclrtMemcpy(dev_mem[0], host_mem[0], size, AXCL_MEMCPY_HOST_TO_DEVICE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memcpy from host memory[0] %p to device memory[0] %p failed, ret = 0x%x", host_mem[0], dev_mem[0], ret);
        goto __END__ERROR;
    }

    SAMPLE_LOG_I("memcpy device memory[0] %p to device memory[1] %p", dev_mem[0], dev_mem[1]);
    if (ret = axclrtMemcpy(dev_mem[1], dev_mem[0], size, AXCL_MEMCPY_DEVICE_TO_DEVICE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memcpy from device memory %p to device memory %p failed, ret = 0x%x", dev_mem[0], dev_mem[1], ret);
        goto __END__ERROR;
    }

    SAMPLE_LOG_I("memcpy device memory[1] %p to host memory[0] %p", dev_mem[1], host_mem[1]);
    if (ret = axclrtMemcpy(host_mem[1], dev_mem[1], size, AXCL_MEMCPY_DEVICE_TO_HOST); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memcpy from device memory %p to host memory %p failed, ret = 0x%x", dev_mem[1], host_mem[1], ret);
        goto __END__ERROR;
    }

    if (0 == ::memcmp(host_mem[0], host_mem[1], size)) {
        SAMPLE_LOG_I("compare host memory[0] %p and host memory[1] %p success", host_mem[0], host_mem[1]);
    } else {
        SAMPLE_LOG_E("compare host memory[0] %p and host memory[1] %p failure failed", host_mem[0], host_mem[1]);
    }

    for (int i = 0; i < 2; ++i) {
        if (host_mem[i]) {
            axclrtFreeHost(host_mem[i]);
        }

        if (dev_mem[i]) {
            axclrtFree(dev_mem[i]);
        }
    }
    return 0;
__END__ERROR:
for (int i = 0; i < 2; ++i) {
    if (host_mem[i]) {
        axclrtFreeHost(host_mem[i]);
    }

    if (dev_mem[i]) {
        axclrtFree(dev_mem[i]);
    }
}
    return -1;
}



static int sample_sys_alloc_cache_free() {
    AX_S32 ret, i;
    AX_U64 PhyAddr[10]= { 0 };
    AX_VOID *pVirAddr[10] = { 0 };
    AX_U32 BlockSize = 1 * 1024 * 1024;
    AX_U32 align = 0x1000;
    AX_S8 blockname[20];

    SAMPLE_LOG_I("sys_alloc_cache_free begin");

    for (i = 0; i < 10; i++) {
        sprintf((char *)blockname, "cache_block_%d", i);
        ret = AXCL_SYS_MemAllocCached(&PhyAddr[i], (AX_VOID **)&pVirAddr[i], BlockSize, align, blockname);
        if (AXCL_SUCC != ret) {
            SAMPLE_LOG_E("AXCL_SYS_MemAllocCached failed");
            return -1;
        }
        SAMPLE_LOG_I("alloc PhyAddr= 0x%llx,pVirAddr=%p", PhyAddr[i], pVirAddr[i]);
    }

    for (i = 0; i < 10; i++) {
        ret = AXCL_SYS_MemFree(PhyAddr[i], (AX_VOID *)pVirAddr[i]);
        if (AXCL_SUCC != ret) {
            SAMPLE_LOG_E("AXCL_SYS_MemFree failed");
            return -1;
        }
        SAMPLE_LOG_I("free PhyAddr= 0x%llx,pVirAddr=%p", PhyAddr[i], pVirAddr[i]);
    }

    SAMPLE_LOG_I("sys_alloc_cache_free end success");
    return ret;
}

static int sample_sys_commom_pool() {
    AX_S32 ret = 0;
    AX_POOL_FLOORPLAN_T PoolFloorPlan = { 0 };
    AX_U64 BlkSize;
    AX_BLK BlkId;
    AX_POOL PoolId;
    AX_U64 PhysAddr, MetaPhysAddr;

    SAMPLE_LOG_I("sys_commom_pool begin");

    /* step 1:SYS init */
    ret = AXCL_SYS_Init();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Init failed!!ret:0x%x", ret);
        return -1;
    } else {
        SAMPLE_LOG_I("AXCL_SYS_Init success!");
    }

    /* step 2:exit pool */
    ret = AXCL_POOL_Exit();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_Exit failed!!ret:0x%x", ret);
        goto error0;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Exit success!");
    }

    /* step 3:config common pool */
    memset(&PoolFloorPlan, 0, sizeof(AX_POOL_FLOORPLAN_T));
    PoolFloorPlan.CommPool[0].MetaSize   = 0x2000;
    PoolFloorPlan.CommPool[0].BlkSize   = 1 * 1024 * 1024;
    PoolFloorPlan.CommPool[0].BlkCnt    = 5;
    PoolFloorPlan.CommPool[0].CacheMode = POOL_CACHE_MODE_NONCACHE;
    PoolFloorPlan.CommPool[1].MetaSize   = 0x2000;
    PoolFloorPlan.CommPool[1].BlkSize   = 2 * 1024 * 1024;
    PoolFloorPlan.CommPool[1].BlkCnt    = 5;
    PoolFloorPlan.CommPool[1].CacheMode = POOL_CACHE_MODE_NONCACHE;
    PoolFloorPlan.CommPool[2].MetaSize   = 0x2000;
    PoolFloorPlan.CommPool[2].BlkSize   = 3 * 1024 * 1024;
    PoolFloorPlan.CommPool[2].BlkCnt    = 5;
    PoolFloorPlan.CommPool[2].CacheMode = POOL_CACHE_MODE_NONCACHE;

    /* The default value of PartitionName is anonymous. The partition name must exist when the cmm ko is loaded */
    memset(PoolFloorPlan.CommPool[0].PartitionName, 0, sizeof(PoolFloorPlan.CommPool[0].PartitionName));
    memset(PoolFloorPlan.CommPool[1].PartitionName, 0, sizeof(PoolFloorPlan.CommPool[1].PartitionName));
    memset(PoolFloorPlan.CommPool[2].PartitionName, 0, sizeof(PoolFloorPlan.CommPool[2].PartitionName));
    strcpy((char *)PoolFloorPlan.CommPool[0].PartitionName, "anonymous");
    strcpy((char *)PoolFloorPlan.CommPool[1].PartitionName, "anonymous");
    strcpy((char *)PoolFloorPlan.CommPool[2].PartitionName, "anonymous");

    ret = AXCL_POOL_SetConfig(&PoolFloorPlan);

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_SetConfig failed!ret:0x%x", ret);
        goto error0;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_SetConfig success!");
    }

    /* step 4:init pool */
    ret = AXCL_POOL_Init();
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_Init failed!!ret:0x%x", ret);
        goto error0;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Init success!");
    }

    /*
    * step 5:get block from common pool
    */

    BlkSize = 3 * 1024 * 1024;
    BlkId = AXCL_POOL_GetBlock(AX_INVALID_POOLID, BlkSize, NULL);

    if (BlkId == AX_INVALID_BLOCKID) {
        SAMPLE_LOG_E("AXCL_POOL_GetBlock failed!");
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_GetBlock success!BlkId:0x%X", BlkId);
    }

    /* step 6:get poolid from blkid */
    PoolId = AXCL_POOL_Handle2PoolId(BlkId);

    if (PoolId == AX_INVALID_POOLID) {
        SAMPLE_LOG_E("AXCL_POOL_Handle2PoolId failed!");
        AXCL_POOL_ReleaseBlock(BlkId);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Handle2PoolId success!(Blockid:0x%X --> PoolId=%d)", BlkId, PoolId);
    }

    /* step 7:get block phyaddr from blkid */
    PhysAddr = AXCL_POOL_Handle2PhysAddr(BlkId);

    if (!PhysAddr) {
        SAMPLE_LOG_E("AXCL_POOL_Handle2PhysAddr failed!");
        AXCL_POOL_ReleaseBlock(BlkId);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Handle2PhysAddr success!(Blockid:0x%X --> PhyAddr=0x%llx)", BlkId, PhysAddr);
    }

    /* step 8:get meta phyaddr from blkid */
    MetaPhysAddr = AXCL_POOL_Handle2MetaPhysAddr(BlkId);

    if (!MetaPhysAddr) {
        SAMPLE_LOG_E("AXCL_POOL_Handle2MetaPhysAddr failed!");
        AXCL_POOL_ReleaseBlock(BlkId);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Handle2MetaPhysAddr success!(Blockid:0x%X --> MetaPhyAddr=0x%llx)", BlkId, MetaPhysAddr);
    }

    /* step 9:release block */
    ret = AXCL_POOL_ReleaseBlock(BlkId);

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_ReleaseBlock failed!ret:0x%x", ret);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_ReleaseBlock success!Blockid=0x%x", BlkId);
    }

    /* step 10:exit pool */
    ret = AXCL_POOL_Exit();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_Exit failed!!ret:0x%x", ret);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Exit success!");
    }

    /* step 11:SYS deinit */
    ret = AXCL_SYS_Deinit();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Deinit failed!!ret:0x%x", ret);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_SYS_Deinit success!");
    }

    SAMPLE_LOG_I("sys_commom_pool end success!");
    return 0;

error1:
    AXCL_POOL_Exit();

error0:
    AXCL_SYS_Deinit();

    SAMPLE_LOG_I("sys_commom_pool end failed!");
    return -1;
}

static int sample_sys_private_pool() {
    AX_S32 ret = 0;
    AX_U64 BlkSize;
    AX_BLK BlkId;
    AX_POOL PoolId;
    AX_U64 PhysAddr, MetaPhysAddr;
    AX_POOL_CONFIG_T PoolConfig = { 0 };
    AX_POOL UserPoolId0 , UserPoolId1;

    SAMPLE_LOG_I("sys_private_pool begin");

    /* step 1:SYS init */
    ret = AXCL_SYS_Init();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Init failed!!ret:0x%x", ret);
        return -1;
    } else {
        SAMPLE_LOG_I("AXCL_SYS_Init success!");
    }

    /* step 2:exit pool */
    ret = AXCL_POOL_Exit();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_Exit failed!!ret:0x%x", ret);
        goto error0;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Exit success!");
    }

    /* step 3:AX_POOL_CreatePool */

    /* user_pool_0 :blocksize=1*1024*1024,metasize=0x1000,block count =2 ,noncache */
    memset(&PoolConfig, 0, sizeof(AX_POOL_CONFIG_T));
    PoolConfig.MetaSize = 0x1000;
    PoolConfig.BlkSize = 1 * 1024 * 1024;
    PoolConfig.BlkCnt = 2;
    PoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
    memset(PoolConfig.PartitionName,0, sizeof(PoolConfig.PartitionName));
    strcpy((char *)PoolConfig.PartitionName, "anonymous");

    UserPoolId0 = AXCL_POOL_CreatePool(&PoolConfig);

    if (AX_INVALID_POOLID == UserPoolId0) {
        SAMPLE_LOG_E("AXCL_POOL_CreatePool error!!!");
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_CreatePool[%d] success", UserPoolId0);
    }

    /* user_pool_1:blocksize=2*1024*1024,metasize=0x1000,block count =3 ,noncache */
    memset(&PoolConfig, 0, sizeof(AX_POOL_CONFIG_T));
    PoolConfig.MetaSize = 0x1000;
    PoolConfig.BlkSize = 2 * 1024 * 1024;
    PoolConfig.BlkCnt = 3;
    PoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
    memset(PoolConfig.PartitionName,0, sizeof(PoolConfig.PartitionName));
    strcpy((char *)PoolConfig.PartitionName, "anonymous");

    UserPoolId1 = AXCL_POOL_CreatePool(&PoolConfig);

    if (AX_INVALID_POOLID == UserPoolId1) {
        SAMPLE_LOG_E("AXCL_POOL_CreatePool error!!!");
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_CreatePool[%d] success", UserPoolId1);
    }

    /* step 4:get block from user pool */
    BlkSize = 2 * 1024 * 1024;
    BlkId = AXCL_POOL_GetBlock(UserPoolId1, BlkSize, NULL);

    if (BlkId == AX_INVALID_BLOCKID) {
        SAMPLE_LOG_E("AXCL_POOL_GetBlock failed!");
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_GetBlock success!BlkId:0x%X", BlkId);
    }

    /* step 5:get poolid from blkid */
    PoolId = AXCL_POOL_Handle2PoolId(BlkId);

    if (PoolId == AX_INVALID_POOLID) {
        SAMPLE_LOG_E("AXCL_POOL_Handle2PoolId failed!");
        AXCL_POOL_ReleaseBlock(BlkId);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Handle2PoolId success!(Blockid:0x%X --> PoolId=%d)", BlkId, PoolId);
    }

    /* step 6:get block phyaddr from blkid */
    PhysAddr = AXCL_POOL_Handle2PhysAddr(BlkId);

    if (!PhysAddr) {
        SAMPLE_LOG_E("AXCL_POOL_Handle2PhysAddr failed!");
        AXCL_POOL_ReleaseBlock(BlkId);
        goto error1;
    } else {
        SAMPLE_LOG_I("AX_POOL_Handle2PhysAddr success!(Blockid:0x%X --> PhyAddr=0x%llx)", BlkId, PhysAddr);
    }

    /* step 7:get metadata phyaddr from blkid */
    MetaPhysAddr = AXCL_POOL_Handle2MetaPhysAddr(BlkId);

    if (!MetaPhysAddr) {
        SAMPLE_LOG_E("AXCL_POOL_Handle2MetaPhysAddr failed!");
        AXCL_POOL_ReleaseBlock(BlkId);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_Handle2MetaPhysAddr success!(Blockid:0x%X --> MetaPhyAddr=0x%llx)", BlkId, MetaPhysAddr);
    }

    /* step 8:release block */
    ret = AXCL_POOL_ReleaseBlock(BlkId);

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_ReleaseBlock failed!ret:0x%x", ret);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_ReleaseBlock success!Blockid=0x%x", BlkId);
    }

    /* step 9:destroy pool */
    ret = AXCL_POOL_DestroyPool(UserPoolId1);
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_DestroyPool failed!ret:0x%x", ret);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_DestroyPool[%d] success!", UserPoolId1);
    }

    ret = AXCL_POOL_DestroyPool(UserPoolId0);
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_POOL_DestroyPool failed!ret:0x%x", ret);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_POOL_DestroyPool[%d] success!", UserPoolId0);
    }

    /* step 10:SYS deinit */
    ret = AXCL_SYS_Deinit();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Deinit failed!!ret:0x%x", ret);
        goto error1;
    } else {
        SAMPLE_LOG_I("AXCL_SYS_Deinit success!");
    }

    SAMPLE_LOG_I("sys_private_pool end success!");
    return 0;

error1:
    AXCL_POOL_Exit();

error0:
    AXCL_SYS_Deinit();

    SAMPLE_LOG_I("sys_private_pool end failed!");
    return -1;
}

static int sample_sys_link() {
    AX_S32 ret = 0;
    AX_MOD_INFO_T srcMod;
    AX_MOD_INFO_T dstMod;
    AX_MOD_INFO_T dstMod2;
    AX_MOD_INFO_T tempMod;
    AX_LINK_DEST_T linkDest;

    SAMPLE_LOG_I("sample_sys_link begin");

    memset(&srcMod, 0, sizeof(srcMod));
    memset(&dstMod, 0, sizeof(dstMod));
    memset(&dstMod2, 0, sizeof(dstMod2));
    memset(&tempMod, 0, sizeof(tempMod));
    memset(&linkDest, 0, sizeof(linkDest));

    /* step 1:SYS init */
    ret = AXCL_SYS_Init();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Init failed!!ret:0x%x", ret);
        return -1;
    } else {
        SAMPLE_LOG_I("AXCL_SYS_Init success!");
    }

    /* one src --> two dst */
    srcMod.enModId = AX_ID_IVPS;
    srcMod.s32GrpId = 1;
    srcMod.s32ChnId = 1;

    dstMod.enModId = AX_ID_VENC;
    dstMod.s32GrpId = 0;
    dstMod.s32ChnId = 1;

    dstMod2.enModId = AX_ID_VENC;
    dstMod2.s32GrpId = 0;
    dstMod2.s32ChnId = 2;

    /* step 2:link */
    ret = AXCL_SYS_Link(&srcMod, &dstMod);
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Link failed,ret:0x%x", ret);
        goto error;
    }

    ret = AXCL_SYS_Link(&srcMod, &dstMod2);
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Link failed,ret:0x%x", ret);
        goto error;
    }

    /* step 3:get src by dest */
    ret = AXCL_SYS_GetLinkByDest(&dstMod, &tempMod);
    if ((AXCL_SUCC != ret) || (tempMod.enModId != srcMod.enModId) ||
        (tempMod.s32GrpId != srcMod.s32GrpId) ||
        (tempMod.s32ChnId != srcMod.s32ChnId)) {
        SAMPLE_LOG_E("AXCL_SYS_GetLinkByDest failed,ret:0x%x", ret);
        goto error;
    }

    /* step 4:get dest by src */
    ret = AXCL_SYS_GetLinkBySrc(&srcMod, &linkDest);
    if ((AXCL_SUCC != ret) || (linkDest.u32DestNum != 2) ||
        (linkDest.astDestMod[0].enModId != AX_ID_VENC) ||
        (linkDest.astDestMod[1].enModId != AX_ID_VENC)) {
        SAMPLE_LOG_E("AXCL_SYS_GetLinkBySrc failed,ret:0x%x", ret);
        goto error;
    }

    /* step 5:unlink */
    ret = AXCL_SYS_UnLink(&srcMod, &dstMod);
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_UnLink failed,ret:0x%x", ret);
        goto error;
    }

    ret = AXCL_SYS_UnLink(&srcMod, &dstMod2);
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_UnLink failed,ret:0x%x", ret);
        goto error;
    }

    /* step 6:SYS deinit */
    ret = AXCL_SYS_Deinit();

    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("AXCL_SYS_Deinit failed!!ret:0x%x", ret);
        goto error;
    } else {
        SAMPLE_LOG_I("AXCL_SYS_Deinit success!");
    }

    SAMPLE_LOG_I("sample_sys_link end success!");
    return 0;

error:
    AXCL_SYS_Deinit();

    SAMPLE_LOG_I("sample_sys_link end failed!");
    return -1;
}