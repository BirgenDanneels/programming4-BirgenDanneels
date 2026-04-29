#include "Minigin/Components/Component.h"
#include "Minigin/Events/Subject.h"
#include "Minigin/Events/Observer.h"

enum class TankEvents;

namespace dae
{

	class PointsComponent final : public Component, public Observer<TankEvents>
	{
	public:
		PointsComponent(GameObject& pOwner);
		~PointsComponent() override;
		void Update(float deltaTime) override;
		void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		void Render() const override {};
		int GetPoints() const { return m_Points; }

		void OnNotify(TankEvents event) override;

		Subject<int>& OnPointsChanged() { return m_onPointsChangedSubject; }

	private:

		int m_Points{ 0 };
		Subject<int> m_onPointsChangedSubject;

		void AddPoints(int points);
	};
}