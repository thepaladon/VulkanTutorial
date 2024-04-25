#pragma once

#include <set>
#include <stdexcept>

#include "wVkQueueFamilies.h"
#include "BEARVulkan/wVkConstants.h"
#include "BEARVulkan/wVkGlobalVariables.h"
#include "vulkan/vulkan.h"


namespace wVkHelpers {


	inline VkDevice createLogicalDevice(QueueFamilyIndices indices) {

		// Creating the Graphics Queue ------------
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(wVkConstants::deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = wVkConstants::deviceExtensions.data();

		if (wVkConstants::enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(wVkConstants::validationLayers.size());
			createInfo.ppEnabledLayerNames = wVkConstants::validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		// Creating the Presentation Queue ------------
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value(), indices.graphicsAndComputeFamily.value() };

		float pres_queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &pres_queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();


		VkDevice device;
		if (vkCreateDevice(wVkGlobals::g_PhysicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		return device;
	}

}