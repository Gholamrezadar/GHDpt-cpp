#pragma once

#include <vector>
#include <glad/glad.h>
#include <iostream>

class GHDImage {
private:
    int width, height;
    std::vector<unsigned char> pixels; // Flat array to store RGBA pixels
    GLuint textureId;
    bool textureInitialized;

public:
    // default constructor
    GHDImage() {}
    GHDImage(int w, int h) : width(w), height(h), textureInitialized(false) {
        pixels.resize(width * height * 4, 255); // Initialize with white (RGBA)
    }

    void set_pixel(int i, int j, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
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

    void bind_texture() {
        if (!textureInitialized) {
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            textureInitialized = true;
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }

    GLuint get_texture_id() const {
        return textureId;
    }

    void* get_imgui_texture_id() const {
        return reinterpret_cast<void*>(static_cast<intptr_t>(textureId));
    }

    int get_width() const { return width; }
    int get_height() const { return height; }

    ~GHDImage() {
        if (textureInitialized) {
            glDeleteTextures(1, &textureId);
        }
    }
};

// Example usage:
// Image img(256, 256);
// img.set_pixel(10, 10, 255, 0, 0); // Set pixel (10, 10) to red
// img.bind_texture();
// ImGui::Image(img.get_imgui_texture_id(), ImVec2(img.get_width(), img.get_height()));
