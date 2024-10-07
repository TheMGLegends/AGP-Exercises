struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOR; // Colour of the vertex, passed through to the pixel shader
};

struct VOut
{
    float4 position : SV_Position; // The position of the vertex in screen space, passed to the pixel shader
    float4 colour : COLOR; // Colour of the vertex, passed through to the pixel shader
};

cbuffer ConstantBuffer0
{
    matrix WVP; // World View Projection matrix, used to transform the vertices into screen space
};

VOut main(VIn input)
{
    VOut output;
    output.position = mul(WVP, float4(input.position, 1.0f));
    output.colour = input.colour;
    return output;
}