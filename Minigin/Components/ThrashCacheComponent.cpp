#include "ThrashCacheComponent.h"
#include <imgui.h>
#include <imgui_plot.h>
#include <chrono>
#include <algorithm>
#include <numeric>

dae::ThrashCacheComponent::ThrashCacheComponent(GameObject& owner)
	: Component(owner)
{
	m_exercise1Data.resize(11);
	m_exercise21Data.resize(11);
	m_exercise22Data.resize(11);
}

void dae::ThrashCacheComponent::Update(float deltaTime)
{
	(void)deltaTime;

	if (m_trash1TheCachePressed)
		TrashTheCacheEx1();

	if (m_trash21TheCachePressed)
		TrashTheCacheEx21();

	if (m_trash22TheCachePressed)
		TrashTheCacheEx22();
}

void dae::ThrashCacheComponent::RenderUI()
{
	ImColor graph1Color{ 1.f, 0.4f, 0.f,  1.f };
	ImColor graph21Color{ 0.55f, 0.91f, 0.45f };
	ImColor graph22Color{ 0.03f, 0.64f, 0.66f };

	//=======================
	//Exercise 1 window
	//=======================
	ImGui::Begin("Exercise 1");

	// Display the current value as an input field
	ImGui::PushItemWidth(200); // Set the width of the input field
	ImGui::InputInt("# samples", &m_nrOfSamplesEx1);
	ImGui::PopItemWidth();

	// Clamp the value to the specified range
	if (m_nrOfSamplesEx1 < 0) m_nrOfSamplesEx1 = 0;


	if (!m_trash1TheCachePressed)
	{
		if (ImGui::Button("Trash the cache"))
			m_trash1TheCachePressed = true;
	}
	else
		ImGui::Text("Wait for it...");

	if (m_ex1Finished)
		DrawGraph(m_exercise1Data, graph1Color);

	ImGui::End();


	//=======================
	//Exercise 2 window
	//=======================
	ImGui::Begin("Exercise 2");

	//Set sample amount
	ImGui::PushItemWidth(200);
	ImGui::InputInt("# samples", &m_nrOfSamplesEx2);
	ImGui::PopItemWidth();


	if (m_nrOfSamplesEx2 < 0) m_nrOfSamplesEx2 = 0;


	ImGui::PushID("Graph1");
	//GameObject3D button and graph
	if (!m_trash21TheCachePressed)
	{
		if (ImGui::Button("Trash the cache with GameObject3D"))
			m_trash21TheCachePressed = true;
	}
	else
		ImGui::Text("Wait for it...");

	if (m_ex21Finished)
		DrawGraph(m_exercise21Data, graph21Color);

	ImGui::PopID();


	ImGui::PushID("Graph2");
	//GameObject3DAlt button and graph
	if (!m_trash22TheCachePressed)
	{
		if (ImGui::Button("Trash the cache with GameObject3DAlt"))
			m_trash22TheCachePressed = true;
	}
	else
		ImGui::Text("Wait for it...");

	if (m_ex22Finished)
		DrawGraph(m_exercise22Data, graph22Color);

	ImGui::PopID();

	ImGui::PushID("Graph3");
	if (m_ex21Finished && m_ex22Finished)
		DrawCombinedGraph(graph21Color, graph22Color);
	ImGui::PopID();

	ImGui::End();
}

void dae::ThrashCacheComponent::TrashTheCacheEx1()
{
	m_ex1Finished = false;

	std::vector<int> arr(m_nrOfSamplesEx1);

	int vectorIdx = 0;

	for (int stepsize{ 1 }; stepsize <= 1024; stepsize *= 2, ++vectorIdx)
	{
		std::vector<float> timingVect;
		timingVect.reserve(m_nrOfRepeats);

		for (int r = 0; r < m_nrOfRepeats; r++)
		{

			const auto startTime = std::chrono::high_resolution_clock().now();
			for (int i = 0; i < m_nrOfSamplesEx1; i += stepsize)
			{
				arr[i] *= 2;
			}
			const auto endTime = std::chrono::high_resolution_clock().now();

			timingVect.push_back(
				static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(
					endTime - startTime).count()));
		}

		RemoveOutliers(timingVect, 0.1f); // Remove top and bottom 10%

		m_exercise1Data[vectorIdx] = CalculateAverage(timingVect);

	}

	m_trash1TheCachePressed = false;
	m_ex1Finished = true;
}

void dae::ThrashCacheComponent::TrashTheCacheEx21()
{
	m_ex21Finished = false;

	std::vector<GameObject3D> arr(m_nrOfSamplesEx2);

	int vectorIdx = 0;

	for (int stepsize{ 1 }; stepsize <= 1024; stepsize *= 2, ++vectorIdx)
	{
		std::vector<float> timingVect;
		timingVect.reserve(m_nrOfRepeats);

		for (int r = 0; r < m_nrOfRepeats; r++)
		{

			const auto startTime = std::chrono::high_resolution_clock().now();
			for (int i = 0; i < m_nrOfSamplesEx2; i += stepsize)
			{
				arr[i].ID *= 2;
			}
			const auto endTime = std::chrono::high_resolution_clock().now();

			timingVect.push_back(
				static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(
					endTime - startTime).count()));
		}

		RemoveOutliers(timingVect, 0.1f); // Remove top and bottom 10%

		m_exercise21Data[vectorIdx] = CalculateAverage(timingVect);

	}

	m_trash21TheCachePressed = false;
	m_ex21Finished = true;
}

void dae::ThrashCacheComponent::TrashTheCacheEx22()
{
	m_ex22Finished = false;

	std::vector<GameObject3DAlt> arr(m_nrOfSamplesEx2);

	int vectorIdx = 0;

	for (int stepsize{ 1 }; stepsize <= 1024; stepsize *= 2, ++vectorIdx)
	{
		std::vector<float> timingVect;
		timingVect.reserve(m_nrOfRepeats);

		for (int r = 0; r < m_nrOfRepeats; r++)
		{

			const auto startTime = std::chrono::high_resolution_clock().now();
			for (int i = 0; i < m_nrOfSamplesEx2; i += stepsize)
			{
				arr[i].ID *= 2;
			}
			const auto endTime = std::chrono::high_resolution_clock().now();

			timingVect.push_back(
				static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(
					endTime - startTime).count()));
		}
		
		RemoveOutliers(timingVect, 0.1f); // Remove top and bottom 10%

		m_exercise22Data[vectorIdx] = CalculateAverage(timingVect);
	}

	m_trash22TheCachePressed = false;
	m_ex22Finished = true;
}

void dae::ThrashCacheComponent::DrawGraph(std::vector<float> dataVector, ImColor clr) const
{
	// Calculate min and max values
	float minVal = *std::min_element(dataVector.begin(), dataVector.end());
	float maxVal = *std::max_element(dataVector.begin(), dataVector.end());

	// Y-axis padding
	float padding = 0.1f * (maxVal - minVal);
	minVal -= padding;
	maxVal += padding;

	// Configure plot
	ImGui::PlotConfig conf;
	conf.values.xs = m_xAxisValues.data();
	conf.values.ys = dataVector.data();
	conf.values.color = clr;
	conf.values.count = (int)dataVector.size();
	conf.scale.min = minVal;
	conf.scale.max = maxVal;
	conf.tooltip.show = true;
	conf.tooltip.format = "Step: %.2f; Time(ms): = %.2f ";

	conf.grid_y.show = true; // Show Y-axis grid
	conf.grid_y.size = (maxVal - minVal) / (int)dataVector.size();

	conf.frame_size = ImVec2(200, 100); // Size of the plot

	ImGui::Plot("", conf);
}

void dae::ThrashCacheComponent::DrawCombinedGraph(ImColor clr1, ImColor clr2) const
{
	// Calculate min and max values
	float minVal1 = *std::min_element(m_exercise21Data.begin(), m_exercise21Data.end());
	float minVal2 = *std::min_element(m_exercise22Data.begin(), m_exercise22Data.end());

	if (minVal1 < minVal2) minVal1 = minVal2;

	float maxVal1 = *std::max_element(m_exercise21Data.begin(), m_exercise21Data.end());
	float maxVal2 = *std::max_element(m_exercise22Data.begin(), m_exercise22Data.end());

	if (maxVal1 < maxVal2) maxVal1 = maxVal2;

	// Add some padding to the Y-axis
	float padding = 0.1f * (maxVal1 - minVal1);
	minVal1 -= padding;
	maxVal1 += padding;

	// Configure the plot
	ImGui::PlotConfig conf;

	conf.values.xs = m_xAxisValues.data();

	conf.values.ys_count = 2; // Number of additional lines
	conf.values.ys_list = new const float* [2]; // Create array for sperate graph lines
	conf.values.ys_list[0] = m_exercise21Data.data(); // First line data
	conf.values.ys_list[1] = m_exercise22Data.data(); // Second line data

	// Set color
	conf.values.colors = new const ImU32[2]{ clr1, clr2 };

	conf.values.count = (int)m_xAxisValues.size();	// Number of data points in a line
	conf.scale.min = minVal1;
	conf.scale.max = maxVal1;
	conf.tooltip.show = true;
	conf.tooltip.format = "Step: %.2f; Time(ms): %.2f";

	conf.grid_y.show = true; // Show Y-axis grid
	conf.grid_y.size = (maxVal1 - minVal1) / (int)m_xAxisValues.size();

	conf.frame_size = ImVec2(200, 100); // Size of the plot

	ImGui::Plot("", conf);

	delete[] conf.values.ys_list;
	delete[] conf.values.colors;
}

void dae::ThrashCacheComponent::RemoveOutliers(std::vector<float>& timingVect, float percentage)
{
	if (timingVect.empty())
		return;
	std::sort(timingVect.begin(), timingVect.end());
	int removeCount = static_cast<int>(timingVect.size() * percentage);
	timingVect.erase(timingVect.begin(), timingVect.begin() + removeCount);
	timingVect.erase(timingVect.end() - removeCount, timingVect.end());
}

float dae::ThrashCacheComponent::CalculateAverage(const std::vector<float>& timingVect)
{
	if (timingVect.empty())
		return 0.0f;
	float sum = std::accumulate(timingVect.begin(), timingVect.end(), 0.0f);
	return sum / static_cast<float>(timingVect.size());
}
