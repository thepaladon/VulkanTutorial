#include "wVkGlobalVariables.h"


namespace wVkGlobals
{
	// Vulkan setup
	VkInstance g_Instance = VK_NULL_HANDLE;
	VkDevice g_Device = VK_NULL_HANDLE;
	VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE; // Assuming you need access to the physical device
	VkQueue g_Queue = VK_NULL_HANDLE;
	VkCommandPool g_CommandPool = VK_NULL_HANDLE;
	VkCommandBuffer g_CommandBuffer = VK_NULL_HANDLE;
	VkRenderPass g_RenderPass = VK_NULL_HANDLE;
	VkFormat g_ColorFormat = {};

	VkDebugUtilsMessengerEXT g_DebugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR g_Surface = VK_NULL_HANDLE;

	uint32_t g_CurrentImageIndex = 0;

	VkQueue g_GraphicsQueue = VK_NULL_HANDLE;
	VkQueue g_PresentQueue = VK_NULL_HANDLE;
	VkQueue g_ComputeQueue = VK_NULL_HANDLE;

	wVkHelpers::wVkSwapchain g_SwapChain = {};
	std::vector<VkImage> g_SwapChainImages;
	std::vector<VkImageView> g_SwapChainImageViews;
	std::vector<VkFramebuffer> g_SwapChainFramebuffers;

	// ToDo Support depth as texture
	VkImage g_DepthImage = VK_NULL_HANDLE;;
	VkDeviceMemory g_DepthImageMemory = VK_NULL_HANDLE;;
	VkImageView g_DepthImageView = VK_NULL_HANDLE;;

	// ImGui
	ImGui_ImplVulkanH_Window g_ImGuiWindow;
	VkDescriptorPool g_ImguiPool = VK_NULL_HANDLE;
	VkRenderPass g_ImGuiRenderPass = VK_NULL_HANDLE;

} // namespace Ball::GlobalDX12