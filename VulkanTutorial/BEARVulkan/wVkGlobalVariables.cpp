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
	VkSwapchainKHR g_SwapChain = VK_NULL_HANDLE;
	VkRenderPass g_RenderPass = VK_NULL_HANDLE;
	VkFormat g_ColorFormat = {};

	VkDebugUtilsMessengerEXT g_DebugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR g_Surface = VK_NULL_HANDLE;

	uint32_t g_CurrentImageIndex = 0;

	// Useful variables
	uint32_t g_FrameIndex = 0;
	uint32_t g_RTVDescSize = 0;
	uint32_t g_CBV_SRV_UAVDescSize = 0;
	uint32_t g_SamplerDescSize = 0;
} // namespace Ball::GlobalDX12