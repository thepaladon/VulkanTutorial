#include "Utils/ConsoleLogger.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <set>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdint>		// Necessary for uint32_t
#include <limits>		// Necessary for std::numeric_limits
#include <algorithm>	// Necessary for std::clamp
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <vector>

#include "ImGuizmo.h"

#include "Utils/FreeCamera.h"

#include <random>

#include "framework.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "BEARHeaders/BackEndRenderer.h"
#include "BEARHeaders/BLAS.h"
#include "BEARHeaders/Buffer.h"
#include "BEARHeaders/CommandList.h"
#include "BEARHeaders/ComputePipelineDescription.h"
#include "BEARHeaders/ResourceDescriptorHeap.h"
#include "BEARHeaders/Sampler.h"
#include "BEARHeaders/SamplerDescriptorHeap.h"
#include "BEARHeaders/ShaderLayout.h"
#include "BEARHeaders/Texture.h"
#include "BEARHeaders/TLAS.h"

#include "BEARVulkan/TypeDefs.h"
#include "BEARVulkan/wVkGlobalVariables.h"
#include "BEARVulkan/wVkHelpers/wVkCommands.h"
#include "BEARVulkan/wVkHelpers/wVkDepth.h"
#include "BEARVulkan/wVkHelpers/wVkImGui.h"
#include "BEARVulkan/wVkHelpers/wVkInstance.h"
#include "BEARVulkan/wVkHelpers/wVkQueueFamilies.h"
#include "BEARVulkan/wVkHelpers/wVkSwapchain.h"
#include "BEARVulkan/wVkHelpers/wVkTemp.h"
#include "BEARVulkan/wVkHelpers/wVkTexture.h"


constexpr uint32_t WIDTH = 1600;
constexpr uint32_t HEIGHT = 1200;



// Goofy but keep it global for now
// The annoyances of working with this "HelloTriangleApplication" class
std::vector<VkDynamicState> g_dynamicStatesOps = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
};


constexpr int PARTICLE_COUNT = (5000 * 256);
struct Particle {
	glm::vec3 position;
	float pad0;
	glm::vec3 color;
	float pad1;
	glm::vec3 velocity;
	float pad2;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Particle);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Particle, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Particle, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Particle, velocity);


		return attributeDescriptions;
	}

};


struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, uv);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

const std::vector<Vertex> g_vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> g_indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

 void EditTransform(FreeCamera& camera, glm::mat4* matrix)
{
	// Settings / Default Params
	constexpr ImGuiKey translateKey = ImGuiKey_1;
	constexpr ImGuiKey rotateKey = ImGuiKey_2;
	constexpr ImGuiKey scaleKey = ImGuiKey_3;
	constexpr ImGuiKey universalKey = ImGuiKey_4;
	constexpr ImGuiKey snapTogglKey = ImGuiKey_GraveAccent;

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	static glm::vec3 snapStaticPos = { 1.0, 1.0, 1.0 };
	static float snapStaticRot = 30.0;
	static float snapStaticScale = 1.0;
	static bool useSnap = false;
	static bool drawGizmo = false;
	static bool drawGrid = false;

	ImGui::Checkbox("Draw Gizmo", &drawGizmo);
	ImGui::Checkbox("Draw Grid", &drawGrid);

	ImGuizmo::Enable(drawGizmo);

	if (ImGui::IsKeyPressed(translateKey))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(rotateKey))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(scaleKey)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::IsKeyPressed(universalKey)) // r Key
		mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents((float*)matrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)matrix);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	if (ImGui::IsKeyPressed(snapTogglKey))
		useSnap = !useSnap;
	ImGui::Checkbox("Snapping", &useSnap);
	ImGui::SameLine();

	glm::vec3 snap;
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		snap = snapStaticPos;
		ImGui::InputFloat3("Snap", &snapStaticPos.x);
		break;
	case ImGuizmo::ROTATE:
		snap = { snapStaticRot, 0.0f, 0.0f };
		ImGui::InputFloat("Angle Snap", &snapStaticRot);
		break;
	case ImGuizmo::SCALE:
		snap = { snapStaticScale, 0.0f, 0.0f };
		ImGui::InputFloat("Scale Snap", &snapStaticScale);
		break;
	default:
		assert("Shouldn't be able to get here");
	}

	const ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	auto view = camera.GetView();
	auto proj = camera.GetProjection();

	if (drawGizmo)
		ImGuizmo::Manipulate(&view[0][0], &proj[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, (float*)matrix, NULL, useSnap ? &snap.x : NULL);

	auto t = glm::identity<glm::mat4>();

	if (drawGrid)
		ImGuizmo::DrawGrid(&view[0][0], &proj[0][0], &t[0][0], 50);
}

class HelloTriangleApplication {
public:
	void Run() {

		float i = 35.f;
		fnDependencyProj(i);

		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}

private:
	void InitWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow(WIDTH, HEIGHT, m_WindowName.c_str(), nullptr, nullptr);

		//Window Error Check
		if (!m_Window) {
			glfwTerminate();
			LOG_INFO("GLFW failed to create the window.");
		}
		else {
			glfwMakeContextCurrent(m_Window);
			LOG_INFO("Successfully created GLFW window.");
		}

	}

	// ToDo: Delete this, it's stupid. PipelineInfo is basically this :P 
	struct FixedFunctionStagesInfo
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};

		// Real Objects
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		VkPipelineViewportStateCreateInfo viewportState = {};
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		VkPipelineDynamicStateCreateInfo dynamicState = {};

		// This is stupid, but at least it'll keep me aware
		void setupRef()
		{
			// Check for default states
			// assert(viewport.width != 0.f);
			// assert(scissor.extent.width != 0);
			// viewportState.scissorCount = 1;
			// viewportState.pScissors = &scissor;
			// viewportState.viewportCount = 1;
			// viewportState.pViewports = &viewport;

			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(g_dynamicStatesOps.size());
			dynamicState.pDynamicStates = g_dynamicStatesOps.data();

			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
		}
	};

	void createGraphicsPipeline()
	{
		const auto vertShaderCode = wVkHelpers::readFile(wVkConstants::shaderDir + "vert.spv");
		const auto fragShaderCode = wVkHelpers::readFile(wVkConstants::shaderDir + "frag.spv");

		m_VertShaderModule = wVkHelpers::createShaderModule(vertShaderCode);
		m_FragShaderModule = wVkHelpers::createShaderModule(fragShaderCode);


		// To Use the Shaders we need a PipelineShaderStage
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_VertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_FragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[2] = { vertShaderStageInfo, fragShaderStageInfo };

		// Goofy ah static solution
		static auto vBindDescrpt = Vertex::getBindingDescription();
		static auto vAttdDescrpt = Vertex::getAttributeDescriptions();

		// Vertex input - Empty for now, vert hardcoded in shader
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vAttdDescrpt.size());
		vertexInputInfo.pVertexBindingDescriptions = &vBindDescrpt;
		vertexInputInfo.pVertexAttributeDescriptions = vAttdDescrpt.data();

		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Viewport State
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		/// Wireframe for Free :D 
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;

		/// Culling
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //Vertex order to be considered from/back face

		/// Depth bias for free (shadow mapping use-case)
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


		// Multisampling (Anti-Aliasing)
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional


		// General Info
		VkPipelineColorBlendStateCreateInfo colorBlendingCI{};
		colorBlendingCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingCI.logicOpEnable = VK_FALSE;
		colorBlendingCI.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlendingCI.attachmentCount = 1;
		colorBlendingCI.blendConstants[0] = 0.0f; // Optional
		colorBlendingCI.blendConstants[1] = 0.0f; // Optional
		colorBlendingCI.blendConstants[2] = 0.0f; // Optional
		colorBlendingCI.blendConstants[3] = 0.0f; // Optional


		// Blend Formula Set-up (Attachments?)
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1; // Optional
		pipelineLayoutInfo.pSetLayouts = &m_DescSetLayout; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
		pipelineLayoutInfo.setLayoutCount = 1;

		if (vkCreatePipelineLayout(wVkGlobals::g_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		FixedFunctionStagesInfo ffs = {};
		ffs.depthStencil = depthStencil;
		ffs.viewportState = viewportState;
		ffs.vertexInputInfo = vertexInputInfo;
		ffs.inputAssembly = inputAssembly;
		ffs.rasterizer = rasterizer;
		ffs.multisampling = multisampling;
		ffs.colorBlending = colorBlendingCI;
		ffs.colorBlendAttachment = colorBlendAttachment;

		m_FixedFuncStages = ffs;
		m_FixedFuncStages.setupRef();

		m_ShaderStages[0] = shaderStages[0];
		m_ShaderStages[1] = shaderStages[1];
	}

	void createRenderPass() {

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		// Shader Stages
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = m_ShaderStages;

		// Fixed Stages
		pipelineInfo.pVertexInputState = &m_FixedFuncStages.vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &m_FixedFuncStages.inputAssembly;
		pipelineInfo.pViewportState = &m_FixedFuncStages.viewportState;
		pipelineInfo.pRasterizationState = &m_FixedFuncStages.rasterizer;
		pipelineInfo.pMultisampleState = &m_FixedFuncStages.multisampling;
		pipelineInfo.pDepthStencilState = &m_FixedFuncStages.depthStencil; // Optional
		pipelineInfo.pColorBlendState = &m_FixedFuncStages.colorBlending;
		pipelineInfo.pDynamicState = &m_FixedFuncStages.dynamicState;

		pipelineInfo.layout = m_PipelineLayout; // Empty 
		pipelineInfo.renderPass = wVkGlobals::g_RenderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(wVkGlobals::g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}


		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Goofy ah static solution
		const auto vBindDescrpt = Particle::getBindingDescription();
		const auto vAttdDescrpt = Particle::getAttributeDescriptions();

		// Vertex input - Empty for now, vert hardcoded in shader
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vAttdDescrpt.size());
		vertexInputInfo.pVertexBindingDescriptions = &vBindDescrpt;
		vertexInputInfo.pVertexAttributeDescriptions = vAttdDescrpt.data();

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		// Shader Stages
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = m_ShaderStages;

		// Fixed Stages
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &m_FixedFuncStages.viewportState;
		pipelineInfo.pRasterizationState = &m_FixedFuncStages.rasterizer;
		pipelineInfo.pMultisampleState = &m_FixedFuncStages.multisampling;
		pipelineInfo.pDepthStencilState = &m_FixedFuncStages.depthStencil; // Optional
		pipelineInfo.pColorBlendState = &m_FixedFuncStages.colorBlending;
		pipelineInfo.pDynamicState = &m_FixedFuncStages.dynamicState;

		pipelineInfo.layout = m_PipelineLayout; // Empty 
		pipelineInfo.renderPass = wVkGlobals::g_RenderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(wVkGlobals::g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipelinePoints) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

	}


	void createCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = wVkGlobals::g_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = wVkConstants::g_MaxFramesInFlight;

		if (vkAllocateCommandBuffers(wVkGlobals::g_Device, &allocInfo, m_CommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		m_ComputeCmdList.Initialize();

		
	}

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = wVkGlobals::g_RenderPass;
		renderPassInfo.framebuffer = wVkGlobals::g_SwapChainFramebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = wVkGlobals::g_SwapChain.swapChainExtent;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a};
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		// Bind scissor and other shit here if dynamic
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(wVkGlobals::g_SwapChain.swapChainExtent.width);
		viewport.height = static_cast<float>(wVkGlobals::g_SwapChain.swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = wVkGlobals::g_SwapChain.swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { m_VertexBuffer->GetGPUHandleRef().m_Buffers };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetGPUHandleRef().m_Buffers, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[currentFrame], 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(g_indices.size()), 1, 0, 0, 0);
		
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipelinePoints);

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_ParticleBuffers[currentFrame]->GetGPUHandleRef().m_Buffers, offsets);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[currentFrame], 0, nullptr);

		vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		// Synchronize depth as both the ImGui Render Pass and the
		// Draw render pass will try to run at the same time (WAW error)
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT ;
		barrier.image = wVkGlobals::g_DepthImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, // Source stage
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, // Destination stage
			0, // Dependency flags
			0, nullptr, // Memory barrier count and pointer
			0, nullptr, // Buffer barrier count and pointer
			1, &barrier); // Image barrier count and pointer

		// ImGui Render Pass
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = wVkGlobals::g_ImGuiRenderPass;
			info.framebuffer = wVkGlobals::g_SwapChainFramebuffers[imageIndex];
			info.renderArea.offset = { 0 , 0 };
			info.renderArea.extent = wVkGlobals::g_SwapChain.swapChainExtent;
			info.clearValueCount = static_cast<uint32_t>(clearValues.size());
			info.pClearValues = clearValues.data();
			vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record Imgui Draw Data and draw funcs into command buffer
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void createSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; //To make sure we get to our first frame

		for (size_t i = 0; i < wVkConstants::g_MaxFramesInFlight; i++) {
			if (vkCreateSemaphore(wVkGlobals::g_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(wVkGlobals::g_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(wVkGlobals::g_Device, &fenceInfo, nullptr, &m_InFlightFence[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create semaphores!");
			}

			if (vkCreateSemaphore(wVkGlobals::g_Device, &semaphoreInfo, nullptr, &m_ComputeFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(wVkGlobals::g_Device, &fenceInfo, nullptr, &m_ComputeInFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create compute synchronization objects for a frame!");
			}
		}
	}
 
	void recreateSwapChain()
	{
		vkDeviceWaitIdle(wVkGlobals::g_Device);

		m_BackEndRenderer.ResizeFrameBuffers(m_Window);
	}


	void createUniformBuffers() {
		for (size_t i = 0; i < wVkConstants::g_MaxFramesInFlight; i++) {
			constexpr BufferFlags uboFlags = BufferFlags::CBV;
			const std::string uboName = "Camera Ubo " + std::to_string(i);
			m_CameraBuffer[i] = new Buffer(nullptr, sizeof(UniformBufferObject), 1, uboFlags, uboName);
		}
	}

	void createDtBuffers() {

		VkDeviceSize bufferSize = sizeof(float);

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(wVkGlobals::g_PhysicalDevice, &properties);

		// Alignment requirements could force a larger allocation:
		const VkDeviceSize minUboAlignment = properties.limits.minMemoryMapAlignment;
		bufferSize = (bufferSize + minUboAlignment - 1) & ~(minUboAlignment - 1);

		for (size_t i = 0; i < wVkConstants::g_MaxFramesInFlight; i++) {
			const BufferFlags flags = BufferFlags::CBV;
			std::string name = "DeltaTime Buffer for Particles " + std::to_string(i);
			m_DtConstbuffer[i] = new Buffer(nullptr, sizeof(float), 1, flags, name);
			m_ColourBuffer[i] = new Buffer(nullptr, sizeof(glm::vec4), 1, flags, name);
		}
	}

	void createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;

		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		const std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(wVkGlobals::g_Device, &layoutInfo, nullptr, &m_DescSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void createDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight);


		if (vkCreateDescriptorPool(wVkGlobals::g_Device, &poolInfo, nullptr, &m_DescPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}

	}

	void createDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(wVkConstants::g_MaxFramesInFlight, m_DescSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(wVkConstants::g_MaxFramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(wVkConstants::g_MaxFramesInFlight);
		if (vkAllocateDescriptorSets(wVkGlobals::g_Device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < wVkConstants::g_MaxFramesInFlight; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_CameraBuffer[i]->GetGPUHandleRef().m_Buffers;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_Texture->GetGPUHandleRef().m_TextureImageView;
			imageInfo.sampler = m_Sampler->GetGPUHandleRef().m_Sampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(wVkGlobals::g_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}


	void createTextureImage()
	{
		int texWidth, texHeight, texChannels;
		constexpr auto filePath = R"(Resources\Images\bricks.png)";
		stbi_uc* pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		TextureSpec spec{
			texWidth,
			texHeight,
			TextureFormat::R8G8B8A8_SRGB,
			TextureType::R_TEXTURE,
			TextureFlags::MIPMAP_GENERATE
		};
		
		m_Texture = new Texture(pixels, spec, "Test Texture");

		stbi_image_free(pixels);
	}




	void createShaderStorageBuffers()
	{
		// Initialize particles
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

		// Initial particle positions on a circle
		std::vector<Particle> particles(PARTICLE_COUNT);
		for (auto& particle : particles) {
			// Spherical coordinates
			float phi = acos(2.0f * rndDist(rndEngine) - 1.0f);  // phi angle
			float theta = rndDist(rndEngine) * 2.0f * glm::pi<float>();  // theta angle

			constexpr float RADIUS = .5f;        // Radius of the sphere
			// Conversion to Cartesian coordinates
			float x = RADIUS * sin(phi) * cos(theta);
			float y = RADIUS * sin(phi) * sin(theta);
			float z = RADIUS * cos(phi);
			particle.position = glm::vec3(x, y, z) * glm::vec3(rndDist(rndEngine));

			// Velocity outward from the center
			glm::vec3 direction = (particle.position); // Normalize to get the direction
			constexpr float initialSpeed = 0.01f;  // Modify this value to adjust the initial speed
			particle.velocity = direction * initialSpeed;

			// Random color for each particle
			particle.color = glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine));
		}

		const BufferFlags partFlags = BufferFlags::UAV | BufferFlags::ALLOW_UA | BufferFlags::VERTEX_BUFFER; 
		for(int i = 0; i < wVkConstants::g_MaxFramesInFlight; i++)
		{
			std::string name = "Particle Buffer " + std::to_string(i);
			m_ParticleBuffers[i] = new Buffer(particles.data(), sizeof(Particle), particles.size(), partFlags, name);
		}
	}

	void InitVulkan() {

		m_BackEndRenderer.Initialize(m_Window, nullptr, nullptr);

		createUniformBuffers();
		createDtBuffers();

		createTextureImage();
		m_Sampler = new Sampler(MinFilter::NEAREST_MIPMAP_NEAREST, MagFilter::NEAREST, WrapUV::MIRRORED_REPEAT);

		createDescriptorSetLayout();
		createDescriptorPool();
		createDescriptorSets();
		createGraphicsPipeline();
		createRenderPass();

		const BufferFlags vbFlags = BufferFlags::SRV | BufferFlags::VERTEX_BUFFER;
		const std::string vbName = "Vertex Buffer";
		m_VertexBuffer = new Buffer(g_vertices.data(), sizeof(g_vertices[0]), g_vertices.size(), vbFlags, vbName);

		const BufferFlags ibFlags = BufferFlags::SRV | BufferFlags::INDEX_BUFFER;
		const std::string ibName = "Index Buffer";
		m_IndexBuffer = new Buffer(g_indices.data(), sizeof(g_indices[0]), g_indices.size(), ibFlags, ibName);

		// Compute Stuff
		createShaderStorageBuffers();

		m_ParticleLayout.AddParameter(ShaderParameter::CBV);
		m_ParticleLayout.AddParameter(ShaderParameter::CBV);
		m_ParticleLayout.AddParameter(ShaderParameter::SRV);
		m_ParticleLayout.AddParameter(ShaderParameter::UAV);
		m_ParticleLayout.Initialize();

		m_ParticlePipeline.Initialize(wVkConstants::shaderDir + "particle.spv", m_ParticleLayout);

		createCommandBuffer();
		createSyncObjects();
	}


	void updateUniformBuffer(uint32_t currentImage, double dt) {

		UniformBufferObject ubo;
		ubo.proj = camera.GetProjection();
		ubo.view = camera.GetView();
		ubo.model = cubeModel.GetModelMatrix();

		// Y Coordinate of Clip Coordinates is flipped, this fixes that.
		ubo.proj[1][1] *= -1;

		m_CameraBuffer[currentImage]->UpdateData(&ubo, sizeof(ubo));

	}

	void updateDtBuffer(uint32_t currentImage, float dt) {
		m_DtConstbuffer[currentImage]->UpdateData(&dt, sizeof(float));
		m_ColourBuffer[currentImage]->UpdateData(&m_ParticleColor, sizeof(m_ParticleColor));
	}

	void drawFrame(double dt)
	{
		VkResult res = VK_SUCCESS;

		const auto& compCommandBuffer = m_ComputeCmdList.GetCommandListHandleRef().m_CommandBuffer[currentFrame];
		const auto& computeFences = m_ComputeInFlightFences[currentFrame];
		const auto& computeSemaph = m_ComputeFinishedSemaphores[currentFrame];

		// Compute submission
		vkWaitForFences(wVkGlobals::g_Device, 1, &computeFences, VK_TRUE, UINT64_MAX);

		updateDtBuffer(currentFrame, (float)dt);

		vkResetFences(wVkGlobals::g_Device, 1, &computeFences);

		vkResetCommandBuffer(compCommandBuffer, /*VkCommandBufferResetFlagBits*/ 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(compCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		m_ComputeCmdList.SetComputePipeline(m_ParticlePipeline);
		m_ComputeCmdList.BindResourceCBV(0, *m_DtConstbuffer[currentFrame]);
		m_ComputeCmdList.BindResourceCBV(1, *m_ColourBuffer[currentFrame]);
		m_ComputeCmdList.BindResourceSRV(2, *m_ParticleBuffers[(currentFrame - 1) % wVkConstants::g_MaxFramesInFlight]);
		m_ComputeCmdList.BindResourceUAV(3, *m_ParticleBuffers[currentFrame % wVkConstants::g_MaxFramesInFlight]);
		m_ComputeCmdList.Dispatch((int)currentFrame, PARTICLE_COUNT / 256, 1, 1);

		VkSubmitInfo compSubmitInfo{};
		compSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		compSubmitInfo.commandBufferCount = 1;
		compSubmitInfo.pCommandBuffers = &compCommandBuffer;
		compSubmitInfo.signalSemaphoreCount = 1;
		compSubmitInfo.pSignalSemaphores = &computeSemaph;

		if (vkEndCommandBuffer(compCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		if (vkQueueSubmit(wVkGlobals::g_ComputeQueue, 1, &compSubmitInfo, computeFences) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		}

		const auto commandBuffer = m_CommandBuffer[currentFrame];
		const auto renderedS = m_RenderFinishedSemaphore[currentFrame];
		const auto inFlightFence = m_InFlightFence[currentFrame];

		// Graphics submission
		vkWaitForFences(wVkGlobals::g_Device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		const auto imageAvailableS = m_ImageAvailableSemaphore[currentFrame];
		res = vkAcquireNextImageKHR(wVkGlobals::g_Device, wVkGlobals::g_SwapChain.swapChain, UINT64_MAX, imageAvailableS, VK_NULL_HANDLE, &imageIndex);

		if (res == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		updateUniformBuffer(currentFrame, dt);

		vkResetFences(wVkGlobals::g_Device, 1, &inFlightFence);

		vkResetCommandBuffer(commandBuffer, 0);
		recordCommandBuffer(commandBuffer, imageIndex);

		const VkSemaphore waitSemaphores[] = { computeSemaph, imageAvailableS };
		const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(std::size(waitSemaphores));
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderedS;

		if (vkQueueSubmit(wVkGlobals::g_GraphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderedS;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &wVkGlobals::g_SwapChain.swapChain;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr; // Optional
		res = vkQueuePresentKHR(wVkGlobals::g_PresentQueue, &presentInfo);

		if (res == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		currentFrame = (currentFrame + 1) % wVkConstants::g_MaxFramesInFlight;
	}


	void MainLoop() {

		double lastFpsTime = glfwGetTime();
		int frameCount = 0;
		double deltaTimeAccumulator = 0;  // Accumulator for delta times

		camera = FreeCamera();
		camera.m_Transform.SetPosition(0.0, 2.0, 10.0);

		Assimp::Importer importer;

		// Attempt to load the model file at the given filePath
		auto filePath = R"(Resources\Models\DamagedHelmet\DamagedHelmet.glb)";
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

		// Check if the import was successful
		if (scene) {
			LOG_INFO("Successfully loaded model: %s ", filePath);
			LOG_INFO("Number of meshes: %i", scene->mNumMeshes);
		}
		else {
			LOG_ERROR("Failed to load model: %s", filePath);
			LOG_ERROR("ASSIMP Error: %s", importer.GetErrorString());
		}

		int width, height, channels;
		// Load the image
		filePath = R"(Resources\Images\bricks.png)";
		if (unsigned char* data = stbi_load(filePath, &width, &height, &channels, 0)) {
			LOG_INFO("Loaded image: %s ", filePath);
			LOG_INFO("Dimensions: %i x %i", width, height);
			LOG_INFO("Channels: %i", channels);

			// Free the image memory
			stbi_image_free(data);
		}
		else {
			LOG_ERROR("Failed to load image: %s", filePath);
			LOG_ERROR("STB_IMAGE Error: %s", stbi_failure_reason());
		}

		while (!glfwWindowShouldClose(m_Window)) {
			static double lastTime = glfwGetTime();
			static double runningTime = glfwGetTime();
			const double currentTime = glfwGetTime();
			const double deltaTimeMs = (currentTime - lastTime) * 1000;
			lastTime = currentTime;
			runningTime += deltaTimeMs;

			glfwPollEvents();

			if(wVkGlobals::g_errorValidationLayerTriggered)
			{
				LOG_ERROR("Validation layer error(s) detected");
				assert(false);
			}

			// Only draw if the window is not minimized
			if (glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) != GLFW_TRUE) {

				ImGui_ImplVulkan_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				ImGuizmo::BeginFrame();

				ImGuiIO& io = ImGui::GetIO();
				// Assuming the keys are mapped and captured through ImGui
				const float forwardBackward = ((float)io.KeysDown['W'] - (float)io.KeysDown['S']); // W = 1, S = -1
				const float leftRight = ((float)io.KeysDown['D'] - (float)io.KeysDown['A']); // D = 1, A = -1
				const float upDown = ((float)io.KeysDown['R'] - (float)io.KeysDown['F']); // R = 1, F = -1

				// Define the key codes for arrow keys
				constexpr int keyLeft = GLFW_KEY_LEFT;
				constexpr int keyRight = GLFW_KEY_RIGHT;
				constexpr int keyUp = GLFW_KEY_UP;
				constexpr int keyDown = GLFW_KEY_DOWN;

				// Calculate horizontal and vertical movement
				const float horizontal = ((float)io.KeysDown[keyRight] - (float)io.KeysDown[keyLeft]); // Right = 1, Left = -1
				const float vertical = ((float)io.KeysDown[keyUp] - (float)io.KeysDown[keyDown]); // Up = 1, Down = -1

				// Create the movement vector
				const glm::vec2 moveVector(horizontal, vertical);
				// Create and update the camera movement vector
				const glm::vec3 cameraMoveVector(leftRight, upDown, forwardBackward);

				camera.CameraInput((float)deltaTimeMs / 1000, cameraMoveVector, moveVector);
				camera.UpdateCamera(io.DisplaySize.x, io.DisplaySize.y);

				ImGui::Begin("Gizmo Transform");

				ImGui::ColorEdit4("Clear Color", &m_ClearColor[0]);
				ImGui::ColorEdit4("Particles Color", &m_ParticleColor[0]);
				EditTransform(camera, cubeModel.GetModelMatrixPtr());
				ImGui::End();

				ImGui::Render();

				drawFrame(runningTime / 1000);
				frameCount++;  // Increase frame count
				deltaTimeAccumulator += deltaTimeMs;  // Accumulate delta time in milliseconds

				// Calculate time passed and update FPS and average delta time every second
				if (currentTime - lastFpsTime >= 1.0) {
					const double fps = frameCount / (currentTime - lastFpsTime);
					const double averageDeltaTimeMs = deltaTimeAccumulator / frameCount;  // Calculate average delta time per frame
					lastFpsTime = currentTime;
					frameCount = 0;
					deltaTimeAccumulator = 0;  // Reset delta time accumulator

					char title[256];  // Buffer to hold the window title
					sprintf_s(title, "%s - FPS: %.0f Avg Dt: %.2f ms", m_WindowName.c_str(), fps, averageDeltaTimeMs);
					LOG_INFO("%s", title);
					glfwSetWindowTitle(m_Window, title);
				}
			}

		}

	}

	void Cleanup() {

		// Wait until everything is completed until we clean-up
		vkDeviceWaitIdle(wVkGlobals::g_Device);

		vkDestroyDescriptorSetLayout(wVkGlobals::g_Device, m_DescSetLayout, nullptr);
		vkDestroyDescriptorPool(wVkGlobals::g_Device, m_DescPool, nullptr);

		for (size_t i = 0; i < wVkConstants::g_MaxFramesInFlight; i++) {

			delete m_CameraBuffer[i];
			delete m_ParticleBuffers[i];
			delete m_DtConstbuffer[i];
			delete m_ColourBuffer[i];

			vkDestroySemaphore(wVkGlobals::g_Device, m_ImageAvailableSemaphore[i], nullptr);
			vkDestroySemaphore(wVkGlobals::g_Device, m_RenderFinishedSemaphore[i], nullptr);
			vkDestroyFence(wVkGlobals::g_Device, m_InFlightFence[i], nullptr);

			vkDestroySemaphore(wVkGlobals::g_Device, m_ComputeFinishedSemaphores[i], nullptr);
			vkDestroyFence(wVkGlobals::g_Device, m_ComputeInFlightFences[i], nullptr);
		}


		delete m_Texture;
		delete m_Sampler;

		// Destroy our draw buffers
		delete m_VertexBuffer;
		delete m_IndexBuffer;

		m_ParticlePipeline.Destroy();

		vkDestroyPipeline(wVkGlobals::g_Device, m_GraphicsPipeline, nullptr);
		vkDestroyPipeline(wVkGlobals::g_Device, m_GraphicsPipelinePoints, nullptr);

		vkDestroyPipelineLayout(wVkGlobals::g_Device, m_PipelineLayout, nullptr);

		vkDestroyShaderModule(wVkGlobals::g_Device, m_VertShaderModule, nullptr);
		vkDestroyShaderModule(wVkGlobals::g_Device, m_FragShaderModule, nullptr);

		m_BackEndRenderer.Shutdown();

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}



	uint32_t currentFrame = 0;
	Transform cubeModel;

	FreeCamera camera;

	BackEndRenderer m_BackEndRenderer;

	// Commands
	VkCommandBuffer m_CommandBuffer[wVkConstants::g_MaxFramesInFlight] = {};
	CommandList m_ComputeCmdList;

	// Sync
	VkSemaphore m_ImageAvailableSemaphore[wVkConstants::g_MaxFramesInFlight] = {};
	VkSemaphore m_RenderFinishedSemaphore[wVkConstants::g_MaxFramesInFlight] = {};
	VkFence m_InFlightFence[wVkConstants::g_MaxFramesInFlight] = {};

	// Shaders;
	VkShaderModule m_VertShaderModule = VK_NULL_HANDLE;
	VkShaderModule m_FragShaderModule = VK_NULL_HANDLE;

	// Pipeline and Render Pass Setup
	VkPipelineShaderStageCreateInfo m_ShaderStages[2] = {};
	FixedFunctionStagesInfo m_FixedFuncStages;

	// Pipeline
	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

	// Descriptors
	VkDescriptorSetLayout m_DescSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_DescPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	// Encompasses Pipeline and Renderpass into 1
	VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
	VkPipeline m_GraphicsPipelinePoints = VK_NULL_HANDLE;

	// Drawing Data
	Buffer* m_VertexBuffer = nullptr;
	Buffer* m_IndexBuffer = nullptr;

	// Uniform Buffers
	Buffer* m_CameraBuffer[wVkConstants::g_MaxFramesInFlight] = {};

	// Textures
	Texture* m_Texture = nullptr;
	Sampler* m_Sampler = nullptr;

	// Compute Stuff
	Buffer* m_ParticleBuffers[wVkConstants::g_MaxFramesInFlight] = {};
	Buffer* m_DtConstbuffer[wVkConstants::g_MaxFramesInFlight] = {};

	glm::vec4 m_ParticleColor = glm::vec4(1.0f);
	glm::vec4 m_ClearColor = glm::vec4(0.0f);
	Buffer* m_ColourBuffer[wVkConstants::g_MaxFramesInFlight] = {};

	ShaderLayout m_ParticleLayout;
	ComputePipelineDescription m_ParticlePipeline;

	VkFence m_ComputeInFlightFences[wVkConstants::g_MaxFramesInFlight] = {};
	VkSemaphore m_ComputeFinishedSemaphores[wVkConstants::g_MaxFramesInFlight] = {};

	// GLFW
	std::string m_WindowName = "VulkanTutorial";
	GLFWwindow* m_Window = nullptr;

};

int main() {
	HelloTriangleApplication app;

	try {
		app.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}