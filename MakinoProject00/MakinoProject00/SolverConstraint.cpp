#include "SolverConstraint.h"

// Constructor
Mkpe::SolverConstraint::SolverConstraint(CWeakPtr<CColliderWrapper> wrapperA, CWeakPtr<CColliderWrapper> wrapperB)
    : wrapperA(wrapperA)
    , wrapperB(wrapperB)
    , torqueA(Vector3f::Zero())
    , torqueB(Vector3f::Zero())
    , angularAxisA(Vector3f::Zero())
    , angularAxisB(Vector3f::Zero())
    , linearNormalA(Vector3f::Zero())
    , linearNormalB(Vector3f::Zero())
    , denomOfImpulse(0.0f)
    , appliedImpulse(0.0f)
    , velocityImpulse(0.0f)
    , penetrationImpulse(0.0f)
    , friction(0.0f)
    , lowerLimit(0.0f)
    , upperLimit(0.0f)
    , synchroSolverIndex(-1)
{ }
