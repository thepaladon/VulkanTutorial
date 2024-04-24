#pragma once
#include <string>

#include "ShaderLayout.h"
#include "BEARVulkan/TypeDefs.h"


class ComputePipelineDescription 
{
public:
	ComputePipelineDescription() = default;
	~ComputePipelineDescription() = default;

	void Initialize(const std::string& shaderName, ShaderLayout& layout);
	GPUComputePipelineHandle& GetPipelineHandleRef() { return m_PipelineHandle; }
	ShaderLayout GetShaderLayout() const { return m_ShaderLayout; }
	ShaderLayout& GetShaderLayoutRef() { return m_ShaderLayout; }

private:
	ShaderLayout m_ShaderLayout;
	GPUComputePipelineHandle m_PipelineHandle;

	std::string m_ShaderName = "NOT_INITIALIZED";
};

