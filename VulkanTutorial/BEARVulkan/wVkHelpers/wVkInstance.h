#pragma once

#include <vector>

#include "wVkDebug.h"
#include "BEARVulkan/wVkConstants.h"
#include "GLFW/glfw3.h"

#include <stdexcept>

namespace wVkHelpers {

	inline std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (wVkConstants::enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}


	inline VkInstance createInstance()
	{
		VkInstance instance;

		//Validation Layer Check
		if (wVkConstants::enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		//Check for available extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		LOG_INFO("Vulkan Available extensions:");
		for (const auto& extension : extensions) {
			printf("\t %s \n", extension.extensionName);
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Get Extensions
		auto glfwExtensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
		createInfo.ppEnabledExtensionNames = glfwExtensions.data();

		//Get Layers in Debug
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (wVkConstants::enableValidationLayers) {

			createInfo.enabledLayerCount = static_cast<uint32_t>(wVkConstants::validationLayers.size());
			createInfo.ppEnabledLayerNames = wVkConstants::validationLayers.data();

			VkValidationFeatureEnableEXT enables[] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
			VkValidationFeaturesEXT features{};
			features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
			features.enabledValidationFeatureCount = 1;
			features.pEnabledValidationFeatures = enables;

//			createInfo.pNext = &features;


			populateDebugMessengerCreateInfo(debugCreateInfo);
			debugCreateInfo.pNext = &features;
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}


		VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}

		return instance;
	}
}