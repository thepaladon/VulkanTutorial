#pragma once
#include <cstdint>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32


class Window;
class Texture;
class CommandList;
class BackEndRenderer
{
public:
	BackEndRenderer();
	~BackEndRenderer() {};

	void Initialize(GLFWwindow* window, Texture** mainRenderTargets, CommandList* cmdList);
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	void EndTracing();
	void ImguiBeginFrame();
	void ImguiEndFrame();

	// Resizes Render Targets
	void ResizeFrameBuffers(GLFWwindow* window); // Diverged from OG BEAR

	// Gets the ID of the current Render Target
	uint32_t GetCurrentBackBufferIndex() const;

	void PresentFrame();
	void WaitForCmdQueueExecute();

	uint32_t GetFrameIndex() const { return m_FrameIndex; }
	uint32_t GetFrameCounter() const { return m_FrameCounter; }

private:
	uint32_t m_FrameIndex;
	uint32_t m_FrameCounter;
};
