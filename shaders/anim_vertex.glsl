#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONES_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 tCoord;

void main(){
    vec4 finPosition = vec4(0.0);
    for (int i =0; i < MAX_BONES_INFLUENCE; ++i){
        if (boneIds[i] == -1) continue;
        if (boneIds[i] >= MAX_BONES){
            finPosition = vec4(pos, 1.0);
            break;
        }

        vec4 localPos = finalBonesMatrices[boneIds[i]] * vec4(pos, 1.0);
        finPosition += localPos * weights[i];
        // vec3 localNorm = mat3(finalBonesMatrices[boneIds])*norm;
    }

    // vec4 localPos = finalBonesMatrices[0] * vec4(pos, 1.0);
    // finPosition += localPos;

    // gl_Position = proj*view*model*finPosition;
    gl_Position = proj * view * model * finPosition;
    tCoord = texCoord;
}