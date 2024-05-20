#pragma once
#include <vector>

#include "imgui_impl_vulkan.h"
#include "vulkan/vulkan.h"


namespace wVkHelpers
{
	// ToDo: This doesn't feel right
	// Consider making this SwapChainData and removing VkSwapchainKHR
	struct wVkSwapchain
	{
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		VkFormat swapChainImageFormat = {};
		VkExtent2D swapChainExtent = {};
		uint32_t minImageCount = 0;
		uint32_t imageCount = 0;
	};
}

namespace wVkGlobals
{

	// Break the program at the start of the new frame if an issue has been found
	static bool g_errorValidationLayerTriggered = false;

	// Vulkan setup
	extern VkDevice g_Device;
	extern VkPhysicalDevice g_PhysicalDevice; // Assuming you need access to the physical device
	extern VkInstance g_Instance;
	extern VkCommandPool g_CommandPool;
	extern VkCommandBuffer g_CommandBuffer;
	extern uint32_t g_CurrentImageIndex;
	extern VkRenderPass g_RenderPass;
	extern VkFormat g_ColorFormat;

	extern VkDebugUtilsMessengerEXT g_DebugMessenger;

	extern VkSurfaceKHR g_Surface;

	// These all evaluate to the same queue on my machine
	extern VkQueue g_GraphicsQueue;
	extern VkQueue g_PresentQueue;
	extern VkQueue g_ComputeQueue;

	extern wVkHelpers::wVkSwapchain g_SwapChain;
	extern std::vector<VkImage> g_SwapChainImages;
	extern std::vector<VkImageView> g_SwapChainImageViews;
	extern std::vector<VkFramebuffer> g_SwapChainFramebuffers;

	// Depth
	extern VkImage g_DepthImage;
	extern VkDeviceMemory g_DepthImageMemory;
	extern VkImageView g_DepthImageView;

	// ImGui
	extern ImGui_ImplVulkanH_Window g_ImGuiWindow;
	extern VkDescriptorPool g_ImguiPool;
	extern VkRenderPass g_ImGuiRenderPass;

	// Rasterization
	extern VkRenderPass g_StandardRenderPass;
}
