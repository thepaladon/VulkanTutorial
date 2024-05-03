#pragma once
#include <algorithm>
#include <vector>

#include "BEARVulkan/wVkGlobalVariables.h"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

namespace wVkHelpers {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


	inline SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		//Check details of Surface
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, wVkGlobals::g_Surface, &details.capabilities);

		//Check Format 
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, wVkGlobals::g_Surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, wVkGlobals::g_Surface, &formatCount, details.formats.data());
		}

		// Check Present mode
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, wVkGlobals::g_Surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, wVkGlobals::g_Surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	inline VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	inline VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

		// ToDo, make into a toggleable thing based on the options
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	//Makes sure to coordinates match pixels to screen coordinates 
	inline VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	inline wVkSwapchain createSwapChain(GLFWwindow* window) {

		wVkSwapchain swapchain = {};
		const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(wVkGlobals::g_PhysicalDevice);

		const VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		const VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		const VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

		uint32_t imageCount = wVkConstants::g_NumSwapChainImages;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = wVkGlobals::g_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		// The imageUsage bit field specifies what kind of operations we'll use the images in the swap chain 
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		// This places it directly at the output, for after-use (ex:post-processing) another flag is required 

		const wVkHelpers::QueueFamilyIndices indices = wVkHelpers::findQueueFamilies(wVkGlobals::g_PhysicalDevice);
		const uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		// This is pretty consistent, check [VkTutorial](https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Swap_chain)
		// if you need to touch this
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_FALSE;

		// For swapchain recreation, ex:resizing
		// Ref: https://vulkan-tutorial.com/en/Drawing_a_triangle/Swap_chain_recreation
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(wVkGlobals::g_Device, &createInfo, nullptr, &swapchain.swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		swapchain.minImageCount = swapChainSupport.capabilities.minImageCount;
		swapchain.imageCount = imageCount;
		swapchain.swapChainImageFormat = surfaceFormat.format;
		swapchain.swapChainExtent = extent;


		return swapchain;

	}

}
