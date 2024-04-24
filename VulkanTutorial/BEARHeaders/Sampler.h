#pragma once

#include <string>

#include "BEARVulkan/TypeDefs.h"


enum class MinFilter
{
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR
};

enum class MagFilter
{
	NEAREST,
	LINEAR
};

enum class WrapUV
{
	CLAMP_TO_EDGE,
	MIRRORED_REPEAT,
	REPEAT
};

struct SamplerState
{
	SamplerState() { valid = false; }
	SamplerState(MinFilter minFilter, MagFilter magFilter, WrapUV wrapUV)
	{
		MinFilter = minFilter;
		MagFilter = magFilter;
		WrapUV = wrapUV;
		valid = true;
	}
	std::string GetMinFilter()
	{
		std::string returnString = "null";
		switch (MinFilter)
		{
		case MinFilter::NEAREST:
			returnString = "nearest";
			break;
		case MinFilter::LINEAR:
			returnString = "linear";
			break;
		case MinFilter::NEAREST_MIPMAP_NEAREST:
			returnString = "nearest mipmap nearest";
			break;
		case MinFilter::LINEAR_MIPMAP_NEAREST:
			returnString = "linear mipmap nearest";
			break;
		case MinFilter::NEAREST_MIPMAP_LINEAR:
			returnString = "nearest mipmap linear";
			break;
		case MinFilter::LINEAR_MIPMAP_LINEAR:
			returnString = "linear mipmap linear";
			break;
		}

		return returnString;
	}

	std::string GetMapFilter()
	{
		std::string returnString = "null";

		switch (MagFilter)
		{
		case MagFilter::NEAREST:
			returnString = "nearest";
			break;
		case MagFilter::LINEAR:
			returnString = "linear";
			break;
		}

		return returnString;
	}

	std::string GetWrapUV()
	{
		std::string returnString = "null";

		switch (WrapUV)
		{
		case WrapUV::CLAMP_TO_EDGE:
			returnString = "clamp to edge";
			break;
		case WrapUV::MIRRORED_REPEAT:
			returnString = "mirrored repeat";
			break;
		case WrapUV::REPEAT:
			returnString = "repeat";
			break;
		}

		return returnString;
	}

	bool valid = false;
	MinFilter MinFilter;
	MagFilter MagFilter;
	WrapUV WrapUV;
};

// Define a generic GPU sampler class
class Sampler
{
public:
	// Constructor and destructor
	Sampler(MinFilter minFilter, MagFilter magFilter, WrapUV wrapUV);
	~Sampler() = default;

	GPUSamplerHandle GetGPUHandle() const { return m_SamplerHandle; }
	GPUSamplerHandle& GetGPUHandleRef() { return m_SamplerHandle; }
	SamplerState GetSamplerState() { return m_SamplerState; }

private:
	GPUSamplerHandle m_SamplerHandle;
	SamplerState m_SamplerState;
};

