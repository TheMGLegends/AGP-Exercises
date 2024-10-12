struct VIn
{
    float4 position : SV_Position;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
};

Texture2D texture0;
sampler sampler0;

float4 main(VIn input) : SV_TARGET
{
    float4 sampled = texture0.Sample(sampler0, input.uv);
    return input.colour * sampled;
}