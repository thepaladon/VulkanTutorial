#include "BEARHeaders/CommandList.h"

CommandList::~CommandList()
{
}

void CommandList::Initialize()
{

}

void CommandList::Destroy()
{

}

void CommandList::SetDescriptorHeaps(ResourceDescriptorHeap* heapR, SamplerDescriptorHeap* heapS)
{

}

void CommandList::BindResource32BitConstants(const uint32_t layoutLocation, const void* data, const uint32_t num)
{

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

