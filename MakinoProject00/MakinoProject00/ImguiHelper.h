/**
 * @file   ImguiHelper.h
 * @brief  This file handles helper class for imgui.
 * 
 * @author Shodai Makino
 * @date   2024/1/25
 */

#ifndef __IMGUI_HELPER_H__
#define __IMGUI_HELPER_H__

#include "Singleton.h"
#include "DescriptorHeap.h"
#include "SharedPtr.h"

/**
   @brief Function for ImGui window
   @return Returns false if this function is erased, true otherwise
*/
using ImguiWindowFunction = std::function<bool()>;

/** @brief Helper class for imgui */
class CImguiHelper : public ACMainThreadSingleton<CImguiHelper> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CImguiHelper() = default;

    /**
       @brief Initialize
    */
    void Initialize();

    /**
       @brief Drawing process
    */
    void Draw();

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CImguiHelper> {
    public:
        // Friend declaration
        using ACInnerClass<CImguiHelper>::ACInnerClass;

        /**
           @brief Add window function
           @param function Sync shared pointer to function
           @details
           It is recommended to keep a sync shared pointer to the captured variable as an argument,
           as it may cause problems if the captured variable becomes invalid
        */
        void AddWindowFunction(CSyncSharedPtr<ImguiWindowFunction> function);

        /**
           @brief Add window function
           @param function Function object
           @details
           Windows set with this function can only be deleted by returning false itself
        */
        void AddWindowFunction(ImguiWindowFunction function);

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
    CImguiHelper() : ACMainThreadSingleton(1000) {}

    /**
       @brief Process to be called at instance destruction
    */
    void OnDestroy() override;

private:
    /** @brief Descriptor heap */
    CDescriptorHeap m_descriptorHeap;
    /** @brief Instances for imgui window */
    std::vector<CSyncSharedPtr<ImguiWindowFunction>> m_windowInstance;
    /** @brief Instances for imgui window to be created in the future */
    std::vector<CSyncSharedPtr<ImguiWindowFunction>> m_windowInstanceToCreate;
    /** @brief mutex to make variable 'm_windowInstanceToCreate' thread-safe */
    std::mutex m_mutexToCreate;
};

#endif // !__IMGUI_HELPER_H__
