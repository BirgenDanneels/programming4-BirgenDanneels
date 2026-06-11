#include "AnimatedSpriteComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Minigin/Loading/LoadingHelpers.h"
#include "ResourceManager.h"

dae::AnimatedSpriteComponent::AnimatedSpriteComponent(GameObject& refOwner)
	:Component(refOwner)
{
}

void dae::AnimatedSpriteComponent::Initialize(const std::string& filename, SDL_FRect firstFrameSrcRect, int numberOfFrames, float secondsPerFrame, bool canLoop, bool centered, float xOffset, float yOffset)
{
	m_spriteSheet = ResourceManager::GetInstance().LoadTexture(filename);
	m_secondsPerFrame = secondsPerFrame;
	m_canLoop = canLoop;

	const float sheetWidth = m_spriteSheet->GetSize().x;

	// Start with the first frame
	m_frames.push_back(firstFrameSrcRect);

	// Generate frames based on the first frame and the number of frames
	for (int i = 0; i < numberOfFrames - 1; ++i)
	{
		SDL_FRect frameRect = m_frames[i];

		float leftoverWidth = sheetWidth - (frameRect.x + frameRect.w);

		if (leftoverWidth < firstFrameSrcRect.w)
		{
			// Move to the next row and keep track of the leftover width to calculate the x position of the next frame
			frameRect.x = firstFrameSrcRect.w - leftoverWidth;
			frameRect.y += firstFrameSrcRect.h;
		}
		else
		{
			frameRect.x += firstFrameSrcRect.w;
		}
		
		m_frames.push_back(frameRect);
	}
	if (centered)
	{
		m_xOffset = -firstFrameSrcRect.w / 2.0f;
		m_yOffset = -firstFrameSrcRect.h / 2.0f;
	}
	else
	{
		m_xOffset = xOffset;
		m_yOffset = yOffset;
	}
}

std::vector<dae::ParamDefinition> dae::AnimatedSpriteComponent::GetExpectedParams() const
{
	return { { "filename", std::string{} },
		{ "srcX", float{} },
		{ "srcY", float{} },
		{ "srcWidth", float{} },
		{ "srcHeight", float{} },
		{ "centered", bool{} },
		{ "xOffset", float{} },
		{ "yOffset", float{} },
		{ "secondsPerFrame", float{} },
		{ "numberOfFrames", int{} },
		{ "canLoop", bool{} } };
}

void dae::AnimatedSpriteComponent::Load(const ParamMap& params)
{
	std::string filename = GetRequiredParam<std::string>(params, "filename");
	SDL_FRect firstFrameSrcRect{};
	firstFrameSrcRect.x = GetRequiredParam<float>(params, "srcX");
	firstFrameSrcRect.y = GetRequiredParam<float>(params, "srcY");
	firstFrameSrcRect.w = GetRequiredParam<float>(params, "srcWidth");
	firstFrameSrcRect.h = GetRequiredParam<float>(params, "srcHeight");
	bool centered = GetRequiredParam<bool>(params, "centered");
	float xOffset = GetRequiredParam<float>(params, "xOffset");
	float yOffset = GetRequiredParam<float>(params, "yOffset");
	float secondsPerFrame = GetRequiredParam<float>(params, "secondsPerFrame");
	int numberOfFrames = GetRequiredParam<int>(params, "numberOfFrames");
	bool canLoop = GetRequiredParam<bool>(params, "canLoop");
	Initialize(filename, firstFrameSrcRect, numberOfFrames, secondsPerFrame, canLoop, centered, xOffset, yOffset);
}

void dae::AnimatedSpriteComponent::Update(float deltaTime)
{
	if (m_animationFinished)
		return;

	m_currentTime += deltaTime;
	if (m_currentTime >= m_secondsPerFrame)
	{
		m_currentTime -= m_secondsPerFrame;
		m_currentFrameIndex++;
		if (m_currentFrameIndex >= m_frames.size())
		{
			if (m_canLoop)
			{
				m_currentFrameIndex = 0;
			}
			else
			{
				m_currentFrameIndex = m_frames.size() - 1;
				m_animationFinished = true;
				m_animationEndedEvent.NotifyObservers();
			}
		}
	}
}

void dae::AnimatedSpriteComponent::Render() const
{
	if (m_spriteSheet == nullptr)
		return;

	const auto& transform = GetOwner()->GetTransform();
	const auto& pos = transform.GetWorldPosition();
	const float angle = transform.GetWorldRotation();
	const auto& scale = transform.GetWorldScale();

	auto* srcRect = &m_frames[m_currentFrameIndex];

	// Apply Transform scale
	float finalWidth = srcRect->w * scale.x;
	float finalHeight = srcRect->h * scale.y;

	float scaledOffsetX = m_xOffset * scale.x;
	float scaledOffsetY = m_yOffset * scale.y;

	SDL_FPoint centerPoint = { -scaledOffsetX, -scaledOffsetY };

	SDL_FRect dst{
		pos.x + scaledOffsetX,
		pos.y + scaledOffsetY,
		finalWidth,
		finalHeight
	};

	Renderer::GetInstance().RenderTexture(*m_spriteSheet, dst, angle, srcRect, &centerPoint);
}
