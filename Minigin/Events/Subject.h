#pragma once

#include <algorithm>
#include <vector>
#include "Observer.h"
#include <memory>
#include <unordered_map>

namespace dae
{
	template<typename... Args>
	class ObserverHandleTemplated;

	class ObserverHandleBase
	{
	public:
		virtual ~ObserverHandleBase() = default;
		virtual void Unsubscribe() = 0;
	};

	class ObserverHandle final
	{
	public:
		ObserverHandle() = default;

		explicit ObserverHandle(std::unique_ptr<ObserverHandleBase> handle)
			: m_handle(std::move(handle))
		{
		}

		void Unsubscribe()
		{
			if (m_handle)
			{
				m_handle->Unsubscribe();
			}
		}

		// Move Only
		ObserverHandle(ObserverHandle&& other) noexcept = default;
		ObserverHandle& operator=(ObserverHandle&& other) noexcept = default;

		ObserverHandle(const ObserverHandle& other) = delete;
		ObserverHandle& operator=(const ObserverHandle& other) = delete;


	private:

		std::unique_ptr<ObserverHandleBase> m_handle;
	};

	template<typename... Args>
	class Subject final
	{
	public:



		// It is possible now to subscribe to the same subject multiple times with the same observer. 
		// But since a handle is returned it has to be done deliberately and the user can decide if they want that.
		ObserverHandle AddObserver(Observer<Args...>* observer)
		{
			auto handle = std::make_unique<ObserverHandleTemplated<Args...>>(this, observer);

			m_Observers[handle.get()] = observer;

			return ObserverHandle(std::move(handle));
		}

		void NotifyObservers(Args... args)
		{
			auto observersCopy = m_Observers; // copy map to allow modification during iteration

			for (auto& [handle, observer] : observersCopy)
			{
				observer->OnNotify(args...);
			}
		}

		~Subject()
		{
			for (auto& [handle, observer] : m_Observers)
			{
				handle->m_Subject = nullptr;
			}
		}

	private:
		friend class ObserverHandleTemplated<Args...>;

		void RemoveObserver(ObserverHandleTemplated<Args...>* handle)
		{
			m_Observers.erase(handle);
		}

		std::unordered_map<ObserverHandleTemplated<Args...>*, Observer<Args...>*> m_Observers;
	};

	template<typename... Args>
	class ObserverHandleTemplated final : public ObserverHandleBase
	{
	public:

		ObserverHandleTemplated(Subject<Args...>* subject, Observer<Args...>* observer)
			: m_Subject(subject), m_Observer(observer)
		{
		}

		void Unsubscribe() override
		{
			if (!m_Observer)
				return;

			if (m_Subject)
			{
				m_Subject->RemoveObserver(this);
			}

			m_Subject = nullptr;
			m_Observer = nullptr;
		}

		// Move Only
		ObserverHandleTemplated(ObserverHandleTemplated&& other) noexcept
			: m_Subject(std::move(other.m_Subject)), m_Observer(other.m_Observer)
		{
			other.m_Observer = nullptr;
		}

		ObserverHandleTemplated& operator=(ObserverHandleTemplated&& other) noexcept
		{
			if (this != &other)
			{
				// Unsubscribe from the current subject
				Unsubscribe();

				m_Subject = std::move(other.m_Subject);
				m_Observer = other.m_Observer;
				other.m_Observer = nullptr;
			}
			return *this;
		}

		ObserverHandleTemplated(const ObserverHandleTemplated& other) = delete;
		ObserverHandleTemplated& operator=(const ObserverHandleTemplated& other) = delete;

		~ObserverHandleTemplated()
		{
			Unsubscribe();
		}

	private:
		friend class Subject<Args...>;

		Subject<Args...>* m_Subject{ nullptr };
		Observer<Args...>* m_Observer{ nullptr };
	};
}
