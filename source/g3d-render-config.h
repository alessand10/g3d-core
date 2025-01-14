#pragma once

struct G3DRenderConfig {
    class IG3DLogicalRenderTarget* m_colorRT; // The color render target, optional
    class IG3DLogicalRenderTarget* m_depthStencilRT; // The depth stencil render target, optional
    class G3DViewport* m_viewport;
    class G3DScissor* m_scissor;
    class IG3DCommandList* m_commandList;
    class G3DCameraComponent* m_cameraComponent;
};