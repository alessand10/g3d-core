#include "pipeline-subsystem.h"
#include "g3d-system.h"
#include "g3d-pipeline-config.h"

G3DPipelineSubsystem* pipelineSubsystemRef = nullptr;

void G3DPipelineSubsystem::init(G3DEngine *engine)
{
    this->engine = engine;
    pipelineSubsystemRef = this;
}

IG3DRenderPipeline *G3DPipelineSubsystem::getPipelineForConfiguration(G3DPipelineConfiguration *config)
{
    G3DPipelineConfiguration* foundConfig = nullptr;

    for (int i = 0; i < numConfigs; i++)
    {
        G3DPipelineConfiguration* pipelineConfig = &m_pipelineConfigurations[i];
        if (*pipelineConfig == *config)
        {
            foundConfig = pipelineConfig;
            break;
        }
    }

    if (foundConfig == nullptr)
    {

        m_pipelineConfigurations[numConfigs] = *config;
        IG3DDevice *device = engine->getDevice();

        G3DRenderPipelineInfo pipelineInfo {
            .device = device,
            .config = m_pipelineStates[config->pipelineStateId],
            .vertexStage = m_vertexStages[config->vertexStageId],
            .fragmentStage = m_fragmentStages[config->fragmentStageId],
            .constantSet = m_constantSets[config->pipelineStateId]
        };

        IG3DResourceSetSignature* supportedSignatures[] { m_resourceSetSignatures[config->resourceSetSignatureId] };

        IG3DRenderPipeline* pipeline = engine->getPipelineFactory()->createRenderPipeline(&pipelineInfo, supportedSignatures, 1);

        m_configPipelineMap[&m_pipelineConfigurations[numConfigs]] = pipeline;

        numConfigs++;
        
        return pipeline;
    }
    else {
        return m_configPipelineMap[foundConfig];
    }
}

FGID G3DPipelineSubsystem::get_SetDepthTestNode(G3DFrameGraph *frameGraph)
{
    FGID actionNodeId = frameGraph->addNode<FGActionNode>();
    FGActionNode *actionNode = frameGraph->get<FGActionNode>(actionNodeId);

    actionNode->setAction([](FGActionNode* node) {
        bool* depthTestEnabled = node->getInputData<CLASS_HASH(bool)>(0);
        pipelineSubsystemRef->m_currentPipelineConfiguration->depthTestEnabled = depthTestEnabled;
    });

    return actionNodeId;
}

FGID G3DPipelineSubsystem::get_SetDepthWriteNode(G3DFrameGraph* frameGraph) {
    FGID actionNodeId = frameGraph->addNode<FGActionNode>();
    FGActionNode *actionNode = frameGraph->get<FGActionNode>(actionNodeId);

    actionNode->setAction([](FGActionNode* node) {
        bool* depthWriteEnabled = node->getInputData<CLASS_HASH(bool)>(0);
        pipelineSubsystemRef->m_currentPipelineConfiguration->depthWriteEnabled = depthWriteEnabled;
    });

    return actionNodeId;
}

FGID G3DPipelineSubsystem::get_SetVertexShaderNode(G3DFrameGraph* frameGraph) {
    FGID actionNodeId = frameGraph->addNode<FGActionNode>();
    FGActionNode *actionNode = frameGraph->get<FGActionNode>(actionNodeId);

    actionNode->setAction([](FGActionNode* node) {
        int* vertexShaderId = node->getInputData<CLASS_HASH(int)>(0);
        pipelineSubsystemRef->m_currentPipelineConfiguration->vertexStageId = *vertexShaderId;
    });

    return actionNodeId;
}

FGID G3DPipelineSubsystem::get_SetFragmentShaderNode(G3DFrameGraph* frameGraph) {
    FGID actionNodeId = frameGraph->addNode<FGActionNode>();
    FGActionNode *actionNode = frameGraph->get<FGActionNode>(actionNodeId);

    actionNode->setAction([](FGActionNode* node) {
        int* fragmentShaderId = node->getInputData<CLASS_HASH(int)>(0);
        pipelineSubsystemRef->m_currentPipelineConfiguration->fragmentStageId = *fragmentShaderId;
    });

    return actionNodeId;
}

FGID G3DPipelineSubsystem::get_UpdatePipelineNode(G3DFrameGraph *frameGraph)
{
    FGID actionNodeId = frameGraph->addNode<FGActionNode>();
    FGActionNode *actionNode = frameGraph->get<FGActionNode>(actionNodeId);

    actionNode->setAction([](FGActionNode* node) {
        auto matchingPipelineConfig = std::find(pipelineSubsystemRef->m_pipelineConfigurations.begin(),
            pipelineSubsystemRef->m_pipelineConfigurations.end(), 
            pipelineSubsystemRef->m_currentPipelineConfiguration
        );

        if (matchingPipelineConfig == pipelineSubsystemRef->m_pipelineConfigurations.end()) {
            // Create a new pipeline config
            pipelineSubsystemRef->m_pipelineConfigurations[pipelineSubsystemRef->numConfigs++] = (*pipelineSubsystemRef->m_currentPipelineConfiguration);
        } 


    });

    return actionNodeId;
}
