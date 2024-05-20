#pragma once
#include <unordered_map>

#include "vulkan/vulkan.h"

#include "wVkConstants.h"

// Naming convention
// w - wrapper
// Vk - Vulkan

enum class ShaderParameter;

struct wVkCommandList
{
	VkCommandBuffer m_CommandBuffer[wVkConstants::g_MaxFramesInFlight] = {};
};

struct wVkDescriptorHeap
{
	VkDescriptorPool m_Pool = VK_NULL_HANDLE;
	VkDescriptorSet m_Handle = VK_NULL_HANDLE;
};

enum class DataType
{
	BUFFER = 0,
	TEXTURE = 1,
	MAX_ENUM = 0xffff
};


// We do the Pipeline Construction before the first use of Dispatch
struct ShaderBindingData
{
	uint32_t m_BindingLocation = UINT32_MAX;
	DataType m_Type = DataType::MAX_ENUM;
	void* m_ResourceLocation = nullptr;  // cast from DataType
	VkDescriptorSetLayoutBinding m_Layout;
};

struct wVkPipelineLayout
{
	std::vector<ShaderBindingData> m_CurrentDescSetBindings;
	std::unordered_map<std::size_t, std::vector<ShaderBindingData>> m_BindingsCache;
	std::unordered_map<std::size_t, VkDescriptorSet> m_DescriptorSetCache;
};

struct wVkComputePipeline
{
	// Initial Set-up
	VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
	VkPipeline m_Pipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_DescSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	// Run-time
	//std::vector<VkDescriptorSet> m_DescriptorSets;
	
};

struct wVkRenderTarget
{
	VkImageLayout m_State = VK_IMAGE_LAYOUT_GENERAL;
	VkImage m_Texture = VK_NULL_HANDLE;
};

struct wVkBuffer
{
	// For uniform resources we create 2 per frame
	VkBuffer m_Buffers;
	VkDeviceMemory m_BuffersMemory;
};

struct wVkTexture2D
{
	uint32_t m_TexMipLevels = 0;
	VkImage m_TextureImage = VK_NULL_HANDLE;
	VkDeviceMemory m_TextureImageMemory = VK_NULL_HANDLE;
	VkImageView m_TextureImageView = VK_NULL_HANDLE;
};

struct wVkSampler
{
	VkSampler m_Sampler = VK_NULL_HANDLE;
};

struct wVkBLAS
{
	VkBuffer m_Scratch; // Scratch memory for AS builder
	VkBuffer m_Result; // Where the AS is
	VkBuffer m_TransformBuffer; // Hold the matrices of the instances
	// BLAS Generator object
};

struct wVkTLAS
{
	VkBuffer m_Scratch; // Scratch memory for AS builder
	VkBuffer m_Result; // Where the AS is
	VkBuffer m_InstanceDesc; // Hold the matrices of the instances
	// TLAS Generator object
};

typedef wVkPipelineLayout GPUShaderLayoutHandle;
typedef wVkBuffer GPUBufferHandle;
typedef wVkTexture2D GPUTextureHandle;
typedef wVkRenderTarget GPURenderTarget;
typedef wVkBLAS GPUBlasHandle;
typedef wVkTLAS GPUTlasDescHandle;
typedef wVkDescriptorHeap GPUDescriptorHeapHandle;
typedef wVkSampler GPUSamplerHandle;
typedef wVkCommandList GPUCommandListHandle;
typedef wVkComputePipeline GPUComputePipelineHandle;