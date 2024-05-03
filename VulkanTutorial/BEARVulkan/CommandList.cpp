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
}

void CommandList::Destroy()
{
	ASSERT(false, "Not Implemented");
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
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_BindingCache.push_back(shaderBindingData);
}

void CommandList::BindResourceSRV(const uint32_t layoutLocation, Buffer& buffer)
{
	const auto  shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &buffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_BindingCache.push_back(shaderBindingData);
}

void CommandList::BindResourceUAV(const uint32_t layoutLocation, Buffer& buffer)
{
	const auto  shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &buffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_BindingCache.push_back(shaderBindingData);
}

void CommandList::BindResourceSRV(const uint32_t layoutLocation, Texture& texture)
{
	const auto shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &texture, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_BindingCache.push_back(shaderBindingData);
}

void CommandList::BindResourceUAV(const uint32_t layoutLocation, Texture& texture)
{
	const auto  shaderBindingData = wVkHelpers::createShaderBindingData(layoutLocation, &texture, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
	g_boundPipeline->GetShaderLayoutRef().GetShaderLayoutHandleRef().m_BindingCache.push_back(shaderBindingData);
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
}

void CommandList::Reset()
{
	ASSERT(false, "Not Implemented");

}

void CommandList::Dispatch(int currentFrame, const uint32_t numThreadGroupsX, const uint32_t numThreadGroupsY,
	const uint32_t numThreadGroupsZ, bool syncBeforeDispatch)
{
	auto& shaderLayout = g_boundPipeline->GetShaderLayoutRef();
	
	// Create all required Descriptor Layout and Sets
	const auto& shaderParams = shaderLayout.GetShaderLayoutHandleRef().m_BindingCache;

	// CREATE DESCRIPTOR SET LAYOUT -------------
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	bindings.reserve(shaderParams.size());
	int numUniformBuffers = 0;
	int numStorageBuffers = 0;
	for(auto test : shaderParams)
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

	auto& boundPipeline = g_boundPipeline->GetPipelineHandleRef();

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

	// CREATE DESCRIPTOR SETS ----------
	const std::vector<VkDescriptorSetLayout> layouts(wVkConstants::g_MaxFramesInFlight, boundPipeline.m_DescSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = boundPipeline.m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight);
	allocInfo.pSetLayouts = layouts.data();

	boundPipeline.m_DescriptorSets.resize(wVkConstants::g_MaxFramesInFlight);
	if (vkAllocateDescriptorSets(wVkGlobals::g_Device, &allocInfo, boundPipeline.m_DescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("1 failed to allocate compute descriptor sets!");
	}

	std::vector<VkWriteDescriptorSet> descriptorWrites;

	for (auto test : shaderParams)
	{
		// For now we only work with buffers
		Buffer* buffer = static_cast<Buffer*>(test.m_Resource);

		VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
		bufferInfo->buffer = buffer->GetGPUHandleRef().m_Buffers; //deltaTime
		bufferInfo->offset = 0;
		bufferInfo->range = buffer->GetSizeBytes();

		VkWriteDescriptorSet descWrite{};
		descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrite.dstSet = boundPipeline.m_DescriptorSets[currentFrame];
		descWrite.dstBinding = test.m_BindingLocation;
		descWrite.dstArrayElement = 0;
		descWrite.descriptorType = test.m_Layout.descriptorType;
		descWrite.descriptorCount = 1;
		descWrite.pBufferInfo = bufferInfo;

		descriptorWrites.push_back(descWrite);
	}

	vkUpdateDescriptorSets(wVkGlobals::g_Device, (uint32_t)shaderParams.size(), descriptorWrites.data(), 0, nullptr);

	for(auto& desc : descriptorWrites)
	{
		delete desc.pBufferInfo;
	}

	// ToDo
	// END CREATE DESCRIPTOR SETS -------

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


	// ToDo : Command Buffer indexing, won't work without it
	const VkCommandBuffer commandBuffer = m_CmdListHandle.m_CommandBuffer[currentFrame];

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, boundPipeline.m_Pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, boundPipeline.m_PipelineLayout, 0, 1, &boundPipeline.m_DescriptorSets[currentFrame], 0, 0);

	vkCmdDispatch(commandBuffer, numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);

}

void CommandList::Execute()
{

}

