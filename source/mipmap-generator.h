#pragma once

class MipmapGenerator {
    public:
    inline static class IG3DComputePipeline* computePipeline = nullptr;
    inline static class IG3DResourceSetSignature* computeShaderRSet = nullptr;
    inline static class IG3DResourceSetInstance* computeShaderRSetInstance = nullptr;

    inline static class G3DEngine* engine = nullptr;
    
    static void init(class G3DEngine* application);

    static void generateMipmaps(class IG3DImage* image);
};