#pragma once
#include <fstream>
#include <stdexcept>

#include "../TypeDefs.h"

#include "vulkan/vulkan.h"

#include "wVkDepth.h"
#include "wVkSwapchain.h"
#include "BEARVulkan/wVkGlobalVariables.h"
#include "Utils/ConsoleLogger.h"


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

		default:;
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

	// ToDo: Hashing might be faster than doing this
	inline bool compareShaderBindingData(const ShaderBindingData& a, const ShaderBindingData& b) {
		return a.m_BindingLocation == b.m_BindingLocation &&
			a.m_Type == b.m_Type &&
			a.m_ResourceLocation == b.m_ResourceLocation &&
			a.m_Layout.binding == b.m_Layout.binding &&
			a.m_Layout.descriptorType == b.m_Layout.descriptorType &&
			a.m_Layout.descriptorCount == b.m_Layout.descriptorCount &&
			a.m_Layout.stageFlags == b.m_Layout.stageFlags;
	}

	inline bool compareShaderBindingDataVectors(const std::vector<ShaderBindingData>& vec1, const std::vector<ShaderBindingData>& vec2) {
		if (vec1.size() != vec2.size()) {
			return false;
		}

		for (std::size_t i = 0; i < vec1.size(); ++i) {
			if (!compareShaderBindingData(vec1[i], vec2[i])) {
				return false;
			}
		}

		return true;
	}

	// Hash function for VkDescriptorSetLayoutBinding
	inline std::size_t hashVkDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& binding) {
		std::size_t hash = std::hash<uint32_t>{}(binding.binding);
		hash ^= std::hash<uint32_t>{}(binding.descriptorType) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint32_t>{}(binding.descriptorCount) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint32_t>{}(binding.stageFlags) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}

	// Hash function for ShaderBindingData
	inline std::size_t hashShaderBindingData(const ShaderBindingData& data) {
		std::size_t hash = std::hash<uint32_t>{}(data.m_BindingLocation);
		hash ^= std::hash<int>{}(static_cast<int>(data.m_Type)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<std::uintptr_t>{}(reinterpret_cast<std::uintptr_t>(data.m_ResourceLocation)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= hashVkDescriptorSetLayoutBinding(data.m_Layout) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}

	// Hash function for an array of ShaderBindingData
	inline std::size_t hashArrayOfShaderBindingData(const std::vector<ShaderBindingData>& dataArray) {
		std::size_t combinedHash = 0;
		for (const auto& data : dataArray) {
			combinedHash ^= hashShaderBindingData(data) + 0x9e3779b9 + (combinedHash << 6) + (combinedHash >> 2);
		}
		return combinedHash;
	}


	inline VkRenderPass createRenderPass()
	{
		// same as in wVkCreateSwapchain
		const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(wVkGlobals::g_PhysicalDevice);
		const VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = surfaceFormat.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;   // No Stencil yet
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // No Stencil yet

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // ImGui sets it to "Present" 
		// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
		// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = wVkHelpers::findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Sub-passes and attachments
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		// The index of the attachment in this array is directly referenced from the fragment shader with the layout(location = 0) out vec4 outColor directive!

		// This dependency is for vkAcquireNextImageKHR if I understood
		// We want to wait for the swapchain to finish reading the image
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(std::size(attachments));
		renderPassInfo.pAttachments = &attachments[0];
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkRenderPass renderpass;
		if (vkCreateRenderPass(wVkGlobals::g_Device, &renderPassInfo, nullptr, &renderpass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		return renderpass;
	}


}