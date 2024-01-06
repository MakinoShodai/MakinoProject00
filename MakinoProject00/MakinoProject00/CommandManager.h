/**
 * @file   CommandManager.h
 * @brief  This file handles command lists, command allocators, and command queue.
 * 
 * @author Shodai Makino
 * @date   2023/11/14
 */

#ifndef __COMMAND_MANAGER_H__
#define __COMMAND_MANAGER_H__

#include "Singleton.h"
#include "SwapChain.h"

/** @brief Command type */
enum class CommandType {
    /** @brief Graphics command type */
    Graphics,
};

/** 
   @brief this class handles objects related to DirectX12 commands.
   @details
   Must always call 'Initialize' function before use
*/
class CCommandManager : public ACMainThreadSingleton<CCommandManager> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CCommandManager();

    /**
       @brief Initialize
    */
    void Initialize();

    /**
       @brief Execute registered commands
       @param type Execute command type
       @param isWaitForGPU Do you want to wait until the process is complete?
    */
    void CommandsExecute(CommandType type, bool isWaitForGPU);

    /**
       @brief Executes registered graphics commands, but does not wait or clear, so it is only used in special situations
    */
    void UnsafeGraphicsCommandsExecute();

    /** @brief Get command queue */
    ID3D12CommandQueue* GetCommandQueue() { return m_cmdQueue.Get(); }
    /** @brief Get command list for graphics */
    ID3D12GraphicsCommandList* GetGraphicsCmdList() { return m_graphicsCmdList[CSwapChain::GetMain().GetNextBackBufferIndex()].Get(); }

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CCommandManager> {
    public:
        // Friend declaration
        using ACInnerClass<CCommandManager>::ACInnerClass;

        /**
           @brief Wait for the GPU to complete processing
        */
        void WaitForGPU();

        /**
           @brief Clear graphics command list and graphics command allocator, However, it is not associated with execution and is used only in special situations
           @param index Clear the index of graphics command list and graphics command allocator
        */
        void UnsafeGraphicsCommandsClear(UINT index);
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
    CCommandManager() : ACMainThreadSingleton(-50), m_fenceValue(0){}

private:
    /** @brief Command queue */
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cmdQueue;
    /** @brief Fence object */
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    /** @brief Fence value */
    UINT64 m_fenceValue;

    /** @brief Command allocator for graphics */
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_graphicsCmdAllocator[SCREEN_BUFFERING_NUM];
    /** @brief Command list for graphics */
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_graphicsCmdList[SCREEN_BUFFERING_NUM];
};

#ifdef _DEBUG
/** @brief Send event start command for PIX debugging (Only debug build) */
#define PIX_BEGIN_EVENT(cmdList, color, eventName) PIXBeginEvent(cmdList, color, eventName);
/** @brief Send end event commands for PIX debugging (Only debug build) */
#define PIX_END_EVENT(cmdList) PIXEndEvent(cmdList)
#else
/** @brief Empty macro */
#define PIX_BEGIN_EVENT(cmdList, color, eventName) 
/** @brief Empty macro */
#define PIX_END_EVENT(cmdList)
#endif // _DEBUG

#endif // !__COMMAND_MANAGER_H__
