#include <iostream>
#include <math.h>  // sqrt
#include <vector>
#include "CImg.h"

using namespace cimg_library;

CImg<float> calculate_gradient(CImg<float> &src);
void forward_energy(CImg<float> &energy);
std::vector<int> find_vertical_seam(CImg<float> &energy);
std::vector<int> find_horizontal_seam(CImg<float> &energy);
CImg<float> remove_vertical_seam(CImg<float> &src, std::vector<int> x_positions);

int main(int argc, char **argv) {
    CImg<float> image("src_img.png");
    CImg<float> new_img;

    for (int i = 0; i < 10; ++i) {
        CImg<float> energy = calculate_gradient(image);
        forward_energy(energy);
        std::vector<int> seam = find_vertical_seam(energy);
        new_img = remove_vertical_seam(image, seam);

        seam.clear();
    }
    new_img.save("out.png");

    // auto iter = seam.end();
    // for (int y = 0; y < image.height(), iter != seam.begin(); ++y, --iter) {
    //     seams(*iter, y) = 255;
    // }

    // std::vector<int> seam = find_horizontal_seam(energy);

    // auto iter = seam.end();
    // for (int x = 0; x < image.width(), iter != seam.begin(); ++x, --iter) {
    //     std::cout << *iter << std::endl;
    //     seams(x, *iter) = 255;
    // }

    // CImgDisplay main_disp(image, "Source Image");
    // while (!main_disp.is_closed()) {
    //     main_disp.wait();
    // }
    return 0;
}

CImg<float> calculate_gradient(CImg<float> &src) {
    unsigned char red_x, red_y, blue_x, blue_y, green_x, green_y;
    int grad_x, grad_y;
    CImg<float> energy(src.width(), src.height());

    for (int y = 0; y < src.height(); ++y) {
        for (int x = 0; x < src.width(); ++x) {
            if (x == 0 || y == 0 || x == src.width() - 1 || y == src.height() - 1) {
                energy(x, y) = 1000;
            } else {
                // calculate x gradients for each colour channel
                red_x = src(x + 1, y, 0, 0) - src(x - 1, y, 0, 0);
                green_x = src(x + 1, y, 0, 1) - src(x - 1, y, 0, 1);
                blue_x = src(x + 1, y, 0, 2) - src(x - 1, y, 0, 2);

                // calculate y gradients for each colour channel
                red_y = src(x, y + 1, 0, 0) - src(x, y - 1, 0, 0);
                green_y = src(x, y + 1, 0, 1) - src(x, y - 1, 0, 1);
                blue_y = src(x, y + 1, 0, 2) - src(x, y - 1, 0, 2);

                grad_x = red_x*red_x + blue_x*blue_x + green_x*green_x; 
                grad_y = red_y*red_y + blue_y*blue_y + green_y*green_y; 

                energy(x, y) = sqrt(grad_x + grad_y);
            }
        }
    }
    return energy;
}

void forward_energy(CImg<float> &energy) {
    // start at 1 due to edges being set to known 1000
    for (int y = 1; y < energy.height(); ++y) { 
        for (int x = 1; x < energy.width(); ++x) { 
            // find minimum element energy on top of current element
            int min_element = std::min({energy(x-1, y-1), energy(x, y-1), energy(x, y+1)});

            // forward pass update
            energy(x, y) += min_element;
        }
    }

}

std::vector<int> find_vertical_seam(CImg<float> &energy) {
    std::vector<int> min_element_idx;
    int min_idx, min_element;

    // find smallest element in second from bottom row
    min_element = INT_MAX;
    for (int x = 1; x < energy.width(); ++x) {
        if (energy(x, energy.height() - 1) < min_element) {
            min_element = energy(x, energy.height() - 1);
            min_idx = x;
        }
    }
    // push back bottom row (border element)
    min_element_idx.push_back(min_idx);
    // push back _actual_ element
    min_element_idx.push_back(min_idx);

    int next_min_shift;
    for (int y = energy.height() - 2; y > 0; --y) {
        min_element = energy(min_idx - 1, y);
        next_min_shift = -1;
        if (energy(min_idx, y) < min_element) {
            min_element = energy(min_idx, y);
            next_min_shift = 0;
        }
        if (energy(min_idx + 1, y) < min_element) {
            min_element = energy(min_idx + 1, y);
            next_min_shift = 1;
        }
        min_idx += next_min_shift;
        min_element_idx.push_back(min_idx);
    }
    // push back final (top) row
    min_element_idx.push_back(min_idx);
    return min_element_idx;
}

std::vector<int> find_horizontal_seam(CImg<float> &energy) {
    std::vector<int> min_element_idx;
    int min_idx, min_element;

    // find smallest element in second from bottom col
    min_element = INT_MAX;
    for (int y = 1; y < energy.height(); ++y) {
        if (energy(energy.width() - 1, y) < min_element) {
            min_element = energy(energy.width() - 1, y);
            min_idx = y;
        }
    }
    // push back last col (border element)
    min_element_idx.push_back(min_idx);
    // push back _actual_ element
    min_element_idx.push_back(min_idx);

    int next_min_shift;
    for (int x = energy.width() - 2; x > 0; --x) {
        min_element = energy(x, min_idx - 1);
        next_min_shift = -1;
        if (energy(x, min_idx) < min_element) {
            min_element = energy(x, min_idx);
            next_min_shift = 0;
        }
        if (energy(x, min_idx + 1) < min_element) {
            min_element = energy(x, min_idx + 1);
            next_min_shift = 1;
        }
        min_idx += next_min_shift;
        min_element_idx.push_back(min_idx);
    }
    // push back final (top) col
    min_element_idx.push_back(min_idx);
    return min_element_idx;
}

CImg<float> remove_vertical_seam(CImg<float> &src, std::vector<int> x_positions) {
    CImg<float> new_img(src.width() - 1, src.height());
    auto iter = x_positions.end();
    for (int y = 0; y < src.height(), iter != x_positions.begin(); ++y, --iter) {
        for (int x = *iter; x < src.width() - 1; ++x) {
            // shift pixels from right hand side filling removed pixel
            // src(x, y) = src(x + 1, y);
            src(x, y, 0, 0) = src(x + 1, y, 0, 0);
            src(x, y, 0, 1) = src(x + 1, y, 0, 1);
            src(x, y, 0, 2) = src(x + 1, y, 0, 2);

        }
    }
    new_img = src.get_crop(0, 0, 0, 0, src.width() - 1, src.height(), 0, 3);
    return new_img;
    // src.crop(0, 0, 0, 0, src.width() - 1, src.height(), 0, 3);
}