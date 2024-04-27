#include "BEARHeaders/Texture.h"

#include <stdexcept>
#include <cmath>

#include "wVkGlobalVariables.h"
#include "Utils/ConsoleLogger.h"
#include "wVkHelpers/wVkHelpers.h"
#include "wVkHelpers/wVkTemp.h"
#include "wVkHelpers/wVkTexture.h"

VkImageUsageFlags DetermineImageUsageFlags(TextureType type) {
	VkImageUsageFlags flags = VK_IMAGE_USAGE_SAMPLED_BIT; // Basic flag for all textures to be readable

	switch (type) {
	case TextureType::RENDER_TARGET:
		flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		break;
	case TextureType::R_TEXTURE:
		// No additional flags needed
		break;
	case TextureType::RW_TEXTURE:
		flags |= VK_IMAGE_USAGE_STORAGE_BIT;
		break;
	default:
		ASSERT(false, "Using a TextureType that's not supported");
	}

	// Flags for creating an image and sampling it 
	flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; 

	return flags;
}


VkFormat GetVulkanFormat(TextureFormat format) {
	switch (format) {
	case TextureFormat::R8G8B8A8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case TextureFormat::R8G8B8A8_SNORM:
		return VK_FORMAT_R8G8B8A8_SNORM;
	case TextureFormat::R32_FLOAT:
		return VK_FORMAT_R32_SFLOAT;
	case TextureFormat::R32_G32_FLOAT:
		return VK_FORMAT_R32G32_SFLOAT;
	case TextureFormat::R32_G32_B32_A32_FLOAT:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case TextureFormat::R8G8B8A8_SRGB:
		return VK_FORMAT_R8G8B8A8_SRGB;

	default:
		ASSERT(false, "Using a flag that's not supported");
		return VK_FORMAT_UNDEFINED;
	}
}


// Internal, as not to be confused with the temp one in wVkTempBuffer
void createImage2DInternal(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional

	if (vkCreateImage(wVkGlobals::g_Device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(wVkGlobals::g_Device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = wVkHelpers::findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(wVkGlobals::g_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(wVkGlobals::g_Device, image, imageMemory, 0);
}

Texture::Texture(const void* data, TextureSpec spec, const std::string& name) : m_Spec(spec), m_Name(name)
{
	const bool generateMips = (spec.m_Flags & TextureFlags::MIPMAP_GENERATE) == (TextureFlags::MIPMAP_GENERATE);

	auto& mips = m_TextureHandle.m_TexMipLevels;
	if (generateMips)
		mips = static_cast<uint32_t>(std::floor(std::log2(std::max(spec.m_Width, spec.m_Height)))) + 1;
	else
		mips = 1;

	m_Channels = 4;
	m_BytesPerChannel = 1;
	m_SizeInBytes = spec.m_Width * spec.m_Height * m_Channels;

	auto format = GetVulkanFormat(spec.m_Format);
	auto usageFlags = DetermineImageUsageFlags(spec.m_Type);



	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	wVkHelpers::createBuffer(m_SizeInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* dataMappedData;
	vkMapMemory(wVkGlobals::g_Device, stagingBufferMemory, 0, m_SizeInBytes, 0, &dataMappedData);
	memcpy(dataMappedData, data, static_cast<size_t>(m_SizeInBytes));
	vkUnmapMemory(wVkGlobals::g_Device, stagingBufferMemory);

	createImage2DInternal(spec.m_Width, spec.m_Height, mips, format, VK_IMAGE_TILING_OPTIMAL, usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureHandle.m_TextureImage, m_TextureHandle.m_TextureImageMemory);

	// Transition image to a state for data to get INTO it
	wVkHelpers::transitionImageLayout(m_TextureHandle.m_TextureImage, format, mips, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy Buffer to that image
	wVkHelpers::copyBufferToImage(stagingBuffer, m_TextureHandle.m_TextureImage, static_cast<uint32_t>(spec.m_Width), static_cast<uint32_t>(spec.m_Height));

	// generateMipmaps Transitions the image, if not, we do it manually.
	if (generateMips)
		wVkHelpers::generateMipmaps(m_TextureHandle.m_TextureImage, format, spec.m_Width, spec.m_Height, mips);
	else
		wVkHelpers::transitionImageLayout(m_TextureHandle.m_TextureImage, format, 1, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_TextureHandle.m_TextureImageView = wVkHelpers::createImageView(m_TextureHandle.m_TextureImage, mips, format, VK_IMAGE_ASPECT_COLOR_BIT);

	vkDestroyBuffer(wVkGlobals::g_Device, stagingBuffer, nullptr);
	vkFreeMemory(wVkGlobals::g_Device, stagingBufferMemory, nullptr);
}

void Texture::UpdateTexture(const void* data)
{


}

void Texture::ResizeTexture(int newWidth, int newHeight)
{

}

Texture::~Texture()
{
	vkDestroyImageView(wVkGlobals::g_Device, m_TextureHandle.m_TextureImageView, nullptr);
	vkDestroyImage(wVkGlobals::g_Device, m_TextureHandle.m_TextureImage, nullptr);
	vkFreeMemory(wVkGlobals::g_Device, m_TextureHandle.m_TextureImageMemory, nullptr);
}
