#pragma once
#include "vulkan/vulkan.h"

#include "wVkConstants.h"

// Naming convention
// w - wrapper
// Vk - Vulkan

struct wVkBuffer
{
    // For uniform resources we create 2 per frame
    VkBuffer m_Buffers;
    VkDeviceMemory m_BuffersMemory;
};

struct wVkCommandList
{
    VkCommandBuffer m_CommandBuffer[wVkConstants::g_MaxFramesInFlight] = {};
};

struct wVkDescriptorHeap
{
    VkDescriptorPool m_Pool = VK_NULL_HANDLE;
    VkDescriptorSet m_Handle = VK_NULL_HANDLE;
};

struct wVkRootSignature
{
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
};

struct wVkSampler
{
    VkSampler m_Sampler = VK_NULL_HANDLE;
};

struct wVkComputePipeline
{
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
};

struct wVkRenderTarget
{
    VkImageLayout m_State = VK_IMAGE_LAYOUT_GENERAL;
    VkImage m_Texture = VK_NULL_HANDLE;
};

struct wVkTexture2D
{
    VkImageLayout m_State = VK_IMAGE_LAYOUT_GENERAL;
    VkImage m_Texture = VK_NULL_HANDLE;
    VkBuffer m_Uploader = VK_NULL_HANDLE;
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

typedef wVkRootSignature GPUShaderLayoutHandle;
typedef wVkBuffer GPUBufferHandle;
typedef wVkTexture2D GPUTextureHandle;
typedef wVkRenderTarget GPURenderTarget;
typedef wVkBLAS GPUBlasHandle;
typedef wVkTLAS GPUTlasDescHandle;
typedef wVkDescriptorHeap GPUDescriptorHeapHandle;
typedef wVkSampler GPUSamplerHandle;
typedef wVkCommandList GPUCommandListHandle;
typedef wVkComputePipeline GPUComputePipelineHandle;