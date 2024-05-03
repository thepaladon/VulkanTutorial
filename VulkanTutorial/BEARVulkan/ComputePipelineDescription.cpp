#include "BEARHeaders/ComputePipelineDescription.h"

#include "wVkGlobalVariables.h"
#include "wVkHelpers/wVkHelpers.h"

void ComputePipelineDescription::Initialize(const std::string& shaderName, ShaderLayout& layout)
{
	// Shader Init;
	const auto computeShaderCode = wVkHelpers::readFile(shaderName);
	m_PipelineHandle.m_ShaderModule = wVkHelpers::createShaderModule(computeShaderCode);

	// The Rest is created and cached before Dispatch:
	// - Descriptor Pool
	// - Descriptor Sets & Layouts
	// - Pipeline
}

void ComputePipelineDescription::Destroy()
{
	vkDestroyShaderModule(wVkGlobals::g_Device, m_PipelineHandle.m_ShaderModule, nullptr);
	vkDestroyPipeline(wVkGlobals::g_Device, m_PipelineHandle.m_Pipeline, nullptr);
	vkDestroyPipelineLayout(wVkGlobals::g_Device, m_PipelineHandle.m_PipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(wVkGlobals::g_Device, m_PipelineHandle.m_DescSetLayout, nullptr);
	vkDestroyDescriptorPool(wVkGlobals::g_Device, m_PipelineHandle.m_DescriptorPool, nullptr);

}


