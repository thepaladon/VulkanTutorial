// HLSL Fragment Shader
Texture2D texSampler : register(t1);
SamplerState texSamplerState : register(s1);

struct PSInput {
    float3 color : COLOR0;
    float2 texCoord : TEXCOORD0;
    float pointSize : PSIZE;
};

float4 main(PSInput input) : SV_Target {
    float4 texColor = texSampler.Sample(texSamplerState, input.texCoord);
    return texColor + float4(input.color.rgb, 1.0);
}
