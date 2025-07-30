#include "texture_generator.h"
#include <iostream>
#include <fstream>
#include <string>

std::vector<unsigned char> TextureGenerator::GenerateCheckerboard(int width, int height, int tileSize) {
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            
            // Create checkerboard pattern
            bool isEvenTile = ((x / tileSize) + (y / tileSize)) % 2 == 0;
            
            if (isEvenTile) {
                data[index] = 255;     // R
                data[index + 1] = 255; // G
                data[index + 2] = 255; // B
            } else {
                data[index] = 100;     // R
                data[index + 1] = 100; // G
                data[index + 2] = 100; // B
            }
        }
    }
    
    return data;
}

std::vector<unsigned char> TextureGenerator::GenerateColorTexture(int width, int height, unsigned char r, unsigned char g, unsigned char b) {
    std::vector<unsigned char> data(width * height * 3);
    
    for (int i = 0; i < width * height; ++i) {
        data[i * 3] = r;     // R
        data[i * 3 + 1] = g; // G
        data[i * 3 + 2] = b; // B
    }
    
    return data;
}

bool TextureGenerator::SaveTextureToFile(const std::vector<unsigned char>& data, int width, int height, const std::string& filename) {
    // Simple PPM format for debugging
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file << "P6\n" << width << " " << height << "\n255\n";
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    return true;
} 