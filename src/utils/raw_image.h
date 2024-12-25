#pragma once

#include <vector>
#include <glad/glad.h>
#include <iostream>
#include "color.h"

class RawImage {
private:
    int width, height;
    std::vector<double> pixels; // Flat array to store RGBA pixels
    friend class Renderer;
    friend class GHDImage;

public:
    // default constructor
    RawImage() {}
    RawImage(int w, int h) : width(w), height(h) {
        pixels.resize(width * height * 4, 0.0); // Initialize with white (RGBA)
    }

    //get pixel
    color get_pixel(int i, int j) {
        if (i < 0 || i >= height || j < 0 || j >= width) {
            std::cerr << "Pixel coordinates out of bounds: (" << i << ", " << j << ")\n";
            return color(0, 0, 0);
        }

        int index = (i * width + j) * 4;
        return color(pixels[index], pixels[index + 1], pixels[index + 2]);
    }

    void set_pixel(int i, int j, double r, double g, double b, double a = 1.0) {
        if (i < 0 || i >= height || j < 0 || j >= width) {
            std::cerr << "Pixel coordinates out of bounds: (" << i << ", " << j << ")\n";
            return;
        }

        int index = (i * width + j) * 4;
        pixels[index] = r;
        pixels[index + 1] = g;
        pixels[index + 2] = b;
        pixels[index + 3] = a;
    }
    int get_width() const { return width; }
    int get_height() const { return height; }

};
// Example usage:
// Image img(256, 256);
// img.set_pixel(10, 10, 255, 0, 0); // Set pixel (10, 10) to red
// img.bind_texture();
// ImGui::Image(img.get_imgui_texture_id(), ImVec2(img.get_width(), img.get_height()));
