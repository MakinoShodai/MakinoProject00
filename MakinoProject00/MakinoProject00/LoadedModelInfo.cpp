#include "LoadedModelInfo.h"
#include "TextureRegistry.h"
#include "UtilityForException.h"
#include "UtilityForString.h"

// Index of animation ID array for main animation
const UINT MAIN_ANIM_INDEX = 0;
// Index of animation ID array for blend animation
const UINT BLEND_ANIM_INDEX = 1;

// Data that the model to be animated must have for each instance
ModelInfo::PerInstanceData::PerInstanceData(UINT nodeNum, UINT animNum)
    : animMode(AnimMode::BindPose)
    , currentBlendTime(0.0f)
    , totalBlendTime(0.0f) {
    // Initialize current play ID
    std::fill(std::begin(currentPlayID), std::end(currentPlayID), 0);

    // Make array of unique ptr
    animations = CUniquePtr<DynamicAnimation[]>::Make(animNum);
    animTransforms = CUniquePtr<Transformf[]>::Make(nodeNum);
    nodeMatrices = CUniquePtr<DirectX::XMMATRIX[]>::Make(nodeNum);
}

// Static data per model
CStaticModelData::CStaticModelData() 
    : m_modelScale(1.0f)
    , m_standardInterpolationTime(0.0f)
{ }

// Loading process for model
void CStaticModelData::LoadModel(const void* data, size_t size, const std::wstring& filePath, const ModelInfo::Load::ModelDesc& desc) {
    // Make setting to load
    UINT flag = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_MakeLeftHanded;
    
    // Load model with assimp
    Assimp::Importer importer;
    const aiScene* assimpScene = importer.ReadFileFromMemory(data, size, flag);
    if (!assimpScene) {
        throw Utl::Error::CFatalError(Utl::Str::string2WString(importer.GetErrorString()));
    }

    // Set descriptor data
    m_modelScale = desc.scale;
    m_standardInterpolationTime = desc.standardInterpolationTime;

    // Make nodes of the model
    MakeNodes(assimpScene);

    // Initialize the array of the meshes
    m_meshes.Resize(assimpScene->mNumMeshes);

    // Set data of each meshes to the array
    aiVector3D zero(0.0f, 0.0f, 0.0f);
    for (UINT i = 0; i < assimpScene->mNumMeshes; ++i) {
        // Initialize the array of the vertices
        m_meshes[i].vertices.Resize(assimpScene->mMeshes[i]->mNumVertices);

        // Set vertices data of the mesh to the array
        for (UINT j = 0; j < assimpScene->mMeshes[i]->mNumVertices; ++j) {
            // Get each data
            aiVector3D pos = assimpScene->mMeshes[i]->mVertices[j];
            aiVector3D uv = assimpScene->mMeshes[i]->HasTextureCoords(0) ? assimpScene->mMeshes[i]->mTextureCoords[0][j] : zero;
            aiVector3D normal = assimpScene->mMeshes[i]->mNormals[j];

            // Change the coordinate of read vertex by the coordinate system for loading
            switch (desc.coordSystem) {
            // Standard coordinate system
            case ModelInfo::Load::CoordinateSystem::Standard:
                break;
            // Coordinate system for blender
            case ModelInfo::Load::CoordinateSystem::Blender:
                pos = aiVector3D(pos.x, -pos.z, -pos.y);
                normal = aiVector3D(normal.x, -normal.z, -normal.y);
                break;
            }

            // Set each data
            m_meshes[i].vertices[j].pos = Vector3f(pos.x, pos.y, pos.z) * desc.scale;
            m_meshes[i].vertices[j].uv = Vector2f(uv.x, uv.y);
            m_meshes[i].vertices[j].normal = Vector3f(normal.x, normal.y, normal.z);
            std::fill(std::begin(m_meshes[i].vertices[j].weight), std::end(m_meshes[i].vertices[j].weight), 0.0f);
            std::fill(std::begin(m_meshes[i].vertices[j].index), std::end(m_meshes[i].vertices[j].index), 0);
        }

        // Make bones property of the mesh
        MakeBone(assimpScene, i);

        // Initialize the array of the indices
        m_meshes[i].indices.Resize(assimpScene->mMeshes[i]->mNumFaces * 3);

        // Set indices
        for (UINT j = 0; j < assimpScene->mMeshes[i]->mNumFaces; ++j) {
            const aiFace& face = assimpScene->mMeshes[i]->mFaces[j];
            m_meshes[i].indices[j * 3 + 0] = face.mIndices[0];
            m_meshes[i].indices[j * 3 + 1] = face.mIndices[2];
            m_meshes[i].indices[j * 3 + 2] = face.mIndices[1];
        }

        // Set material index
        m_meshes[i].materialIndex = assimpScene->mMeshes[i]->mMaterialIndex;

        // Create mesh buffer
        m_meshes[i].meshbuffer.GetVertexEntity()->Create<ModelInfo::Vertex>(m_meshes[i].vertices.Get(), m_meshes[i].vertices.Size());
        m_meshes[i].meshbuffer.GetIndexEntity()->Create<UINT>(m_meshes[i].indices.Get(), m_meshes[i].indices.Size());
        m_meshes[i].meshbuffer.SetInfo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    
    // Initialize the array of the indices
    if (assimpScene->mNumMaterials > 0) {
        // Extract only the directory part of the file path
        /* "Model/xx.fbx" -> "Model/" */
        std::wstring dir = filePath.substr(0, filePath.rfind('/') + 1);

        // Initialize the array of the materials
        m_materials.Resize(assimpScene->mNumMaterials);

        // Set data of each materials to the array
        for (UINT i = 0; i < assimpScene->mNumMaterials; ++i) {
            // Read texture
            aiString path;
            if (assimpScene->mMaterials[i]->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS) {
                // Create texture file path with directory
                std::wstring file = dir + Utl::Str::string2WString(path.C_Str());

                // If the texture registry contains this texture file, set handle for its texture
                if (CTextureRegistry::GetAny().IsContains(file)) {
                    m_materials[i].texture = CUniquePtr<Utl::Dx::SRVPropertyHandle>::Make(CTextureRegistry::GetAny().GetSRVPropertyHandle(file));
                }
                else {
                    throw Utl::Error::CFatalError(L"Texture data held by the model don't exist" + file);
                }
            }
        }
    }
}

// Loading process for animation
ModelInfo::AnimID CStaticModelData::LoadAnimation(const void* data, size_t size, const ModelInfo::Load::AnimDesc& desc) {
    // Make setting to load
    UINT flag = aiProcess_MakeLeftHanded;
    // Load animation with assimp
    Assimp::Importer importer;
    const aiScene* assimpScene = importer.ReadFileFromMemory(data, size, flag);
    if (!assimpScene) {
        throw Utl::Error::CFatalError(Utl::Str::string2WString(importer.GetErrorString()));
    }

    // Does the loaded scene contain animation?
    if (!assimpScene->HasAnimations()) {
        throw Utl::Error::CFatalError(L"Non-animated fbx is loaded as animated");
    }

    // Get animation data
    ModelInfo::StaticAnimation anim;
    aiAnimation* assimpAnim = assimpScene->mAnimations[0];
    
    // Set fixed play rate of the animation
    anim.fixedPlayRate = desc.fixedPlayRate;

    // Convert the animation total time to seconds
    double invTicksPerSecond = 1.0 / assimpAnim->mTicksPerSecond;
    float invPlayRate = 1.0f / desc.fixedPlayRate;
    anim.totalTime = (float)(assimpAnim->mDuration * invTicksPerSecond) * invPlayRate;

    // Convert keyframe time to seconds
    float maxTime = 0.0f;
    auto convertSec = [invTicksPerSecond, invPlayRate, &maxTime](double time) -> float {
        // Calculation animation duration
        float ret = (float)(time * invTicksPerSecond) * invPlayRate;
        // Get the larger time
        maxTime = (std::max)(ret, maxTime);
        return ret;
    };

    // Create channels for animation
    anim.channels.reserve(assimpAnim->mNumChannels);
    for (UINT i = 0; i < assimpAnim->mNumChannels; ++i) {
        // Set data for each channel
        aiNodeAnim* assimpChannel = assimpAnim->mChannels[i];
        auto mappingIt = m_nameToNodeMap.find(assimpChannel->mNodeName.data);
        if (mappingIt == m_nameToNodeMap.end()) {
            continue;
        }
        // Set index of node
        ModelInfo::Channel channel;
        channel.index = mappingIt->second;

        // Set key frames for position
        channel.pos.reserve(assimpChannel->mNumPositionKeys);
        for (UINT i = 0; i < assimpChannel->mNumPositionKeys; ++i) {
            const aiVector3D& val = assimpChannel->mPositionKeys[i].mValue;
            channel.pos.emplace_back(Vector3f(val.x, val.y, val.z) * m_modelScale, convertSec(assimpChannel->mPositionKeys[i].mTime));
        }
        // Set key frames for rotation
        channel.rotation.reserve(assimpChannel->mNumRotationKeys);
        for (UINT i = 0; i < assimpChannel->mNumRotationKeys; ++i) {
            const aiQuaternion& val = assimpChannel->mRotationKeys[i].mValue;
            channel.rotation.emplace_back(Quaternionf(val.x, val.y, val.z, val.w), convertSec(assimpChannel->mRotationKeys[i].mTime));
        }
        // Set key frames for scale
        channel.scale.reserve(assimpChannel->mNumScalingKeys);
        for (UINT i = 0; i < assimpChannel->mNumScalingKeys; ++i) {
            const aiVector3D& val = assimpChannel->mScalingKeys[i].mValue;
            channel.scale.emplace_back(Vector3f(val.x, val.y, val.z), convertSec(assimpChannel->mScalingKeys[i].mTime));
        }

        // Add the channel data to the array
        anim.channels.emplace_back(std::move(channel));
    }
    anim.channels.shrink_to_fit();

    // If the total animation time is to be adjusted
    if (desc.isAdjustToKeyFrame) {
        anim.totalTime = maxTime;
    }

    // Add the animation data to the array
    m_animations.emplace_back(std::move(anim));

    return (ModelInfo::AnimID)m_animations.size() - 1;
}

// Add setting of animation to animation interpolation
void CStaticModelData::AddAnimInterpolationSetting(ModelInfo::AnimID sourceID, ModelInfo::AnimID destID, const ModelInfo::InterpolationSetting& setting) {
    if (sourceID >= m_animations.size() && destID >= m_animations.size()) { return; }
    m_animations[sourceID].interpolationMap.emplace(destID, setting);
}

// Make nodes of this model
void CStaticModelData::MakeNodes(const aiScene* scene) {
    // Rambda to convert assimp node to node
    std::function<ModelInfo::NodeIndex(aiNode*, ModelInfo::NodeIndex)> convertAssimpNode
        = [&convertAssimpNode, this](aiNode* pNode, ModelInfo::NodeIndex parent) {
        // Set each property
        m_nodes.emplace_back();
        ModelInfo::Node& node = m_nodes.back();
        node.name = pNode->mName.data;
        node.parentIndex = parent;
        const aiMatrix4x4& mat = pNode->mTransformation;
        node.offset = DirectX::XMFLOAT4X4(
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4 * m_modelScale, mat.b4 * m_modelScale, mat.c4 * m_modelScale, mat.d4);

        // Set the name of the node with its index
        ModelInfo::NodeIndex index = (ModelInfo::NodeIndex)m_nodes.size() - 1;
        m_nameToNodeMap.emplace(node.name, index);

        // All child nodes also do the conversion
        node.childIndices.reserve(pNode->mNumChildren);
        for (UINT i = 0; i < pNode->mNumChildren; ++i) {
            ModelInfo::NodeIndex num = convertAssimpNode(pNode->mChildren[i], index);
            m_nodes[index].childIndices.push_back(num);
        }

        return index;
    };

    // Make nodes
    m_nodes.clear();
    m_nameToNodeMap.clear();
    convertAssimpNode(scene->mRootNode, -1);
}

// Make bones of this model
void CStaticModelData::MakeBone(const aiScene* scene, UINT meshIndex) {
    // If the mesh has bones
    if (scene->mMeshes[meshIndex]->HasBones()) {
        // Set bone data to the bone array of the mesh
        m_meshes[meshIndex].bones.Resize(scene->mMeshes[meshIndex]->mNumBones);
        for (UINT j = 0; j < scene->mMeshes[meshIndex]->mNumBones; ++j) {
            // Check if the name of a bone is contained in the map
            std::string boneName = std::string(scene->mMeshes[meshIndex]->mBones[j]->mName.data);
            auto it = m_nameToNodeMap.find(boneName);
            if (it != m_nameToNodeMap.end()) {
                // Set index of nodes and an offset matrix
                m_meshes[meshIndex].bones[j].nodeIndex = it->second;
                const aiMatrix4x4& mat = scene->mMeshes[meshIndex]->mBones[j]->mOffsetMatrix;
                m_meshes[meshIndex].bones[j].invOffset = DirectX::XMMATRIX(
                    mat.a1, mat.b1, mat.c1, mat.d1,
                    mat.a2, mat.b2, mat.c2, mat.d2,
                    mat.a3, mat.b3, mat.c3, mat.d3,
                    mat.a4 * m_modelScale, mat.b4 * m_modelScale, mat.c4 * m_modelScale, mat.d4
                );

                // Set weight property
                UINT weightNum = scene->mMeshes[meshIndex]->mBones[j]->mNumWeights;
                for (UINT k = 0; k < weightNum; ++k) {
                    aiVertexWeight weight = scene->mMeshes[meshIndex]->mBones[j]->mWeights[k];
                    for (int l = 0; l < 4; ++l) {
                        if (m_meshes[meshIndex].vertices[weight.mVertexId].weight[l] <= 0.0f) {
                            m_meshes[meshIndex].vertices[weight.mVertexId].index[l] = j;
                            m_meshes[meshIndex].vertices[weight.mVertexId].weight[l] = weight.mWeight;
                            break;
                        }
                    }
                }
            }
            else {
                throw Utl::Error::CFatalError(L"No node matching the bone name exits in the map : " + Utl::Str::string2WString(boneName));
            }
        }
    }
    // If the mesh hasn't nodes
    else {
        // If the name of a mesh is contained in the map
        std::string meshName = std::string(scene->mMeshes[meshIndex]->mName.data);
        auto it = m_nameToNodeMap.find(meshName);
        if (it != m_nameToNodeMap.end()) {
            // Rambda to find non mesh node toward the root node
            std::function<ModelInfo::NodeIndex(ModelInfo::NodeIndex)> findNonMeshNode = [&findNonMeshNode, this, scene](ModelInfo::NodeIndex index) {
                    for (UINT i = 0; i < scene->mNumMeshes; ++i) {
                        if (m_nodes[index].name == scene->mMeshes[i]->mName.data) {
                            return findNonMeshNode(m_nodes[index].parentIndex);
                        }
                    }
                    return index;
                };
            // Rambda to calculate the offset matrix to the root node
            std::function<DirectX::XMMATRIX(ModelInfo::NodeIndex, DirectX::XMMATRIX)> calcOffsetMat =
                [&calcOffsetMat, this](ModelInfo::NodeIndex index, DirectX::XMMATRIX mat) {
                    if (index == -1) return mat;
                    mat = mat * DirectX::XMLoadFloat4x4(&m_nodes[index].offset);
                    return calcOffsetMat(m_nodes[index].parentIndex, mat);
                };

            // Set the mesh's bone matrix to one
            m_meshes[meshIndex].bones.Resize(1);
            for (UINT j = 0; j < m_meshes[meshIndex].vertices.Size(); ++j) {
                m_meshes[meshIndex].vertices[j].weight[0] = 1.0f;
            }

            // Find non mesh node toward the root node
            m_meshes[meshIndex].bones[0].nodeIndex = findNonMeshNode(m_nodes[it->second].parentIndex);
            // Calculate the offset matrix to the root node
            m_meshes[meshIndex].bones[0].invOffset = DirectX::XMMatrixInverse(nullptr,
                calcOffsetMat(m_meshes[meshIndex].bones[0].nodeIndex, DirectX::XMMatrixIdentity())
            );
        }
    }
}

// Controller class for animated models
CDynamicModelController::CDynamicModelController(const CStaticModelData& modelData)
    : m_staticData(&modelData)
    , m_dynamicData((UINT)modelData.GetNodes()->size(), modelData.GetAnimationNum())
{ }

// Advance the time of animation
void CDynamicModelController::StepAnim(float timeStep) {
    // If the animation mode is bind pose, return
    if (m_dynamicData.animMode == ModelInfo::AnimMode::BindPose) { return; }

    // Apply a main animation to the animation transfrom
    ApplyAnimTransforms(m_dynamicData.currentPlayID[MAIN_ANIM_INDEX]);

    // If the animation mode is blending, apply a blend animation to the animation transfrom
    if (m_dynamicData.animMode == ModelInfo::AnimMode::Blending) {
        float t = m_dynamicData.currentBlendTime / m_dynamicData.totalBlendTime;
        ApplyMoreAnimTransforms(m_dynamicData.currentPlayID[BLEND_ANIM_INDEX], t);
    }

    // Calculate the node matrices
    CalculateNodesMatrices(0, DirectX::XMMatrixIdentity());

    // Update a main animation
    UpdateAnim(m_dynamicData.currentPlayID[MAIN_ANIM_INDEX], timeStep);
    
    // If the animation mode is blending, update a blend animation
    if (m_dynamicData.animMode == ModelInfo::AnimMode::Blending) {
        UpdateAnim(m_dynamicData.currentPlayID[BLEND_ANIM_INDEX], timeStep);
        m_dynamicData.currentBlendTime += timeStep;

        // If animation blending has been completed, finish animation blending
        if (m_dynamicData.totalBlendTime <= m_dynamicData.currentBlendTime) {
            m_dynamicData.currentBlendTime = 0.0f;
            m_dynamicData.totalBlendTime = 0.0f;
            m_dynamicData.currentPlayID[MAIN_ANIM_INDEX] = m_dynamicData.currentPlayID[BLEND_ANIM_INDEX];
            m_dynamicData.animMode = ModelInfo::AnimMode::Standard;
        }
    }
}

// Play animation
void CDynamicModelController::Play(ModelInfo::AnimID animID, bool isLoop) {
    // If a non ID value is sent, do nothing
    if (animID >= m_staticData->GetAnimationNum()) { return; }

    // If current animation mode is standard
    if (m_dynamicData.animMode == ModelInfo::AnimMode::Standard) {
        // If sent animation ID and current playback animation ID are the same, do nothing
        if (m_dynamicData.currentPlayID[MAIN_ANIM_INDEX] == animID) { return; }

        // Get currently playing animation
        const ModelInfo::StaticAnimation* currentAnim = m_staticData->GetAnimation(m_dynamicData.currentPlayID[MAIN_ANIM_INDEX]);

        // Get time for animation interpolation
        auto it = currentAnim->interpolationMap.find(animID);
        float interpolationTime = (it != currentAnim->interpolationMap.end()) ? it->second.interpolationTime : m_staticData->GetStandardInterpolationTime();
        
        // If interpolation time is not zero, so do blending playback
        if (!Utl::IsFloatZero(interpolationTime)) {
            BlendingPlayback(animID, isLoop, interpolationTime);
            return;
        }
    }

    StandardPlayback(animID, isLoop);
}

// Play blend animation
void CDynamicModelController::PlayBlend(ModelInfo::AnimID animID, bool isLoop, float blendTime) {
    // If a non ID value is sent, do nothing
    if (animID >= m_staticData->GetAnimationNum()) { return; }
    // If the animation is not playing, so do standard playback without blending
    if (m_dynamicData.animMode != ModelInfo::AnimMode::Standard) { 
        Play(animID, isLoop); 
        return; 
    }

    BlendingPlayback(animID, isLoop, blendTime);
}

// Calculate an animation matrix
void CDynamicModelController::CalculateAnimationMatrix(DirectX::XMFLOAT4X4* mat, UINT meshIndex, UINT boneIndex) {
    DirectX::XMStoreFloat4x4(mat,
        m_staticData->GetMesh(meshIndex)->bones[boneIndex].invOffset *
        m_dynamicData.nodeMatrices[m_staticData->GetMesh(meshIndex)->bones[boneIndex].nodeIndex]);
}

// Standard playback animation
void CDynamicModelController::StandardPlayback(ModelInfo::AnimID animID, bool isLoop) {
    // Initialize animation
    InitializeAnim(animID);

    // Set loop flag
    m_dynamicData.animations[animID].isLoop = isLoop;
    // Set the animation ID to the main animation
    m_dynamicData.currentPlayID[MAIN_ANIM_INDEX] = animID;
    // Set the animation mode is standard
    m_dynamicData.animMode = ModelInfo::AnimMode::Standard;
}

// Blending playback animation
void CDynamicModelController::BlendingPlayback(ModelInfo::AnimID animID, bool isLoop, float blendTime) {
    // Initialize animation
    InitializeAnim(animID);

    // Set loop flag
    m_dynamicData.animations[animID].isLoop = isLoop;
    // Set the animation ID to the blend animation
    m_dynamicData.currentPlayID[BLEND_ANIM_INDEX] = animID;
    // Reset current blend time
    m_dynamicData.currentBlendTime = 0.0f;
    // Set blending time of animation
    m_dynamicData.totalBlendTime = blendTime;
    // Set the animation mode is blending
    m_dynamicData.animMode = ModelInfo::AnimMode::Blending;
}

// Initialize an animation
void CDynamicModelController::InitializeAnim(ModelInfo::AnimID animID) {
    ModelInfo::DynamicAnimation& anim = m_dynamicData.animations[animID];
    anim.currentTime = 0.0f;
    anim.playRate = 1.0f;
    anim.isLoop = false;
}

// Update an animation
void CDynamicModelController::UpdateAnim(ModelInfo::AnimID animID, float timeStep) {
    // Get the animation data
    const ModelInfo::StaticAnimation* staticAnimData = m_staticData->GetAnimation(animID);
    ModelInfo::DynamicAnimation& anim = m_dynamicData.animations[animID];
    
    // Advance current playback time of animation
    anim.currentTime += anim.playRate * timeStep;

    // If it's looping, correct the current playback time
    if (anim.isLoop) {
        while (anim.currentTime >= staticAnimData->totalTime)
            anim.currentTime -= staticAnimData->totalTime;
    }

}

// Calculate matrices of nodes
void CDynamicModelController::CalculateNodesMatrices(ModelInfo::NodeIndex index, const DirectX::XMMATRIX& parentMat) {
    const Transformf& transform = m_dynamicData.animTransforms[index];

    // Convert quaternion to XMVECTORF32
    DirectX::XMVECTORF32 quaternion = { transform.rotation.x(), transform.rotation.y(), transform.rotation.z(), transform.rotation.w() };

    // Create matrix of this node
    DirectX::XMMATRIX mat = 
        DirectX::XMMatrixScaling(transform.scale.x(), transform.scale.y(), transform.scale.z()) *
        DirectX::XMMatrixRotationQuaternion(quaternion) *
        DirectX::XMMatrixTranslation(transform.pos.x(), transform.pos.y(), transform.pos.z());
    
    // Take the product of the matrix with the parent node
    m_dynamicData.nodeMatrices[index] = mat * parentMat;

    // Update matrix of children nodes
    const std::vector<ModelInfo::Node>& nodes = *m_staticData->GetNodes();
    for (ModelInfo::NodeIndex it : nodes[index].childIndices) {
        CalculateNodesMatrices(it, m_dynamicData.nodeMatrices[index]);
    }
}

// Apply an animation to the animation transforms
void CDynamicModelController::ApplyAnimTransforms(ModelInfo::AnimID animID) {
    ModelInfo::DynamicAnimation&  anim = m_dynamicData.animations[animID];
    Vector3f    pos;
    Quaternionf quat;
    Vector3f    scale;
    
    const std::vector<ModelInfo::Channel>& channels = m_staticData->GetAnimation(animID)->channels;
    for (const auto& channelIt : channels) {
        pos = ComputeValueFromKeyFrame<Vector3f>(channelIt.pos, anim.currentTime, Utl::Math::Lerp);
        quat = ComputeValueFromKeyFrame<Quaternionf>(channelIt.rotation, anim.currentTime, Utl::Math::Slerp);;
        scale = ComputeValueFromKeyFrame<Vector3f>(channelIt.scale, anim.currentTime, Utl::Math::Lerp);

        m_dynamicData.animTransforms[channelIt.index].pos = pos;
        m_dynamicData.animTransforms[channelIt.index].rotation = quat;
        m_dynamicData.animTransforms[channelIt.index].scale = scale;
    }
}

// Apply more type animation to the animation transforms
void CDynamicModelController::ApplyMoreAnimTransforms(ModelInfo::AnimID animID, float t) {
    ModelInfo::DynamicAnimation& anim = m_dynamicData.animations[animID];
    Vector3f    pos;
    Quaternionf quat;
    Vector3f    scale;

    const std::vector<ModelInfo::Channel>& channels = m_staticData->GetAnimation(animID)->channels;
    for (const auto& channelIt : channels) {
        pos = ComputeValueFromKeyFrame<Vector3f>(channelIt.pos, anim.currentTime, Utl::Math::Lerp);
        quat = ComputeValueFromKeyFrame<Quaternionf>(channelIt.rotation, anim.currentTime, Utl::Math::Slerp);;
        scale = ComputeValueFromKeyFrame<Vector3f>(channelIt.scale, anim.currentTime, Utl::Math::Lerp);

        m_dynamicData.animTransforms[channelIt.index].pos = Utl::Math::Lerp(m_dynamicData.animTransforms[channelIt.index].pos, pos, t);
        m_dynamicData.animTransforms[channelIt.index].rotation = Utl::Math::Slerp(m_dynamicData.animTransforms[channelIt.index].rotation, quat, t);
        m_dynamicData.animTransforms[channelIt.index].scale = Utl::Math::Lerp(m_dynamicData.animTransforms[channelIt.index].scale, scale, t);
    }
}
