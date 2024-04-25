#pragma once
#include <string>
#include <vector>

#include "vulkan/vulkan.h"

namespace wVkConstants {

	// Useful variables
	constexpr uint32_t g_MaxFramesInFlight = 2; // Assuming you manage multiple frames in flight
	constexpr uint32_t g_NumSwapChainImages = 2;

	// Validation Layers
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,

		// Dependencies for Hardware RT Support:
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
		VK_KHR_SPIRV_1_4_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,

		// Hardware RT Extensions
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_RAY_QUERY_EXTENSION_NAME,
	};


#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


#define USE_HLSL 1
#ifdef USE_HLSL
	const std::string shaderDir = "Shaders/Compiled/HLSL/";
#else
	const std::string shaderDir = "Shaders/Compiled/GLSL/";
#endif

}