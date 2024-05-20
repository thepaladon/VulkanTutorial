#include "BEARHeaders/BackEndRenderer.h"

#include "wVkGlobalVariables.h"
#include "wVkHelpers/wVkCommands.h"
#include "wVkHelpers/wVkImGui.h"
#include "wVkHelpers/wVkInstance.h"
#include "wVkHelpers/wVkLogicalDevice.h"
#include "wVkHelpers/wVkPhysicalDevice.h"
#include "wVkHelpers/wVkTemp.h"
#include "wVkHelpers/wVkTexture.h"
#include "wVkHelpers/wVkHelpers.h"

using namespace wVkGlobals;

BackEndRenderer::BackEndRenderer() {}

void destroySwapchain()
{
	if (g_SwapChain.swapChain != VK_NULL_HANDLE) {

		for (const auto imageView : g_SwapChainImageViews) {
			vkDestroyImageView(g_Device, imageView, nullptr);
		}
		vkDestroySwapchainKHR(g_Device, g_SwapChain.swapChain, nullptr);
	}

	for (auto& framebuffer : g_SwapChainFramebuffers) {
		vkDestroyFramebuffer(g_Device, framebuffer, nullptr);
	}


	vkDestroyImageView(g_Device, g_DepthImageView, nullptr);
	vkDestroyImage(g_Device, g_DepthImage, nullptr);
	vkFreeMemory(g_Device, g_DepthImageMemory, nullptr);
}

void createSwapchainData(GLFWwindow* window)
{
	// Destroy previous swapchain, if valid
	destroySwapchain();

	// For intellisense - to get my values for debugging
	auto& swapchain = g_SwapChain;
	auto& swapchainImage = g_SwapChainImages;
	auto& swapchainViews = g_SwapChainImageViews;

	// Create new swapchain
	swapchain = wVkHelpers::createSwapChain(window);

	auto& imgCount = swapchain.imageCount;
	vkGetSwapchainImagesKHR(g_Device, swapchain.swapChain, &imgCount, nullptr);
	swapchainImage.resize(imgCount);
	vkGetSwapchainImagesKHR(g_Device, swapchain.swapChain, &imgCount, swapchainImage.data());

	swapchainViews.resize(swapchainImage.size());

	for (uint32_t i = 0; i < swapchainImage.size(); i++) {
		swapchainViews[i] = wVkHelpers::createImageView(swapchainImage[i], 1, swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	const VkFormat depthFormat = wVkHelpers::findDepthFormat();

	const auto& ext = g_SwapChain.swapChainExtent;
	wVkHelpers::createImage2D(ext.width, ext.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, g_DepthImage, g_DepthImageMemory);

	g_DepthImageView = wVkHelpers::createImageView(g_DepthImage, 1, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	g_SwapChainFramebuffers.resize(g_SwapChainImageViews.size());
	for (int i = 0; i < g_SwapChainImageViews.size(); i++) {
		g_SwapChainFramebuffers[i] = wVkHelpers::createFramebuffer(i);
	}
}


// ToDo, make this work with const uint32_t width, const uint32_t height, same comment as HWND
void BackEndRenderer::ResizeFrameBuffers(GLFWwindow* window)
{
	createSwapchainData(window);
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

	g_RenderPass = wVkHelpers::createRenderPass();
	createSwapchainData(window);


	g_CommandPool = wVkHelpers::createCommandPool();

	wVkHelpers::initImgui(window, g_ImGuiRenderPass, g_ImguiPool);


}

void BackEndRenderer::EndTracing()
{

}

void BackEndRenderer::ImguiEndFrame()
{

}

void BackEndRenderer::EndFrame()
{
	m_FrameIndex = (m_FrameIndex + 1) % wVkConstants::g_MaxFramesInFlight;
	m_FrameCounter++;

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
	destroySwapchain();

	// Shut Down ImGui
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	vkDestroyDescriptorPool(g_Device, g_ImguiPool, nullptr);
	vkDestroyRenderPass(g_Device, g_ImGuiRenderPass, nullptr);

	vkDestroyCommandPool(g_Device, g_CommandPool, nullptr);

	if (wVkConstants::enableValidationLayers) {
		wVkHelpers::DestroyDebugUtilsMessengerEXT(g_Instance, g_DebugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(g_Instance, g_Surface, nullptr);
	vkDestroyRenderPass(g_Device, g_RenderPass, nullptr);

	

	// Need to be last
	vkDestroyDevice(g_Device, nullptr);
	vkDestroyInstance(g_Instance, nullptr);
}

void BackEndRenderer::ImguiBeginFrame()
{

}

void BackEndRenderer::WaitForCmdQueueExecute()
{

}