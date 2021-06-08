#pragma once
#include "ShadowMap.h"
class OmniShadowMap :
    public ShadowMap {

public:
    OmniShadowMap();

    bool Init(unsigned width, unsigned height);
    void Write();
    void Read(GLenum textureUnit);

    ~OmniShadowMap();
private:



};

