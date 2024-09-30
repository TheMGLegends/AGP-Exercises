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

VOut main(VIn input)
{
    VOut output;
    output.position = float4(input.position, 1);
    output.colour = input.colour;
    return output;
}