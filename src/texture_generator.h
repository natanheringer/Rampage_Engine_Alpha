#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>

class TextureGenerator {
public:
    // Generate a checkerboard texture
    static std::vector<unsigned char> GenerateCheckerboard(int width, int height, int tileSize);
    
    // Generate a simple colored texture
    static std::vector<unsigned char> GenerateColorTexture(int width, int height, unsigned char r, unsigned char g, unsigned char b);
    
    // Save texture data to file (for debugging)
    static bool SaveTextureToFile(const std::vector<unsigned char>& data, int width, int height, const std::string& filename);
}; 