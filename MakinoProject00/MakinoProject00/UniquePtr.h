/**
 * @file   UniquePtr.h
 * @brief  This file handles unique pointer that can generate weak reference pointer.
 * 
 * @author Shodai Makino
 * @date   2023/11/28
 */

#ifndef __UNIQUE_PTR_H__
#define __UNIQUE_PTR_H__

#include "WeakPtr.h"

/**
   @brief Unique pointer that can generate weak reference pointer
   @tparam T Instance type
*/
template <class T>
class CUniquePtrWeakable {
    /** @brief Correct instance type that is not array type */
    using InstanceType = std::remove_extent<T>::type;

    // Friend declaration
    template<class U>
    friend class CUniquePtrWeakable;

public:
    /** @brief Disallow the copy constructor */
    CUniquePtrWeakable(const CUniquePtrWeakable&) = delete;
    /** @brief Disallow the copy assignment operator */
    CUniquePtrWeakable& operator=(const CUniquePtrWeakable&) = delete;

    /**
       @brief Constructor
       @param instance Instance of passing ownership
    */
    CUniquePtrWeakable(InstanceType* instance = nullptr);

    /**
       @brief Move constructor
       @param other Move source
    */
    CUniquePtrWeakable(CUniquePtrWeakable&& other) noexcept;

    /**
       @brief Move constructor for cast
       @tparam Class in inheritance relationship with T

       @param other Move source
    */
    template<class U>
        requires IsConvertibleSmartPtr<T, U>
    CUniquePtrWeakable(CUniquePtrWeakable<U>&& other) noexcept;

    /**
       @brief Destructor
    */
    ~CUniquePtrWeakable() { Reset(); }

    /** @brief Move assignment operator */
    CUniquePtrWeakable& operator=(CUniquePtrWeakable&& other) noexcept;
    /** @brief Move assignment operator for cast */
    template <class U>
        requires IsConvertibleSmartPtr<T, U>
    CUniquePtrWeakable& operator=(CUniquePtrWeakable<U>&& other) noexcept;

    /**
       @brief Release a instance and set ownership of a new instance
       @param instance Instance of passing ownership
    */
    void Reset(InstanceType* instance = nullptr);

    /**
       @brief Make unique pointer
       @tparam Args Instance constructor arguments

       @param args Instance constructor arguments
    */
    template <typename... Args>
    inline static CUniquePtrWeakable<T> Make(Args&&... args) requires (!std::is_array_v<T>);

    /**
       @brief Make unique pointer for array
       @param size Size of instance array
    */
    inline static CUniquePtrWeakable<T> Make(size_t size) requires std::is_array_v<T>;

    /** @brief Get instance */
    inline InstanceType* Get() { return m_instance; }
    /** @brief Get instance */
    inline const InstanceType* Get() const { return m_instance; }
    /** @brief Get weak ptr */
    inline CWeakPtr<T> GetWeakPtr() { return CWeakPtr<T>(m_refCount); }
    /** @brief Get weak ptr */
    inline const CWeakPtr<T> GetWeakPtr() const { return CWeakPtr<T>(m_refCount); }

    /** @brief Instance reference operator */
    inline InstanceType* operator->() { return m_instance; }
    /** @brief Instance reference operator */
    inline const InstanceType* operator->() const { return m_instance; }

    /** @brief Operator for array */
    InstanceType& operator[](size_t index) requires std::is_array_v<T> { return m_instance[index]; }
    /** @brief Operator for array */
    const InstanceType& operator[](size_t index) const requires std::is_array_v<T> { return m_instance[index]; }

    /** @brief Dereference operator */
    InstanceType& operator*() { assert(m_instance != nullptr); return *m_instance; }
    /** @brief Dereference operator */
    const InstanceType& operator*() const { assert(m_instance != nullptr); return *m_instance; }

    /** @brief Comparison operator */
    bool operator==(const CUniquePtrWeakable& other) const { return m_instance == other.m_instance; }
    /** @brief Comparison operator */
    bool operator!=(const CUniquePtrWeakable& other) const { return m_instance != other.m_instance; }

    /** @brief Bool check operator */
    explicit operator bool() const { return  m_instance != nullptr; }

private:
    /** @brief Instance */
    InstanceType* m_instance;
    /** @brief Reference count structure */
    RefCount<T>* m_refCount;
};

/** @brief Class that wraps a unique pointer weakable of array type. Internally has the size of an array  */
template<class T>
using CArrayUniquePtrWeakable = CArraySmartPtr<CUniquePtrWeakable, T>;

// Constructor
template<class T>
CUniquePtrWeakable<T>::CUniquePtrWeakable(InstanceType* instance)
    : m_instance(instance)
    , m_refCount(nullptr) {
    if (m_instance != nullptr) {
        m_refCount = new RefCount<T>(m_instance);

        // If the class creates a weak pointer with confidence, pass a reference count variable
        if constexpr (std::is_base_of<IWeakPtrFromThis, T>::value) {
            m_instance->SetRefCount(m_refCount);
        }
    }
}

// Move constructor
template<class T>
CUniquePtrWeakable<T>::CUniquePtrWeakable(CUniquePtrWeakable&& other) noexcept
    : m_instance((&other != nullptr) ? other.m_instance : nullptr)
    , m_refCount((&other != nullptr) ? other.m_refCount : nullptr) {
    // If ownership has been successfully transferred, release the other's ownership
    if (m_instance != nullptr) {
        other.m_instance = nullptr;
        other.m_refCount = nullptr;
    }
}

// Move constructor for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CUniquePtrWeakable<T>::CUniquePtrWeakable(CUniquePtrWeakable<U>&& other) noexcept
    : m_instance((&other != nullptr) ? CastSmartPtrInstance<U, T>(other.m_instance) : nullptr)
    , m_refCount((&other != nullptr) ? RefCount<U>::Cast<T>(other.m_refCount) : nullptr){
    // If ownership has been successfully transferred, release the other's ownership
    if (m_instance != nullptr) {
        other.m_instance = nullptr;
        other.m_refCount = nullptr;
    }
}

// Move assignment operator
template<class T>
CUniquePtrWeakable<T>& CUniquePtrWeakable<T>::operator=(CUniquePtrWeakable&& other) noexcept {
    if (this != &other) {
        Reset();
        m_instance = (&other != nullptr) ? other.m_instance : nullptr;
        m_refCount = (&other != nullptr) ? other.m_refCount : nullptr;
        // If ownership has been successfully transferred, release the other's ownership
        if (m_instance != nullptr) {
            other.m_instance = nullptr;
            other.m_refCount = nullptr;
        }
    }
    return *this;
}

// Move assignment operator for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CUniquePtrWeakable<T>& CUniquePtrWeakable<T>::operator=(CUniquePtrWeakable<U>&& other) noexcept {
    // #NOTE : T and U are guaranteed not to be the same object because they have different types
    Reset();
    m_instance = (&other != nullptr) ? CastSmartPtrInstance<U, T>(other.m_instance) : nullptr;
    m_refCount = (&other != nullptr) ? RefCount<U>::Cast<T>(other.m_refCount) : nullptr;
    // If ownership has been successfully transferred, release the other's ownership
    if (m_instance != nullptr) {
        other.m_instance = nullptr;
        other.m_refCount = nullptr;
    }
    
    return *this;
}

// Release a instance and set ownership of a new instance
template<class T>
void CUniquePtrWeakable<T>::Reset(InstanceType* instance) {
    // Delete instance
    if (m_instance != nullptr) {
        if constexpr (std::is_array_v<T>) {
            delete[] m_instance;
        }
        else {
            delete m_instance;
        }

        // Decrement references to instances by replacing them with nullptr
        if (m_refCount) {
            m_refCount->m_ref = nullptr;
            RefCount<T>::Decrement(m_refCount);
            m_refCount = nullptr;
        }
    }

    // If it gains ownership of a new instance, it also recreates the reference count structure
    m_instance = instance;
    if (m_instance != nullptr) {
        m_refCount = new RefCount<T>(m_instance);

        // If the class creates a weak pointer with confidence, pass a reference count variable
        if constexpr (std::is_base_of<IWeakPtrFromThis, T>::value) {
            m_instance->SetRefCount(m_refCount);
        }
    }
}

// Make unique pointer
template<class T>
template<typename ...Args>
inline CUniquePtrWeakable<T> CUniquePtrWeakable<T>::Make(Args&&... args) requires (!std::is_array_v<T>) {
    return CUniquePtrWeakable<T>(new T(std::forward<Args>(args)...));
}

// Make unique pointer for array
template<class T>
inline CUniquePtrWeakable<T> CUniquePtrWeakable<T>::Make(size_t size) requires std::is_array_v<T> {
    return CUniquePtrWeakable<T>(new CUniquePtrWeakable<T>::InstanceType[size]);
}


/**
   @brief Unique pointer that can't generate weak reference pointer
   @tparam T Instance type
*/
template <class T>
class CUniquePtr {
    /** @brief Correct instance type that is not array type */
    using InstanceType = std::remove_extent<T>::type;

    // Friend declaration
    template<class U>
    friend class CUniquePtr;

public:
    /** @brief Disallow the copy constructor */
    CUniquePtr(const CUniquePtr&) = delete;
    /** @brief Disallow the copy assignment operator */
    CUniquePtr& operator=(const CUniquePtr&) = delete;

    /**
       @brief Constructor
       @param instance Instance of passing ownership
    */
    CUniquePtr(InstanceType* instance = nullptr) : m_instance(instance) {}

    /**
       @brief Move constructor
       @param other Move source
    */
    CUniquePtr(CUniquePtr&& other) noexcept;

    /**
       @brief Move constructor for cast
       @tparam Class in inheritance relationship with T

       @param other Move source
    */
    template<class U>
        requires IsConvertibleSmartPtr<T, U>
    CUniquePtr(CUniquePtr<U>&& other) noexcept;

    /**
       @brief Destructor
    */
    ~CUniquePtr() { Reset(); }

    /** @brief Move assignment operator */
    CUniquePtr& operator=(CUniquePtr&& other) noexcept;
    /** @brief Move assignment operator for cast */
    template <class U>
        requires IsConvertibleSmartPtr<T, U>
    CUniquePtr& operator=(CUniquePtr<U>&& other) noexcept;

    /**
       @brief Release a instance and set ownership of a new instance
       @param instance Instance of passing ownership
    */
    void Reset(InstanceType* instance = nullptr);

    /**
       @brief Make unique pointer
       @tparam Args Instance constructor arguments

       @param args Instance constructor arguments
    */
    template <typename... Args>
    inline static CUniquePtr<T> Make(Args&&... args) requires (!std::is_array_v<T>) { return CUniquePtr<T>(new T(std::forward<Args>(args)...)); }

    /**
       @brief Make unique pointer for array
       @param size Size of instance array
    */
    inline static CUniquePtr<T> Make(size_t size) requires std::is_array_v<T> { return CUniquePtr<T>(new CUniquePtr<T>::InstanceType[size]); }

    /** @brief Get instance */
    inline InstanceType* Get() { return m_instance; }
    /** @brief Get instance */
    inline const InstanceType* Get() const { return m_instance; }
    /** @brief Instance reference operator */
    inline InstanceType* operator->() { return m_instance; }
    /** @brief Instance reference operator */
    inline const InstanceType* operator->() const { return m_instance; }

    /** @brief Operator for array */
    InstanceType& operator[](size_t index) requires std::is_array_v<T> { return m_instance[index]; }
    /** @brief Operator for array */
    const InstanceType& operator[](size_t index) const requires std::is_array_v<T> { return m_instance[index]; }

    /** @brief Dereference operator */
    InstanceType& operator*() { assert(m_instance != nullptr); return *m_instance; }
    /** @brief Dereference operator */
    const InstanceType& operator*() const { assert(m_instance != nullptr); return *m_instance; }

    /** @brief Comparison operator */
    bool operator==(const CUniquePtr& other) const { return m_instance == other.m_instance; }
    /** @brief Comparison operator */
    bool operator!=(const CUniquePtr& other) const { return m_instance != other.m_instance; }

    /** @brief Bool check operator */
    explicit operator bool() const { return  m_instance != nullptr; }

private:
    /** @brief Instance */
    InstanceType* m_instance;
};

/** @brief Class that wraps a unique pointer of array type. Internally has the size of an array  */
template<class T>
using CArrayUniquePtr = CArraySmartPtr<CUniquePtr, T>;

// Move constructor
template<class T>
CUniquePtr<T>::CUniquePtr(CUniquePtr<T>&& other) noexcept
    : m_instance((&other != nullptr) ? other.m_instance : nullptr) {
    // If ownership has been successfully transferred, release the other's ownership
    if (m_instance != nullptr) {
        other.m_instance = nullptr;
    }
}

// Move constructor for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CUniquePtr<T>::CUniquePtr(CUniquePtr<U>&& other) noexcept
    : m_instance((&other != nullptr) ? CastSmartPtrInstance<U, T>(other.m_instance) : nullptr) {
    // If ownership has been successfully transferred, release the other's ownership
    if (m_instance != nullptr) {
        other.m_instance = nullptr;
    }
}

// Move assignment operator
template<class T>
CUniquePtr<T>& CUniquePtr<T>::operator=(CUniquePtr<T>&& other) noexcept {
    if (this != &other) {
        Reset();
        m_instance = (&other != nullptr) ? other.m_instance : nullptr;
        // If ownership has been successfully transferred, release the other's ownership
        if (m_instance != nullptr) {
            other.m_instance = nullptr;
        }
    }
    return *this;
}

// Move assignment operator for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CUniquePtr<T>& CUniquePtr<T>::operator=(CUniquePtr<U>&& other) noexcept {
    // #NOTE : T and U are guaranteed not to be the same object because they have different types
    Reset();
    m_instance = (&other != nullptr) ? CastSmartPtrInstance<U, T>(other.m_instance) : nullptr;
    // If ownership has been successfully transferred, release the other's ownership
    if (m_instance != nullptr) {
        other.m_instance = nullptr;
    }
    
    return *this;
}

// Release a instance and set ownership of a new instance
template<class T>
void CUniquePtr<T>::Reset(InstanceType* instance) {
    // Delete instance
    if (m_instance != nullptr) {
        if constexpr (std::is_array_v<T>) {
            delete[] m_instance;
        }
        else {
            delete m_instance;
        }
    }

    // Set new instance
    m_instance = instance;
}

#endif // !__UNIQUE_PTR_H__
