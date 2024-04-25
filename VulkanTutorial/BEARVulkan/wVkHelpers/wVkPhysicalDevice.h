#pragma once
#include <set>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>

#include "wVkQueueFamilies.h"
#include "wVkSwapchain.h"
#include "BEARVulkan/wVkGlobalVariables.h"
#include "Utils/ConsoleLogger.h"

namespace wVkHelpers {

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		//checking for swapchain support
		std::set<std::string> requiredExtensions(wVkConstants::deviceExtensions.begin(), wVkConstants::deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}


	inline bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);

		const bool extensionsSupported = checkDeviceExtensionSupport(device);

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	inline VkPhysicalDevice pickPhysicalDevice() {

		//Check whether devices exist 
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(wVkGlobals::g_Instance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			VK_LOG_ERROR("Failed to find GPUs with Vulkan support!");
			throw std::runtime_error("");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(wVkGlobals::g_Instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			VK_LOG_INFO("%s", deviceProperties.deviceName);

			if (isDeviceSuitable(device)) {
				return device;
			}
		}

		VK_LOG_ERROR("Failed to find a suitable GPU!");
		throw std::runtime_error("");
		return VK_NULL_HANDLE;
	}
}
