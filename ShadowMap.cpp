#include "ShadowMap.h"

ShadowMap::ShadowMap() {

    FBO = 0;
    shadowMap = 0;

}

bool ShadowMap::Init(GLuint width, GLuint height) {

    shadowWidth = width;
    shadowHeight = height;

    // Generation the shadow texture
    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Defining how the texture will act
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER
   
    float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glTextureParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);

    // Defining filters to mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR -> blur effect 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST -> pixelated


    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // GL_DEPTH_ATTACHMENT -> using the depth values of the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    glDrawBuffer(GL_NONE); // for GL_COLOR_ATTACHMENT, but here is NONE, because we need only depth
    glReadBuffer(GL_NONE);

    // Check for errors on the shadow framebuffer 
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {

        printf("Shadow Framebuffer error: %i \n", status);

        return false;

    }

    // Unbinding the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void ShadowMap::Write() {

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

}

void ShadowMap::Read(GLenum textureUnit) {

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, shadowMap);

}

ShadowMap::~ShadowMap() {

    if (FBO) {

        glDeleteFramebuffers(1, &FBO);

    }

    if (shadowMap) {

        glDeleteTextures(1, &shadowMap);

    }

}
