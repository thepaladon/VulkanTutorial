// HLSL Compute Shader for processing particles
cbuffer ParameterUBO : register(b0) {
    float deltaTime;
};

struct Particle {
    float3 position;
    float3 color;
    float3 velocity;
};

StructuredBuffer<Particle> particlesIn : register(t1); // Read-only input buffer
RWStructuredBuffer<Particle> particlesOut : register(u2); // Read-write output buffer

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;

    Particle particleIn = particlesIn[index];

    particlesOut[index].position = particleIn.position + particleIn.velocity * deltaTime;
    particlesOut[index].velocity = particleIn.velocity;
    // Assuming color remains unchanged; if needed, add a line to update it
}
