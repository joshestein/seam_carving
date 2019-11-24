#include <iostream>
#include <math.h>  // sqrt
#include "CImg.h"

using namespace cimg_library;

void calculate_gradient(CImg<float> &src, CImg<float> &energy);

int main(int argc, char **argv) {
    CImg<float> image("src_img.png");
    CImg<float> energy(image.width(), image.height());

    calculate_gradient(image, energy);

    CImgDisplay main_disp(energy, "Source Image");
    while (!main_disp.is_closed()) {
        main_disp.wait();
    }
    return 0;
}

void calculate_gradient(CImg<float> &src, CImg<float> &energy) {
    unsigned char red_x, red_y, blue_x, blue_y, green_x, green_y;
    int grad_x, grad_y;

    for (int y = 0; y < src.height(); ++y) {
        for (int x = 0; x < src.width(); ++x) {
            if (x == 0 || y == 0 || x == src.width() - 1 || y == src.height() - 1) {
                energy(x, y) = 1000;
            } else {
                red_x = src(x + 1, y, 0, 0) - src(x - 1, y, 0, 0);
                green_x = src(x + 1, y, 0, 1) - src(x - 1, y, 0, 1);
                blue_x = src(x + 1, y, 0, 2) - src(x - 1, y, 0, 2);

                red_y = src(x, y + 1, 0, 0) - src(x, y - 1, 0, 0);
                green_y = src(x, y + 1, 0, 1) - src(x, y - 1, 0, 1);
                blue_y = src(x, y + 1, 0, 2) - src(x, y - 1, 0, 2);

                grad_x = red_x*red_x + blue_x*blue_x + green_x*green_x; 
                grad_y = red_y*red_y + blue_y*blue_y + green_y*green_y; 

                energy(x, y) = sqrt(grad_x + grad_y);
            }
        }
    }

}