/**
 * @file   LoadedModelInfo.h
 * @brief  This file handles mesh information of loaded model.
 * 
 * @author Shodai Makino
 * @date   2023/12/11
 */

#ifndef __LOADED_MODEL_INFO_H__
#define __LOADED_MODEL_INFO_H__

#include "UniquePtr.h"
#include "Texture.h"
#include "MeshBuffer.h"
#include "UtilityForDirectX.h"

namespace ModelInfo {
    /** @brief Index of node */
    using NodeIndex = int;

    /** @brief Vertex structure */
    struct Vertex {
        /** @brief Position of vertex */
        Vector3f pos;
        /** @brief Texture corrdinate */
        Vector2f uv;
        /** @brief Normal vector */
        Vector3f normal;
        /** @brief Weight parameter for vertex blend */
        float weight[4];
        /** @brief Index for vertex blend */
        UINT index[4];
    };

    /** @brief Node information */
    struct Node {
        /** @brief Name of node */
        std::string name;
        /** @brief Index of parent node */
        NodeIndex parentIndex;
        /** @brief Indices of children nodes */
        std::vector<NodeIndex> childIndices;
        /** @brief Offset matrix of node */
        DirectX::XMFLOAT4X4 offset;
    };

    /** @brief Bone information */
    struct Bone {
        /** @brief Index of node to be associated */
        NodeIndex nodeIndex;
        /** @brief Inverse of offset matrix */
        DirectX::XMMATRIX invOffset;
    };

    /** @brief Material data of mesh */
    struct Material {
        /**
           @brief Array of default and additional textures
           @details 
           index = 0 for default textures, and later for additional textures
        */
        CArrayUniquePtr<Utl::Dx::SRVPropertyHandle> texture;
    };

    /** @brief Mesh data */
    struct Mesh {
        /** @brief Vertex buffer data */
        CArrayUniquePtr<Vertex> vertices;
        /** @brief Index buffer data */
        CArrayUniquePtr<UINT> indices;
        /** @brief Bone information */
        CArrayUniquePtr<Bone> bones;
        /** @brief Mesh buffer */
        CMeshBufferFull meshbuffer;
        /** @brief Index of material to be associated */
        UINT materialIndex;
    };

    /** @brief Indentifier of animation */
    using AnimID = unsigned int;
    /** @brief Number of concurrent playing animation */
    const UINT CONCURRENT_ANIMPLAY_NUM = 2;

    /** @brief Animation mode */
    enum class AnimMode {
        /** @brief Bind pose */
        BindPose = 0,
        /** @brief Standard playback */
        Standard,
        /** @brief Blend playback */
        Blending,
    };

    /** @brief Key frame of animation */
    template<class T>
    struct KeyFrame {
        /** @brief Key value */
        T value;
        /** @brief Seconds of key */
        float time;

        /** @brief Constructor */
        KeyFrame(const T& value, float time) : value(value), time(time) {}
    };
    /** @brief 3 valued key frame */
    using KeyFrame3 = KeyFrame<Vector3f>;
    /** @brief 4 valued key frame */
    using KeyFrame4 = KeyFrame<Quaternionf>;

    /** @brief Setting for animation to animation interpolation */
    struct InterpolationSetting {
        /** @brief Interpolation time (seconds) */
        float interpolationTime;

        /**
           @brief Constructor
           @param time Interpolation time (seconds)
        */
        InterpolationSetting(float time) : interpolationTime(time) {}
    };

    /** @brief Channel of animation */
    struct Channel {
        /** @brief Index of node */
        NodeIndex index;
        /** @brief Key frames for position */
        std::vector<KeyFrame3> pos;
        /** @brief Key frames for rotation */
        std::vector<KeyFrame4> rotation;
        /** @brief Key frames for scale */
        std::vector<KeyFrame3> scale;
    };

    /** @brief Animation data per instance */
    struct DynamicAnimation {
        /** @brief Current play time */
        float currentTime;
        /** @brief Loop setting */
        bool isLoop;

        /** @brief Constructor */
        DynamicAnimation() : currentTime(0.0f), isLoop(false) {}
    };

    /** @brief Animation data per model */
    struct StaticAnimation {
        /** @brief Channels with keyframe information */
        std::vector<Channel> channels;
        /** @brief Total seconds of animation */
        float totalTime;
        /** @brief Fixed play rate on loading animation */
        float fixedPlayRate;
        /** @brief Map for animation to animation interpolation setting */
        std::unordered_map<AnimID, InterpolationSetting> interpolationMap;
    };

    /** @brief Parameter for animation during playback */
    struct AnimPlayBackParam {
        /** @brief ID of this animation */
        AnimID id;
        /** @brief Current play time */
        float currentTime;
        /** @brief Loop setting */
        bool isLoop;

        /**
           @brief Constructor
           @param id ID of this animation
           @param currentTime Current play time
           @param isLoop Loop setting
        */
        AnimPlayBackParam(AnimID id, float currentTime, bool isLoop)
            : id(id), currentTime(currentTime), isLoop(isLoop) {}
    };

    /** @brief Parameter for blending animation */
    struct AnimBlendParam {
        /** @brief Current time of blending animation */
        float currentBlendTime;
        /** @brief Total time of blending animation */
        float totalBlendTime;

        /**
           @brief Constructor
           @param id ID of blending animation
           @param currentBlendTime Current time of blending animation
           @param totalBlendTime Total time of blending animation
        */
        AnimBlendParam(float currentBlendTime, float totalBlendTime)
            : currentBlendTime(currentBlendTime), totalBlendTime(totalBlendTime) {}
    };

    /** @brief Data that the model to be animated must have for each instance */
    struct PerInstanceData {
        /** @brief Current play rate */
        float playRate;
        /** @brief Dynamic array of parameters for blending animation */
        std::vector<AnimBlendParam> blendParams;
        /** @brief Animations during playback */
        std::vector<AnimPlayBackParam> playbackAnimations;

        /** @brief Animation transforms */
        CUniquePtr<Transformf[]> animTransforms;
        /** @brief Each node matrices */
        CUniquePtr<DirectX::XMMATRIX[]> nodeMatrices;

        /**
           @brief Constructor
           @param nodeNum The number of nodes
           @param animNum The number of animations
        */
        PerInstanceData(UINT nodeNum, UINT animNum);

        /**
           @brief Erase the blend parameter at the specified index and all preceding blend and playback parameters
           @param index Index to be erased
        */
        void EraseBlendAnims(UINT index);

        /**
           @brief Get playback parameter from blending animation
           @param index Index of blending animation
           @return Reference to playback parameter
        */
        AnimPlayBackParam& GetPlaybackParamFromBlend(UINT index) { return playbackAnimations[index + 1]; }

        /**
           @brief Get blending parameter from playback animation
           @param index Index of playback animation
           @return Reference to blending parameter
        */
        AnimBlendParam* GetBlendParamFromPlayBack(UINT index) { return (index != 0) ? &blendParams[index + 1] : nullptr; }

        /**
           @brief Get ID of the currently main animation
           @return ID of the currently main animation
        */
        AnimID GetCurrentMainID();

        /** @brief Is the animation currently playing standardly? */
        bool IsStandardPlayback() { return playbackAnimations.size() == 1; }
    };

    /** @brief For loading */
    namespace Load {
        /** @brief ID when overriding additional texture as default texture */
        const UINT ADDTEX_OVERRIDE_ID = 0;

        /** @brief Coordinate system of model to be loaded */
        enum class CoordinateSystem {
            /** @brief For model exported with Y-Up, Z-Forward */
            Standard,
            /** @brief For model exported with Z-Up, Y-Forward */
            Blender,
        };

        /** @brief Information of texture that model has */
        struct ModelTex {
            /** @brief Path of texture file */
            std::wstring filePath;
            /** @brief Index of this texture in model */
            UINT index;

            /**
               @brief Constructor
               @param index Index of the material to which this texture is assigned
               @param filePath Path of texture file
            */
            ModelTex(UINT index, const std::wstring& filePath) : index(index), filePath(filePath) {}
        };

        /** @brief Additional texture of model */
        struct AdditionalModelTex {
            /** @brief All texture information for this ID */
            std::unordered_map<UINT, std::wstring> infoMap;
            /** @brief ID of an additional texture */
            UINT id;
            
            /**
               @brief Constructor
               @param id 0 for override as default texture, 1 or later for additional texture
               @param textures All texture information for this ID
            */
            AdditionalModelTex(UINT id, std::initializer_list<ModelTex> textures)
                : id(id) {
                for (auto& it : textures) {
                    infoMap.emplace(it.index, it.filePath);
                }
            }
        };

        /** @brief Descriptor to load model */
        struct ModelDesc {
            /** @brief Scale of model to be loaded */
            float scale;
            /** @brief Loading coordinate system */
            CoordinateSystem coordSystem;
            /** @brief Interpolation time (seconds) if animation interpolation setting is not set */
            float standardInterpolationTime;
            /** @brief Additional texture of model */
            std::vector<AdditionalModelTex> additionalTex;

            /**
               @brief Constructor
               @param scale Scale of model to be loaded
               @param coordSystem Loading coordinate system
               @param interpolationTime Interpolation time (seconds) if animation interpolation setting is not set
            */
            ModelDesc(float scale = 1.0f, CoordinateSystem coordSystem = CoordinateSystem::Standard, float interpolationTime = 0.0f)
                : scale(scale), coordSystem(coordSystem), standardInterpolationTime(interpolationTime) {}
        };

        /** @brief Descriptor to load animation */
        struct AnimDesc {
            /** @brief Fixed play rate at loading */
            float fixedPlayRate;
            /** @brief Adjust the total animation time to the maximum keyframe time? */
            bool isAdjustToKeyFrame;

            /**
               @brief Constructor
               @param fixedPlayRate Fixed play rate at loading
               @param isAdjustToKeyFrame Adjust the total animation time to the maximum keyframe time?
            */
            AnimDesc(float fixedPlayRate = 1.0f, bool isAdjustToKeyFrame = false)
                : fixedPlayRate(fixedPlayRate), isAdjustToKeyFrame(isAdjustToKeyFrame) {}
        };
    } // namespace Load
} // namespace ModelInfo

/** @brief Static data per model */
class CStaticModelData {
public:
    /** @brief Disallow the copy constructor */
    CStaticModelData(const CStaticModelData&) = delete;
    /** @brief Disallow the copy assignment operator */
    CStaticModelData& operator=(const CStaticModelData&) = delete;
    
    /**
       @brief Constructor
    */
    CStaticModelData();

    /** @brief Move constructor */
    CStaticModelData(CStaticModelData&& other) = default;

    /**
       @brief Loading process for model
       @param data Pointer to array of model data
       @param size Size of model data array
       @param filePath Path of model file
       @param desc Descriptor to load model
    */
    void LoadModel(const void* data, size_t size, const std::wstring& filePath, const ModelInfo::Load::ModelDesc& desc);

    /**
       @brief Loading process for animation
       @param data Pointer to array of animation data
       @param size Size of animation data array
       @param desc Descriptor to load animation
       @return ID of animation loaded
    */
    ModelInfo::AnimID LoadAnimation(const void* data, size_t size, const ModelInfo::Load::AnimDesc& desc);

    /**
       @brief Add setting of animation to animation interpolation
       @param sourceID Transition source
       @param destID Transition destination
       @param setting Setting
    */
    void AddAnimInterpolationSetting(ModelInfo::AnimID sourceID, ModelInfo::AnimID destID, const ModelInfo::InterpolationSetting& setting);

    /** @brief Get a mesh of this model */
    const ModelInfo::Mesh* GetMesh(UINT index) const { return &m_meshes[index]; }
    /** @brief Get the number of meshes that this model has */
    const UINT GetMeshNum() const { return m_meshes.Size(); }

    /** @brief Get a material of this model */
    const ModelInfo::Material* GetMaterial(UINT index) const { return &m_materials[index]; }

    /** @brief Get an animation of this model */
    const ModelInfo::StaticAnimation* GetAnimation(ModelInfo::AnimID animID) const { return &m_animations[animID]; }
    /** @brief Get the number of animations that this model has */
    const UINT GetAnimationNum() const { return (UINT)m_animations.size(); }

    /** @brief Get the nodes array of this model */
    const std::vector<ModelInfo::Node>* GetNodes() const { return &m_nodes; }

    /** @brief Get the interpolation time (seconds) if animation interpolation setting is not set */
    const float GetStandardInterpolationTime() const { return m_standardInterpolationTime; }

private:
    /**
       @brief Make nodes of this model
       @param scene Scene class with assimp
    */
    void MakeNodes(const aiScene* scene);

    /**
       @brief Make bones of the mesh that this model has
       @param scene Scene class with assimp
       @param meshIndex Index of mesh
    */
    void MakeBone(const aiScene* scene, UINT meshIndex);

private:
    /** @brief Scale of model on loading */
    float m_modelScale;
    /** @brief Interpolation time (seconds) if animation interpolation setting is not set */
    float m_standardInterpolationTime;
    /** @brief Meshes of this model */
    CArrayUniquePtr<ModelInfo::Mesh> m_meshes;
    /** @brief Materials of this model */
    CArrayUniquePtr<ModelInfo::Material> m_materials;
    /** @brief Nodes of this model */
    std::vector<ModelInfo::Node> m_nodes;
    /** @brief Map relating the name of nodes to the index of nodes */
    std::unordered_map<std::string, ModelInfo::NodeIndex> m_nameToNodeMap;
    /** @brief Animations of this model */
    std::vector<ModelInfo::StaticAnimation> m_animations;
};

/** @brief Controller class for animated models */
class CDynamicModelController {
public:
    /**
       @brief Constructor
       @param modelData Static data of model to be used
    */
    CDynamicModelController(const CStaticModelData& modelData);

    /**
       @brief Advance the time of animation
       @param timeStep Time step to be advanced
    */
    void StepAnim(float timeStep);

    /**
       @brief Play animation
       @param animID ID of animation to be played
       @param isLoop Does it play on a loop?
    */
    void Play(ModelInfo::AnimID animID, bool isLoop);

    /**
       @brief Play blend animation
       @param animID ID of animation to be played
       @param isLoop Does it play on a loop?
       @param blendTime Time required for blending
    */
    void PlayBlend(ModelInfo::AnimID animID, bool isLoop, float blendTime);

    /**
       @brief Set the animation mode is bind pose
    */
    void BindPose() { m_dynamicData.playbackAnimations.clear(); }

    /**
       @brief Calculate an animation matrix
       @param mat Return value variable to receive the matrix
       @param meshIndex Index of mesh
       @param boneIndex Index of bone
    */
    void CalculateAnimationMatrix(DirectX::XMFLOAT4X4* mat, UINT meshIndex, UINT boneIndex);

    /** @brief Is this model currently in a bind pose? */
    bool IsBindPose() { return m_dynamicData.playbackAnimations.empty(); }

    /** @brief Set playrate of overall animation */
    void SetAnimPlayRate(float playRate = 1.0f) { m_dynamicData.playRate = playRate; }
    /** @brief Get the number of loaded animation */
    UINT GetLoadedAnimNum() const { return m_staticData->GetAnimationNum(); }

    /** @brief Get static data of this model */
    const CStaticModelData* GetStaticData() const { return m_staticData; }
    /** @brief Get dynamic data of this model */
    const ModelInfo::PerInstanceData* GetDynamicData() const { return &m_dynamicData; }

    /** @brief Get the number of bones that the mesh has */
    const UINT GetMeshBoneNum(UINT meshIndex) const { return m_staticData->GetMesh(meshIndex)->bones.Size(); }

private:
    /**
       @brief Standard playback animation
       @param animID ID of animation to be played
       @param isLoop Does it play on a loop?
    */
    void StandardPlayback(ModelInfo::AnimID animID, bool isLoop);

    /**
       @brief Blending playback animation
       @param animID ID of animation to be played
       @param isLoop Does it play on a loop?
       @param blendTime Time required for blending
    */
    void BlendingPlayback(ModelInfo::AnimID animID, bool isLoop, float blendTime);
    
    /**
       @brief Update an animation
       @param anim Parameter of animation playback
       @param timeStep Time step to be advanced
    */
    void UpdateAnim(ModelInfo::AnimPlayBackParam* anim, float timeStep);

    /**
       @brief Update blend animations
       @param timeStep Time step to be advanced
    */
    void UpdateBlendAnims(float timeStep);

    /**
       @brief Calculate matrices of nodes
       @param index Index of node
       @param parent Matrix of parent node
    */
    void CalculateNodesMatrices(ModelInfo::NodeIndex index, const DirectX::XMMATRIX& parentMat);
    
    /**
       @brief Apply an animation to the animation transforms
       @param anim Parameter of animation playback
    */
    void ApplyAnimTransforms(const ModelInfo::AnimPlayBackParam& anim);

    /**
       @brief Apply more type animation to the animation transforms
       @param anim Parameter of animation playback
       @param t Parameters to interpolate from the current animation transform
    */
    void ApplyMoreAnimTransforms(const ModelInfo::AnimPlayBackParam& anim, float t);

    /**
       @brief Compute current time value from keyframes
       @tparam T Value type
       @tparam LerpFunc Linear interpolation function type

       @param keyFrames Dynamic array of key frames
       @param currentTime Current time value
       @param lerpFunc Linear interpolation function
       @return Current time value
    */
    template<typename T, typename LerpFunc>
    T ComputeValueFromKeyFrame(const std::vector<ModelInfo::KeyFrame<T>>& keyFrames, float currentTime, const LerpFunc& lerpFunc) const;

private:
    /** @brief Static data of model */
    const CStaticModelData* const m_staticData;
    /** @brief Dynamic data per instance */
    ModelInfo::PerInstanceData m_dynamicData;
};

// Compute current time value from keyframes
template<typename T, typename LerpFunc>
T CDynamicModelController::ComputeValueFromKeyFrame(const std::vector<ModelInfo::KeyFrame<T>>& keyFrames, float currentTime, const LerpFunc& lerpFunc) const {
    // If the current time is before the first key, the value of the first key are used
    if (currentTime <= keyFrames[0].time) { 
        return keyFrames[0].value; 
    }
    // If the current time is after the last key, the value of the last key are used
    else if (keyFrames.back().time <= currentTime) { 
        return keyFrames.back().value; 
    }
    // If not, compute current time value
    else {
        UINT size = (UINT)keyFrames.size() - 1;
        for (UINT i = 0; i < size; ++i) {
            if (keyFrames[i].time < currentTime && currentTime <= keyFrames[i + 1].time) {
                float timeLen = keyFrames[i + 1].time - keyFrames[i].time;
                float rate = (currentTime - keyFrames[i].time) / timeLen;
                return lerpFunc(keyFrames[i].value, keyFrames[i + 1].value, rate);
            }
        }
    }
    // Return the last value in case of a problem
    return keyFrames.back().value;
}

#endif // !__LOADED_MODEL_INFO_H__
