/**
 * @file   Component.h
 * @brief  This file handles abstract class for component used in update processing.
 * 
 * @author Shodai Makino
 * @date   2023/12/1
 */

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "UniquePtr.h"
#include "UtilityForMath.h"

// Forward declarations
class CGameObject;
class CScene;
class CCallbackSystem;
class ACCollider3D;
class CContactOpponent;

/** @brief This is abstract class for component used in update processing */
class ACComponent : public ACWeakPtrFromThis<ACComponent> {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
    */
    ACComponent(CGameObject* owner);

    /**
       @brief Processing when this component is added to an object
    */
    virtual void Awake() {}

    /**
       @brief Processing for the first update frame
    */
    virtual void Start() {}

    /**
       @brief Update processing
    */
    virtual void Update() {}

    /**
       @brief Pre-update process for physics simulation 
    */
    virtual void PrePhysicsUpdate() {}

    /**
       @brief Process to be called at instance destruction
    */
    virtual void OnDestroy() {}

    /**
       @brief Processing when this component is enabled
       @details
       Also called when the object is created
    */
    virtual void OnEnable() {}

    /**
       @brief Processing when this component is disabled
       @details
       Also called when the object is destroyed
    */
    virtual void OnDisable() {}

    /**
       @brief Processing when collision begins
       @param opponent Information of a contacted collider
    */
    virtual void OnCollisionBegin(const CContactOpponent& opponent) {}

    /**
       @brief Processing during collision
       @param opponent Information of a contacted collider
    */
    virtual void OnCollisionHit(const CContactOpponent& opponent) {}

    /**
       @brief Processing when collision ends
       @param opponent Weak pointer to a contacted collider
    */
    virtual void OnCollisionEnd(const CWeakPtr<ACCollider3D>& opponent) {}

    /** @brief Get game object that this component has */
    CWeakPtr<CGameObject> GetGameObject() { return m_gameObj; }
    /** @brief Get game object that this component has */
    CWeakPtr<const CGameObject> GetGameObject() const { return m_gameObj; }

    /** @brief Get transform of the object */
    const Transformf& GetTransform() const;

    /** @brief Get scene where this component exists  */
    CWeakPtr<CScene> GetScene();
    /** @brief Get scene where this component exists  */
    CWeakPtr<const CScene> GetScene() const;

    /** @brief Get the callback system that the gameobject has */
    CCallbackSystem* GetCallbackSystem();

    /** @brief Set the active flag of this component */
    void SetIsActive(bool isActive);
    /** @brief Is this component itself active? */
    bool IsActiveSelf() const { return m_isActive; }
    /** @brief Is this component itself and the game object that owns it active? */
    bool IsActiveOverall() const;

protected:
    /** @brief Game object that is the owner of this component */
    CWeakPtr<CGameObject> m_gameObj;
    /** @brief Is this component active? */
    bool m_isActive;
};

#endif // !__COMPONENT_H__
