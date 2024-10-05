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
    float3 position;
    float padding;
};

VOut main(VIn input)
{
    VOut output;
    output.position = float4(input.position + position, 1.0f);
    output.colour = input.colour;
    return output;
}