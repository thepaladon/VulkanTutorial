#pragma once

#include <vector>

#include "Sampler.h"
#include "BEARVulkan/TypeDefs.h"


class SamplerDescriptorHeap
{
public:
	SamplerDescriptorHeap() = default;
	~SamplerDescriptorHeap() = default;

	void Initialize(int maxNumberResources);

	// Pushes a sampler to the heap
	int AddSampler(Sampler& sampler);
	// Switches a resource in the heap to the new one
	void SwitchSampler(Sampler& newSampler, int heapID);

	GPUDescriptorHeapHandle& GetDescriptorHeapHandleRef() { return m_DescriptorHeapHandle; }

	// Get size of texture element string vector array
	int GetSamplerStateCount() { return static_cast<int>(m_SamplerStates.size()); }

	// Function is inside header because then we dont need two implementations for the two platforms
	// Get string of the texture by id
	SamplerState GetSamplerState(int index)
	{
		SamplerState samplerState = SamplerState();

		// Check if index is valid
		if (index >= 0 && index < m_SamplerStates.size())
		{
			samplerState = m_SamplerStates.at(index);
		}

		return samplerState;
	}

private:
	GPUDescriptorHeapHandle m_DescriptorHeapHandle;
	int m_NumElements = 0;
	[[maybe_unused]] int m_MaxSize = 0;

	// Array that stores the samplers with their sampling states
	std::vector<SamplerState> m_SamplerStates;
};

