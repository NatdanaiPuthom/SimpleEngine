#pragma once

//Only For global.cpp

namespace Simple
{
	class Engine;
	class GraphicsEngine;
}

namespace Drawer
{
	class Renderer;
}

namespace Impl
{
	class SimpleGlobalEngine final
	{
		friend class Simple::Engine;
	private:
		static void SetEngine(Simple::Engine* aEngine);
		static void SetResolution(const Math::Vector2ui& aResolution);
		static void SetWindowSize(const Math::Vector2ui& aWindowSize);
	};

	class SimpleGlobalGraphics final
	{
		friend class Simple::GraphicsEngine;
	private:
		static void SetGraphicsEngine(Simple::GraphicsEngine* aGraphicsEngine);
		static void UpdateFPSCounter();
		static void ResetDrawCalls();
	};

	class SimpleGlobalRenderer final
	{
		friend class Drawer::Renderer;
	private:
		static void IncreaseDrawCall();
	};
}