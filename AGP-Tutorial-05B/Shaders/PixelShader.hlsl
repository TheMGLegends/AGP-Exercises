struct VIn
{
    float4 position : SV_Position;
    float4 colour : COLOR;
};

float4 main(VIn input) : SV_TARGET
{
    return input.colour;
}