#pragma once
#include "g3d-defines.h"

class PBRMaterial {
    uint32_t width = 0U, height = 0U;

    // The resource set signature representing all pbr materials
    inline static class IG3DResourceSetSignature *resourceSetSignature = nullptr;

    class IG3DResourceSetInstance *resourceSetInstance = nullptr;

    public:

    static void initSignature(class G3DEngine* engine);

    static inline uint8_t mipLevels = 4U;

    static inline uint32_t pipelineBindingCount = 4U;

    static inline G3DPipelineBinding pipelineBindings[] {

        { // The albedo texture
            .type = G3DPipelineBindingType::G3DImageViewSample,
            .shaderBitmap = G3DShader::G3DFragmentShader,
        },

        { // The normal texture
            .type = G3DPipelineBindingType::G3DImageViewSample,
            .shaderBitmap = G3DShader::G3DFragmentShader,
        },

        { // The roughness texture
            .type = G3DPipelineBindingType::G3DImageViewSample,
            .shaderBitmap = G3DShader::G3DFragmentShader,
        },
 
        { // The ambient occlusion texture
            .type = G3DPipelineBindingType::G3DImageViewSample,
            .shaderBitmap = G3DShader::G3DFragmentShader,
        }
    };

    // The images are stored in the order albedo, normal, roughness, ao
    class IG3DImage* images[4];
    class IG3DImageView* imageViews[4];

    void init (
            class G3DEngine* engine,
            // The images are stored in the order albedo, normal, roughness, ao
            class Image* pbrTextures[4]
        );

    class IG3DResourceSetInstance* getResourceSetInstance() {
        return resourceSetInstance;
    } 

    static IG3DResourceSetSignature* getResourceSetSignature() {
        return resourceSetSignature;
    }
};