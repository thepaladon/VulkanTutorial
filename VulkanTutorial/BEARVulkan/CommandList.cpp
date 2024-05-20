#include "BEARHeaders/CommandList.h"

#include "wVkConstants.h"

#include "wVkGlobalVariables.h"
#include "BEARHeaders/Buffer.h"
#include "BEARHeaders/ComputePipelineDescription.h"
#include "BEARHeaders/Texture.h"
#include "Utils/ConsoleLogger.h"
#include "wVkHelpers/wVkCommands.h"
#include "wVkHelpers/wVkHelpers.h"

static ComputePipelineDescription* g_boundPipeline;

CommandList::~CommandList()
{
}

void CommandList::Initialize()
{
	VkCommandBufferAllocateInfo compAllocInfo{};
	compAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	compAllocInfo.commandPool = wVkGlobals::g_CommandPool;
	compAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	compAllocInfo.commandBufferCount = wVkConstants::g_MaxFramesInFlight;

	if (vkAllocateCommandBuffers(wVkGlobals::g_Device, &compAllocInfo, m_CmdListHandle.m_CommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	// Create sync objects
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; //To make sure we get to our first frame

	for (size_t i = 0; i < wVkConstants::g_MaxFramesInFlight; i++) {
		if (vkCreateSemaphore(wVkGlobals::g_Device, &semaphoreInfo, nullptr, &m_CmdListHandle.m_FinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(wVkGlobals::g_Device, &fenceInfo, nullptr, &m_CmdListHandle.m_InFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute synchronization objects for a frame!");
		}
	}

}

void CommandList::Destroy()
{
	vkFreeCommandBuffers(wVkGlobals::g_Device, wVkGlobals::g_CommandPool, wVkConstants::g_MaxFramesInFlight, m_CmdListHandle.m_CommandBuffer);

	for (size_t i = 0; i < wVkConstants::g_MaxFramesInFlight; i++) {
		vkDestroySemaphore(wVkGlobals::g_Device, m_CmdListHandle.m_FinishedSemaphores[i], nullptr);
		vkDestroyFence(wVkGlobals::g_Device, m_CmdListHandle.m_InFlightFences[i], nullptr);
	}
}

void CommandList::SetDescriptorHeaps(ResourceDescriptorHeap* heapR, SamplerDescriptorHeap* heapS)
{
	ASSERT(false, "Not Implemented");
}

void CommandList::BindResource32BitConstants(const uint32_t layoutLocation, const void* data, const uint32_t num)
{
	ASSERT(false, "Not Implemented");
}

void CommandList::BindResourceCBV(const uint32_t layoutLocation, Buffer& buffer)
{
	auto shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &buffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

	// ToDo checks whether there is something already bound to this slot.
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_CurrentDescSetBindings.push_back(shaderBindingData);
}

void CommandList::BindResourceSRV(const uint32_t layoutLocation, Buffer& buffer)
{
	const auto  shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &buffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_CurrentDescSetBindings.push_back(shaderBindingData);
}

void CommandList::BindResourceUAV(const uint32_t layoutLocation, Buffer& buffer)
{
	const auto  shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &buffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_CurrentDescSetBindings.push_back(shaderBindingData);
}

void CommandList::BindResourceSRV(const uint32_t layoutLocation, Texture& texture)
{
	const auto shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &texture, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_CurrentDescSetBindings.push_back(shaderBindingData);
}

void CommandList::BindResourceUAV(const uint32_t layoutLocation, Texture& texture)
{
	const auto  shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &texture, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_CurrentDescSetBindings.push_back(shaderBindingData);
}

void CommandList::BindResourceSRV(const uint32_t layoutLocation, TLAS& tlas)
{
	ASSERT(false, "Not Implemented");
}

void CommandList::CopyResource(Buffer& bufferDst, Buffer& bufferSrc)
{
	ASSERT(bufferSrc.GetSizeBytes() == bufferDst.GetSizeBytes(), "Buffers must be equal in size");

	// REMEMBER TO FIX THIS 
	const VkCommandBuffer commandBuffer = m_CmdListHandle.m_CommandBuffer[1];

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = bufferSrc.GetSizeBytes();
	vkCmdCopyBuffer(commandBuffer, bufferSrc.GetGPUHandleRef().m_Buffers, bufferDst.GetGPUHandleRef().m_Buffers, 1, &copyRegion);

	//wVkHelpers::endSingleTimeCommand(commandBuffer);
}


void CommandList::CopyResource(Texture& textureDst, Texture& textureSrc)
{
	ASSERT(false, "Not Implemented");

}

void CommandList::SetComputePipeline(ComputePipelineDescription& cpd)
{
	g_boundPipeline = &cpd;
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_CurrentDescSetBindings.clear();
}

void CommandList::Reset()
{
	ASSERT(false, "Not Implemented");
}

void CommandList::Dispatch(const uint32_t numThreadGroupsX, const uint32_t numThreadGroupsY,
	const uint32_t numThreadGroupsZ, bool syncBeforeDispatch)
{
	auto& shaderLayout = g_boundPipeline->GetShaderLayoutRef();
	auto& boundPipeline = g_boundPipeline->GetPipelineHandleRef();

	const auto& shaderParams = shaderLayout.GetShaderLayoutHandleRef().m_CurrentDescSetBindings;

	static bool doOnce = true;
	if (doOnce) {
		// CREATE DESCRIPTOR SET LAYOUT -------------
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		bindings.reserve(shaderParams.size());
		int numUniformBuffers = 0;
		int numStorageBuffers = 0;
		for (auto test : shaderParams)
		{
			bindings.push_back(test.m_Layout);

			if (test.m_Layout.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				numUniformBuffers++;

			if (test.m_Layout.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
				numStorageBuffers++;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(wVkGlobals::g_Device, &layoutInfo, nullptr, &boundPipeline.m_DescSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute descriptor set layout!");
		}
		// END CREATE DESCRIPTOR SET LAYOUT -------------


		// CREATE DESCRIPTOR POOL -----------------------------
		std::vector<VkDescriptorPoolSize> poolSizes(2);
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight) * numUniformBuffers;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight) * numStorageBuffers;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight * 1);

		if (vkCreateDescriptorPool(wVkGlobals::g_Device, &poolInfo, nullptr, &boundPipeline.m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
		// END DESCRIPTOR POOL CREATION -----------------------------

		// CREATE PIPELINE -----------------
		VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
		computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		computeShaderStageInfo.module = boundPipeline.m_ShaderModule;
		computeShaderStageInfo.pName = "main";

		VkDescriptorSetLayout layout[] = { boundPipeline.m_DescSetLayout };
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(std::size(layout));
		pipelineLayoutInfo.pSetLayouts = layout;

		if (vkCreatePipelineLayout(wVkGlobals::g_Device, &pipelineLayoutInfo, nullptr, &boundPipeline.m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline layout!");
		}

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = boundPipeline.m_PipelineLayout;
		pipelineInfo.stage = computeShaderStageInfo;


		if (vkCreateComputePipelines(wVkGlobals::g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &boundPipeline.m_Pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline!");
		}
		// END CREATE PIPELINE -----------------

		doOnce = false;
	}

	auto& cache = shaderLayout.GetShaderLayoutHandleRef().m_BindingsCache;
	auto hash = wVkHelpers::hashArrayOfShaderBindingData(shaderParams);

	VkDescriptorSet currentFramesDescriptorSet;

	auto it = cache.find(hash);
	if (it == cache.end()) {

		// CREATE DESCRIPTOR SETS ----------
		const std::vector<VkDescriptorSetLayout> layouts(wVkConstants::g_MaxFramesInFlight, boundPipeline.m_DescSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		VkDescriptorSet descSet;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = boundPipeline.m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(wVkGlobals::g_Device, &allocInfo, &descSet) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate compute descriptor sets!");
		}

		// Try to get an existing cache heap
		std::vector<VkWriteDescriptorSet> descriptorWrites;

		for (auto bindingData : shaderParams)
		{
			// For now we only work with buffers
			Buffer* buffer = static_cast<Buffer*>(bindingData.m_ResourceLocation);

			VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
			bufferInfo->buffer = buffer->GetGPUHandleRef().m_Buffers; //deltaTime
			bufferInfo->offset = 0;
			bufferInfo->range = buffer->GetSizeBytes();

			VkWriteDescriptorSet descWrite{};
			descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descWrite.dstSet = descSet;
			descWrite.dstBinding = bindingData.m_BindingLocation;
			descWrite.dstArrayElement = 0;
			descWrite.descriptorType = bindingData.m_Layout.descriptorType;
			descWrite.descriptorCount = 1;
			descWrite.pBufferInfo = bufferInfo;

			descriptorWrites.push_back(descWrite);
		}

		vkUpdateDescriptorSets(wVkGlobals::g_Device, (uint32_t)shaderParams.size(), descriptorWrites.data(), 0, nullptr);

		for (auto& desc : descriptorWrites)
		{
			delete desc.pBufferInfo;
		}

		auto& descSetCache = shaderLayout.GetShaderLayoutHandleRef().m_DescriptorSetCache;

		descSetCache[hash] = descSet;
		cache[hash] = shaderParams;
		currentFramesDescriptorSet = descSet;

		// Check to make sure we're not going above our Descriptor Set Pool
		assert(wVkConstants::g_MaxDecriptorSets >= descSetCache.size());

	}else
	{
		currentFramesDescriptorSet = shaderLayout.GetShaderLayoutHandleRef().m_DescriptorSetCache[hash];
	}
	// END CREATE DESCRIPTOR SETS -------

	const VkCommandBuffer commandBuffer = m_CmdListHandle.m_CommandBuffer[m_FrameIndex];

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, boundPipeline.m_Pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, boundPipeline.m_PipelineLayout, 0, 1, &currentFramesDescriptorSet, 0, 0);
	vkCmdDispatch(commandBuffer, numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);
}

VkSemaphore& CommandList::GetSyncObject()
{
	return m_CmdListHandle.m_FinishedSemaphores[m_FrameIndex];
}

void CommandList::Begin(uint32_t test)
{
	//m_FrameIndex = (m_FrameIndex + 1) % wVkConstants::g_MaxFramesInFlight;
	m_FrameIndex = test;

	const auto& fence = m_CmdListHandle.m_InFlightFences[m_FrameIndex];
	vkWaitForFences(wVkGlobals::g_Device, 1, &fence, VK_TRUE, UINT64_MAX);

	vkResetFences(wVkGlobals::g_Device, 1, &fence);

	const auto& cb = m_CmdListHandle.m_CommandBuffer[m_FrameIndex];
	vkResetCommandBuffer(cb, /*VkCommandBufferResetFlagBits*/ 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(cb, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
}

void CommandList::Execute()
{
	const auto& cb = m_CmdListHandle.m_CommandBuffer[m_FrameIndex];
	const auto& semaph = m_CmdListHandle.m_FinishedSemaphores[m_FrameIndex];
	const auto& fence = m_CmdListHandle.m_InFlightFences[m_FrameIndex];

	VkSubmitInfo compSubmitInfo{};
	compSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	compSubmitInfo.commandBufferCount = 1;
	compSubmitInfo.pCommandBuffers = &cb;
	compSubmitInfo.signalSemaphoreCount = 1;
	compSubmitInfo.pSignalSemaphores = &semaph;

	if (vkEndCommandBuffer(cb) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

	if (vkQueueSubmit(wVkGlobals::g_ComputeQueue, 1, &compSubmitInfo, fence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit compute command buffer!");
	}

}

