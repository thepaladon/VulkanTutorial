#pragma once

#include <stdexcept>

#include "wVkQueueFamilies.h"
#include "BEARVulkan/wVkGlobalVariables.h"
#include "vulkan/vulkan.h"

namespace wVkHelpers
{
	inline VkCommandBuffer beginSingleTimeCommand()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		// ToDo Optimize this
		allocInfo.commandPool = wVkGlobals::g_CommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(wVkGlobals::g_Device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	inline  void endSingleTimeCommand(VkCommandBuffer& commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(wVkGlobals::g_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(wVkGlobals::g_GraphicsQueue);

		vkFreeCommandBuffers(wVkGlobals::g_Device, wVkGlobals::g_CommandPool, 1, &commandBuffer);
	}

	inline VkCommandPool createCommandPool() {
		const QueueFamilyIndices queueFamilyIndices = findQueueFamilies(wVkGlobals::g_PhysicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		VkCommandPool cmdPool;
		if (vkCreateCommandPool(wVkGlobals::g_Device, &poolInfo, nullptr, &cmdPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}

		return cmdPool;
	}

}