#include "ReleaseSingleton.h"

// Initialize static member variables
std::vector<std::pair<int, ACReleaseSingleton*>> ACReleaseSingleton::ms_allInstance     = {};
bool                                             ACReleaseSingleton::ms_isCantRemove    = false;

// Release all instance
void ACReleaseSingleton::AllRelease() {
    // Sort from low to high
    auto sortFunc = [](const std::pair<int, ACReleaseSingleton*>& a, const std::pair<int, ACReleaseSingleton*>& b) {
        return a.first < b.first;
        };
    std::sort(ms_allInstance.begin(), ms_allInstance.end(), sortFunc);

    // Removing instances from the array is not allowed
    ms_isCantRemove = true;

    // Release and clear all instance
    for (auto& it : ms_allInstance) {
        it.second->DestroyInstance();
        it.second = nullptr;
    }
    ms_allInstance.clear();

    // Removing instances from the array is allowed
    ms_isCantRemove = false;
}

// Add the created instance to the array
void ACReleaseSingleton::AddInstanceToArray(int priority, ACReleaseSingleton* instance) {
    ms_allInstance.push_back(std::make_pair(priority, instance));
}

// Remove the instance in the array
void ACReleaseSingleton::RemoveInstanceToArray(ACReleaseSingleton* instance) {
    // early return
    if (ms_isCantRemove) { return; }

    for (int i = 0; i < ms_allInstance.size(); ++i) {
        // Find the same instance in the array
        if (ms_allInstance[i].second == instance) {
            // Remove it from the array
            std::swap(ms_allInstance[i], ms_allInstance.back());
            ms_allInstance.pop_back();
        }
    }
}
