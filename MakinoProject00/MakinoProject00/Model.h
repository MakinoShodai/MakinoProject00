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
    // Constructor
    using ACGraphicsComponent::ACGraphicsComponent;

    /**
       @brief Destructor
    */
    virtual ~ACModel() = default;

    /**
       @brief Get static data per model
    */
    virtual const CStaticModelData* GetStaticData() = 0;

protected:
    /**
       @brief Create mesh data and texture data
    */
    void CreateMesh();
};

/** @brief Non-animated model */
class CBasicModel : public ACModel {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param modelPath Path of model file
    */
    CBasicModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath);

    /** @brief Destructor */
    ~CBasicModel() override = default;

    /** @brief Get static data per model */
    const CStaticModelData* GetStaticData() override { return m_model; }

private:
    /** @brief Static data per model */
    const CStaticModelData* m_model;
};

/** @brief Model animated with bones */
class CSkeletalModel : public ACModel {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param modelPath Path of model file
    */
    CSkeletalModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath);

    /** @brief Destructor */
    ~CSkeletalModel() override = default;

    /**
       @brief Pre drawing processing
    */
    void PreDraw() override;

    /** @brief Get static data per model */
    const CStaticModelData* GetStaticData() override { return m_model.GetStaticData(); }

    /** @brief Get controller class for animated models */
    CDynamicModelController* GetController() { return &m_model; }

private:
    /** @brief Controller class for animated models */
    CDynamicModelController m_model;
};

#endif // !__MODEL_H__
