#include "Minigin/Components/Component.h"
#include "Minigin/Events/Subject.h"
#include "Minigin/Events/Observer.h"

enum class TankEvents;

	
class PointsComponent final : public dae::Component, public dae::Observer<TankEvents>
{
public:
	PointsComponent(dae::GameObject& pOwner);
	~PointsComponent() override;

	void Update(float) override {};
	void FixedUpdate(float) override {};
	void Render() const override {};
	int GetPoints() const { return m_Points; }

	void OnNotify(TankEvents event) override;

	dae::Subject<int>& OnPointsChanged() { return m_onPointsChangedSubject; }

	//Loading functions
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override { return{}; };
	virtual void Load(const dae::ParamMap&) override {};

private:

	int m_Points{ 0 };
	dae::Subject<int> m_onPointsChangedSubject;

	void AddPoints(int points);
};
