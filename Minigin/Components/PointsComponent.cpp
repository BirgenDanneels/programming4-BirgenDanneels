#include "PointsComponent.h"
#include "TankComponent.h"
#include "Scene.h"

dae::PointsComponent::PointsComponent(GameObject& pOwner)
	: Component(pOwner), m_Points(0)
{
}

dae::PointsComponent::~PointsComponent() = default;

void dae::PointsComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::PointsComponent::OnNotify(TankEvents event)
{
	switch (event)
	{
	case TankEvents::KillEnemy:
		AddPoints(100);
		break;
	case TankEvents::PickupOrb:
		AddPoints(10);
		break;
	default:
		break;
	}
}

void dae::PointsComponent::AddPoints(int points)
{
	m_Points += points;
	m_onPointsChangedSubject.NotifyObservers(m_Points);
	
	//Broadcast a game event
	GetOwner()->GetScene()->GetGameEventQueue().Enqueue(Event{ make_sdbm_hash("PointsChanged"), {m_Points } });
}
