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
    const uint32_t MAX_MESHES = 12U;
    class G3DEngine* engine;

    G3DRendererPipelineConfig rendererPipelineConfig;
    IG3DCommandList* commandListPerFrame[4];

    class IG3DSampler* sampler;
    class IG3DBuffer* worldConstBuffer;
    class IG3DBufferView* worldConstBufferView;
    void* mappedWorldConstBuffer;
    class IG3DBuffer* viewProjConstBuffer;
    void* mappedViewProjConstBuffer;
    class IG3DBufferView* viewProjConstBufferView;

    void renderScene(G3DRenderConfig* renderConfig);

    public:
    void init(class G3DEngine* engine);
    void render(int frameNumber, class IG3DLogicalRenderTarget* screenColorRT, class IG3DLogicalRenderTarget* screenDepthStencilRT);

    G3DRendererPipelineConfig* getPipelineConfig() { return &rendererPipelineConfig; }

    void destroy();
};