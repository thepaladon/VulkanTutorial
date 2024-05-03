#include "BEARHeaders/ShaderLayout.h"

#include <stdexcept>

#include "wVkGlobalVariables.h"
#include "Utils/ConsoleLogger.h"


void ShaderLayout::Initialize()
{
}

void ShaderLayout::AddParameter(ShaderParameter type)
{

}

void ShaderLayout::Add32bitConstParameter(int num32bit)
{
	ASSERT(false, "Add32bitConstParameter Not yet implemented");
	//m_ShaderLayout.m_Parameters.push_back(num32bit);
}

void ShaderLayout::AddParameters(ShaderParameter type, int num)
{

}