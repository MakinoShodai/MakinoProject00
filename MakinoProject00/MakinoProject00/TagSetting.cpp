#include "TagSetting.h"
#include "ColliderTag.h"

// Perform setting tag of collider
void TagSetting() {
    CColliderTagRegistry& registry = CColliderTagRegistry::GetAny();

    // Unset
    registry.SetConfig(ColliderTagConfigKind::Collide, ColliderTag::Empty, ColliderTag::Empty);


    registry.SetConfig(ColliderTagConfigKind::OneSideCollideOverlap, ColliderTag::Player, ColliderTag::Terrain);

    // Finish writing
    registry.FinishWrite();
}
