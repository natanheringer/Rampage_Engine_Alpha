#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

class Texture {
public:
    Texture();
    ~Texture();

    // Load texture from file
    bool LoadFromFile(const std::string& path);
    
    // Load texture from data
    bool LoadFromData(const std::vector<unsigned char>& data, int width, int height, int channels);
    
    // Bind texture to a texture unit
    void Bind(unsigned int unit = 0) const;
    
    // Unbind texture
    void Unbind() const;
    
    // Get texture ID
    GLuint GetID() const { return m_TextureID; }
    
    // Get texture dimensions
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    GLuint m_TextureID = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_Channels = 0;
}; 