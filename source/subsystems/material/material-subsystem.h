#pragma once
#include <vector>
#include <unordered_map>

typedef int MaterialID;

class G3DMaterialSubsystem {
    class G3DEngine* m_engine = nullptr;
    std::vector<class IG3DResourceSetSignature*> m_pipelineResourceSetSignatures = {};
    std::vector<class IG3DResourceSetInstance*> m_pipelineResourceSetInstance = {};

    std::unordered_map<MaterialID, class IG3DResourceSetInstance*> m_materialResourceSetInstances = {};
    
    public:
    G3DMaterialSubsystem() {};
    ~G3DMaterialSubsystem() {};

    void init(class G3DEngine* engine) {
        m_engine = engine;
    }

    MaterialID createMaterial(class IG3DResourceSetSignature* signature, class IG3DResourceSetInstance* instance);

    class IG3DResourceSetInstance* getMaterialResourceSetInstance(MaterialID id) {
        return m_materialResourceSetInstances[id];
    }

    class IG3DResourceSetSignature* getPipelineResourceSetSignature(MaterialID id);

    void destroy();
};