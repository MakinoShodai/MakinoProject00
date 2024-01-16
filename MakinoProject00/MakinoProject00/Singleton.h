/**
 * @file   Singleton.h
 * @brief  File handling abstract singleton class.
 * 
 * @author Shodai Makino
 * @date   2023/07/10
 */

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include "ReleaseSingleton.h"
#include "UniquePtr.h"
#include "UtilityForException.h"

/**
   @brief Abstract inner class
   @tparam Outer Outer class of derived class
   @details
   Constructor should be using
*/
template<class Outer>
class ACInnerClass {
public:
    /** @brief Disallow the copy constructor */
    ACInnerClass(const ACInnerClass&) = delete;
    /** @brief Disallow the copy assignment operator */
    ACInnerClass& operator=(const ACInnerClass&) = delete;
    /** @brief Disallow the move constructor */
    ACInnerClass(ACInnerClass&&) = delete;
    /** @brief Disallow the move assignment constructor */
    ACInnerClass& operator=(ACInnerClass&&) = delete;

    /**
       @brief Constructor
       @param owner Pointer to owner of this class
    */
    ACInnerClass(Outer* owner) : m_owner(owner) {}

    /** @brief Destructor */
    virtual ~ACInnerClass() = default;

protected:
    /** @brief Pointer to owner of this class */
    Outer* m_owner;
};

/**
   @brief Abstract singleton base class (CRTP Pattern)
   @tparam Derived Derived class that inherit from this abstract class themselves
   @details 
   This class doesn't have Get function
   The following must be done in the derived class.
   1. Declare this abstract class as friend.
   2. Declare destructor in public.
   3. Declare constructor in protected(or private).
*/
template<class Derived>
class ACSingletonBase : public ACReleaseSingleton {
public:
    /** @brief Disallow the copy constructor */
    ACSingletonBase(const ACSingletonBase&) = delete;
    /** @brief Disallow the copy assignment operator */
    ACSingletonBase& operator=(const ACSingletonBase&) = delete;
    /** @brief Disallow the move constructor */
    ACSingletonBase(ACSingletonBase&&) = delete;
    /** @brief Disallow the move assignment constructor */
    ACSingletonBase& operator=(ACSingletonBase&&) = delete;

    /**
       @brief Discard instance
    */
    void DestroyInstance() override final;

protected:
    /**
       @brief Constructor
    */
    ACSingletonBase(int priority) : ACReleaseSingleton() { ACReleaseSingleton::AddInstanceToArray(priority, this); }
    /**
       @brief Destructor
    */
    ~ACSingletonBase() override { ACReleaseSingleton::RemoveInstanceToArray(this); }

    /**
       @brief Process to be called at instance destruction
    */
    virtual void OnDestroy() {}

    /**
       @brief Get singleton instance
       @return Instance
    */
    inline static CUniquePtr<Derived>& GetProtected();

private:
};

/**
   @brief Abstract singleton class for main thread only (CRTP Pattern)
   @tparam Derived Derived class that inherit from this abstract class themselves
   @details
   This singleton class is basically accessible only from the main thread.
   If you want functionality that can be accessed from any thread, define an internal class.
   The following must be done in the derived class.
   1. Declare ACSingleton as friend.
   2. Declare destructor in public.
   3. Declare constructor in protected(or private).
*/
template <class Derived>
class ACMainThreadSingleton : public ACSingletonBase<Derived> {
public:
    /**
       @brief Get singleton instance for main thread
       @return Instance
       @details
       If this function is called from threads other than the main thread,
       it throws an exception
    */
    inline static Derived& GetMain() {
        if (!Utl::CMainThreadChecker::IsMainThread()) {
            throw Utl::Error::CFatalError(L"This class can't be accessed from any thread other than the main thread");
        }
        return *ACSingletonBase<Derived>::GetProtected();
    }

protected:
    // Using constructor
    using ACSingletonBase<Derived>::ACSingletonBase;
};


/**
   @brief Abstract singleton class for any thread (CRTP Pattern)
   @tparam Derived Derived class that inherit from this abstract class themselves
   @details
   All functions should be thread-safe on the side of classes that inherit from this class.
   The following must be done in the derived class.
   1. Declare ACSingleton as friend.
   2. Declare destructor in public.
   3. Declare constructor in protected(or private).
*/
template <class Derived>
class ACAnyThreadSingleton : public ACSingletonBase<Derived> {
public:
    /**
       @brief Get singleton instance for any thread
       @return Instance
    */
    inline static Derived& GetAny() { return *ACSingletonBase<Derived>::GetProtected(); }

protected:
    // Using constructor
    using ACSingletonBase<Derived>::ACSingletonBase;
};

//  Discard instance
template<class Derived>
void ACSingletonBase<Derived>::DestroyInstance() {
    // Call process at instance destruction
    GetProtected()->OnDestroy();

    // Reset instance
    GetProtected().Reset();
}

// Get singleton instance
template<class Derived>
inline CUniquePtr<Derived>& ACSingletonBase<Derived>::GetProtected() {
    static CUniquePtr<Derived> instance = CUniquePtr<Derived>(new Derived());
    return instance;
}

#endif // !__SINGLETON_H__
