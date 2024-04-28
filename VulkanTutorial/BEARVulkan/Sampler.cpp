#include "BEARHeaders/Sampler.h"

#include <stdexcept>

#include "wVkGlobalVariables.h"
#include "Utils/ConsoleLogger.h"

Sampler::Sampler(MinFilter minFilter, MagFilter magFilter, WrapUV wrapUV)
{
    VkFilter vkMinFilter, vkMagFilter;
    VkSamplerMipmapMode mipmapMode;
    VkSamplerAddressMode vkWrapU, vkWrapV, vkWrapW;

    // Map MinFilter enum to Vulkan types
    switch (minFilter) {
    case MinFilter::NEAREST:
        vkMinFilter = VK_FILTER_NEAREST;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case MinFilter::LINEAR:
        vkMinFilter = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case MinFilter::NEAREST_MIPMAP_NEAREST:
        vkMinFilter = VK_FILTER_NEAREST;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case MinFilter::LINEAR_MIPMAP_NEAREST:
        vkMinFilter = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case MinFilter::NEAREST_MIPMAP_LINEAR:
        vkMinFilter = VK_FILTER_NEAREST;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;
    case MinFilter::LINEAR_MIPMAP_LINEAR:
        vkMinFilter = VK_FILTER_LINEAR;
        mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;
    default:
        ASSERT(false, "Min Filter case not implemented");
    }

    // Map MagFilter enum to Vulkan types
    switch (magFilter) {
    case MagFilter::NEAREST:
        vkMagFilter = VK_FILTER_NEAREST;
        break;
    case MagFilter::LINEAR:
        vkMagFilter = VK_FILTER_LINEAR;
        break;
    default:
        ASSERT(false, "Mag Filter case not implemented");
    }

    // Map WrapUV enum to Vulkan types
    switch (wrapUV) {
    case WrapUV::CLAMP_TO_EDGE:
        vkWrapU = vkWrapV = vkWrapW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        break;
    case WrapUV::MIRRORED_REPEAT:
        vkWrapU = vkWrapV = vkWrapW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        break;
    case WrapUV::REPEAT:
        vkWrapU = vkWrapV = vkWrapW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
    default:
        ASSERT(false, "Wrap UV case not implemented");
    }

    //  Remove this if you're planning on making millions of samplers
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(wVkGlobals::g_PhysicalDevice, &properties);

    // Create sampler
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = vkMagFilter;
    samplerInfo.minFilter = vkMinFilter;
    samplerInfo.addressModeU = vkWrapU;
    samplerInfo.addressModeV = vkWrapV;
    samplerInfo.addressModeW = vkWrapW;
    samplerInfo.mipmapMode = mipmapMode;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

    VkSampler sampler;
    if (vkCreateSampler(wVkGlobals::g_Device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler!");
    }

    m_SamplerHandle.m_Sampler = sampler;
    m_SamplerState = SamplerState{
        minFilter,
        magFilter,
        wrapUV,
    };
}

void Sampler::Destroy() 
{
    vkDestroySampler(wVkGlobals::g_Device, m_SamplerHandle.m_Sampler, nullptr);
}

