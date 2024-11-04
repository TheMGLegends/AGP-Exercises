TextureCube cube0;
sampler sampler0;

struct VIn
{
    float4 position : SV_Position;
    float4 colour : COLOR;
    float3 uvw : TEXCOORD;
};

float4 main(VIn input) : SV_TARGET
{
    float4 sampled = cube0.Sample(sampler0, input.uvw);
    return input.colour * sampled;
}