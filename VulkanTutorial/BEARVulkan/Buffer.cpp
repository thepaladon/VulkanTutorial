#include "BEARHeaders/Buffer.h"

#include <stdexcept>

#include "wVkGlobalVariables.h"
#include "wVkTempBuffer.h"
#include "Utils/ConsoleLogger.h"
#include "wVkHelpers/wVkCommands.h"




Buffer::Buffer(const void* data, const size_t stride, const size_t count, BufferFlags flags,
	const std::string& name)
{
	bool writeNow = true;
	bool gpuOnly = false;

	if (data == nullptr)
	{
		writeNow = false;
	}

	VkBufferUsageFlags usageFlags = 0;
	VkMemoryPropertyFlags memoryFlags = 0;

	// We always want to use it on the GPU;
	usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	if ((flags & (BufferFlags::CBV)) == (BufferFlags::CBV)) {
		usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		memoryFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		gpuOnly = false;
	}

	if (((flags & (BufferFlags::SRV)) == BufferFlags::SRV) || (flags & (BufferFlags::UAV)) == BufferFlags::UAV) {
		memoryFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		gpuOnly = true;
	}

	if ((flags & (BufferFlags::VERTEX_BUFFER)) == (BufferFlags::VERTEX_BUFFER)) {
		usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}

	if ((flags & (BufferFlags::INDEX_BUFFER)) == (BufferFlags::INDEX_BUFFER)) {
		usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}

	if ((flags & (BufferFlags::ALLOW_UA)) == (BufferFlags::ALLOW_UA)) {
		usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}

	if ((flags & (BufferFlags::UAV)) == (BufferFlags::UAV)) {
		usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}

	// Buffer options:
	// CBV = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	// CBV = Push-Constant
	// CBV = Host Coherent(as we will often update it)

	// SRV = VK_BUFFER_USAGE_TRANSFER_DST_BIT

	// UAV = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT

	// VK_BUFFER_USAGE_INDEX_BUFFER_BIT 
	// VK_BUFFER_USAGE_VERTEX_BUFFER_BIT 

	const VkDeviceSize bufferSize = stride * count;

	// Both SRV and YAV flags are set - Buffer exclusively lives on GPU
	if (gpuOnly) {

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		wVkHelpers::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* bufferData;
		vkMapMemory(wVkGlobals::g_Device, stagingBufferMemory, 0, bufferSize, 0, &bufferData);
		memcpy(bufferData, data, bufferSize);
		vkUnmapMemory(wVkGlobals::g_Device, stagingBufferMemory);

		wVkHelpers::createBuffer(bufferSize, usageFlags, memoryFlags, m_BufferHandle.m_Buffers, m_BufferHandle.m_BuffersMemory);

		wVkHelpers::copyBufferNewCmd(stagingBuffer, m_BufferHandle.m_Buffers, bufferSize);

		vkDestroyBuffer(wVkGlobals::g_Device, stagingBuffer, nullptr);
		vkFreeMemory(wVkGlobals::g_Device, stagingBufferMemory, nullptr);
	}
	else if ((int)flags & (int)(BufferFlags::CBV))
	{
		wVkHelpers::createBuffer(bufferSize, usageFlags, memoryFlags, m_BufferHandle.m_Buffers, m_BufferHandle.m_BuffersMemory);

		if (writeNow) {
			void* bufferDataMapped;
			vkMapMemory(wVkGlobals::g_Device, m_BufferHandle.m_BuffersMemory, 0, bufferSize, 0, &bufferDataMapped);
			memcpy(bufferDataMapped, data, bufferSize);
			vkUnmapMemory(wVkGlobals::g_Device, m_BufferHandle.m_BuffersMemory);
		}
	}
	else
	{
		ASSERT(false, "Buffer with flags %i Not Supported", static_cast<int>(flags));
	}

}

void Buffer::UpdateData(const void* data, size_t dataSizeInBytes)
{
	// Consider not doing this...
	void* bufferDataMapped = nullptr;
	vkMapMemory(wVkGlobals::g_Device, m_BufferHandle.m_BuffersMemory, 0, dataSizeInBytes, 0, &bufferDataMapped);
	memcpy(bufferDataMapped, data, dataSizeInBytes);
	vkUnmapMemory(wVkGlobals::g_Device, m_BufferHandle.m_BuffersMemory);
}

Buffer::~Buffer()
{
	vkDestroyBuffer(wVkGlobals::g_Device, m_BufferHandle.m_Buffers, nullptr);
	vkFreeMemory(wVkGlobals::g_Device, m_BufferHandle.m_BuffersMemory, nullptr);
}

