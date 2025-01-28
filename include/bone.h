#ifndef BONE_H
#define BONE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp_helper.h"

struct KeyPosition{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

class Bone{
public:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;

    int m_NumPositions;
    int m_NumRotations;
    int m_NumScales;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;

    Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        :m_Name(name), m_ID(ID), m_LocalTransform(1.0){
            m_NumPositions = channel->mNumPositionKeys;
            for (int i =0; i < m_NumPositions; ++i){
                aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
                float timeStamp = channel->mPositionKeys[i].mTime;

                KeyPosition data;
                data.position = AssimpHelper::GetGLMVec(aiPosition);
                data.timeStamp = timeStamp;
                m_Positions.push_back(data);
            }

            m_NumRotations = channel->mNumRotationKeys;
            for (int i =0; i < channel->mNumRotationKeys; ++i){
                aiQuaternion aiOrientation = channel->mRotationKeys[i].mValue;
                float timeStamp = channel->mRotationKeys[i].mTime;

                KeyRotation data;
                data.orientation = AssimpHelper::GetGLMQuat(aiOrientation);
                data.timeStamp = timeStamp;
                m_Rotations.push_back(data);
            }

            m_NumScales = channel->mNumScalingKeys;
            for (int i =0; i < channel->mNumScalingKeys; ++i){
                aiVector3D aiScale = channel->mScalingKeys[i].mValue;
                float timeStamp = channel->mScalingKeys[i].mTime;

                KeyScale data;
                data.scale = AssimpHelper::GetGLMVec(aiScale);
                data.timeStamp = timeStamp;
                m_Scales.push_back(data);
            }

        }

        void update(float animationTime){
            glm::mat4 translation = interpolatePosition(animationTime);
            glm::mat4 rotation = interpolateRotation(animationTime);
            glm::mat4 scale = interpolateScale(animationTime);
            m_LocalTransform = translation*rotation*scale;
        }

        glm::mat4 getLocalTransform() const {return m_LocalTransform;}
        std::string getBoneName() const {return m_Name;}
        int getBoneID() const { return m_ID;}

        template<typename T>
        int getIndex(std::vector<T>& v, size_t sz, float animationTime){
            for (int i =0; i < sz - 1; ++i){
                if (animationTime < v[i+1].timeStamp)
                    return i;
            }
            assert(0);
        }

private:
    float getScaleFactor(float lastT, float nextT, float animationTime){
        return (animationTime-lastT)/(nextT-lastT);
    }

    glm::mat4 interpolatePosition(float animationTime){
        if (m_NumPositions == 1)
            return glm::translate(glm::mat4(1.0), m_Positions[0].position);
        int i1 = getIndex(m_Positions, m_NumPositions, animationTime);
        int i2 = i1 + 1;
        float percent = getScaleFactor(m_Positions[i1].timeStamp, m_Positions[i2].timeStamp, animationTime);
        glm::vec3 finPosition = glm::mix(m_Positions[i1].position, m_Positions[i2].position, percent);
        return glm::translate(glm::mat4(1.0), finPosition);
    }

    glm::mat4 interpolateRotation(float animationTime){
        if (m_NumRotations== 1){
            auto rotation = glm::normalize(m_Rotations[0].orientation);
            return glm::toMat4(rotation);
        }
            // return glm::translate(glm::mat4(1.0), m_Positions[0].position);
        int i1 = getIndex(m_Rotations, m_NumRotations, animationTime);
        int i2 = i1 + 1;
        float percent = getScaleFactor(m_Rotations[i1].timeStamp, m_Rotations[i2].timeStamp, animationTime);
        glm::quat finOrient = glm::slerp(m_Rotations[i1].orientation, m_Rotations[i2].orientation, percent);
        return glm::toMat4(finOrient); 
    }

    glm::mat4 interpolateScale(float animationTime){
        if (m_NumScales == 1){
            return glm::scale(glm::mat4(1.0), m_Scales[0].scale);
        }
        int i1 = getIndex(m_Scales, m_NumScales, animationTime);
        int i2 = i1 + 1;
        float percent = getScaleFactor(m_Scales[i1].timeStamp, m_Scales[i2].timeStamp, animationTime);
        glm::vec3 finScale = glm::mix(m_Scales[i1].scale, m_Scales[i2].scale, percent);
        return glm::scale(glm::mat4(1.0), finScale);
    }

};


#endif