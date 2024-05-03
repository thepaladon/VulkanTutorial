#pragma once
#include <stdexcept>

#include "BEARVulkan/wVkGlobalVariables.h"
#include "vulkan/vulkan.h"

namespace wVkHelpers
{
	inline VkPipeline cretePipeline(VkPipelineShaderStageCreateInfo shader, VkDescriptorSetLayout* layout, VkPipelineLayout pipelineLayout)
	{
		VkPipelineLayout test;
		VkPipeline pipeline;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = layout;

		if (vkCreatePipelineLayout(wVkGlobals::g_Device, &pipelineLayoutInfo, nullptr, &test) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline layout!");
		}

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = test;
		pipelineInfo.stage = shader;

		if (vkCreateComputePipelines(wVkGlobals::g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline!");
		}

		return pipeline;
	}
}