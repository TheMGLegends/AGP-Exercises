struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOR;

};

struct VOut
{
    float4 position : SV_Position;
    float4 colour : COLOR;
};

cbuffer ConstantBuffer0
{
    matrix WVP;
};

VOut main(VIn input)
{
    VOut output;
    output.position = mul(WVP, float4(input.position, 1.0f));
    output.colour = input.colour;
    return output;
}