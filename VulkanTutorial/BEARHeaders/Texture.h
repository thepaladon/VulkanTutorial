#pragma once
#include <string>

#include "BEARVulkan/TypeDefs.h"

enum class TextureFormat
{
	R8G8B8A8_UNORM,
	R8G8B8A8_SNORM,
	R8G8B8A8_SRGB, // Diverged from OG BEAR
	R32_FLOAT,
	R32_G32_FLOAT,
	R32_G32_B32_A32_FLOAT,
	// New types will be added when needed
};

enum class TextureFlags
{
	NONE = 0,
	MIPMAP_GENERATE = 1 << 1,
	ALLOW_UA = 1 << 2 // Will allow a DX12 texture to be writable in a shader
};

enum class TextureType
{
	RENDER_TARGET = 0,
	R_TEXTURE = 1,
	RW_TEXTURE = 2,
};

// Enable bitwise operations on the TextureFlags enum
inline TextureFlags operator|(TextureFlags a, TextureFlags b)
{
	return static_cast<TextureFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline TextureFlags operator&(TextureFlags a, TextureFlags b)
{
	return static_cast<TextureFlags>(static_cast<int>(a) & static_cast<int>(b));
}

struct TextureSpec
{
	int m_Width;
	int m_Height;
	TextureFormat m_Format;
	TextureType m_Type;
	TextureFlags m_Flags;
};

class Texture
{
public:
	Texture() = default;

	// Constructing a texture from raw data
	Texture(const void* data, TextureSpec spec, const std::string& name = "default_name");

	~Texture();

	// Delete copy constructor and copy assignment operator as it mirrors GPU resource
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Explicitly define move constructor and move assignment operator as it mirrors GPU resource
	Texture(Texture&& other) noexcept {}
	Texture& operator=(Texture&& other) noexcept {
		return *this;
	}

	// Only works for RenderTarget - Windows
	// Workaround for `ResizeFrameBuffers`
	// ToDo : For PS5 (all) and Windows (R and RW Tex).
	void ResizeTexture(int newWidth, int newHeight);
	void UpdateTexture(const void* data);

	// Getters
	std::string GetName() const { return m_Name; }
	TextureSpec GetSpec() const { return m_Spec; }
	TextureFlags GetFlags() const { return m_Spec.m_Flags; }
	TextureFormat GetFormat() const { return m_Spec.m_Format; }
	TextureType GetType() const { return m_Spec.m_Type; }

	int GetWidth() const { return m_Spec.m_Width; }
	int GetHeight() const { return m_Spec.m_Height; }

	GPUTextureHandle& GetGPUHandleRef() { return m_TextureHandle; }

private:
	TextureSpec m_Spec;
	GPUTextureHandle m_TextureHandle = {};
	uint32_t m_Channels = 0;

	[[maybe_unused]] uint32_t m_SizeInBytes = 0;
	[[maybe_unused]] uint32_t m_BytesPerChannel = 1;

	std::string m_Name;
};

