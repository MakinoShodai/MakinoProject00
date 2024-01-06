/**
 * @file   DynamicCbRegistry.h
 * @brief  This file handles the registry that manages all constant buffer allocators per object.
 * 
 * @author Shodai Makino
 * @date   2023/11/18
 */

#ifndef __DYNAMIC_CB_REGISTORY_H__
#define __DYNAMIC_CB_REGISTORY_H__

#include "Singleton.h"
#include "DynamicCbAllocator.h"

/** @brief This class handles all constant buffer allocator per object */
class CDynamicCbRegistry : public ACMainThreadSingleton<CDynamicCbRegistry> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CDynamicCbRegistry() override {}

    /**
       @brief Initialize
    */
    void Initialize();

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void FrameRefresh();

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CDynamicCbRegistry> {
    public:
        // Friend declaration
        using ACInnerClass<CDynamicCbRegistry>::ACInnerClass;

        /**
           @brief Get constant buffer class with matching name
           @param name Constant buffer name
           @return If a match is found, return its constant buffer. If no match, return nullptr
        */
        IDynamicResource* GetCbAllocator(const std::string& name) { auto it = m_owner->m_nameToCBs.find(name); return (it != m_owner->m_nameToCBs.end()) ? it->second : nullptr; }

        /**
           @brief Get constant buffer class with matching class type
           @tparam T Class type of constant buffer to be gotten
           @return If a match is found, return its constant buffer. If no match, return nullptr
        */
        template<class T>
        T* GetCbAllocator() { auto it = m_owner->m_typeToCBs.find(std::type_index(typeid(T))); return (it != m_owner->m_typeToCBs.end()) ? dynamic_cast<T*>(it->second) : nullptr; }
    };

    /** @brief Get feature for thread-safe */
    inline static CThreadSafeFeature& GetAny() {
        static CThreadSafeFeature instance(&GetProtected());
        return instance;
    }

protected:
    /**
       @brief Constructor
    */
    CDynamicCbRegistry() : ACMainThreadSingleton(-5) {}

private:
    /**
       @brief Emplace constant buffer allocator
       @tparam T Constant buffer allocator class inheriting from 'ICbAllocator'
       @param maxUsePerFrame The maximum number of buffers that can be used in one frame
    */
    template<typename T>
    requires std::is_base_of<IDynamicResource, T>::value
    void EmplaceCbAllocator(UINT maxUsePerFrame);

private:
    /** @brief All constant buffer allocators */
    std::vector<CUniquePtr<IDynamicResource>> m_cbAllocators;
    /** @brief Map to search constant buffer classes by cbuffer names */
    std::unordered_map<std::string, IDynamicResource*> m_nameToCBs;
    /** @brief Map to search constant buffer classes by class type */
    std::unordered_map<std::type_index, IDynamicResource*> m_typeToCBs;
};

// Emplace constant buffer allocator
template<typename T>
requires std::is_base_of<IDynamicResource, T>::value
void CDynamicCbRegistry::EmplaceCbAllocator(UINT maxUsePerFrame) {
    m_cbAllocators.emplace_back(CUniquePtr<T>::Make(maxUsePerFrame));
    IDynamicResource* allocator = m_cbAllocators.back().Get();
    m_nameToCBs.emplace(allocator->GetName(), allocator);
    m_typeToCBs.emplace(std::type_index(typeid(T)), allocator);
}

#endif // !__DYNAMIC_CB_REGISTORY_H__
