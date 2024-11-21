struct VIn
{
    float4 position : SV_Position;
    float4 colour : COLOR;
    float2 uv : TEXCOORD0;
    float3 reflectedUVW : TEXCOORD1;
};

Texture2D texture0 : register(t0);
TextureCube skybox0 : register(t1);

sampler sampler0;

float4 main(VIn input) : SV_TARGET
{
    float4 sampled = texture0.Sample(sampler0, input.uv);
    float4 reflectedSampled = skybox0.Sample(sampler0, input.reflectedUVW);
    float4 combined = ((input.colour * sampled) * 0.9) + (reflectedSampled * 0.1);
    return saturate(combined);
}