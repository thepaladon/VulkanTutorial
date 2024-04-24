#pragma once
#include "vulkan/vulkan.h"


	namespace wVkGlobals
	{
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

		// Useful variables
		extern uint32_t g_MaxFramesInFlight; // Assuming you manage multiple frames in flight
		extern uint32_t g_NumSwapChainImages;
		extern uint32_t g_RTVDescSize; // Render Target View Descriptor Size
		extern uint32_t g_CBV_SRV_UAVDescSize; // Constant Buffer View, Shader Resource View, Unordered Access View Descriptor Size
		extern uint32_t g_SamplerDescSize;

	} 
