#pragma once
#include "allocators/allocator-v2.h"
#include "allocators/g3d-stack-allocator.h"
#include "components/renderable-component.h"
#include "g3d-engine.h"
#include "g3d-execution/g3d-command-list.h"
#include "frame-graph/fg-action-node.h"
#include "frame-graph/frame-graph.h"
#include <array>
#include <vector>


struct G3DPipelineConfiguration {
    bool depthTestEnabled = true;
    bool depthWriteEnabled = true;
    int vertexStageId = -1;
    int fragmentStageId = -1;
    int pipelineStateId = -1;
    int vertexBindingInputId = -1;
    int resourceSetSignatureId = -1;

    bool operator==(const G3DPipelineConfiguration other) {
        return depthTestEnabled == other.depthTestEnabled &&
                depthWriteEnabled == other.depthWriteEnabled &&
                vertexStageId == other.vertexStageId &&
                fragmentStageId == other.fragmentStageId &&
                pipelineStateId == other.pipelineStateId &&
                vertexBindingInputId == other.vertexBindingInputId &&
                resourceSetSignatureId == other.resourceSetSignatureId;
    }

    bool operator==(const G3DPipelineConfiguration* other) {
        return *this == *other;
    }
};

class G3DPipelineSubsystem {

    /**
     * TODO:
     * - Store active depth stencil state
     * 
     */

    G3DEngine* engine = nullptr;

    // The current pipeline configuration
    G3DPipelineConfiguration* m_currentPipelineConfiguration;

    std::unordered_map<G3DPipelineConfiguration*, class IG3DRenderPipeline*> m_configPipelineMap = {};

    // This will be replaced with a faster technique, for now store up to 12 pipeline configurations
    std::array<G3DPipelineConfiguration, 12> m_pipelineConfigurations = {};

    // Pipeline state cache
    std::vector<G3DVertexBindingInputs> m_vertexBindingInputs = {};
    std::vector<G3DPipelineStageVertex> m_vertexStages = {};
    std::vector<G3DPipelineStageFragment> m_fragmentStages = {};
    std::vector<G3DPipelineState> m_pipelineStates = {};
    std::vector<class IG3DResourceSetSignature*> m_resourceSetSignatures = {};

    public:

    void init(class G3DEngine* engine);

    int addVertexBindingInputs(G3DVertexBindingInputs inputs) {
        m_vertexBindingInputs.push_back(inputs);
        return m_vertexBindingInputs.size() - 1;
    }

    int addVertexStage(G3DPipelineStageVertex stage) {
        m_vertexStages.push_back(stage);
        return m_vertexStages.size() - 1;
    }

    int addFragmentStage(G3DPipelineStageFragment stage) {
        m_fragmentStages.push_back(stage);
        return m_fragmentStages.size() - 1;
    }


    int addPipelineState(G3DPipelineState state) {
        m_pipelineStates.push_back(state);
        return m_pipelineStates.size() - 1;
    }

    int addResourceSetSignature(class IG3DResourceSetSignature* signature) {
        m_resourceSetSignatures.push_back(signature);
        return m_resourceSetSignatures.size() - 1;
    }

    int numConfigs = 0;
    int createPipelineConfiguration(G3DPipelineConfiguration config) {
        m_pipelineConfigurations[numConfigs++] = config;
        return m_pipelineConfigurations.size() - 1;
    }

    class IG3DRenderPipeline* getPipelineForConfiguration(G3DPipelineConfiguration* config);

    FGID get_SetDepthTestNode(G3DFrameGraph* frameGraph);

    FGID get_SetDepthWriteNode(G3DFrameGraph* frameGraph);

    FGID get_SetVertexShaderNode(G3DFrameGraph* frameGraph);

    FGID get_SetFragmentShaderNode(G3DFrameGraph* frameGraph);


    /**
     * Ensures that the correct pipeline for the desired configuration is bound
     *
     * @note If no matching pipeline is found, a new pipeline is created then bound
     * 
     * @note If a matching pipeline is found, it is bound
     */
    FGID get_UpdatePipelineNode(G3DFrameGraph* frameGraph);

    
};