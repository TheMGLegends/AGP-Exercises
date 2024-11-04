struct VIn
{
	float4 position : SV_POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(VIn input) : SV_TARGET
{
    float distsq = input.uv.x * input.uv.x + input.uv.y * input.uv.y;
    clip(1.0f - distsq);
    return input.colour;
}