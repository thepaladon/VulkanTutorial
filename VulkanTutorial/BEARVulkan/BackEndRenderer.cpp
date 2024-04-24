#include "BEARHeaders/BackEndRenderer.h"

#include "wVkGlobalVariables.h"
#include "wVkHelpers/wVkInstance.h"

using namespace wVkGlobals;

BackEndRenderer::BackEndRenderer() {}

void BackEndRenderer::ResizeFrameBuffers(const uint32_t width, const uint32_t height)
{

}

void BackEndRenderer::Initialize(Window* window, Texture** mainRenderTargets, CommandList* cmdList)
{
	wVkGlobals::g_Instance = wVkHelpers::createInstance();
	wVkGlobals::g_DebugMessenger = wVkHelpers::setupDebugMessenger();

}

void BackEndRenderer::EndTracing()
{

}

void BackEndRenderer::ImguiEndFrame()
{

}

void BackEndRenderer::EndFrame()
{

}

void BackEndRenderer::BeginFrame()
{

}

void BackEndRenderer::PresentFrame()
{

}

uint32_t BackEndRenderer::GetCurrentBackBufferIndex() const
{
	return 0;
}

void BackEndRenderer::Shutdown()
{
	if (wVkConstants::enableValidationLayers) {
		wVkHelpers::DestroyDebugUtilsMessengerEXT(g_Instance, g_DebugMessenger, nullptr);
	}

	vkDestroyInstance(g_Instance, nullptr);
}

void BackEndRenderer::ImguiBeginFrame()
{

}

void BackEndRenderer::WaitForCmdQueueExecute()
{

}