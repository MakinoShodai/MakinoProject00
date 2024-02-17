/**
 * @file   RenderPassAsset.h
 * @brief  This file handles abstract class for rendering pass asset that scene uses.
 * 
 * @author Shodai Makino
 * @date   2024/1/26
 */

#ifndef __RENDER_PASS_ASSET_H__
#define __RENDER_PASS_ASSET_H__

#include "WeakPtr.h"

// Forward declaration
class CScene;

/** @brief Abstract class for rendering pass asset that scene uses */
class ACRenderPassAsset {
public:
    /**
       @brief Constructor
    */
    ACRenderPassAsset() = default;

    /**
       @brief Detructor
    */
    virtual ~ACRenderPassAsset() = default;

    /**
       @brief Call 'Start' function
       @param scene Weak pointer to scene that has this class
    */
    void CallStart(CWeakPtr<CScene> scene);

    /**
       @brief Drawing process
    */
    virtual void Draw() = 0;

protected:
    /**
       @brief Processing when switching to this scene
       @details
       This function is called outside the main thread
    */
    virtual void Start() = 0;
    
protected:
    /** @brief Weak pointer to scene that has this class */
    CWeakPtr<CScene> m_scene;
};


/** @brief Class for registering scenes to map */
class ACRegistrarForRenderPassAsset {
public:
    /** @brief Type of map associating class names with the function that created it */
    using ClassMap = std::map<std::string, std::function<ACRenderPassAsset* ()>>;

    /** @brief Constructor */
    ACRegistrarForRenderPassAsset() = default;
    /** @brief Destructor */
    virtual ~ACRegistrarForRenderPassAsset() = default;

    /**
       @brief Create render pass asset
       @param className Name of class to be created
       @return Returns a pointer to the class if it is successfully created, nullptr if not
    */
    static ACRenderPassAsset* CreateRenderPassAsset(const std::string& className);

    /** @brief Get map associating class names with the function that created it */
    static const ClassMap& GetClassMap() { return GetClassMapProtected(); }

protected:
    /** @brief Get map associating class names with the function that created it */
    static ClassMap& GetClassMapProtected() { static ClassMap classMap; return classMap; }

};

/** @brief Is this a child of ACRenderPassAsset? */
template <class T>
concept IsACRenderPassAssetChild = std::is_base_of_v<ACRenderPassAsset, T>;

/**
   @brief Class for registering scene asset to map
   @details This class is called from a macro. Do not make the usual declarations
*/
template<IsACRenderPassAssetChild T>
class CRegistrarRenderPassAsset : public ACRegistrarForRenderPassAsset {
public:
    /** @brief Constructor */
    CRegistrarRenderPassAsset(std::string className) {
        ACRegistrarForRenderPassAsset::GetClassMapProtected().emplace(className, []()->ACRenderPassAsset* { return new T(); });
    }

    /** @brief Destructor */
    ~CRegistrarRenderPassAsset() override = default;
};

/** @brief Macro to register an asset and publish it to the hierarchy */
#define REGISTER_RENDERPASS_CLASS(CLASS_NAME) static CRegistrarRenderPassAsset<CLASS_NAME> registrar_##CLASS_NAME(#CLASS_NAME); 


#endif // !__RENDER_PASS_ASSET_H__
