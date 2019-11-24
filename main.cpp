#include <iostream>
#include <math.h>  // sqrt
#include <vector>
#include "CImg.h"

using namespace cimg_library;

CImg<float> calculate_gradient(CImg<float> &src);
void forward_energy(CImg<float> &energy);
std::vector<int> find_vertical_seam(CImg<float> &energy);
std::vector<int> find_horizontal_seam(CImg<float> &energy);

int main(int argc, char **argv) {
    CImg<float> image("src_img.png");
    CImg<float> energy(image.width(), image.height());

        CImg<float> energy = calculate_gradient(image);
    forward_energy(energy);
        std::vector<int> seam = find_vertical_seam(energy);
        new_img = remove_vertical_seam(image, seam);

    CImgDisplay main_disp(energy, "Source Image");
    while (!main_disp.is_closed()) {
        main_disp.wait();
    }
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
