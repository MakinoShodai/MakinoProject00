/**
 * @file   UtilityForDirectX.h
 * @brief  This file defines utility functions that are useful for DirectX.
 * 
 * @author Shodai Makino
 * @date   2023/11/13
 */

#ifndef __UTILITY_FOR_DIRECTX_H__
#define __UTILITY_FOR_DIRECTX_H__

#include "UniquePtr.h"

namespace Utl {
    namespace Dx {
        // Forward declaration
        class SRVPropertyHandle;

        /** @brief D3D12_HEAP_PROPERTIES interface */
        struct HEAP_PROPERTIES : public D3D12_HEAP_PROPERTIES {
            /**
               @brief Create simple heap properties
               @param type Heap type
            */
            HEAP_PROPERTIES(D3D12_HEAP_TYPE type);

            operator const D3D12_HEAP_PROPERTIES& () const noexcept { return *this; }
        };

        /** @brief D3D12_RESOURCE_DESC interface */
        struct RESOURCE_DESC : public D3D12_RESOURCE_DESC {
            /**
               @brief Create resource descriptor for simple buffer 
               @param size Buffer size
            */
            RESOURCE_DESC(UINT64 size);

            operator const D3D12_RESOURCE_DESC& () const noexcept { return *this; }
        };

        /** @brief D3D12_RESOURCE_BARRIER interface */
        struct RESOURCE_BARRIER : public D3D12_RESOURCE_BARRIER {
            /**
               @brief Create transition type resource barrier
               @param res Resource to be transitioned
               @param before State before transition
               @param after State after transition
            */
            RESOURCE_BARRIER(ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

            /**
               @brief Swap state before transition and state after transition
            */
            void StateSwap();

            operator const D3D12_RESOURCE_BARRIER& () const noexcept { return *this; }
        };

        /** @brief Shader type */
        enum class ShaderType {
            /** @brief Vertex shader */
            Vertex,
            /** @brief Pixel shader */
            Pixel,
        };

        /** @brief D3D12_CPU_DESCRIPTOR_HANDLE interface */
        struct CPU_DESCRIPTOR_HANDLE : public D3D12_CPU_DESCRIPTOR_HANDLE {
            /** @brief Constructor */
            CPU_DESCRIPTOR_HANDLE(const D3D12_CPU_DESCRIPTOR_HANDLE& handle) { ptr = handle.ptr; }
            /** @brief Constructor */
            CPU_DESCRIPTOR_HANDLE(SIZE_T s = NULL) { ptr = s; }
            /** @brief Assignment operator */
            CPU_DESCRIPTOR_HANDLE& operator=(const D3D12_CPU_DESCRIPTOR_HANDLE& handle) { ptr = handle.ptr; return *this; }
            /** @brief Assignment operator */
            CPU_DESCRIPTOR_HANDLE& operator=(SIZE_T s) { ptr = s; return *this; }

            /** @brief Comparison operator */
            bool operator==(SIZE_T s) const { return ptr == s; }
            /** @brief Comparison operator */
            bool operator!=(SIZE_T s) const { return ptr != s; }

            /** @brief Add assignment operator */
            CPU_DESCRIPTOR_HANDLE& operator+=(SIZE_T s) { ptr += s; return *this; }
            /** @brief Subtract assignment operator */
            CPU_DESCRIPTOR_HANDLE& operator-=(SIZE_T s) { ptr -= s; return *this; }
            /** @brief Multiply assignment operator */
            CPU_DESCRIPTOR_HANDLE& operator*=(SIZE_T s) { ptr *= s; return *this; }
            /** @brief Divison assignment operator */
            CPU_DESCRIPTOR_HANDLE& operator/=(SIZE_T s) { ptr /= s; return *this; }

            /** @brief Add operator */
            CPU_DESCRIPTOR_HANDLE operator+(SIZE_T s) const { return CPU_DESCRIPTOR_HANDLE(ptr + s); }
            /** @brief Subtract operator */
            CPU_DESCRIPTOR_HANDLE operator-(SIZE_T s) const { return CPU_DESCRIPTOR_HANDLE(ptr - s); }
            /** @brief Multiply operator */
            CPU_DESCRIPTOR_HANDLE operator*(SIZE_T s) const { return CPU_DESCRIPTOR_HANDLE(ptr * s); }
            /** @brief Divison operator */
            CPU_DESCRIPTOR_HANDLE operator/(SIZE_T s) const { return CPU_DESCRIPTOR_HANDLE(ptr / s); }

            operator const D3D12_CPU_DESCRIPTOR_HANDLE& () const noexcept { return *this; }
        };

        /** @brief D3D12_GPU_DESCRIPTOR_HANDLE interface */
        struct GPU_DESCRIPTOR_HANDLE : public D3D12_GPU_DESCRIPTOR_HANDLE {
            /** @brief Constructor */
            GPU_DESCRIPTOR_HANDLE(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { ptr = handle.ptr; }
            /** @brief Constructor */
            GPU_DESCRIPTOR_HANDLE(UINT64 u) { ptr = u; }
            /** @brief Assignment operator */
            GPU_DESCRIPTOR_HANDLE& operator=(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { ptr = handle.ptr; return *this; }
            /** @brief Assignment operator */
            GPU_DESCRIPTOR_HANDLE& operator=(UINT64 u) { ptr = u; return *this; }

            /** @brief Add assignment operator */
            GPU_DESCRIPTOR_HANDLE& operator+=(UINT64 u) { ptr += u; return *this; }
            /** @brief Subtract assignment operator */
            GPU_DESCRIPTOR_HANDLE& operator-=(UINT64 u) { ptr -= u; return *this; }
            /** @brief Multiply assignment operator */
            GPU_DESCRIPTOR_HANDLE& operator*=(UINT64 u) { ptr *= u; return *this; }
            /** @brief Divison assignment operator */
            GPU_DESCRIPTOR_HANDLE& operator/=(UINT64 u) { ptr /= u; return *this; }

            /** @brief Add operator */
            GPU_DESCRIPTOR_HANDLE operator+(UINT64 u) const { return GPU_DESCRIPTOR_HANDLE(ptr + u); }
            /** @brief Subtract operator */
            GPU_DESCRIPTOR_HANDLE operator-(UINT64 u) const { return GPU_DESCRIPTOR_HANDLE(ptr - u); }
            /** @brief Multiply operator */
            GPU_DESCRIPTOR_HANDLE operator*(UINT64 u) const { return GPU_DESCRIPTOR_HANDLE(ptr * u); }
            /** @brief Divison operator */
            GPU_DESCRIPTOR_HANDLE operator/(UINT64 u) const { return GPU_DESCRIPTOR_HANDLE(ptr / u); }

            operator const D3D12_GPU_DESCRIPTOR_HANDLE& () const noexcept { return *this; }
        };

        /** @brief Resource state controller */
        class ResStateController : public ACWeakPtrFromThis<ResStateController> {
        public:
            /**
               @brief Transition resource state
               @param state Post transition state
            */
            void StateTransition(D3D12_RESOURCE_STATES state);

            /**
               @brief Determine the exact current resource state on the GPU
            */
            void ApplyCurrentStateOnGpu();

            /**
               @brief Modify the current resource state to the resource state on the gpu
            */
            void ModifyToCurrentStateOnGpu();

        protected:
            /**
               @brief Constructor
            */
            ResStateController() : m_resource(nullptr), m_currentState(D3D12_RESOURCE_STATE_COMMON), m_currentStateOnGpu(D3D12_RESOURCE_STATE_COMMON) {}

        protected:
            /** @brief Pointer to this resource */
            ID3D12Resource* m_resource;
            /** @brief Current resource state */
            D3D12_RESOURCE_STATES m_currentState;
            /** @brief Exact current resource state on GPU */
            D3D12_RESOURCE_STATES m_currentStateOnGpu;
        };

        /** @brief This class make 'ResStateController' */
        class ResStateMaker : public ResStateController {
        public:
            /**
               @brief Constructor
            */
            ResStateMaker() : ResStateController() {}

            /**
               @brief Create resource state controller
               @param resource Pointer to the resource
               @param currentState Current resource state
            */
            void Create(ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState) { m_resource = resource; m_currentState = currentState; m_currentStateOnGpu = currentState; }
        };

        /** @brief Class with an attribute added to the string indicating which shader */
        class ShaderString {
        public:
            /**
               @brief Constructor
               @param type Shader type
               @param str String to be stored
            */
            ShaderString(Utl::Dx::ShaderType type, std::string str);

            /** @brief Copy constructor */
            ShaderString(const ShaderString& other) = default;
            /** @brief Copy assignment operator */
            ShaderString& operator=(const ShaderString& other) = default;
            /** @brief Move constructor */
            ShaderString(ShaderString&& other) = default;
            /** @brief Move assignment operator */
            ShaderString& operator=(ShaderString&& other) = default;

            /** @brief Equivalence operator */
            bool operator==(const ShaderString& other) const { return m_attributeStr == other.m_attributeStr; }
            /** @brief Get string */
            const std::string& GetStr() const { return m_attributeStr; }
            /** @brief Get the shader type */
            const Utl::Dx::ShaderType GetShaderType() const;

        private:
            /**
               @brief Add attribute to the string indicating which shader
               @param type Shader type
               @param str Pointer to string to be stored
               @return Pointer to string with an attribute added indicating which shader
            */
            std::string* AddAttributeToStr(Utl::Dx::ShaderType type, std::string* str);

        private:
            /** @brief String with an attribute added indicating which shader */
            std::string m_attributeStr;
        };

        /** @brief Structure for user defined hash function of class 'ShaderString' */
        struct ShaderStringHash {
            /** @brief Hash operator */
            std::size_t operator()(const ShaderString& shaderString) const {
                return std::hash<std::string>()(shaderString.GetStr());
            }
        };

        /** @brief Abstruct structure of properties required for shader resource view */
        struct ASRVProperty : public ACWeakPtrFromThis<ASRVProperty> {
            /** @brief CPU handle by a descriptor heap */
            CPU_DESCRIPTOR_HANDLE cpuHandle;
            /** @brief Width of resolution */
            UINT width;
            /** @brief Height of resolution */
            UINT height;

            /** @brief Get resource state controller */
            virtual ResStateController* GetResStateController() = 0;

            /** @brief Get handle for this property */
            SRVPropertyHandle GetHandle();
        };

        /** @brief Properties required for shader resource view */
        struct SRVProperty : public ASRVProperty {
            /** @brief Resource state maker */
            CUniquePtrWeakable<ResStateMaker> stateMaker;

            /** @brief Constructor */
            SRVProperty() : stateMaker(CUniquePtrWeakable<ResStateMaker>::Make()) {}

            /** @brief Get resource state controller */
            ResStateController* GetResStateController() override { return stateMaker.Get(); }
        };

        /** @brief Properties required for shader resource view. Doesn't have a resource state maker */
        struct SRVPropertyNoStateMaker : public ASRVProperty {
            /** @brief Resource state controller */
            CWeakPtr<ResStateController> stateController;
            /** @brief Get resource state controller */
            ResStateController* GetResStateController() override { return stateController.Get(); }
        };

        /** @brief Handle class for a SRVProperty that can only be created from within the SRVProperty */
        class SRVPropertyHandle {
        public:
            /** @brief Constructor */
            SRVPropertyHandle() : m_srvProperty(nullptr) {}
            /** @brief Constructor */
            SRVPropertyHandle(ASRVProperty* srvProperty) : m_srvProperty(srvProperty->WeakFromThis()) {}

            /** @brief Destructor */
            ~SRVPropertyHandle() = default;
            
            /** @brief Is the property instance valid? */
            bool IsInstance() const { return m_srvProperty != nullptr; }
            /** @brief Get CPU handle */
            CPU_DESCRIPTOR_HANDLE GetCPUHandle() { return m_srvProperty->cpuHandle; }
            /** @brief Get resource state controller */
            ResStateController* GetStateController() { return m_srvProperty->GetResStateController(); }
            /** @brief Get width of resolution */
            UINT GetWidth() { return m_srvProperty->width; }
            /** @brief Get height of resolution */
            UINT GetHeight() { return m_srvProperty->height; }

            /** @brief Copy constructor */
            SRVPropertyHandle(const SRVPropertyHandle& other) = default;
            /** @brief Copy assignment operator */
            SRVPropertyHandle& operator=(const SRVPropertyHandle& other) = default;
            /** @brief Move constructor */
            SRVPropertyHandle(SRVPropertyHandle&& other) = default;
            /** @brief Move assignment operator */
            SRVPropertyHandle& operator=(SRVPropertyHandle&& other) = default;

        private:
            /** @brief Pointer to SRV property */
            CWeakPtr<ASRVProperty> m_srvProperty;
        };

        /** @brief Convert 'SHADER_TYPE' to 'D3D12_SHADER_VISIBILITY' */
        D3D12_SHADER_VISIBILITY ConvertShaderType2ShaderVisibility(ShaderType type);

        /** @brief Get the resource state for texture, applicable to the shader type */
        D3D12_RESOURCE_STATES ConvertShaderType2TextureResourceState(ShaderType type);

        /** @brief Convert 'DirectX::TEX_DIMENSION' to 'D3D12_SRV_DIMENSION' */
        D3D12_SRV_DIMENSION ConvertTexDimension2SRVDimension(DirectX::TEX_DIMENSION texDimension);

    } // namespace Dx
} // namespace Utl

#endif // !__UTILITY_FOR_DIRECTX_H__
