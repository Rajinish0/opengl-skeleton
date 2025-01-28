#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "animation.h"

class Animator{
public:
    Animator(Animation *animation){
        m_CurrentTime = 0.0f;
        m_CurrentAnimation = animation;
        m_FinalBoneMatrices.reserve(100);
        for (int i =0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void updateAnimation(float dt){
        m_DeltaTime = dt;
        // std::cout << "TOP HERE" << std::endl;
        if (m_CurrentAnimation){
            // std::cout << "HERE" << std::endl;
            m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());
            // std::cout << "CALCULATING BONE TRANSFORM" << std::endl;
            calculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
            // std::cout << "OUT OF THE BONE TRANSFORM" << std::endl;
        }
    }

    void playAnimation(Animation *animation){
        m_CurrentAnimation = animation;
        m_CurrentTime = 0.0;
    }

    void calculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform){
        if (!node) return;
        // std::cout << "IN THE FUNCTION " << std::endl;
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;
        // std::cout << "IN THE FUNCTION 2" << std::endl;

        Bone *bone = m_CurrentAnimation->FindBone(nodeName);
        // std::cout << "IN THE FUNCTION 3" << std::endl;
        if (bone){
            bone->update(m_CurrentTime);
            nodeTransform = bone->getLocalTransform();
        }
        // std::cout << "IN THE FUNCTION 4" << std::endl;
        glm::mat4 globalTrans = parentTransform * nodeTransform;
        // std::cout << "IN THE FUNCTION 5" << std::endl;
        auto boneInfoMap = m_CurrentAnimation->getBoneIdMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()){
            int idx = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            if (idx < m_FinalBoneMatrices.size())
                m_FinalBoneMatrices[idx] = globalTrans * offset;
        }
        // std::cout << "IN THE FUNCTION 6" << std::endl;
        // std::cout << node->childrenCount << std::endl;
        if (node->childrenCount > 0){
            // std::cout << " I AM GOING IN " << std::endl;
            for (int i =0; i < node->childrenCount; i++){
                // std::cout << " AND I AM STUCK " << std::endl;
                calculateBoneTransform(&node->children[i], globalTrans);
                // std::cout << "BACK HERE " << std::endl;
            }
        }
        // std::cout << "IN THE FUNCTION 7" << std::endl;
        // std::cout << "IN THE FUNCTION 7 back" << std::endl;
    }

    std::vector<glm::mat4> getFinalBoneMatrices(){
        return m_FinalBoneMatrices;
    }
private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation *m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};

#endif