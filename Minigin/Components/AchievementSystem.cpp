#include "AchievementSystem.h"
#include "Scene.h"
#include "Utils/HashUtil.h"
#include "Steam/SteamAchievements.h"
extern CSteamAchievements* g_SteamAchievements;

dae::AchievementSystem::AchievementSystem(GameObject& pOwner)
	: Component(pOwner)
{
	// Subscribe to the game event queue to listen for events that might unlock achievements.
	
	//m_subscriptionHandle = GetOwner()->GetScene()->GetGameEventQueue().SubscribeAll([this](const Event& event) { CheckAchievements(event); });
	if (IsAchievementValid("ACH_WIN_ONE_GAME"))
		m_winSubscriptionHandle = GetOwner()->GetScene()->GetGameEventQueue().Subscribe(make_sdbm_hash("PointsChanged"), [this](const Event& event) { CheckPointsForWinAchievement(event); });
}

dae::AchievementSystem::~AchievementSystem() = default;

/*void dae::AchievementSystem::CheckAchievements(const Event& event)
{
	if (event.id == make_sdbm_hash("PointsChanged"))
	{
		if(std::get<int>(event.args[0]) >= m_pointsForWin)

		UnlockAchievement("ACH_WIN_ONE_GAME");
	}
	// other achievement checks based on events
}*/



void dae::AchievementSystem::CheckPointsForWinAchievement(const Event& event)
{
	if (std::get<int>(event.GetArgs()[0]) >= m_pointsForWin)
	{
		UnlockAchievement("ACH_WIN_ONE_GAME");

		// Unsubscribe after unlocking the achievement
		GetOwner()->GetScene()->GetGameEventQueue().Unsubscribe(m_winSubscriptionHandle);
	}
}

bool dae::AchievementSystem::IsAchievementValid(const std::string& achievementName)
{
	if (g_SteamAchievements == nullptr || SteamUserStats() == nullptr)
		return false;

	bool alreadyUnlocked{ false };
	const bool found = SteamUserStats()->GetAchievement(achievementName.c_str(), &alreadyUnlocked);

	if (!found)
		return false; // unknown ID

	return !alreadyUnlocked; // valid if not already unlocked
}

void dae::AchievementSystem::UnlockAchievement(const std::string& achievementName)
{
	if (!IsAchievementValid(achievementName))
		return;

	g_SteamAchievements->SetAchievement(achievementName.c_str());
}
