#pragma once
#include "image.h"

class ImageLoader {
    
    public:
    static G3DHostImage loadJPEGImage(const char* path, uint32_t byteAlignment = 0U);
};