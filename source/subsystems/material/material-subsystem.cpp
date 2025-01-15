#include "material-subsystem.h"
#include <algorithm>
#include "g3d-pipeline-config.h"

MaterialID G3DMaterialSubsystem::createMaterial(IG3DResourceSetSignature *signature, IG3DResourceSetInstance *instance)
{
    if (std::find(m_pipelineResourceSetSignatures.begin(), m_pipelineResourceSetSignatures.end(), signature) == m_pipelineResourceSetSignatures.end()) {
        m_pipelineResourceSetSignatures.push_back(signature);
    }
    m_pipelineResourceSetInstance.push_back(instance);

    MaterialID id = m_pipelineResourceSetInstance.size() - 1;
    m_materialResourceSetInstances[id] = instance;

    return id;
}

IG3DResourceSetSignature *G3DMaterialSubsystem::getPipelineResourceSetSignature(MaterialID id)
{
    
    return getMaterialResourceSetInstance(id)->getSignature();
    
}

void G3DMaterialSubsystem::destroy()
{
    for (auto &signature : m_pipelineResourceSetSignatures) {
        signature->destroy();
    }
}
