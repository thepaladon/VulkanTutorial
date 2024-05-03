#pragma once
#include <fstream>
#include <stdexcept>

#include "BEARVulkan/wVkGlobalVariables.h"
#include "Utils/ConsoleLogger.h"
#include "vulkan/vulkan.h"

namespace wVkHelpers
{
	inline uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(wVkGlobals::g_PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}


	inline std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		const size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}


	inline VkShaderModule createShaderModule(const std::vector<char>& code) {

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(wVkGlobals::g_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}


	inline DataType getTypeFromDescriptor(const VkDescriptorType type)
	{
		switch (type)
		{
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return DataType::BUFFER;
			break;

		default: ;
			ASSERT(false, "Not Implemented");
		}

		// Dummy
		return DataType::BUFFER;
	}


	inline ShaderBindingData createShaderBindingData(const uint32_t layoutLocation, void* resource, const VkDescriptorType type)
	{
		// ToDo checks whether there is something already bound to this slot in CommandList
		VkDescriptorSetLayoutBinding binding{};
		binding.binding = layoutLocation;
		binding.descriptorCount = 1;
		binding.descriptorType = type;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		const ShaderBindingData shaderBindingData{
			layoutLocation, 
			getTypeFromDescriptor(type),
			resource,
			binding
		};

		return shaderBindingData;
	}


}