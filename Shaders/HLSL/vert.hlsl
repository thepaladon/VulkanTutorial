// HLSL Vertex Shader
cbuffer UniformBufferObject : register(b0) {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

struct VSInput {
    float3 pos : POSITION;
    float3 color : COLOR0;
    float2 texCoord : TEXCOORD0;
};


struct PSInput {
    float4 pos : SV_Position;
    float3 color : COLOR0;
    float2 texCoord : TEXCOORD0;
    [[vk::builtin("PointSize")]]
    float pointSize : PSIZE;
};


PSInput main(VSInput input) {
    PSInput output;
    float4 worldPosition = mul(model, float4(input.pos, 1.0));
    float4 viewPosition = mul(view, worldPosition);
    output.pos = mul(proj, viewPosition);
    output.color = input.color;
    output.texCoord = input.texCoord;
    output.pointSize = 5.0;
    return output;
}
