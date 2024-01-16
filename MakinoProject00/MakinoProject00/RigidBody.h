/**
 * @file   RigidBody.h
 * @brief  This file handles rigid body component.
 * 
 * @author Shodai Makino
 * @date   2023/07/20
 */

#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "Component.h"
#include "GameObject.h"
#include "Collider3D.h"
#include "StableHandleVector.h"
#include "SimulationBody.h"

// Forward declaration
namespace Mkpe { class CColliderWrapper; }

/** @brief Rigid body material */
struct RigidBodyMaterial {
    /** @brief Friction factor */
    float    friction;
    /** @brief Restitution factor */
    float    restitution;

    /**
       @brief Constructor 
       @param friction Friction factor
       @param restitution Restitution factor
    */
    RigidBodyMaterial(float friction = 0.5f, float restitution = 0.0f);
};

/** @brief Bit namespace for locking rigid body axis */
namespace RigidBodyAxisLock {
    /** @brief Bit variable type for locking rigid body axis */
    using Bit = uint8_t;

    /** @brief Flag constants */
    namespace Flag {
        /** @brief No flag */
        const Bit NONE = 0;
        /** @brief X axis */
        const Bit _X = 1 << 0;
        /** @brief Y axis */
        const Bit _Y = 1 << 1;
        /** @brief Z axis */
        const Bit _Z = 1 << 2;
        /** @brief All axis */
        const Bit ALL = _X | _Y | _Z;
    } // namespace Flag
} // namespace RigidBodyAxisLock

/** @brief Rigid body type */
enum class RigidBodyType {
    /** @brief Static */
    Static,
    /** @brief Kinematic */
    Kinematic,
    /** @brief Dynamic */
    Dynamic,
};

/** @brief Constant indicating that the gravity scale is not used. */
constexpr float GRAVITYSCALE_NULL = -1.0f;
/** @brief Name of callback function to send rigid body to collider wrapper */
static inline const std::string CALLBACK_SEND_RIGIDBODY_TO_WRAPPER = "SendRigidBodyToWrapper";
/** @brief Name of callback function to send simulation body to collider wrapper */
static inline const std::string CALLBACK_SEND_SIMULATIONBODY_TO_WRAPPER = "SendSimulationBodyToWrapper";

/** @brief Rigid body component */
class CRigidBody : public ACComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
    */
    CRigidBody(CGameObject* owner);
    
    /**
       @brief Destructor
    */
    ~CRigidBody() = default;

    /**
       @brief Awake processing
    */
    void Awake() override;

    /**
       @brief Start processing
    */
    void Start() override;

    /**
       @brief Update processing
    */
    void Update() override {}

    /**
       @brief Processing at destruction
    */
    void OnDestroy() override;

    /**
       @brief Processing when this component is disabled
    */
    void OnDisable() override;

    /**
       @brief Add force to this rigid body
       @param force Strength of force
       @param mode Force mode
    */
    void AddForce(const Vector3f& force, ForceMode mode = ForceMode::Force);

    /**
       @brief Add torque to this rigid body
       @param torque Strength of torque
       @param mode Force mode
    */
    void AddTorque(const Vector3f& torque, ForceMode mode = ForceMode::Force);

    /**
       @brief Bind this collider to wrapper
       @param wrapper Wrapper to bind thsi collider
    */
    void BindWrapper(Mkpe::CColliderWrapper* wrapper);

    /**
       @brief Set axis lock for movement
       @param bit Bitstring representing the axis to be locked
    */
    void SetMoveLock(RigidBodyAxisLock::Bit bit);

    /**
       @brief Set axis lock for rotation
       @param bit Bitstring representing the axis to be locked
    */
    void SetRotateLock(RigidBodyAxisLock::Bit bit);

    /**
       @brief Is this rigid body static?
       @return Result
    */
    inline bool IsStatic() const { return m_bodyType == RigidBodyType::Static; }

    /**
       @brief Set the flag to use pseudo velocity
       @param isPseudoVelocity Use pseudo velocity?
       @details
       If this flag is true, collision detection can be performed even if the object's transform is changed directly.
    */
    void SetIsPseudoVelocity(bool isPseudoVelocity);
    /** @brief Use pseudo velocity? */
    bool IsPseudoVelocity() const { return m_isPseudoVelocity; }

    /** @brief Set material of rigid body */
    void SetMaterial(const RigidBodyMaterial& material) { m_material = material; }
    /** @brief Get the material of rigid body */
    const RigidBodyMaterial& GetMaterial() const { return m_material; }

    /** @brief Set rigid body type */
    void SetBodyType(RigidBodyType bodyType);
    /** @brief Get the rigid body type */
    RigidBodyType GetBodyType() const { return m_bodyType; }

    /** @brief Get the linear velocity */
    const Vector3f& GetLinearVelocity() const { return (m_simulationBodyHandle) ? GetSimulation()->GetLinearVelocity() : Vector3f::Zero(); }
    /** @brief Get the angular velocity */
    const Vector3f& GetAngularVelocity() const { return (m_simulationBodyHandle) ? GetSimulation()->GetAngularVelocity() : Vector3f::Zero(); }

    /** @brief Set mass */
    void SetMass(float mass);
    /** @brief Get the inverse of the mass */
    float GetInvMass() const { return m_invMass; }
    /** @brief Get the mass */
    float GetMass() const { return 1.0f / m_invMass; }

    /**
       @brief Set gravity scale
       @param gravityScale Don't use gravity scale in default argument
    */
    void SetGravityScale(float gravityScale = GRAVITYSCALE_NULL);
    /** @brief Get the gravity scale */
    float GetGravityScale() const { return m_gravityScale; }

    /** @brief Get allowable vector for movement */
    const Vector3f& GetMoveAllowable() const { return m_moveAllowable; }
    /** @brief Get allowable vector for rotation */
    const Vector3f& GetRotateAllowable() const { return m_rotateAllowable; }

private:
    /** @brief Get own simulation body */
    CWeakPtr<Mkpe::CSimulationBody> GetSimulation();
    /** @brief Get own simulation body */
    CWeakPtr<const Mkpe::CSimulationBody> GetSimulation() const;
    
    /**
       @brief Set gravity to own simulation body
    */
    void SetSimulationGravity();

private:
    /** @brief Material of rigid body */
    RigidBodyMaterial m_material;
    /** @brief Body type at current frame */
    RigidBodyType m_bodyType;
    /** @brief Allowable vector for movement. For example, (1, 1, 0) would disallow Z-axis movement */
    Vector3f m_moveAllowable;
    /** @brief Allowable vector for rotation. For example, (1, 1, 0) would disallow Z-axis rotation */
    Vector3f m_rotateAllowable;
    /** @brief The inverse of the mass */
    float m_invMass;
    /** @brief Gravity scale */
    float m_gravityScale;
    /** @brief Treat direct transform changes as velocity? */
    bool m_isPseudoVelocity;
    
    /** @brief Handle of itself in dynamic array of rigid bodies inside physics engine */
    WeakStableHandle<CWeakPtr<CRigidBody>> m_rigidBodyHandle;
    /** @brief Handle of itself in dynamic array of simulation bodies inside physics engine */
    WeakStableHandle<CUniquePtrWeakable<Mkpe::CSimulationBody>> m_simulationBodyHandle;
};

#endif // !__RIGIDBODY_H__
