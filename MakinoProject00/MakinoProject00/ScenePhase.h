/**
 * @file   ScenePhase.h
 * @brief  This file handles phase of the scene currently being processed.
 * 
 * @author Shodai Makino
 * @date   2024/1/15
 */

#ifndef __SCENE_PHASE_H__
#define __SCENE_PHASE_H__

#include "UtilityForBit.h"

// Forward declaration
class CGameObject;

/** @brief Phase of the scene currently being processed */
enum class ScenePhase {
    /** @brief Updating process for component */
    Update,
    /** @brief Physics simulation pre-updating process for component */
    PrePhysicsUpdate,
    /** @brief Physics simulation process for physics engine */
    Physics,
    /** @brief Collision response process for component */
    OnCollision,
};

/** @brief Enum to observe transform changes */
enum class TransformObserve : uint8_t {
    None = 0,
    Pos = 1 << 0,
    Rotate = 1 << 1,
    Scale = 1 << 2,
    All = Pos | Rotate | Scale,
};
ENUM_TO_BITFLAG(TransformObserve); // Support for bit operation

/** @brief Class for wrapping and handling multiple scene phases */
class ScenePhasesWrapper {
public:
    /** @brief Constructor */
    ScenePhasesWrapper() = default;

    /**
       @brief Constructor for set scene phases
       @param phases Scene phases to be set
    */
    ScenePhasesWrapper(std::initializer_list<ScenePhase> phases) { SetPhases(phases); }

    /** @brief Destructor */
    ~ScenePhasesWrapper() = default;

    /**
       @brief Set scene phases
       @param phases Scene phases to be set
    */
    void SetPhases(std::initializer_list<ScenePhase> phases);

    /**
       @brief Check the change of transform for the phase this class has
       @param gameObj Game object to be checked
       @param observe Observe bit strings
       @return Result
    */
    bool CheckTransformObserve(CGameObject* gameObj, TransformObserve observe);

private:
    /** @brief Scene phases */
    std::vector<ScenePhase> m_phases;
};

/** @brief Bit string structure for observing whether the transform was changed in the previous process */
struct TransformObserveBits {
    /** @brief Bit string for ScenePhase::Update */
    unsigned int updateBit : 6;
    /** @brief Bit string for ScenePhase::PrePhysicsUpdate */
    unsigned int prePhysicsBit : 6;
    /** @brief Bit string for ScenePhase::Physics */
    unsigned int physicsBit : 6;
    /** @brief Bit string for ScenePhase::OnCollision */
    unsigned int onCollisionBit : 6;

    /**
       @brief Constructor
    */
    TransformObserveBits();

    /**
       @brief Check bit strings in the specified phase
       @param phase Scene phase to be checked
       @param observe Bit for comparison
       @return Result
    */
    bool Check(ScenePhase phase, TransformObserve observe) const;

    /**
       @brief Transfer current bit strings of the current scene phase to previous bit strings
    */
    void Transfer();

    /**
       @brief Stand a bit string of the current scene phase
       @param observe Bits to stand up
    */
    void SetBit(TransformObserve observe);
};

#endif // !__SCENE_PHASE_H__
