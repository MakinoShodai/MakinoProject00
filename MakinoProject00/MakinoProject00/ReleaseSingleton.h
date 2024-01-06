/**
 * @file   ReleaseSingleton.h
 * @brief  Release singleton instances by controlling the order.
 * 
 * @author Shodai Makino
 * @date   2023/11/5
 */

#ifndef __RELEASE_SINGLETON_H__
#define __RELEASE_SINGLETON_H__

/**
   @brief This class is used to control the order and release instance
   @details
   this is an abstract class for ACSingleton class, so don't extend it in other classes!
*/
class ACReleaseSingleton {
public:
    /**
       @brief Release all instance
    */
    static void AllRelease();

    /**
       @brief Discard instance
    */
    virtual void DestroyInstance() = 0;

protected:
    /**
       @brief Constructor
    */
    ACReleaseSingleton() = default;

    /**
       @brief Destructor
    */
    virtual ~ACReleaseSingleton() {}

    /**
       @brief Add the created instance to the array
       @param priority Release priority. release from low to high
       @param instance Add the instance
    */
    static void AddInstanceToArray(int priority, ACReleaseSingleton* instance);

    /**
       @brief Remove the instance in the array
       @param instance Remove the instance
    */
    static void RemoveInstanceToArray(ACReleaseSingleton* instance);

private:
    /** @brief All singleton class instance and release priority */
    static std::vector<std::pair<int, ACReleaseSingleton*>> ms_allInstance;
    /** @brief Is removing instances from the array not allowed? */
    static bool ms_isCantRemove;
};

#endif // !__RELEASE_SINGLETON_H__
