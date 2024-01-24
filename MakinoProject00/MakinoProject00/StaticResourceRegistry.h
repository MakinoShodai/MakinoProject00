/**
 * @file   StaticResourceRegistry.h
 * @brief  This file handles the registry that manages all static resources.
 * 
 * @author Shodai Makino
 * @date   2023/11/22
 */

#ifndef __STATIC_RESOURCE_REGISTRY_H__
#define __STATIC_RESOURCE_REGISTRY_H__

#include "Singleton.h"
#include "StaticResource.h"

// Forward declaration
class ACScene;

/** @brief This class handles all static resources */
class CStaticResourceRegistry : public ACMainThreadSingleton<CStaticResourceRegistry> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CStaticResourceRegistry() override = default;

    /**
       @brief Initialize
    */
    void Initialize();

    /**
       @brief Processing when a new scene is created

    */
    void SceneStart(ACScene* scene);

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void FrameRefresh();

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CStaticResourceRegistry> {
    public:
        // Friend declaration
        using ACInnerClass<CStaticResourceRegistry>::ACInnerClass;

        /**
           @brief Get static resource class with matching name
           @param name Resource name
           @return If a match is found, return its static resource. If no match, return nullptr
        */
        IStaticResource* GetStaticResource(const std::string& name) { auto it = m_owner->m_nameToResouces.find(name); return (it != m_owner->m_nameToResouces.end()) ? it->second : nullptr; }

        /**
           @brief Get static resource allocator class with matching class type
           @tparam T Class type of static resource allocator to be gotten
           @return If a match is found, return its static resource allocator. If no match, return nullptr
        */
        template<class T>
        T* GetStaticResource() { auto it = m_owner->m_typeToCBs.find(std::type_index(typeid(T))); return (it != m_owner->m_typeToCBs.end()) ? dynamic_cast<T*>(it->second) : nullptr; }
    };

    /** @brief Get feature for thread-safe */
    inline static CThreadSafeFeature& GetAny() {
        static CThreadSafeFeature instance(GetProtected().Get());
        return instance;
    }

protected:
    /**
       @brief Constructor
    */
    CStaticResourceRegistry() : ACMainThreadSingleton(-100) {}

private:
    /**
       @brief Emplace static resource class
       @tparam T Static resource class inheriting from 'IStaticResource'
    */
    template<typename T>
        requires std::is_base_of<IStaticResource, T>::value
    void EmplaceStaticAllocator();

private:
    /** @brief All static resources */
    std::vector<CUniquePtr<IStaticResource>> m_staticResources;
    /** @brief Map to search static resource interfaces by resource names */
    std::unordered_map<std::string, IStaticResource*> m_nameToResouces;
    /** @brief Map to search static resource allocator classes by class type */
    std::unordered_map<std::type_index, IStaticResource*> m_typeToCBs;
};

// Emplace static resource class
template<typename T>
    requires std::is_base_of<IStaticResource, T>::value
void CStaticResourceRegistry::EmplaceStaticAllocator() {
    m_staticResources.emplace_back(CUniquePtr<T>::Make());
    IStaticResource* staticResource = m_staticResources.back().Get();
    m_nameToResouces.emplace(staticResource->GetName(), staticResource);
    m_typeToCBs.emplace(std::type_index(typeid(T)), staticResource);
}

#endif // !__STATIC_RESOURCE_REGISTRY_H__
