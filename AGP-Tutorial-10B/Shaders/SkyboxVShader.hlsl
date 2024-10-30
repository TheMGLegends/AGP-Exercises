struct VIn
{
    float4 position : POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct VOut
{
    float4 position : SV_Position;
    float4 colour : COLOR;
    float3 uvw : TEXCOORD;
};

cbuffer CBuffer0
{
    matrix WVP;
};

VOut main(VIn input)
{
    VOut output;
    
    output.position = mul(WVP, input.position);
    
    output.uvw = input.position.xyz;
    
    output.colour = input.colour;
    
	return output;
}