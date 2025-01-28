#ifndef ANIMATION_H
#define ANIMATION_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>
#include "model.h"
#include "mesh.h"
#include "bone.h"


struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation{
public:
    Animation() = default;
    Animation(const std::string& path, Model* model){
        std::cout << "ANIMATION LOADING .. " << std::endl;
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        readHierachy(m_RootNode, scene->mRootNode);
        readMissingBones(animation, *model);
        std::cout << "ANIMATION LOADED .. " << std::endl;
    }

    Bone *FindBone(const std::string& name){
        auto iter = std::find_if(mBones.begin(), mBones.end(), 
        [&](const Bone& Bone){
            return Bone.getBoneName() == name;
        } );
        if (iter == mBones.end()) return nullptr;
        return &(*iter);
    }

    inline float getTicksPerSecond() const { return m_TicksPerSecond; }
    inline float getDuration() const {return m_Duration; }
    inline const AssimpNodeData& getRootNode() {return m_RootNode; }
    inline const std::map<std::string, JointInfo>& getBoneIdMap(){
        return m_BoneInfoMap;
    }

private:

    void readMissingBones(const aiAnimation* animation, Model& model){
        int sz = animation->mNumChannels;
        auto& boneInfoMap = model.getBoneInfoMap();
        int& boneCount = model.getBoneCount();

        for (int i =0; i < sz; ++i){
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;
            if (boneInfoMap.find(boneName) == boneInfoMap.end()){
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            mBones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
            m_BoneInfoMap = boneInfoMap;
        }
    }

    void readHierachy(AssimpNodeData& dest, const aiNode* src){
        assert(src);
        dest.name = src->mName.data;
        dest.transformation = AssimpHelper::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;
        for (int i =0; i < src->mNumChildren; i++){
            AssimpNodeData newData;
            readHierachy(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

    float m_Duration;
    float m_TicksPerSecond;
    std::vector<Bone> mBones;
    AssimpNodeData m_RootNode;
    std::map<std::string, JointInfo> m_BoneInfoMap;
};


#endif