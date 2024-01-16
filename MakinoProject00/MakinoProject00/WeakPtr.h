/**
 * @file   WeakPtr.h
 * @brief  This file handles weak pointer that can be generated from unique pointer and shared pointer.
 * 
 * @author Shodai Makino
 * @date   2023/11/28
 */

#ifndef __WEAK_PTR_H__
#define __WEAK_PTR_H__

#include "Utility.h"

 // Forward declarations
template<class T>
class CUniquePtrWeakable;
template<class T>
class CSharedPtr;

/**
   @brief Is the internal type convertible in smart pointer?
   @tparam T Internal type of own
   @tparam U Internal type of target
*/
template<class T, class U>
concept IsConvertibleSmartPtr = Utl::Type::Relat::IsInheritanceRemoveExtent<T, U> &&
    ((std::is_const_v<T> == std::is_const_v<U>) || (!std::is_const_v<U> && std::is_const_v<T>));

/** @brief Is it not an array type? */
template<typename T>
concept IsNotArrayType = !std::is_array_v<T>;

/**
   @brief Cast smart pointer instance (T to U)
   @tparam T Cast source
   @tparam U Cast destination

   @param instance Instance to be cast
   @return If cast succeeds, return pointer, otherwise nullptr
*/
template<class T, class U>
    requires IsConvertibleSmartPtr<T, U>
std::remove_extent_t<U>* CastSmartPtrInstance(std::remove_extent_t<T>* instance) {
    if constexpr (Utl::Type::Relat::IsBaseOfRemoveExtentNotSame<T, U>) {
        return dynamic_cast<std::remove_extent_t<U>*>(instance);
    }
    else {
        return static_cast<std::remove_extent_t<U>*>(instance);
    }
}

/**
   @brief Structure that handles references and its reference counts
   @tparam T Instance type
*/
template <class T>
struct RefCount {
    /** @brief Correct instance type that is not array type */
    using InstanceType = std::remove_extent<T>::type;

    /** @brief Reference to instance */
    InstanceType* m_ref;
    /** @brief Counting of reference */
    UINT m_refCount;

    /**
       @brief Constructor
       @param ptr Pointer to instance
    */
    RefCount(InstanceType* ptr) : m_ref(ptr), m_refCount(1) {}

    /**
       @brief Increment the reference count
       @param refCount Pointer to variable to be incremented
    */
    static void Increment(RefCount* refCount);

    /**
       @brief Decrement the reference count. If the reference count reaches zero, destroy it
       @param refCount Pointer to variable to be decremented
    */
    static void Decrement(RefCount* refCount);

    /**
       @brief Cast RefCount<T>* to RefCount<U>*
       @param refCount Pointer to variable to be cast
       @return If cast succeeds, return pointer, otherwise nullptr
    */
    template <class U>
        requires IsConvertibleSmartPtr<U, T>
    static RefCount<U>* Cast(RefCount<T>* refCount);
};

/**
   @brief Pointer handling weak reference to an instance of 'CUniquePtrWeakable' and 'CSharedPtr'
   @tparam T Instance type
*/
template <class T>
class CWeakPtr {
    /** @brief Correct instance type that is not array type */
    using InstanceType = std::remove_extent<T>::type;

    // Friend declaration
    template<class U>
    friend class CWeakPtr;

public:
    /**
       @brief Constructor
    */
    CWeakPtr() : m_refCount(nullptr) {}

    /**
       @brief Constructor
       @param refCount Reference count structure
    */
    CWeakPtr(RefCount<T>* refCount) noexcept : m_refCount(refCount) { RefCount<T>::Increment(m_refCount); }

    /** @brief Copy constructor */
    CWeakPtr(const CWeakPtr& copy) noexcept : m_refCount((&copy != nullptr) ? copy.m_refCount : nullptr) { RefCount<T>::Increment(m_refCount); }
    /** @brief Move constructor */
    CWeakPtr(CWeakPtr&& other) noexcept : m_refCount((&other != nullptr) ? other.m_refCount : nullptr) { if (m_refCount != nullptr) { other.m_refCount = nullptr; } }

    /**
       @brief Copy constructor for cast
       @tparam Class in inheritance relationship with T

       @param copy Copy source
    */
    template<class U>
        requires IsConvertibleSmartPtr<T, U>
    CWeakPtr(const CWeakPtr<U>& copy);

    /** @brief Copy assignment operator */
    CWeakPtr& operator=(const CWeakPtr& copy);
    /** @brief Move assignment operator */
    CWeakPtr& operator=(CWeakPtr&& other) noexcept;

    /**
       @brief Destructor
    */
    ~CWeakPtr() { Release(); }

    /** @brief Release weak reference ownership */
    inline void Release() { RefCount<T>::Decrement(m_refCount); m_refCount = nullptr; }

    /** @brief Get instance. If the instance has already been destroyed, return nullptr */
    inline InstanceType* Get() const { assert(m_refCount && m_refCount->m_ref); return m_refCount->m_ref; }

    /** @brief Instance reference operator. If the instance has already been destroyed, return nullptr */
    inline InstanceType* operator->() const { assert(m_refCount && m_refCount->m_ref); return m_refCount->m_ref; }

    /** @brief Dereference operator */
    InstanceType& operator*() const { assert(m_refCount && m_refCount->m_ref); return *m_refCount->m_ref; }

    /** @brief Comparison operator */
    bool operator==(const InstanceType* instance) const { return (m_refCount) ? m_refCount->m_ref == instance : instance == nullptr; }
    /** @brief Comparison operator */
    bool operator!=(const InstanceType* instance) const { return (m_refCount) ? m_refCount->m_ref != instance : instance != nullptr; }
    /** @brief Comparison operator */
    bool operator==(const CWeakPtr& other) const { return m_refCount == other.m_refCount; }
    /** @brief Comparison operator */
    bool operator!=(const CWeakPtr& other) const { return m_refCount != other.m_refCount; }

    /** @brief Bool check operator */
    explicit operator bool() const { return m_refCount != nullptr && m_refCount->m_ref != nullptr; }

private:
    /** @brief Reference count structure */
    RefCount<T>* m_refCount;
};

/** @brief Empty interface to let 'ACWeakPtrFromThis' do type checking without template */
class IWeakPtrFromThis {
public:
    virtual ~IWeakPtrFromThis() = default;
};

/**
   @brief Class inheriting from this class can create WeakPtrs from its instance
   @tparam T Instance type
*/
template<class T>
class ACWeakPtrFromThis : public IWeakPtrFromThis {
    // Friend declarations
    template<class U>
    friend class CUniquePtrWeakable;
    template<class U>
    friend class CSharedPtr;

public:
    /** @brief Destructor */
    virtual ~ACWeakPtrFromThis() = default;

    /** @brief Get weak pointer from this class */
    CWeakPtr<T> WeakFromThis() { return CWeakPtr<T>(m_refCount); }

private:
    /** @brief Set pointer to reference count of own class */
    void SetRefCount(void* refCount) { m_refCount = reinterpret_cast<RefCount<T>*>(refCount); }

private:
    /** @brief Reference count structure */
    RefCount<T>* m_refCount = nullptr;
};


// Increment the reference count
template<class T>
void RefCount<T>::Increment(RefCount* refCount) {
    if (refCount == nullptr) { return; }
    // Increment
    refCount->m_refCount++;
}

// Decrement the reference count. If the reference count reaches zero, destroy it
template<class T>
void RefCount<T>::Decrement(RefCount* refCount) {
    if (refCount == nullptr) { return; }
    // Decrement
    refCount->m_refCount--;

    // If the reference count reaches zero, destroy it
    if (refCount->m_refCount <= 0) {
        delete refCount;
    }
}

// Decrement the reference count. If the reference count reaches zero, destroy it
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<U, T>
RefCount<U>* RefCount<T>::Cast(RefCount<T>* refCount) {
    // Null check
    if (refCount == nullptr) { return nullptr; }
    
    if constexpr (Utl::Type::Relat::IsBaseOfRemoveExtentNotSame<T, U>) {
        std::remove_extent_t<U>* tmp = dynamic_cast<std::remove_extent_t<U>*>(refCount->m_ref);
        if (tmp != nullptr) {
            return reinterpret_cast<RefCount<U>*>(refCount);
        }
        else {
            return nullptr;
        }
    }
    else {
        return reinterpret_cast<RefCount<U>*>(refCount);
    }
}

// Copy constructor for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CWeakPtr<T>::CWeakPtr(const CWeakPtr<U>& copy)
    : m_refCount((&copy != nullptr) ? RefCount<U>::Cast<T>(copy.m_refCount) : nullptr) {
    RefCount<T>::Increment(m_refCount);
}

// Copy assignment operator
template<class T>
CWeakPtr<T>& CWeakPtr<T>::operator=(const CWeakPtr& copy) {
    // Release
    Release();
    // Copy reference count
    m_refCount = (&copy != nullptr) ? copy.m_refCount : nullptr;
    RefCount<T>::Increment(m_refCount);
    return *this;
}

// Move assignment operator
template<class T>
CWeakPtr<T>& CWeakPtr<T>::operator=(CWeakPtr&& other) noexcept {
    if (this != &other) {
        // Release
        Release();

        // Move reference count
        m_refCount = (&other != nullptr) ? other.m_refCount : nullptr;
        // If ownership has been successfully transferred, release the other's ownership
        if (m_refCount != nullptr) {
            other.m_refCount = nullptr;
        }
    }
    return *this;
}

/**
   @brief Class that wraps a smart pointer of array type. Internally has the size of an array 
   @tparam SmartPtr Smart pointer type
   @tparam T Instance type
*/
template<template<class> class SmartPtr, IsNotArrayType T>
class CArraySmartPtr {
public:
    /**
       @brief Constructor
    */
    CArraySmartPtr() : m_ptr(nullptr), m_size(0) {}

    // Copy constructor
    CArraySmartPtr(const CArraySmartPtr& other) requires Utl::Type::Traits::HasCopyConstructor<SmartPtr<T[]>>::value : m_ptr(other.m_ptr), m_size(other.m_size) {}
    // Copy assignment operator
    CArraySmartPtr& operator=(const CArraySmartPtr& other) requires Utl::Type::Traits::HasCopyOperator<SmartPtr<T[]>>::value { m_ptr = other.m_ptr; m_size = other.m_size; return *this; }

    /** @brief Move constructor */
    CArraySmartPtr(CArraySmartPtr&& other) noexcept : m_ptr(std::move(other.m_ptr)), m_size(other.m_size) { other.m_size = 0; }
    /** @brief Move assignment operator */
    CArraySmartPtr& operator=(CArraySmartPtr&& other) noexcept { if (this != &other) { m_ptr = std::move(other.m_ptr); m_size = other.m_size;  other.m_size = 0; } return *this; }

    /**
       @brief Resize an array
       @param size Size of an array
       @details
       If the instance already exists, it is released and rebuilt.
       If 0 is specified, the instance is released
    */
    void Resize(UINT size);

    /** @brief Get size of the array */
    inline const UINT Size() const { return m_size; }

    /** @brief Get instance */
    inline T* Get() { return m_ptr.Get(); }
    /** @brief Get instance */
    inline const T* Get() const { return m_ptr.Get(); }
    /** @brief Instance reference operator */
    inline T* operator->() { return m_ptr.operator->(); }
    /** @brief Instance reference operator */
    inline const T* operator->() const { return m_ptr.operator->(); }

    /** @brief Operator for array */
    inline T& operator[](UINT index) { return m_ptr[index]; }
    /** @brief Operator for array */
    inline const T& operator[](UINT index) const { return m_ptr[index]; }

    /** @brief Dereference operator */
    inline T& operator*() { return *m_ptr; }
    /** @brief Dereference operator */
    inline const T& operator*() const { return *m_ptr; }

    /** @brief Comparison operator */
    inline bool operator==(const CArraySmartPtr& other) const { return m_ptr == other.m_ptr; }
    /** @brief Comparison operator */
    inline bool operator!=(const CArraySmartPtr& other) const { return m_ptr != other.m_ptr; }

    /** @brief Bool check operator */
    inline explicit operator bool() const { return m_ptr.operator bool(); }

private:
    /** @brief Smart pointer */
    SmartPtr<T[]> m_ptr;
    /** @brief Size of an array */
    UINT m_size;
};

// Resize an array
template<template<class> class SmartPtr, IsNotArrayType T>
void CArraySmartPtr<SmartPtr, T>::Resize(UINT size) {
    // If the instance already exists, it is released
    if (m_ptr) {
        m_ptr.Reset();
    }

    // Create array instance
    if (size > 0) {
        m_ptr = SmartPtr<T[]>::Make((size_t)size);
        m_size = size;
    }
    else {
        m_size = 0;
    }
}

#endif // !__WEAK_PTR_H__
