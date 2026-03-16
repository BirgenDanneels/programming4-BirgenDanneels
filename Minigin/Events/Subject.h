#include <vector>
#include "Observer.h"

namespace dae
{
	template<typename... Args>
	class Subject final
	{
	public:
		void AddObserver(Observer<Args...>* observer)
		{
			auto it = std::find(m_Observers.begin(), m_Observers.end(), observer);

			if (it == m_Observers.end())
			{
				m_Observers.push_back(observer);
			}
		}

		void RemoveObserver(Observer<Args...>* observer)
		{
			auto it = std::find(m_Observers.begin(), m_Observers.end(), observer);

			if (it != m_Observers.end())
			{
				m_Observers.erase(it);
			}
		}

		void NotifyObservers(Args... args)
		{
			for (Observer* observer : m_Observers)
			{
				observer->OnNotify(args...);
			}
		}

	private:
		std::vector<Observer<Args...>*> m_Observers;
	};
}
