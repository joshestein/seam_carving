#include <iostream>
#include "CImg.h"

using namespace cimg_library;

int main(int argc, char **argv) {
    CImg<float> image("src_img.png");

    // get x, y gradients
    CImgList<float> grad = image.get_gradient();

    // compute gradient norm
    CImg<unsigned char> energy = (grad[0].pow(2) + grad[1].pow(2)).normalize(0, 255);

    CImgDisplay main_disp(energy, "Source Image");
    while (!main_disp.is_closed()) {
        main_disp.wait();
    }
    return 0;
}