struct VIn
{
    float4 position : POSITION;
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
    output.position = input.position;
    output.colour = input.colour;
    return output;
}