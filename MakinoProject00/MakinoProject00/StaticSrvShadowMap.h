/**
 * @file   StaticSrvShadowMap.h
 * @brief  This file handles allocator class of SRV static resource for shadow map.
 * 
 * @author Shodai Makino
 * @date   2024/1/21
 */

#ifndef __STATIC_SRV_SHADOW_MAP_H__
#define __STATIC_SRV_SHADOW_MAP_H__

#include "StaticSRVAllocator.h"

/** @brief allocator class of SRV static resource for shadow map 1 */
class CStaticSrvShadowMap1 : public ACStaticSRVAllocator {
public:
    /** @brief Constructor */
    CStaticSrvShadowMap1() : ACStaticSRVAllocator("shadowMapTex[0]") {}
};

/** @brief allocator class of SRV static resource for shadow map 2 */
class CStaticSrvShadowMap2 : public ACStaticSRVAllocator {
public:
    /** @brief Constructor */
    CStaticSrvShadowMap2() : ACStaticSRVAllocator("shadowMapTex[1]") {}
};

/** @brief allocator class of SRV static resource for shadow map 3 */
class CStaticSrvShadowMap3 : public ACStaticSRVAllocator {
public:
    /** @brief Constructor */
    CStaticSrvShadowMap3() : ACStaticSRVAllocator("shadowMapTex[2]") {}
};

#endif // !__STATIC_SRV_SHADOW_MAP_H__
