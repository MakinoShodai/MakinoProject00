/**
 * @file   Random.h
 * @brief  This file handles random value generator.
 * 
 * @author Shodai Makino
 * @date   2023/12/27
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

#include "Singleton.h"

 // Random value generator
class CRandom : public ACMainThreadSingleton<CRandom> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /** @brief Destructor */
    ~CRandom() = default;

    /**
       @brief Generate random numbers within a range
       @param min Minimum value
       @param max Maximum value
       @return Random value
    */
    int GenerateRange(int min, int max);

private:
    /** @brief Constructor */
    CRandom();

private:
    /** @brief hardware random value */
    std::random_device m_seed_gen;
    /** @brief Suspected garbled value generator */
    std::mt19937       m_engine;
};

#endif // !__RANDOM_H__
