#include "BEARHeaders/BackEndRenderer.h"

#include "wVkGlobalVariables.h"
#include "wVkHelpers/wVkInstance.h"
#include "wVkHelpers/wVkLogicalDevice.h"
#include "wVkHelpers/wVkPhysicalDevice.h"


using namespace wVkGlobals;

BackEndRenderer::BackEndRenderer() {}

void BackEndRenderer::ResizeFrameBuffers(const uint32_t width, const uint32_t height)
{

}

void BackEndRenderer::Initialize(GLFWwindow* window, Texture** mainRenderTargets, CommandList* cmdList)
{

	g_Instance = wVkHelpers::createInstance();
	g_DebugMessenger = wVkHelpers::setupDebugMessenger();

	// Create Instance
	// ToDo: Figure out how to do it from HWND for Ball
	if (glfwCreateWindowSurface(g_Instance, window, nullptr, &g_Surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	g_PhysicalDevice = wVkHelpers::pickPhysicalDevice();

	const wVkHelpers::QueueFamilyIndices queueIndices = wVkHelpers::findQueueFamilies(wVkGlobals::g_PhysicalDevice);
	g_Device = createLogicalDevice(queueIndices);

	vkGetDeviceQueue(g_Device, queueIndices.graphicsFamily.value(), 0, &g_GraphicsQueue);
	vkGetDeviceQueue(g_Device, queueIndices.presentFamily.value(), 0, &g_PresentQueue);
	vkGetDeviceQueue(g_Device, queueIndices.graphicsAndComputeFamily.value(), 0, &g_ComputeQueue);

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