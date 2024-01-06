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

// Forward declaration
class CGameObject;

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
    virtual void Start() = 0;

    /**
       @brief Update processing
    */
    virtual void Update() = 0;

    /**
       @brief Process to be called at instance destruction
    */
    virtual void OnDestroy() {}

protected:
    /** @brief Game object that is the owner of this component */
    CWeakPtr<CGameObject> m_gameObj;
    /** @brief Is this component active? */
    bool m_isActive;
};

#endif // !__COMPONENT_H__
