#pragma once
#include "vulkan/vulkan.h"

namespace wVkGlobals
{
	// Break the program at the start of the new frame if an issue has been found
	static bool g_errorValidationLayerTriggered = false;

	// Vulkan setup
	extern VkDevice g_Device;
	extern VkPhysicalDevice g_PhysicalDevice; // Assuming you need access to the physical device
	extern VkInstance g_Instance;
	extern VkQueue g_Queue;
	extern VkCommandPool g_CommandPool;
	extern VkCommandBuffer g_CommandBuffer;
	extern VkSwapchainKHR g_SwapChain;
	extern uint32_t g_CurrentImageIndex;
	extern VkRenderPass g_RenderPass;
	extern VkFormat g_ColorFormat;

	extern VkDebugUtilsMessengerEXT g_DebugMessenger;

	// Screen
	extern VkSurfaceKHR g_Surface;





	extern uint32_t g_RTVDescSize; // Render Target View Descriptor Size
	extern uint32_t g_CBV_SRV_UAVDescSize; // Constant Buffer View, Shader Resource View, Unordered Access View Descriptor Size
	extern uint32_t g_SamplerDescSize;

}
