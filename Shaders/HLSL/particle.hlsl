
struct ConstantData
{
    float deltaTime;
};

struct Color
{
    float4 color;
};

struct Particle {
    float4 position;
    float4 color;
    float4 velocity;
};

ConstantBuffer<ConstantData> settings : register(b0);
ConstantBuffer<Color> col : register(b1);
StructuredBuffer<Particle> particlesIn : register(t2);
RWStructuredBuffer<Particle> particlesOut : register(u3);

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;

    Particle particleIn = particlesIn[index];

    particlesOut[index].position = particleIn.position + particleIn.velocity * settings.deltaTime;
    particlesOut[index].velocity = particleIn.velocity;
    particlesOut[index].color = col.color;
    // Assuming color remains unchanged; if needed, add a line to update it
}
