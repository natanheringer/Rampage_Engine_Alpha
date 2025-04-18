#pragma once

// this class is used to create a framebuffer object 
// it is used to render to a texture 
// it is used to create a framebuffer object
// it is used to create a renderbuffer object
// it is used to create a texture object
// framebuffer means that a frame is rendered to a texture
// renderbuffer means that a buffer is used to store the depth and stencil information
// texture means that a texture is used to store the color information
class Framebuffer {
public:
    Framebuffer(unsigned int width, unsigned int height);
    ~Framebuffer();

    void Bind();
    void Unbind();

    unsigned int GetTextureID() const { return m_TextureID; }
    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }

// set the size of the framebuffer 
// width and height are the new size 

    void Resize(unsigned int width, unsigned int height);

    // private means that the function is only accessible within this class
    // it cannot be called from outside 
    // unsignaed int m_FBO = 0 means that 
    // the framebuffer object is not created yet 
    // unsigned int m_TextureID = 0 means that
    // the texture is not created yet
    // unsigned int m_RBO = 0 means 
    // the renderbuffer object is not created yet
    // unsigned int m_Width, m_Height;
    // means that the width and height are not set yet
private:
    unsigned int m_FBO = 0;
    unsigned int m_TextureID = 0;
    unsigned int m_RBO = 0;
    unsigned int m_Width, m_Height;
};