#include <assert.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <thread>
#include <mutex>
#include <tuple>
#include <condition_variable>
#include <future>
#include <variant>
#include <optional>
#include <tchar.h>
#include <string>
#include <fstream>
#include <array>
#include <deque>
#include <Windows.h>
#include <wrl.h>
#include <functional>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <d3d12shader.h>
#include <shlobj.h>
#include <strsafe.h>
#include <random>
#include <typeindex>
#include <typeinfo>
#include <filesystem>
#include <cryptopp/cryptopp/aes.h>
#include <cryptopp/cryptopp/filters.h>
#include <cryptopp/cryptopp/modes.h>
#include <cryptopp/cryptopp/osrng.h>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#ifdef _DEBUG
#include "pix3.h"
#endif // _DEBUG

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// DirectXTex.lib's linker instructions
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/Debug/DirectXTex.lib")
#else 
#pragma comment(lib, "DirectXTex/Release/DirectXTex.lib")
#endif //_DEBUG

#ifdef _DEBUG
#pragma comment(lib, "cryptopp/Debug/cryptlib.lib")
#else
#pragma comment(lib, "cryptopp/Release/cryptlib.lib")
#endif // _DEBUG

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc141-mt.lib")
#else
#pragma comment(lib, "assimp-vc141-mt.lib")
#endif //_DEBUG
