cbuffer cbuffer0
{
	matrix WVP;
    float4 colour;
};

struct VOut
{
    float4 position : SV_Position;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
};

VOut main(float3 pos : POSITION)
{
    VOut output;
    output.position = mul(WVP, float4(pos, 1.0f));
    output.colour = colour;
    output.uv = pos.xy;
	return output;
}