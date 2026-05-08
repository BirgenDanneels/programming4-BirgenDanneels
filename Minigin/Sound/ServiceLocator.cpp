#include "ServiceLocator.h"

std::unique_ptr<dae::SoundSystem> dae::ServiceLocator::m_soundInstance{std::make_unique<NullSoundSystem>()};
std::unique_ptr<dae::PhysicsManager> dae::ServiceLocator::m_physicsInstance{};
