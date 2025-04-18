#include "framebuffer.h"
#include <glad/glad.h>
#include <iostream>
#include <cstdio>  


// Framebuffer constructor
// This constructor initializes the framebuffer object
// It creates a texture object and a renderbuffer object
// It sets the size of the framebuffer object
// It sets the size of the texture object
// It sets the size of the renderbuffer object
Framebuffer::Framebuffer(unsigned int width, unsigned int height)
    : m_Width(width), m_Height(height) {

        // glGenFramebuffers(1, &m_FBO);
        // means that the framebuffer object is created with GL framebuffer
        // glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        // means that the framebuffer object is bound to the GL framebuffer
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // glGenTextures(1, &m_TextureID);  
    // means that the texture object is created with GL texture
    glGenTextures(1, &m_TextureID);
    // glBindTexture(GL_TEXTURE_2D, m_TextureID);
    // means that the texture object is bound to the GL texture
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    // glTexImage2D is used to create a 2D texture
    // it takes the following parameters:
    // target, level, internalformat, width, height, border, format, type, data
    // target is the target texture
    // level is the level of detail
    // internalformat is the internal format of the texture
    // width is the width of the texture
    // height is the height of the texture
    // border is the border of the texture
    // format is the format of the texture
    // type is the type of the texture
    // data is the data of the texture
    // nullptr means that the texture is empty
    // GL_TEXTURE_2D means that the texture is a 2D texture
    // GL_RGB means that the texture is in RGB format
    // GL_UNSIGNED_BYTE means that the texture is in unsigned byte format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // glTexParameteri is used to set the texture parameters
    // it takes the following parameters:
    // target, pname, param
    // target is the target texture
    // pname is the name of the parameter
    // param is the value of the parameter
    // GL_TEXTURE_MIN_FILTER means that the texture is minified
    // GL_LINEAR means that the texture is linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glFramebufferTexture2D is used to attach the texture to the framebuffer
    // it takes the following parameters:
    // target, attachment, textarget, texture, level
    // target is the target framebuffer
    // attachment is the attachment point
    // textarget is the target texture
    // texture is the texture object
    // level is the level of detail
    // GL_FRAMEBUFFER means that the framebuffer is a GL framebuffer
    // GL_COLOR_ATTACHMENT0 means that the attachment point is a color attachment   
    // GL_TEXTURE_2D means that the texture is a 2D texture
    // m_TextureID is the texture object
    // 0 means that the level of detail is 0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    // what is done here is 
    // to create a renderbuffer object
    // it is used to store the depth and stencil information
    // glGenRenderbuffers is used to create a renderbuffer object
    // it takes the following parameters:
    // n, renderbuffers
    // n is the number of renderbuffers to create
    // renderbuffers is the renderbuffer object
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    // This condition checks if the framebuffer is complete
    // glCheckFramebufferStatus is used to check the status of the framebuffer
    // it takes the following parameters:
    // target
    // target is the target framebuffer
    // GL_FRAMEBUFFER means that the framebuffer is a GL framebuffer
    // GL_FRAMEBUFFER_COMPLETE means that the framebuffer is complete
    // the std::cerr is used to print the error message
    // if the framebuffer is not complete  
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Erro: Framebuffer incompleto!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Destructor
// This destructor deletes the framebuffer object
// It deletes the texture object
// It deletes the renderbuffer object
Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_TextureID);
    glDeleteRenderbuffers(1, &m_RBO);
}

// Bind the framebuffer object
// This function binds the framebuffer object
// It sets the viewport size to the size of the framebuffer object
// It takes no parameters
// It returns nothing
void Framebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, m_Width, m_Height);
}

// Unbind the framebuffer object
// This function unbinds the framebuffer object
void Framebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Resize the framebuffer object
// This function resizes the framebuffer object
// It takes the following parameters:
// width, height
// width is the new width of the framebuffer object
// height is the new height of the framebuffer object
void Framebuffer::Resize(unsigned int width, unsigned int height) {
    m_Width = width;
    m_Height = height;

    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
}