﻿/**
 * @file   StableHandleVector.h
 * @brief  This file handles dynamic array capable of generating stable handles.
 * 
 * @author Shodai Makino
 * @date   2023/07/22
 */

#ifndef __STABLE_HANDLE_VECTOR_H__
#define __STABLE_HANDLE_VECTOR_H__

#include "UniquePtr.h"

// Forward declaration
template <typename T>
class StableHandleVector;

/** @brief Readonly handle for stable dynamic array */
template <typename T>
struct ReadOnlyStableHandle {
protected:
    /** @brief Index of an element */
    std::size_t index;

    /**
       @brief Constructor
       @param index Index of an element
    */
    ReadOnlyStableHandle(std::size_t index) : index(index) {}

    /** @brief Comparison operator */
    bool operator==(const ReadOnlyStableHandle& other) const { return this->index == other.index; }

    // Friend declaration
    template <typename T>
    friend class StableHandleVector;
};

/** @brief Handle for stable dynamic array */
template <typename T>
struct StableHandle : public ReadOnlyStableHandle<T> {
private:
    // Use the same constructor as the abstract class
    using ReadOnlyStableHandle<T>::ReadOnlyStableHandle;

    /** @brief Comparison operator */
    bool operator==(const StableHandle& other) const { return this->index == other.index; }

    // Friend declaration
    template <typename T>
    friend class StableHandleVector;
};

/** @brief Stable handle within a weak pointer */
template <typename T>
using WeakStableHandle = CWeakPtr<StableHandle<T>>;
/** @brief Read only stable handle within a weak pointer */
template <typename T>
using WeakROStableHandle = CWeakPtr<ReadOnlyStableHandle<T>>;

/** @brief Dynamic array capable of generating stable handles */
template <typename T>
class StableHandleVector {
public:
    /**
       @brief Constructor
    */
    StableHandleVector() : m_vector(), m_handles() {}

    /**
       @brief Add an element to the end of the dynamic array (copy)
       @param in Element to be added
       @return Pointer to the handle for the element to be added
    */
    WeakStableHandle<T> PushBack(const T& in);

    /**
       @brief Add an element to the end of the dynamic array (move)
       @param in Element to be added
       @return Pointer to the handle for the element to be added
    */
    WeakStableHandle<T> PushBack(T&& in);

    /**
       @brief Build an element directly to the end of the dynamic array
       @tparam Args Instance constructor arguments

       @param args Instance constructor arguments
       @return Pointer to the handle for the element to be builded
    */
    template <typename... Args>
    WeakStableHandle<T> EmplaceBack(Args&&... args);

    /**
       @brief Get handle for the most backward element
       @return Handle for the most backward element
    */
    WeakStableHandle<T> BackHandle() { assert(m_handles.size() > 0); return m_handles.back().GetWeakPtr(); }

    /**
       @brief Erase an element from the array
       @param handle Handle for the element to be erased
       @attention Be sure to nullptr the pointer that held this handle after the function call
    */
    void Erase(StableHandle<T>& handle);

    /**
       @brief Clear the dynamic array of elements
    */
    void Clear() { m_vector.clear(); m_handles.clear(); }

    /** @brief Is this vector empty? */
    bool Empty() { return m_vector.size() <= 0; }

    /** @brief Accessing instance operator */
    T& operator[](const ReadOnlyStableHandle<T>& handle) { return m_vector[handle.index]; }
    /** @brief Accessing instance operator */
    const T& operator[](const ReadOnlyStableHandle<T>& handle) const { return m_vector[handle.index]; }
    
    /** @brief Get the iterator that points to the first element */
    auto Begin() { return m_vector.begin(); }
    /** @brief Get the iterator that points to the next to last element */
    auto End() { return m_vector.end(); }

private:
    /** @brief Dynamic array of elements */
    std::vector<T> m_vector;
    /** @brief Dynamic array of handles for elements */
    std::vector<CUniquePtrWeakable<StableHandle<T>>> m_handles;
};

namespace std {
    /** @brief Overload of std::begin for StableHandleVector */
    template<class T>
    auto begin(StableHandleVector<T>& stableVector) { return stableVector.Begin(); }

    /** @brief Overload of std::end for StableHandleVector */
    template<class T>
    auto end(StableHandleVector<T>& stableVector) { return stableVector.End(); }
} // namespace std

// Add an element to the end of the dynamic array (copy)
template<typename T>
WeakStableHandle<T> StableHandleVector<T>::PushBack(const T& in) {
    // Add element
    m_vector.push_back(in);

    // Create the instance of the handle for new element
    m_handles.push_back(CUniquePtrWeakable<StableHandle<T>>(new StableHandle<T>(m_vector.size() - 1)));

    // Return pointer to handle
    return m_handles.back().GetWeakPtr();
}

// Add an element to the end of the dynamic array (move)
template<typename T>
WeakStableHandle<T> StableHandleVector<T>::PushBack(T&& in) {
    // Add element
    m_vector.push_back(std::move(in));

    // Create the instance of the handle for new element
    m_handles.push_back(CUniquePtrWeakable<StableHandle<T>>(new StableHandle<T>(m_vector.size() - 1)));

    // Return pointer to handle
    return m_handles.back().GetWeakPtr();
}

// Build an element directly to the end of the dynamic array
template<typename T>
template<typename ...Args>
WeakStableHandle<T> StableHandleVector<T>::EmplaceBack(Args&& ...args) {
    // Build an element
    m_vector.emplace_back(std::forward<Args>(args)...);

    // Create the instance of the handle for new element
    m_handles.push_back(CUniquePtrWeakable<StableHandle<T>>(new StableHandle<T>(m_vector.size() - 1)));

    // Return pointer to handle
    return m_handles.back().GetWeakPtr();
}

// Erase an element from the array
template<typename T>
void StableHandleVector<T>::Erase(StableHandle<T>& handle) {
    // Get an index of the handle for the element to be erased
    std::size_t index = handle.index;

    // Swap the element and the handle with end of the dynamic array
    std::swap(m_vector[index], m_vector.back());
    std::swap(m_handles[index], m_handles.back());

    // Update the index of the handle that was the trailing element
    m_handles[index]->index = index;

    // Erase the element and the handle
    m_vector.pop_back();
    m_handles.pop_back();
}

#endif // !__STABLE_HANDLE_VECTOR_H__
