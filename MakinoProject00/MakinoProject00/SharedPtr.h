/**
 * @file   SharedPtr.h
 * @brief  This file handles self-made shared pointer.
 * 
 * @author Shodai Makino
 * @date   2023/12/10
 */

#ifndef __SHARED_PTR_H__
#define __SHARED_PTR_H__

#include "WeakPtr.h"

/** @brief Self-made shared pointer class */
template<class T>
class CSharedPtr {
    /** @brief Correct instance type that is not array type */
    using InstanceType = std::remove_extent<T>::type;

    // Friend declaration
    template<class U>
    friend class CSharedPtr;

public:
    /**
       @brief Constructor
       @param instance Instance of passing ownership
    */
    CSharedPtr(InstanceType* instance = nullptr);

    /**
       @brief Move constructor
       @param other Move source
    */
    CSharedPtr(CSharedPtr&& other) noexcept;
    /**
       @brief Move constructor for cast
       @tparam Class in inheritance relationship with T

       @param other Move source
    */
    template<class U>
        requires IsConvertibleSmartPtr<T, U>
    CSharedPtr(CSharedPtr<U>&& other) noexcept;

    /**
       @brief Copy constructor
       @param other Copy source
    */
    CSharedPtr(const CSharedPtr& other);
    /**
       @brief Copy constructor for cast
       @tparam Class in inheritance relationship with T

       @param other Copy source
    */
    template<class U>
        requires IsConvertibleSmartPtr<T, U>
    CSharedPtr(const CSharedPtr<U>& other);

    /**
       @brief Destructor
    */
    ~CSharedPtr() { Reset(); }

    /** @brief Move assignment operator */
    CSharedPtr& operator=(CSharedPtr&& other) noexcept;
    /** @brief Move assignment operator for cast */
    template <class U>
        requires IsConvertibleSmartPtr<T, U>
    CSharedPtr& operator=(CSharedPtr<U>&& other) noexcept;

    /** @brief Copy assignment operator */
    CSharedPtr& operator=(const CSharedPtr& other);
    /** @brief Copy assignment operator for cast */
    template <class U>
        requires IsConvertibleSmartPtr<T, U>
    CSharedPtr& operator=(const CSharedPtr<U>& other);

    /**
       @brief Release a instance and set ownership of a new instance
       @param instance Instance of passing ownership
    */
    void Reset(InstanceType* instance = nullptr);

    /**
       @brief Make shared pointer
       @tparam Args Instance constructor arguments

       @param args Instance constructor arguments
    */
    template <typename... Args>
    inline static CSharedPtr<T> Make(Args&&... args) requires (!std::is_array_v<T>);

    /**
       @brief Make shared pointer for array
       @param size Size of instance array
    */
    inline static CSharedPtr<T> Make(size_t size) requires std::is_array_v<T>;

    /** @brief Get instance */
    inline InstanceType* Get() { return (m_strongRefCount) ? m_strongRefCount->m_ref : nullptr; }
    /** @brief Get instance */
    inline const InstanceType* Get() const { return (m_strongRefCount) ? m_strongRefCount->m_ref : nullptr; }
    /** @brief Get weak ptr */
    inline CWeakPtr<T> GetWeakPtr() { return CWeakPtr<T>(m_weakRefCount); }
    /** @brief Get weak ptr */
    inline const CWeakPtr<T> GetWeakPtr() const { return CWeakPtr<T>(m_weakRefCount); }

    /** @brief Instance reference operator */
    inline InstanceType* operator->() { assert(m_strongRefCount != nullptr); return m_strongRefCount->m_ref; }
    /** @brief Instance reference operator */
    inline const InstanceType* operator->() const { assert(m_strongRefCount != nullptr); return m_strongRefCount->m_ref; }

    /** @brief Operator for array */
    InstanceType& operator[](size_t index) requires std::is_array_v<T> { assert(m_strongRefCount != nullptr); return m_strongRefCount->m_ref[index]; }
    /** @brief Operator for array */
    const InstanceType& operator[](size_t index) const requires std::is_array_v<T> { assert(m_strongRefCount != nullptr); return m_strongRefCount->m_ref[index]; }

    /** @brief Dereference operator */
    InstanceType& operator*() { assert(m_strongRefCount != nullptr); return *m_strongRefCount->m_ref; }
    /** @brief Dereference operator */
    const InstanceType& operator*() const { assert(m_strongRefCount != nullptr); return *m_strongRefCount->m_ref; }

    /** @brief Comparison operator */
    bool operator==(InstanceType* other) const { return (m_strongRefCount) ? m_strongRefCount->m_ref == other : other == nullptr; }
    /** @brief Comparison operator */
    bool operator!=(InstanceType* other) const { return (m_strongRefCount) ? m_strongRefCount->m_ref != other : other != nullptr; }
    /** @brief Comparison operator */
    bool operator==(const CSharedPtr& other) const { return m_strongRefCount == other.m_strongRefCount; }
    /** @brief Comparison operator */
    bool operator!=(const CSharedPtr& other) const { return m_strongRefCount != other.m_strongRefCount; }

    /** @brief Bool check operator */
    explicit operator bool() const { return  m_strongRefCount != nullptr; }

private:
    /** @brief Strong reference count */
    RefCount<T>* m_strongRefCount;
    /** @brief Weak reference count */
    RefCount<T>* m_weakRefCount;
};

/** @brief Class that wraps a shared pointer of array type. Internally has the size of an array  */
template<class T>
using CArraySharedPtr = CArraySmartPtr<CSharedPtr, T>;

// Constructor
template<class T>
CSharedPtr<T>::CSharedPtr(InstanceType* instance)
    : m_strongRefCount(nullptr) 
    , m_weakRefCount(nullptr) {
    if (instance != nullptr) {
        m_strongRefCount = new RefCount<T>(instance);
        m_weakRefCount = new RefCount<T>(instance);

        // If the class creates a weak pointer with confidence, pass a reference count variable
        if constexpr (std::is_base_of<IWeakPtrFromThis, T>::value) {
            instance->SetRefCount(m_weakRefCount);
        }
    }
}

// Move constructor
template<class T>
CSharedPtr<T>::CSharedPtr(CSharedPtr&& other) noexcept
    : m_strongRefCount((&other != nullptr) ? other.m_strongRefCount : nullptr)
    , m_weakRefCount((&other != nullptr) ? other.m_weakRefCount : nullptr) {
    // If ownership has been successfully transferred, release the other's ownership
    if (m_strongRefCount != nullptr) {
        other.m_strongRefCount = nullptr;
        other.m_weakRefCount = nullptr;
    }
}

// Move constructor for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CSharedPtr<T>::CSharedPtr(CSharedPtr<U>&& other) noexcept
    : m_strongRefCount((&other != nullptr) ? RefCount<U>::Cast<T>(other.m_strongRefCount) : nullptr)
    , m_weakRefCount((&other != nullptr) ? RefCount<U>::Cast<T>(other.m_weakRefCount) : nullptr) {
    // If ownership has been successfully transferred, release the other's ownership
    if (m_strongRefCount != nullptr) {
        other.m_strongRefCount = nullptr;
        other.m_weakRefCount = nullptr;
    }
}

// Copy constructor
template<class T>
CSharedPtr<T>::CSharedPtr(const CSharedPtr& other)
    : m_strongRefCount((&other != nullptr) ? other.m_strongRefCount : nullptr)
    , m_weakRefCount((&other != nullptr) ? other.m_weakRefCount : nullptr) {
    RefCount<T>::Increment(m_strongRefCount);
    RefCount<T>::Increment(m_weakRefCount);
}

// Copy constructor for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CSharedPtr<T>::CSharedPtr(const CSharedPtr<U>& other)
    : m_strongRefCount((&other != nullptr) ? RefCount<U>::Cast<T>(other.m_strongRefCount) : nullptr)
    , m_weakRefCount((&other != nullptr) ? RefCount<U>::Cast<T>(other.m_weakRefCount) : nullptr) {
    RefCount<T>::Increment(m_strongRefCount);
    RefCount<T>::Increment(m_weakRefCount);
}

// Move assignment operator
template<class T>
CSharedPtr<T>& CSharedPtr<T>::operator=(CSharedPtr&& other) noexcept {
    if (this != &other) {
        Reset();
        m_strongRefCount = (&other != nullptr) ? other.m_strongRefCount : nullptr;
        m_weakRefCount = (&other != nullptr) ? other.m_weakRefCount : nullptr;
        // If ownership has been successfully transferred, release the other's ownership
        if (m_strongRefCount != nullptr) {
            other.m_strongRefCount = nullptr;
            other.m_weakRefCount = nullptr;
        }
    }
    return *this;
}

// Move assignment operator for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CSharedPtr<T>& CSharedPtr<T>::operator=(CSharedPtr<U>&& other) noexcept {
    // #NOTE : T and U are guaranteed not to be the same object because they have different types
    Reset();
    m_strongRefCount = (&other != nullptr) ? RefCount<U>::Cast<T>(other.m_strongRefCount) : nullptr;
    m_weakRefCount = (&other != nullptr) ? RefCount<U>::Cast<T>(other.m_weakRefCount) : nullptr;
    // If ownership has been successfully transferred, release the other's ownership
    if (m_strongRefCount != nullptr) {
        other.m_strongRefCount = nullptr;
        other.m_weakRefCount = nullptr;
    }
    
    return *this;
}

// Copy assignment operator
template<class T>
CSharedPtr<T>& CSharedPtr<T>::operator=(const CSharedPtr& other) {
    Reset();
    m_strongRefCount = (&other != nullptr) ? other.m_strongRefCount : nullptr;
    m_weakRefCount = (&other != nullptr) ? other.m_weakRefCount : nullptr;
    RefCount<T>::Increment(m_strongRefCount);
    RefCount<T>::Increment(m_weakRefCount);
    return *this;
}

// Copy assignment operator for cast
template<class T>
template<class U>
    requires IsConvertibleSmartPtr<T, U>
CSharedPtr<T>& CSharedPtr<T>::operator=(const CSharedPtr<U>& other) {
    Reset();
    m_strongRefCount = (&other != nullptr) ? RefCount<U>::Cast<T>(other.m_strongRefCount) : nullptr;
    m_weakRefCount = (&other != nullptr) ? RefCount<U>::Cast<T>(other.m_weakRefCount) : nullptr;
    RefCount<T>::Increment(m_strongRefCount);
    RefCount<T>::Increment(m_weakRefCount);
    return *this;
}

// Release a instance and set ownership of a new instance
template<class T>
void CSharedPtr<T>::Reset(InstanceType* instance) {
    if (m_strongRefCount != nullptr) {
        // If strong reference count changes to 0 now, Release instance and set nullptr for weak reference
        if (m_strongRefCount->m_refCount <= 1) {
            if constexpr (std::is_array_v<T>) {
                delete[] m_strongRefCount->m_ref;
            }
            else {
                delete m_strongRefCount->m_ref;
            }

            if (m_weakRefCount) {
                m_weakRefCount->m_ref = nullptr;
            }
        }

        // Decrement strong reference count
        RefCount<T>::Decrement(m_strongRefCount);
        m_strongRefCount = nullptr;

        // Decrement weak reference count
        if (m_weakRefCount) {
            RefCount<T>::Decrement(m_weakRefCount);
            m_weakRefCount = nullptr;
        }
    }

    // If it gains ownership of a new instance, it also recreates the reference count structure
    if (instance != nullptr) {
        m_strongRefCount = new RefCount<T>(instance);
        m_weakRefCount = new RefCount<T>(instance);

        // If the class creates a weak pointer with confidence, pass a reference count variable
        if constexpr (std::is_base_of<IWeakPtrFromThis, T>::value) {
            instance->SetRefCount(m_weakRefCount);
        }
    }
}

// Make shared pointer
template<class T>
template<typename ...Args>
inline CSharedPtr<T> CSharedPtr<T>::Make(Args && ...args) requires (!std::is_array_v<T>) {
    return CSharedPtr<T>(new T(std::forward<Args>(args)...));
}

// Make shared pointer for array
template<class T>
inline CSharedPtr<T> CSharedPtr<T>::Make(size_t size) requires std::is_array_v<T> {
    return CSharedPtr<T>(new CSharedPtr<T>::InstanceType[size]);
}

#endif // !__SHARED_PTR_H__
