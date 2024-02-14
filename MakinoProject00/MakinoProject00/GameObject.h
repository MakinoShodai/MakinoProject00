/**
 * @file   GameObject.h
 * @brief  This file handles base class for game object.
 * 
 * @author Shodai Makino
 * @date   2023/12/1
 */

#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "UniquePtr.h"
#include "UtilityForMath.h"
#include "CallbackSystem.h"
#include "GraphicsLayer.h"
#include "Component.h"
#include "GraphicsComponent.h"
#include "ScenePhase.h"
#include "UpdateMode.h"

// Forward declaration
class CScene;

/** @brief Is this a child component of ACComponent? */
template <class T>
concept IsACComponentChild = std::is_base_of_v<ACComponent, T>;
/** @brief Is this a child component of ACGraphicsComponent? */
template <class T>
concept IsACGraphicsComponentChild = std::is_base_of_v<ACGraphicsComponent, T>;
/** @brief Is this a child component? */
template <class T>
concept IsComponentChild = IsACComponentChild<T> || IsACGraphicsComponentChild<T>;

/** @brief This is base class for game object */
class CGameObject : public ACWeakPtrFromThis<CGameObject> {
public:
    /**
       @brief Constructor
       @param ownerScene Scene that is the owner of this game object
       @param transform Transform of this game object
    */
    CGameObject(CScene* ownerScene, const Transformf& transform = Transformf());
    /**
       @brief Destructor
    */
    virtual ~CGameObject() = default;

    /**
       @brief Function for creating a prefab of a game object
    */
    virtual void Prefab() {}

    /**
       @brief Processing for the first update frame
    */
    void Start();

    /**
       @brief Update processing
    */
    void Update();

    /**
       @brief Pre-update process for physics simulation
    */
    void PrePhysicsUpdate();

    /**
       @brief Pre drawing processing
    */
    void PreDraw();

    /**
       @brief Process to be called at instance destruction
    */
    void OnDestroy();

    /**
       @brief Processing for collider collisions
    */
    void OnCollision();

    /** @brief Transfer current bit strings of the current scene phase to previous bit strings */
    inline void TransferTransformObserve() { m_transformBits.Transfer(); }

    /** @brief Get the transform */
    const Transformf& GetTransform() const { return m_transform; }
    /** @brief Set transform */
    void SetTransform(const Transformf& in) { m_transform = in; m_transformBits.SetBit(TransformObserve::All); }
    /** @brief Set position */
    void SetPos(const Vector3f& pos) { m_transform.pos = pos; m_transformBits.SetBit(TransformObserve::Pos); }
    /** @brief Set rotation */
    void SetRotation(const Quaternionf& rotation) { m_transform.rotation = rotation; m_transformBits.SetBit(TransformObserve::Rotate); }
    /** @brief Set scale */
    void SetScale(const Vector3f& scale) { m_transform.scale = scale; m_transformBits.SetBit(TransformObserve::Scale); }

    /**
       @brief Check bit strings to observe transform in the specified phase
       @param phase Scene phase to be checked
       @param observe Bit for comparison
       @return Result
    */
    inline bool CheckTransformObserve(ScenePhase phase, TransformObserve observe) { return m_transformBits.Check(phase, observe); }

    /**
       @brief Get a component
       @return Weak pointer to the specified component
    */
    template<IsComponentChild T>
    CWeakPtr<T> GetComponent();
    /**
       @brief Get a graphics component
       @return Weak pointer to the specified graphics component
    */
    template<IsACGraphicsComponentChild T>
    CWeakPtr<T> GetComponent();

    /**
       @brief Get type matched components
       @return Dynamic array of weak pointer to the specified component
    */
    template<IsComponentChild T>
    std::vector<CWeakPtr<T>> GetComponents();
    /**
       @brief Get type matched graphics components
       @return Dynamic array of weak pointer to the specified graphics component
    */
    template<IsACGraphicsComponentChild T>
    std::vector<CWeakPtr<T>> GetComponents();

    /**
       @brief Add a component to the array of the components
       @param args Constructor arguments of the component except own object
       @return Weak pointer to the added component
    */
    template<IsACComponentChild T, typename... Args>
    CWeakPtr<T> AddComponent(Args&&... args);
    /**
       @brief Add a graphics component to the array of the graphics components
       @param args Constructor arguments of the graphics component except own object
       @return Weak pointer to the added graphics component
    */
    template<IsACGraphicsComponentChild T, typename... Args>
    CWeakPtr<T> AddComponent(GraphicsLayer layer, Args&&... args);

    /** @brief Overwrite update mode for the scene that this game object is allowed to update */
    void OverwriteUpdateMode(UpdateMode updateMode) { m_updateMode = updateMode; }
    /** @brief Logical conjunction with own update mode bits */
    inline bool CheckUpdateMode(UpdateMode mode) { return Utl::CheckEnumBit(mode & m_updateMode); }

    /** @brief Get weak pointer to the scene that is the owner of this game object */
    CWeakPtr<CScene> GetScene() { return m_scene; }
    /** @brief Get weak pointer to the scene that is the owner of this game object */
    CWeakPtr<const CScene> GetScene() const { return m_scene; }
    
    /** @brief Set name of this game object */
    void SetName(std::wstring name) { m_name = std::move(name); }
    /** @brief Get the name of this game object */
    const std::wstring& GetName() const { return m_name; }

    /** @brief Set the active flag */
    void SetIsActive(bool isActive);
    /** @brief Is this game object active? */
    bool IsActive() const { return m_isActive; }

    /** @brief Get the callback system */
    CCallbackSystem* GetCallbackSystem();

private:
    /**
       @brief Processing when a collider is added
       @param collider Added collider
    */
    void ColliderAdded(const CWeakPtr<ACCollider3D>& collider);

private:
    /** @brief Scene that is the owner of this game object */
    CWeakPtr<CScene> m_scene;
    /** @brief Transform that has position and scale, rotation */
    Transformf m_transform;
    /** @brief Bit string structure for observing whether the transform was changed in the previous process */
    TransformObserveBits m_transformBits;
    /** @brief Name of this game object */
    std::wstring m_name;
    /** @brief Is this game object active? */
    bool m_isActive;
    /** @brief Update mode for the scene that this game object is allowed to update */
    UpdateMode m_updateMode;

    /** @brief Dynamic array of components of this game object */
    std::vector<CUniquePtrWeakable<ACComponent>> m_components;
    /** @brief Dynamic array of components for graphics of this game object */
    std::vector<CUniquePtrWeakable<ACGraphicsComponent>> m_graphicsComponents;
    /** @brief Object's callback function system. Instantiate when used */
    CUniquePtrWeakable<CCallbackSystem> m_callbackSystem;

    /** @brief Dynamic array of colliders in the array of components */
    std::vector<const ACCollider3D*> m_colliders;
};

/** @brief Class for registering game objects to map */
class ACRegistrarForGameObject {
public:
    /** @brief Type of map associating class names with the function that created it */
    using ClassMap = std::map<std::string, std::function<CGameObject* (CScene*, const Transformf&)>>;

    /** @brief Constructor */
    ACRegistrarForGameObject() = default;
    /** @brief Destructor */
    virtual ~ACRegistrarForGameObject() = default;

    /**
       @brief Create game object
       @param className Name of class to be created
       @param ownerScene Scene that is the owner of this game object
       @param transform Transform of this game object
       @return Returns a pointer to the class if it is successfully created, nullptr if not.
    */
    static CGameObject* CreateGameObject(const std::string& className, CScene* scene, const Transformf& transform);

    /** @brief Get map associating class names with the function that created it */
    static const ClassMap& GetClassMap() { return GetClassMapProtected(); }

protected:
    /** @brief Get map associating class names with the function that created it */
    static ClassMap& GetClassMapProtected() { static ClassMap classMap; return classMap; }

};

/** @brief Is this a child class of CGameObject? */
template <class T>
concept IsCGameObjectChild = std::is_base_of_v<CGameObject, T>;

/**
   @brief Class for registering game object prefab to map
   @details This class is called from a macro. Do not make the usual declarations
*/
template<IsCGameObjectChild T>
class CRegistrarGameObjectPrefab : public ACRegistrarForGameObject {
public:
    /** @brief Constructor */
    CRegistrarGameObjectPrefab(std::string className) {
        ACRegistrarForGameObject::GetClassMapProtected().emplace(className, [](CScene* scene, const Transformf& transform)->CGameObject* { return new T(scene, transform); });
    }

    /** @brief Destructor */
    ~CRegistrarGameObjectPrefab() override = default;
};

/** @brief Macro to register a Prefab and publish it to the hierarchy */
#define REGISTER_PREFABCLASS(CLASS_NAME) static CRegistrarGameObjectPrefab<CLASS_NAME> registrar_##CLASS_NAME(#CLASS_NAME); 

// Get a component
template<IsComponentChild T>
CWeakPtr<T> CGameObject::GetComponent() {
    for (auto& it : m_components) {
        auto castIt = dynamic_cast<T*>(it.Get());
        if (castIt != nullptr) {
            return it.GetWeakPtr();
        }
    }

    return nullptr;
}

// Get a graphics component
template<IsACGraphicsComponentChild T>
CWeakPtr<T> CGameObject::GetComponent() {
    for (auto& it : m_graphicsComponents) {
        auto castIt = dynamic_cast<T*>(it.Get());
        if (castIt != nullptr) {
            return it.GetWeakPtr();
        }
    }

    return nullptr;
}

// Get Type matched components
template<IsComponentChild T>
std::vector<CWeakPtr<T>> CGameObject::GetComponents() {
    std::vector<CWeakPtr<T>> ret;

    for (auto& it : m_components) {
        auto castIt = dynamic_cast<T*>(it.Get());
        if (castIt != nullptr) {
            ret.push_back(it.GetWeakPtr());
        }
    }

    return ret;
}

// Get Type matched graphics components
template<IsACGraphicsComponentChild T>
std::vector<CWeakPtr<T>> CGameObject::GetComponents() {
    std::vector<CWeakPtr<T>> ret;

    for (auto& it : m_graphicsComponents) {
        auto castIt = dynamic_cast<T*>(it.Get());
        if (castIt != nullptr) {
            ret.push_back(it.GetWeakPtr());
        }
    }

    return ret;
}

// Add a component to the array of the components
template<IsACComponentChild T, typename ...Args>
CWeakPtr<T> CGameObject::AddComponent(Args&&... args) {
    // Add component to array
    m_components.emplace_back(CUniquePtrWeakable<T>::Make(this, std::forward<Args>(args)...));

    // Call awake processing
    CWeakPtr<T> addedComponent = m_components.back().GetWeakPtr();
    addedComponent->Awake();
    addedComponent->OnEnable();

    // If T is derived class of ACCollider3D, Add it to the array of colliders
    if constexpr (std::is_base_of_v<ACCollider3D, T>) {
        ColliderAdded(addedComponent);
    }

    // Return a weak pointer to the added component
    return addedComponent;
}

// Add a graphics component to the array of the graphics components
template<IsACGraphicsComponentChild T, typename ...Args>
CWeakPtr<T> CGameObject::AddComponent(GraphicsLayer layer, Args && ...args) {
    // Add component to array
    m_graphicsComponents.emplace_back(CUniquePtrWeakable<T>::Make(this, layer, std::forward<Args>(args)...));

    CWeakPtr<T> addedComponent = m_graphicsComponents.back().GetWeakPtr();
    addedComponent->Awake();

    // Return a weak pointer to the added component
    return addedComponent;
}

#endif // !__GAME_OBJECT_H__
