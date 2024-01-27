/**
 * @file   Model.h
 * @brief  This file handles model class.
 * 
 * @author Shodai Makino
 * @date   2023/12/11
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include "GraphicsComponent.h"
#include "LoadedModelInfo.h"

/** @brief Abstract class of model */
class ACModel : public ACGraphicsComponent {
public:
    /**
       @brief Constructor
       @param type Type of this graphics component
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param isSubstance Is this component for an substance?
       @param additionalTexID ID of additional textures to be used
    */
    ACModel(GraphicsComponentType type, CGameObject* owner, GraphicsLayer layer, bool isSubstance, int additionalTexID);

    /**
       @brief Destructor
    */
    virtual ~ACModel() = default;

    /**
       @brief Processing for the first draw frame
    */
    void Start() override;

    /**
       @brief Process to be called at instance destruction
    */
    void OnDestroy() override;

    /** @brief Get ID of additional textures used */
    int GetAdditionalTexID() { return m_additionalTexID; }

    /** @brief Is this graphics component itself active? */
    inline bool IsActiveSelf() const override { return GetSubstance()->ACGraphicsComponent::IsActiveSelf();  }
    /** @brief Is this graphics component itself and the game object that owns it active? */
    inline bool IsActiveOverall() const override { return GetSubstance()->ACGraphicsComponent::IsActiveOverall(); }
    /** @brief Get color */
    const Colorf& GetColor() const override { return GetSubstance()->ACGraphicsComponent::GetColor(); }

protected:
    /**
       @brief Get static data per model
    */
    virtual const CStaticModelData* GetStaticData() = 0;

    /**
       @brief Get pointer to substance
    */
    virtual const ACModel* GetSubstance() const = 0;

    /**
       @brief Get pointer to transparent component
    */
    virtual ACModel* GetTransparentComponent() = 0;

    /**
       @brief Create opacity mesh data and texture data
    */
    void CreateOpacityMesh();

    /**
       @brief Create transparent mesh data and texture data
    */
    void CreateTransparentMesh();

protected:
    /**
       @brief Is this component for an substance?
       @details
       Since the layers of the component are applied to all meshes,
       it was not possible to draw correctly if translucent and opaque textures were mixed in a single model.
       Therefore, the component for the semi-transparent mesh was created as a variable,
       and the variables it uses refer to the substance component,
       making it appear as if it were a single drawing component.
    */
    const bool m_isSubstance;

private:
    /** @brief ID of additional textures used */
    int m_additionalTexID;
};

/** @brief Non-animated model */
class CBasicModel : public ACModel {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param modelPath Path of model file
       @param additionalTexID If using an additional texture instead of the default texture, specify its ID
    */
    CBasicModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath, int additionalTexID = ModelInfo::NON_ADDITIONAL_TEX_ID);

    /** @brief Destructor */
    ~CBasicModel() override;

protected:
    /** @brief Get static data per model */
    const CStaticModelData* GetStaticData() override { return (m_isSubstance) ? m_aspectParam.substanceParam.model : m_aspectParam.transparentMeshParam.substance->GetStaticData(); }

    /**
       @brief Get pointer to substance
    */
    inline const ACModel* GetSubstance() const override { return (m_isSubstance) ? this : static_cast<ACModel*>(m_aspectParam.transparentMeshParam.substance); }

    /**
       @brief Get pointer to transparent component
    */
    inline ACModel* GetTransparentComponent() { return (m_isSubstance) ? m_aspectParam.substanceParam.transparentMeshes.Get() : this; }

private:
    /**
       @brief Union to use different values for different uses of this component
       @details
       Since the layers of the component are applied to all meshes,
       it was not possible to draw correctly if translucent and opaque textures were mixed in a single model.
       Therefore, the component for the semi-transparent mesh was created as a variable,
       and the variables it uses refer to the substance component,
       making it appear as if it were a single drawing component.
    */
    union AspectParam {
        /**
           @brief Constructor for substance
           @param model Static data per model
        */
        AspectParam(const CStaticModelData* model) : substanceParam(model) {}

        /**
           @brief Constructor for transparent meshes
           @param substance Pointer to the substance that has this component as a variable
        */
        AspectParam(CBasicModel* substancePtr) : transparentMeshParam(substancePtr) {}

        /** @brief Destructor */
        ~AspectParam() {}

        /** @brief For substance parameter */
        struct SubstanceParam {
            /** @brief Static data per model */
            const CStaticModelData* model;
            /**
               @brief The part of the same model that handles transparent meshes
               @details
               The component of this variable are not exposed to the user side.
               Each parameter refers to the component with this variable
            */
            CUniquePtrWeakable<CBasicModel> transparentMeshes;

            /** @brief Constructor */
            SubstanceParam(const CStaticModelData* model) : model(model), transparentMeshes(nullptr) {}
            /** @brief Destructor */
            ~SubstanceParam() = default;
        } substanceParam;

        /** @brief For transparent meshes parameter */
        struct TransparentMeshParam {
            /** @brief Pointer to the substance that has this component as a variable */
            CBasicModel* substance;

            /** @brief Constructor */
            TransparentMeshParam(CBasicModel* substance) : substance(substance) {}
            /** @brief Destructor */
            ~TransparentMeshParam() = default;
        } transparentMeshParam;
    } m_aspectParam;
};

/** @brief Model animated with bones */
class CSkeletalModel : public ACModel {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param modelPath Path of model file
       @param additionalTexID If using an additional texture instead of the default texture, specify its ID
    */
    CSkeletalModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath, int additionalTexID = ModelInfo::NON_ADDITIONAL_TEX_ID);

    /** @brief Destructor */
    ~CSkeletalModel() override;

    /**
       @brief Pre drawing processing
    */
    void PreDraw() override;

    /**
       @brief Get bone matrices
       @param meshIndex Index of mesh
       @return Unique pointer to an array of bone matrices
    */
    CUniquePtr<DirectX::XMFLOAT4X4[]> GetBoneMatrices(UINT meshIndex);

    /** @brief Get the number of bones that mesh has */
    UINT GetMeshBoneNum(UINT meshIndex) { return (m_isSubstance) ? GetController()->GetOpacityMeshBoneNum(meshIndex) : GetController()->GetTransparentMeshBoneNum(meshIndex); }

    /** @brief Get controller class for animated models */
    inline CDynamicModelController* GetController() { return (m_isSubstance) ? m_aspectParam.substanceParam.controller.Get() : m_aspectParam.transparentMeshParam.substance->GetController(); }

protected:
    /** @brief Get static data per model */
    const CStaticModelData* GetStaticData() override { return GetController()->GetStaticData(); }

    /**
       @brief Get pointer to substance
    */
    inline const ACModel* GetSubstance() const override { return (m_isSubstance) ? this : static_cast<ACModel*>(m_aspectParam.transparentMeshParam.substance); }

    /**
       @brief Get pointer to transparent component
    */
    inline ACModel* GetTransparentComponent() { return (m_isSubstance) ? m_aspectParam.substanceParam.transparentMeshes.Get() : this; }

private:
    /**
       @brief Constructor for transparent meshes
       @param substance Pointer to the substance that has this component as a variable
    */
    CSkeletalModel(CSkeletalModel* substance);

private:
    /**
       @brief Union to use different values for different uses of this component
       @details
       Since the layers of the component are applied to all meshes, 
       it was not possible to draw correctly if translucent and opaque textures were mixed in a single model. 
       Therefore, the component for the semi-transparent mesh was created as a variable, 
       and the variables it uses refer to the substance component, 
       making it appear as if it were a single drawing component.
    */
    union AspectParam {
        /**
           @brief Constructor for substance
           @param controller Controller class for animated models
        */
        AspectParam(CUniquePtr<CDynamicModelController> controller) : substanceParam(std::move(controller)) {}

        /**
           @brief Constructor for transparent meshes
           @param substancePtr Pointer to the substance that has this component as a variable
        */
        AspectParam(CSkeletalModel* substancePtr) : transparentMeshParam(substancePtr) {}

        /** @brief Destructor */
        ~AspectParam() {}

        /** @brief For substance parameter */
        struct SubstanceParam {
            /** @brief Controller class for animated models */
            CUniquePtr<CDynamicModelController> controller;
            /**
               @brief The part of the same model that handles transparent meshes
               @details 
               The component of this variable are not exposed to the user side.
               Each parameter refers to the component with this variable
            */
            CUniquePtrWeakable<CSkeletalModel> transparentMeshes;

            /** @brief Constructor */
            SubstanceParam(CUniquePtr<CDynamicModelController> controller) : controller(std::move(controller)), transparentMeshes(nullptr) {}
            /** @brief Destructor */
            ~SubstanceParam() = default;
        } substanceParam;

        /** @brief For transparent meshes parameter */
        struct TransparentMeshParam {
            /** @brief Pointer to the substance that has this component as a variable */
            CSkeletalModel* substance;

            /** @brief Constructor */
            TransparentMeshParam(CSkeletalModel* substance) : substance(substance) {}
            /** @brief Destructor */
            ~TransparentMeshParam() = default;
        } transparentMeshParam;
    } m_aspectParam;
};

#endif // !__MODEL_H__
