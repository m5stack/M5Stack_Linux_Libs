#define cimg_display 0 // Disable display (optional)
#define cimg_use_jpeg  // Uncomment this line if you want to use JPEG format
#include "CImg.h"

using namespace cimg_library;

int main() {
    const int width = 400;
    const int height = 300;

    // Create a blank image
    CImg<unsigned char> image(width, height, 1, 3, 0);

    // Draw a red line on the image
    const unsigned char red[] = {255, 0, 0};
    image.draw_line(50, 100, 350, 200, red);

    // Save the image with the drawn line
    image.save("line.jpg");

    return 0;
}
