#include "BEARHeaders/CommandList.h"

#include "wVkConstants.h"

#include "wVkGlobalVariables.h"
#include "BEARHeaders/Buffer.h"
#include "Utils/ConsoleLogger.h"
#include "wVkHelpers/wVkCommands.h"

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
	//Deallocated from Command Pool
}

void CommandList::SetDescriptorHeaps(ResourceDescriptorHeap* heapR, SamplerDescriptorHeap* heapS)
{

}

void CommandList::BindResource32BitConstants(const uint32_t layoutLocation, const void* data, const uint32_t num)
{
	// DescriptorSet + Something
}

void CommandList::BindResourceCBV(const uint32_t layoutLocation, Buffer& buffer)
{

}

void CommandList::BindResourceSRV(const uint32_t layoutLocation, Buffer& buffer)
{

}

void CommandList::BindResourceSRV(const uint32_t layoutLocation, Texture& texture)
{

}


void CommandList::BindResourceSRV(const uint32_t layoutLocation, TLAS& tlas)
{

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

}

void CommandList::BindResourceUAV(const uint32_t layoutLocation, Buffer& buffer)
{

}

void CommandList::BindResourceUAV(const uint32_t layoutLocation, Texture& texture)
{

}

void CommandList::SetComputePipeline(ComputePipelineDescription& cpd)
{

}

void CommandList::Reset()
{
}

void CommandList::Dispatch(const uint32_t numThreadGroupsX, const uint32_t numThreadGroupsY,
	const uint32_t numThreadGroupsZ, bool syncBeforeDispatch)
{

}

void CommandList::Execute()
{

}

