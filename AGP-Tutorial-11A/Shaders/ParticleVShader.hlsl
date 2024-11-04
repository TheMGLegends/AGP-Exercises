cbuffer cbuffer0
{
	matrix WVP;
    float4 colour;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
};

VOut main(float4 pos : POSITION)
{
    VOut output;
    output.position = mul(WVP, pos);
    output.colour = colour;
    output.uv = pos.xy;
	return output;
}