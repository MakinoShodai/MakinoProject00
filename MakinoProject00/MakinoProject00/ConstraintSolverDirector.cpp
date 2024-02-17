#include "ConstraintSolverDirector.h"
#include "PhysicsWorld.h"

// Constructor
Mkpe::CConstraintSolverDirector::CConstraintSolverDirector(CPhysicsWorld* physicsWorld)
    : m_physicsWorld(physicsWorld)
    , m_contactConstraints()
    , m_frictionConstraints()
    , m_iterativeSolverMax(10)
{ }

// Create constraints in the current frame
void Mkpe::CConstraintSolverDirector::CreateConstraint(float invTimeStep) {
    // Get pairs with potential for collision
    auto* pairs = m_physicsWorld->GetBVTree()->GetPairManager()->GetPair();

    for (auto& it : *pairs) {
        // If contact points is nothing, do nothing
        UINT size = it.contactData.GetContactPointSize();
        if (size <= 0) { continue; }

        // Get the two wrappers
        CWeakPtr<CColliderWrapper> wrapperA = it.GetWrapperA()->WeakFromThis();
        CWeakPtr<CColliderWrapper> wrapperB = it.GetWrapperB()->WeakFromThis();

        // Check that A and B each move
        bool isMoveA = !(wrapperA->IsStatic() || wrapperA->GetCollider()->CheckTagOverlap(wrapperB->GetCollider().Get()));
        bool isMoveB = !(wrapperB->IsStatic() || wrapperB->GetCollider()->CheckTagOverlap(wrapperA->GetCollider().Get()));

        // Get the two simulation bodies
        CSimulationBodyBase* bodyA = wrapperA->GetSimulationBody();
        CSimulationBodyBase* bodyB = wrapperB->GetSimulationBody();

        // Get the position of the two bodies
        Vector3f posA = wrapperA->GetPos();
        Vector3f posB = wrapperB->GetPos();

        // Get the array of contact points
        CUniquePtr<ContactPoint>* cpArray = it.contactData.GetContactPointArray();

        // Perform processing for all contact points
        for (UINT i = 0; i < size; ++i) {
            CUniquePtr<ContactPoint>& cp = cpArray[i];

            // Compute the normal vector of wrapper A
            Vector3f normalWorldOnA = -cp->normalWorldOnB;

            // Get the relative position of the contact point of the two bodies
            Vector3f relativeA = cp->pointWorldOnB - normalWorldOnA * cp->penetDepth - posA;
            Vector3f relativeB = cp->pointWorldOnB - posB;

            // Get the velocity of the contact point of the two bodies
            Vector3f pointVelocityA = bodyA->GetVelocityOfPointInObject(relativeA);
            Vector3f pointVelocityB = bodyB->GetVelocityOfPointInObject(relativeB);

            // Calculate the relative velocity
            Vector3f relPointVelocity = pointVelocityA - pointVelocityB;
            // Calculate the direction component of the normal vector of the relative velocity vector
            float relVelocityDot = cp->normalWorldOnB.Dot(relPointVelocity);

            // Create a contact constraint
            SolverConstraint contactSolver(wrapperA, wrapperB);
            CreateContactConstraint(&contactSolver, cp.Get(), wrapperA.Get(), wrapperB.Get(), bodyA, bodyB, relativeA, relativeB, isMoveA, isMoveB, invTimeStep);
            m_contactConstraints.push_back(std::move(contactSolver));

            // Create a friction constraint
            {
                SolverConstraint frictionSolver(wrapperA, wrapperB);
                
                // Bind the friction constraint to the contact constraint
                frictionSolver.synchroSolverIndex = static_cast<int>(m_contactConstraints.size()) - 1;
            
                // Calculate a friction direction vector
                // #NOTE : Vector obtained by subtracting the component in the repulsive direction from the relative velocity vector
                Vector3f vFriction = relPointVelocity - cp->normalWorldOnB * relVelocityDot;
            
                // If the friction direction vector isn't 0, use the frictional directional vector calculated from the relative vector
                // #NOTE : Anisotropic friction is not handled. If necessary, it will be implemented
                float frictionLength2 = vFriction.LengthSq();
                if (frictionLength2 > FLT_EPSILON * FLT_EPSILON) {
                    // Normalize the friction direction vector
                    vFriction.Normalize();
            
                    // Create a friction contraint
                    CreateFrictionConstraint(vFriction, &frictionSolver, cp.Get(), wrapperA.Get(), wrapperB.Get(), bodyA, bodyB, relativeA, relativeB, isMoveA, isMoveB, invTimeStep);
                }
                // If the friction direction vector is nearly 0, make a tangent direction of repulsion the friction direction vector
                else {
                    // Calculate a tangent vector
                    vFriction = Utl::Math::GetTangentVector(cp->normalWorldOnB);
                    // Create a friction contraint
                    CreateFrictionConstraint(vFriction, &frictionSolver, cp.Get(), wrapperA.Get(), wrapperB.Get(), bodyA, bodyB, relativeA, relativeB, isMoveA, isMoveB, invTimeStep);
                }

                // Add to vector
                m_frictionConstraints.push_back(std::move(frictionSolver));
            }
        }
    }
}

// Contact solving with an iterative solver
void Mkpe::CConstraintSolverDirector::IterativeSolver() {
    // Allowable value that is the square of the speed anomaly
    constexpr float ERROR_PERMISSION = 0.0f;

    // Penetration solving with an iterative solver
    IterativeSolverPenetrationImpulse();

    // Repeat up to the maximum number of iterative solvers
    for (int i = 0; i < m_iterativeSolverMax; ++i) {
        // Perform constraint resolution once
        float maxSquareError = SingleIterationConstraintSolver();

        // If the abnormal value goes below the allowable value, it is finished
        if (maxSquareError <= ERROR_PERMISSION * ERROR_PERMISSION) {
            break;
        }
    }
}

// Clear all constraints
void Mkpe::CConstraintSolverDirector::ClearConstraint() {
    m_contactConstraints.clear();
    m_frictionConstraints.clear();
}

// Create contact constraint
void Mkpe::CConstraintSolverDirector::CreateContactConstraint(SolverConstraint* solver, ContactPoint* cp,
    CColliderWrapper* wrapperA, CColliderWrapper* wrapperB,
    CSimulationBodyBase* bodyA, CSimulationBodyBase* bodyB,
    const Vector3f& relativePosA, const Vector3f& relativePosB, 
    bool isMoveA, bool isMoveB, float invTimeStep) {
    // ERP(Error Reduction Parameter)
    constexpr float ERP = 0.2f;

    // Get the normal vector on body A
    Vector3f normalWorldOnA = -cp->normalWorldOnB;

    // Calculate denominator of impulse and get relative velocity in the direction of repulsion
    float rel_repulsionVel = CalculateDenomOfImpulse(cp->normalWorldOnB, normalWorldOnA, solver, cp, wrapperA, wrapperB, bodyA, bodyB, relativePosA, relativePosB, isMoveA, isMoveB);

    // Calculate bias velocity
    float biasVelocity = -cp->penetDepth * ERP * invTimeStep;

    // Calculate penetration impulse and velocity impulse
    solver->penetrationImpulse = biasVelocity * solver->denomOfImpulse;
    solver->velocityImpulse = -rel_repulsionVel * (1.0f + cp->combinedRestitution) * solver->denomOfImpulse;

    constexpr float ignoreThreshold = -0.04f;
    if (cp->penetDepth > ignoreThreshold) {
        solver->velocityImpulse += solver->penetrationImpulse;
        solver->penetrationImpulse = 0.0f;
    }
}

// Create friction constraint of First rigid body
void Mkpe::CConstraintSolverDirector::CreateFrictionConstraint(const Vector3f& frictionDirA, SolverConstraint* solver, ContactPoint* cp,
    CColliderWrapper* wrapperA, CColliderWrapper* wrapperB, 
    CSimulationBodyBase* bodyA, CSimulationBodyBase* bodyB, 
    const Vector3f& relativePosA, const Vector3f& relativePosB, 
    bool isMoveA, bool isMoveB, float invTimeStep) {
    // Get the direction of friction for bodyB
    Vector3f frictionDirB = -frictionDirA;

    // Calculate denominator of impulse and get relative velocity in the direction of repulsion
    float rel_repulsionVel = CalculateDenomOfImpulse(frictionDirA, frictionDirB, solver, cp, wrapperA, wrapperB, bodyA, bodyB, relativePosA, relativePosB, isMoveA, isMoveB);

    // Calculate velocity impulse
    solver->velocityImpulse = -rel_repulsionVel * solver->denomOfImpulse;

    // Set friction parameters
    solver->friction = cp->combinedFriction;
}

// Calculate denominator of impulse
float Mkpe::CConstraintSolverDirector::CalculateDenomOfImpulse(const Vector3f& dirA, const Vector3f& dirB, 
    SolverConstraint* solver, ContactPoint* cp,
    CColliderWrapper* wrapperA, CColliderWrapper* wrapperB, 
    CSimulationBodyBase* bodyA, CSimulationBodyBase* bodyB, 
    const Vector3f& relativePosA, const Vector3f& relativePosB, 
    bool isMoveA, bool isMoveB) {
    /*
     * For bodies A and B,
     * mAInv, mBInv is the inverse of their respective masses,
     * IAInv,IBInv is the inverse of their inertia tensors,
     * rA,rB is relative position vectors of their contact points,
     * When the impulse is J, the following equations can be used to find the impluse for bodies A and B after collision.
     * J = -'Relative velocity v' * ('repulsion coefficient e' + 1) / (mAInv + mBInv + ((rA x n・IAInv) x rA)・n + ((rB x n・IBInv) x rB)・n)
     * Here is the calculation for the denominator side.
     * 1 / (mAInv + mBInv + 'Linear velocity in normal direction rotation A' + 'Linear velocity in normal direction rotation B')
     * So we start by calculating the linear velocity in each of the normal rotations, and finally find the inverse of the denominator side of J.
     */
    
    // First, find rA x n and rB x n. This represents the torque
    Vector3f rAn = relativePosA.Cross(dirA);
    Vector3f rBn = relativePosB.Cross(dirA);
    
    // Do up to rA x n・IAInv and rB x n・IBInv by multiplying rAn by the inverse of the inertia tensor rAn.
    // In other words, from the formula 'Torque = Inertia tensor ・ Acceleration', the acceleration of the torque at this contact point is calculated.
    Vector3f accelerationA = (bodyA->GetInvInertiaTensor() * rAn) * bodyA->GetRotateAllowable();
    Vector3f accelerationB = (bodyB->GetInvInertiaTensor() * rBn) * bodyB->GetRotateAllowable();
    
    // Calculate up to (rA x n・IAInv) x rA and (rB x n・IBInv) x rB.
    // In other words, 
    // the formula 'Linear velocity at contact point = angular velocity vector x Relative position vector of contact point' 
    // is used to calculate the linear velocity at the contact point.
    // Also, accelerationB contains the inverse vector of the original result, so invert it and use it.
    Vector3f vAnPointV = accelerationA.Cross(relativePosA);
    Vector3f vBnPointV = accelerationB.Cross(relativePosB);
    
    // Calculate up to ((rA x n・IAInv) x rA)・n and ((rB x n・IBInv) x rB)・n.
    // In other words, it calculates the linear velocity in the normal direction of rotation.
    float vAnPointVn = (isMoveA) ? dirA.Dot(vAnPointV) : 0.0f;
    float vBnPointVn = (isMoveB) ? dirA.Dot(vBnPointV) : 0.0f;

    // Calculate the inverse of the denominator side of J
    solver->denomOfImpulse = 1.0f / (wrapperA->GetInvMass() + wrapperB->GetInvMass() + vAnPointVn + vBnPointVn);
    
    // Let each vector hold
    if (isMoveA) {
        solver->linearNormalA = dirA;
        solver->torqueA = rAn;
        solver->angularAxisA = accelerationA;
    }
    if (isMoveB) {
        solver->linearNormalB = dirB;
        solver->torqueB = -rBn;
        solver->angularAxisB = -accelerationB;
    }
    
    // Calculate relative velocity in the direction of repulsion
    float repulsionVelA = dirA.Dot(bodyA->GetTotalLinearVelocity()) + rAn.Dot(bodyA->GetTotalAngularVelocity());
    float repulsionVelB = dirB.Dot(bodyB->GetTotalLinearVelocity()) + -rBn.Dot(bodyB->GetTotalAngularVelocity());
    return repulsionVelA + repulsionVelB;
}

// Penetration solving with an iterative solver
void Mkpe::CConstraintSolverDirector::IterativeSolverPenetrationImpulse() {
    // Allowable value that is the square of the position anomaly
    constexpr float POSITIONERROR_PERMISSION = 0.0f;

    // Repeat up to the maximum number of iterative solvers
    for (int i = 0; i < m_iterativeSolverMax; ++i) {
        float maxSquarePositionError = 0.0f;
        for (auto& it : m_contactConstraints) {
            // Solve penetration impulse and get square of the coordinate anomaly value
            float squarePositionError = SolverPenetrationImpulse(&it);

            // Holds the square of the maximum coordinate anomaly
            maxSquarePositionError = (std::max)(maxSquarePositionError, squarePositionError);
        }
        // If the abnormal value goes below the allowable value, it is finished
        if (maxSquarePositionError <= POSITIONERROR_PERMISSION * POSITIONERROR_PERMISSION) {
            break;
        }
    }
}

// Solve constraints only once
float Mkpe::CConstraintSolverDirector::SingleIterationConstraintSolver() {
    // Solve contact constraint
    float maxSquareError = 0.0f;
    for (auto& it : m_contactConstraints) {
        float error = SolverContactVelocityImpulse(&it);
        maxSquareError = (std::max)(maxSquareError, error * error);
    }

    // Solve friction constraint
    for (auto& it : m_frictionConstraints) {
        // Get applied impulse of synchronized contact constraints
        float appliedImpulse = (it.synchroSolverIndex >= 0) ? m_contactConstraints[it.synchroSolverIndex].appliedImpulse : 0.0f;

        // If the applied impulse is not 0
        if (appliedImpulse > 0) {
            // Make the upper limit value and the lower limit value 'the friction coefficient x the applied impulse'
            // #NOTE : 'friction coefficient = friction fource / repulsive force', so it must be 'friction force = repulsive force x friction coefficient'
            it.upperLimit = it.friction * appliedImpulse;
            it.lowerLimit = -it.upperLimit;

            float error = SolverFrictionVelocityImpulse(&it);
            maxSquareError = (std::max)(maxSquareError, error * error);
        }
    }

    return maxSquareError;
}

// Solve penetration impulse
float Mkpe::CConstraintSolverDirector::SolverPenetrationImpulse(SolverConstraint* solver) {
    // If penetration impulse is zero, do nothing
    if (!solver->penetrationImpulse) { return 0.0f; }

    // Solve impulse
    return SolverImpulse(solver, solver->penetrationImpulse, 
        &CSimulationBodyBase::GetPenetrationLinearVelocity,
        &CSimulationBodyBase::GetPenetrationAngularVelocity,
        &CSimulationBodyBase::SolvePenetrationAtContact);
}

// Solve velocity impulse for contact solver
float Mkpe::CConstraintSolverDirector::SolverContactVelocityImpulse(SolverConstraint* solver) {
    return SolverImpulse(solver, solver->velocityImpulse,
        &CSimulationBodyBase::GetDeltaLinearVelocity,
        &CSimulationBodyBase::GetDeltaAngularVelocity,
        &CSimulationBodyBase::SolveVelocityAtContact);
}

// Solve friction impulse for contact solver
float Mkpe::CConstraintSolverDirector::SolverFrictionVelocityImpulse(SolverConstraint* solver) {
    return SolverImpulse(solver, solver->velocityImpulse,
        &CSimulationBodyBase::GetDeltaLinearVelocity,
        &CSimulationBodyBase::GetDeltaAngularVelocity,
        &CSimulationBodyBase::SolveVelocityAtContact,
        true);
}

// Solve impulse
float Mkpe::CConstraintSolverDirector::SolverImpulse(SolverConstraint* solver, float impulse, 
    SimulationBodyGetVelocityFunc linearFunc, SimulationBodyGetVelocityFunc angularFunc, 
    SimulationBodySolveFunc solveFunc, bool isUpperLimit) {
    // Get the two wrappers
    CColliderWrapper* wrapperA = solver->GetWrapperA();
    CColliderWrapper* wrapperB = solver->GetWrapperB();
    // Get the two simulation bodies
    CSimulationBodyBase* bodyA = wrapperA->GetSimulationBody();
    CSimulationBodyBase* bodyB = wrapperB->GetSimulationBody();

    // Calculate velocity for solving penetration of the two bodies
    float bodyAvelDot = solver->linearNormalA.Dot((bodyA->*linearFunc)()) + solver->torqueA.Dot((bodyA->*angularFunc)());
    float bodyBvelDot = solver->linearNormalB.Dot((bodyB->*linearFunc)()) + solver->torqueB.Dot((bodyB->*angularFunc)());

    // Subtract velocity for solving penetration of the two bodies, from penetration impulse
    float deltaImpulse = impulse;
    deltaImpulse -= bodyAvelDot * solver->denomOfImpulse;
    deltaImpulse -= bodyBvelDot * solver->denomOfImpulse;

    // Add delta impulse to the applied impulse of constraint
    float sum = solver->appliedImpulse + deltaImpulse;
    // If the total impulse is less than the lower limit value of impulse for not penetrate
    if (sum < solver->lowerLimit) {
        // Adjust to fit the lower limit value of impulse for not penetrate
        deltaImpulse = solver->lowerLimit - solver->appliedImpulse;
        solver->appliedImpulse = solver->lowerLimit;
    }
    // If you use the upper limit, do an upper limit comparison
    else if (isUpperLimit && sum > solver->upperLimit) {
        deltaImpulse = solver->upperLimit - solver->appliedImpulse;
        solver->appliedImpulse = solver->upperLimit;
    }
    // Otherwise, assign the applied impulse plus delta impulse
    else {
        solver->appliedImpulse = sum;
    }

    // Apply force to the two bodies
    // #NOTE : The rotational one already takes the inertia tensor into account
    (bodyA->*solveFunc)(solver->linearNormalA * wrapperA->GetInvMass(), solver->angularAxisA, deltaImpulse);
    (bodyB->*solveFunc)(solver->linearNormalB * wrapperB->GetInvMass(), solver->angularAxisB, deltaImpulse);

    // Reverse the coordinate anomaly applied this time
    float posError = deltaImpulse * (1 / solver->denomOfImpulse);
    
    // Return the maximum coordinate anomaly
    return posError;
}
