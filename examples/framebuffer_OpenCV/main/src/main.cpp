#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>
#include <chrono>


int view_information(struct fb_fix_screeninfo &finfo ,struct fb_var_screeninfo &vinfo){


    // デバイス情報の出力
    std::cout << "--------------------------\n"
              << "Frame Buffer Device Information:\n"
              << "ID: " << finfo.id << "\n"
              << "Memory Length: " << finfo.smem_len << " bytes\n"
              << "Type: " << finfo.type << "\n"
              << "Type Aux: " << finfo.type_aux << "\n"
              << "Visual: " << finfo.visual << "\n"
              << "Line Length: " << finfo.line_length << " bytes per line\n"
              << "--------------------------\n";



    // スクリーン情報の出力
    std::cout << "Variable Framebuffer Information:\n"
              << "Resolution: " << vinfo.xres << "x" << vinfo.yres << "\n"
              << "Virtual Resolution: " << vinfo.xres_virtual << "x" << vinfo.yres_virtual << "\n"
              << "Bits Per Pixel: " << static_cast<int>(vinfo.bits_per_pixel) << "\n"
              << "Red: Offset " << static_cast<int>(vinfo.red.offset) << ", Length " << static_cast<int>(vinfo.red.length) << "\n"
              << "Green: Offset " << static_cast<int>(vinfo.green.offset) << ", Length " << static_cast<int>(vinfo.green.length) << "\n"
              << "Blue: Offset " << static_cast<int>(vinfo.blue.offset) << ", Length " << static_cast<int>(vinfo.blue.length) << "\n"
              << "--------------------------\n";

	return 0;
}


int main() {
    // フレームバッファデバイスのファイルディスクリプタ
    int fbfd0 = open("/dev/fb0", O_RDWR);
    if (fbfd0 == -1) {
        perror("Cannot open framebuffer device");
        return 1;
    }

    int fbfd1 = open("/dev/fb1", O_RDWR);
    if (fbfd1 == -1) {
        perror("Cannot open framebuffer device");
        return 1;
    }

    // 固定スクリーン情報の構造体
    struct fb_fix_screeninfo finfo0;
    if (ioctl(fbfd0, FBIOGET_FSCREENINFO, &finfo0)) {
        perror("Error reading fixed screen info");
        close(fbfd0);
        return 1;
    }
    
    // 固定スクリーン情報の構造体
    struct fb_fix_screeninfo finfo1;
    if (ioctl(fbfd1, FBIOGET_FSCREENINFO, &finfo1)) {
        perror("Error reading fixed screen info");
        close(fbfd1);
        return 1;
    }
    
    // 可変スクリーン情報の構造体
    struct fb_var_screeninfo vinfo0;
    if (ioctl(fbfd0, FBIOGET_VSCREENINFO, &vinfo0)) {
        perror("Error reading variable screen info");
        close(fbfd0);
        return 1;
    }
    
    // 可変スクリーン情報の構造体
    struct fb_var_screeninfo vinfo1;
    if (ioctl(fbfd1, FBIOGET_VSCREENINFO, &vinfo1)) {
        perror("Error reading variable screen info");
        close(fbfd1);
        return 1;
    }
    
    std::cout<<"/dev/fb0 information"<<std::endl;
    view_information(finfo0 ,vinfo0);
    std::cout<<"/dev/fb1 information"<<std::endl;
    view_information(finfo1 ,vinfo1);
    
    

    // メモリマッピング
    char* fbAddr0 = static_cast<char*>(mmap(nullptr, finfo0.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd0, 0));
    if (fbAddr0 == MAP_FAILED) {
        perror("Failed to mmap");
        close(fbfd0);
        return 1;
    }

    char* fbAddr1 = static_cast<char*>(mmap(nullptr, finfo1.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd1, 0));
    if (fbAddr1 == MAP_FAILED) {
        perror("Failed to mmap");
        close(fbfd0);
        return 1;
    }



    // 画像の読み込み
    cv::Mat image = cv::imread("06b.jpg");
    if (image.empty()) {
        std::cerr << "Error: Image not found or empty.\n";
        munmap(fbAddr0, finfo0.smem_len);
        munmap(fbAddr1, finfo1.smem_len);

        close(fbfd0);
        close(fbfd1);
        return 1;
    }

    // 画像のリサイズとカラーフォーマット変換
    cv::Mat displayport_image;
    cv::Mat lcd_image;

    cv::resize(image, displayport_image, cv::Size(vinfo0.xres, vinfo0.yres));
    cv::resize(image, lcd_image, cv::Size(vinfo1.xres, vinfo1.yres));

    // DisplayPort(HDMI):BGR888(24bit Color)⇒BGR565(16bit Color)
    cv::cvtColor(displayport_image, displayport_image, cv::COLOR_BGR2BGR565);
	
    //LCD:BGR888(24bit Color)⇒BGR565(16bit Color)
    cv::cvtColor(lcd_image, lcd_image, cv::COLOR_BGR2BGR565);

    // フレームバッファへの書き込み
    size_t screen_size0 = displayport_image.total() * displayport_image.elemSize();
    memcpy(fbAddr0, displayport_image.data, screen_size0);

    size_t screen_size1 = lcd_image.total() * lcd_image.elemSize();
    memcpy(fbAddr1, lcd_image.data, screen_size1);
    

    // リソースの解放
    munmap(fbAddr0, finfo0.smem_len);
    munmap(fbAddr1, finfo1.smem_len);
    
    close(fbfd0);
    close(fbfd1);
    return 0;
}