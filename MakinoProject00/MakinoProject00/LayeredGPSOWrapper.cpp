#include "LayeredGPSOWrapper.h"
#include "CommandManager.h"

// Constructor
CLayeredGPSOWrapper::CLayeredGPSOWrapper()
#ifdef _DEBUG
    : m_name()
#endif // _DEBUG
{ }

// Create graphics pipeline states
void CLayeredGPSOWrapper::Create(ACScene* scene, const std::wstring gpsoName, 
    const LayeredGPSOSetting& setting, std::initializer_list<GraphicsLayer> useLayers) {
    if (m_gpso.size() > 0) {
        throw Utl::Error::CFatalError(L"This gpso wrapper has been already created! : " + gpsoName);
    }

    // Add gpso class to the array
    size_t overrideSize = setting.overrideSettings.size();
    for (size_t i = 0; i < overrideSize + 1; ++i) {
        m_gpso.emplace_back();
    }

    // Symbol to be added to the name
    wchar_t symbol = L'A';

    // Create default gpso
    m_gpso[0].Create(scene, gpsoName + L' ' + symbol, setting.defaultSetting, useLayers, setting.defaultTypes);

    // Create override gpsos
    for (size_t i = 0; i < overrideSize; ++i) {
        // Advance symbol
        symbol++;

        // Create
        m_gpso[i + 1].Create(scene, gpsoName + L' ' + symbol,
            setting.overrideSettings[i].setting.GetOverriddenSetting(setting.defaultSetting),
            useLayers, setting.overrideSettings[i].types);
    }
}

// Call 'SetCommand' function from GPSOs wrapped this class
void CLayeredGPSOWrapper::SetCommand() {
    ID3D12GraphicsCommandList* cmdList = CCommandManager::GetMain().GetGraphicsCmdList();

#ifdef _DEBUG
    // Begin GPSO Wrapper event
    PIX_BEGIN_EVENT(cmdList, PIX_COLOR_DEFAULT, (L"GPSO Wrapper Event: " + m_name).c_str());
#endif // _DEBUG

    for (auto& it : m_gpso) {
        it.SetCommand();
    }

#ifdef _DEBUG
    // End GPSO event
    PIX_END_EVENT(cmdList);
#endif // _DEBUG
}

// Call 'EndFrameProcess' function from GPSOs wrapped this class
void CLayeredGPSOWrapper::EndFrameProcess() {
    for (auto& it : m_gpso) {
        it.EndFrameProcess();
    }
}
