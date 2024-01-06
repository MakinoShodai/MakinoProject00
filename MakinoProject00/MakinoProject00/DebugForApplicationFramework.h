/**
 * @file   DebugForApplicationFramework.h
 * @brief  Debugging features of the application framework.
 * 
 * @author Shodai Makino
 * @date   2023/11/1
 */

#ifndef __DEBUG_FOR_APPLICATION_FRAMEWORK_H__
#define __DEBUG_FOR_APPLICATION_FRAMEWORK_H__

/** @brief Debug namespace */
namespace Dbg {
    
#ifdef _DEBUG
    /**
       @brief Enable directX debug layer 
       @note Debug build only.
    */
    void EnableDirectXDebugLayer();
#endif // _DEBUG

#ifdef _DEBUG
    /**
       @brief Loads the dll required to use Microsoft Pix 
       @note Debug build only.
       @details 
       To use Microsoft Pix, you must call this function.
    */
    void LoadMicrosoftPixDLL();
#endif // _DEBUG

} // namespace Dbg

#endif // !__DEBUG_FOR_APPLICATION_FRAMEWORK_H__
