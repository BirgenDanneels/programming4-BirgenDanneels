#include "Component.h"
#include "Events/GameEventQueue.h"

namespace dae
{

	class AchievementSystem final : public Component
	{
	public:
		AchievementSystem(GameObject& pOwner);
		~AchievementSystem() override;

		void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		void Update(float deltaTime) override { (void)deltaTime; };
		void Render() const override {};

		//void CheckAchievements(const Event& event);
		void CheckPointsForWinAchievement(const Event& event);

	private:

		bool IsAchievementValid(const std::string& achievementName);

		void UnlockAchievement(const std::string& achievementName);
			
		//SubscriptionHandle m_subscriptionHandle;
		SubscriptionHandle m_winSubscriptionHandle;

		int m_pointsForWin{ 500 };
	};
}