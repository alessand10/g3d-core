#pragma once
#include "minimal-type-defines.h"
#include "g3d-render-config.h"

struct G3DRendererPipelineConfig {
    uint32_t pipelineBindingCount = 3U;
    class G3DPipelineBinding* pipelineBindings;

    uint32_t defaultBindingUpdateCount = 3U;
    class G3DBindingUpdate* defaultBindingUpdates[3];
};


// The subsystem of the G3D Engine that performs rendering and manages render states
class G3DRenderer {
    class G3DEngine* engine;

    G3DRendererPipelineConfig rendererPipelineConfig;

    class IG3DSampler* sampler;
    class IG3DBuffer* worldConstBuffer;
    class IG3DBufferView* worldConstBufferView;
    class IG3DBuffer* viewProjConstBuffer;
    class IG3DBufferView* viewProjConstBufferView;

    void renderScene(G3DRenderConfig* renderConfig);

    public:
    void init(class G3DEngine* engine);
    void render();

    G3DRendererPipelineConfig* getPipelineConfig() { return &rendererPipelineConfig; }

    void destroy();
};