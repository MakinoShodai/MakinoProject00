/**
 * @file   DynamicSbRegistry.h
 * @brief  This file handles the registry that manages all structured buffer allocators per object.
 * 
 * @author Shodai Makino
 * @date   2023/12/20
 */

#ifndef __DYNAMIC_SB_REGISTRY_H__
#define __DYNAMIC_SB_REGISTRY_H__

#include "DynamicSbAllocator.h"

 /** @brief This class handles all structured buffer allocator per object */
class CDynamicSbRegistry {
public:
    /**
       @brief Constructor
    */
    CDynamicSbRegistry();

    /**
       @brief Destructor
    */
    ~CDynamicSbRegistry() = default;

    /**
       @brief Get structured buffer class with matching name
       @param name Structured buffer name
       @return If a match is found, return its structured buffer. If no match, return nullptr
    */
    IDynamicSbAllocator* GetSbAllocator(const std::string& name) { auto it = m_nameToSBs.find(name); return (it != m_nameToSBs.end()) ? it->second : nullptr; }

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void FrameRefresh();

    /**
       @brief Release strutured buffers corresponding to the component sent
       @param component component that wants to release buffer
    */
    void ReleaseBuffer(ACGraphicsComponent* component);

private:
    /**
       @brief Emplace structrued buffer allocator
       @tparam T Structrued buffer allocator class inheriting from 'ICbAllocator'
    */
    template<typename T>
        requires std::is_base_of<IDynamicSbAllocator, T>::value
    void EmplaceSbAllocator();

private:
    /** @brief All structured buffer allocators */
    std::vector<CUniquePtr<IDynamicSbAllocator>> m_sbAllocators;
    /** @brief Map to search structrued buffer classes by structured buffer names */
    std::unordered_map<std::string, IDynamicSbAllocator*> m_nameToSBs;
};

// Emplace structrued buffer allocator
template<typename T>
    requires std::is_base_of<IDynamicSbAllocator, T>::value
void CDynamicSbRegistry::EmplaceSbAllocator() {
    m_sbAllocators.emplace_back(CUniquePtr<T>::Make());
    IDynamicSbAllocator* allocator = m_sbAllocators.back().Get();
    m_nameToSBs.emplace(allocator->GetName(), allocator);
}

#endif // !__DYNAMIC_SB_REGISTRY_H__
