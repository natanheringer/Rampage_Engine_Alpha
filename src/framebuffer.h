#pragma once

class Framebuffer {
public:
    Framebuffer(unsigned int width, unsigned int height);
    ~Framebuffer();

    void Bind();
    void Unbind();

    unsigned int GetTextureID() const { return m_TextureID; }
    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }


    void Resize(unsigned int width, unsigned int height);

private:
    unsigned int m_FBO = 0;
    unsigned int m_TextureID = 0;
    unsigned int m_RBO = 0;
    unsigned int m_Width, m_Height;
};