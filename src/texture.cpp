#include "texture.h"
#include <iostream>
#include <vector>

// Include stb_image for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() {
    glGenTextures(1, &m_TextureID);
}

Texture::~Texture() {
    if (m_TextureID != 0) {
        glDeleteTextures(1, &m_TextureID);
    }
}

bool Texture::LoadFromFile(const std::string& path) {
    // Load image using stb_image
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Determine format based on number of channels
    GLenum format = GL_RGB;
    if (m_Channels == 4) {
        format = GL_RGBA;
    } else if (m_Channels == 1) {
        format = GL_RED;
    }

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Free image data
    stbi_image_free(data);

    std::cout << "Loaded texture: " << path << " (" << m_Width << "x" << m_Height << ")" << std::endl;
    return true;
}

bool Texture::LoadFromData(const std::vector<unsigned char>& data, int width, int height, int channels) {
    m_Width = width;
    m_Height = height;
    m_Channels = channels;

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Determine format based on number of channels
    GLenum format = GL_RGB;
    if (channels == 4) {
        format = GL_RGBA;
    } else if (channels == 1) {
        format = GL_RED;
    }

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout << "Created texture from data (" << width << "x" << height << ")" << std::endl;
    return true;
}

void Texture::Bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
} 