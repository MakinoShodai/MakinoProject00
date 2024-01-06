#include "Random.h"

// Generate random numbers within a range
int CRandom::GenerateRange(int min, int max) {
    // Generate range instance
    std::uniform_int_distribution<int> dist(min, max);

    // Return random value
    return dist(m_engine);
}

// Constructor
CRandom::CRandom()
    : ACSingletonBase(0)
    , m_engine(m_seed_gen())
{ }
