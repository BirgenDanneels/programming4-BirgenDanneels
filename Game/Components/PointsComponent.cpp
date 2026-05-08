#include "PointsComponent.h"
#include "TankComponent.h"
#include "Minigin/Utils/HashUtil.h"
#include "Minigin/Scene.h"

PointsComponent::PointsComponent(dae::GameObject& pOwner)
	: dae::Component(pOwner), m_Points(0)
{
}

PointsComponent::~PointsComponent() = default;

void PointsComponent::OnNotify(TankEvents event)
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

void PointsComponent::AddPoints(int points)
{
	m_Points += points;
	m_onPointsChangedSubject.NotifyObservers(m_Points);
	
	//Broadcast a game event
	GetOwner()->GetScene()->GetGameEventQueue().Enqueue({dae::Event::ToEventID("PointsChanged"), {m_Points } });
}
