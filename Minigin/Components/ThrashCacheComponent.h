#include "Component.h"
#include <vector>

struct ImColor;

namespace dae
{
	class ThrashCacheComponent final : public Component
	{
	public:

		ThrashCacheComponent(GameObject& owner);

		virtual void Update(float deltaTime) override;
		virtual void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		virtual void Render() const override {};
		virtual void RenderUI() override;

	private:

		struct Transform
		{
			float matrix[16] = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1 };
		};
		class GameObject3D
		{
		public:
			Transform transform;
			int ID;
		};
		class GameObject3DAlt
		{
		public:
			Transform* transform;
			int ID;
		};

		bool m_trash1TheCachePressed{ false };
		bool m_trash21TheCachePressed{ false };
		bool m_trash22TheCachePressed{ false };

		bool m_ex1Finished{ false };
		bool m_ex21Finished{ false };
		bool m_ex22Finished{ false };

		int m_nrOfSamplesEx1{ 10'000'000 };
		int m_nrOfSamplesEx2{ 1'000'000 };

		int m_nrOfRepeats{ 50 }; //this is the times it gets repeated for each stepsize, so we can remove outliers and get a more accurate average time

		std::vector<float> m_exercise1Data{};
		std::vector<float> m_exercise21Data{};
		std::vector<float> m_exercise22Data{};
		std::vector<float> m_xAxisValues{ 1,2,4,8,16,32,64,128,256,512,1024 };

		void TrashTheCacheEx1();
		void TrashTheCacheEx21();
		void TrashTheCacheEx22();

		void DrawGraph(std::vector<float> dataVector, ImColor clr) const;
		void DrawCombinedGraph(ImColor clr1, ImColor clr2) const;

		void RemoveOutliers(std::vector<float>& timingVect, float percentage);
		float CalculateAverage(const std::vector<float>& timingVect);
	};
}