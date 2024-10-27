struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOR; // Colour of the vertex, passed through to the pixel shader
    float2 uv : TEXCOORD; // UV coordinates of the vertex, passed through to the pixel shader
    float3 normal : NORMAL; // Normal of the vertex
};

struct VOut
{
    float4 position : SV_Position; // The position of the vertex in screen space, passed to the pixel shader
    float4 colour : COLOR; // Colour of the vertex, passed through to the pixel shader
    float2 uv : TEXCOORD; // UV coordinates of the vertex, passed through to the pixel shader
};

cbuffer ConstantBuffer0
{
    matrix WVP; // World View Projection matrix, used to transform the vertices into screen space
    float4 ambientLightCol; // The colour of the ambient light
    float4 directionalLightDir; // The direction of the directional light
    float4 directionalLightCol; // The colour of the directional light
};

VOut main(VIn input)
{
    VOut output;
    
    // INFO: Position
    output.position = mul(WVP, float4(input.position, 1.0f));
    
    // INFO: Texture Coords
    output.uv = input.uv;
    
    // INFO: Lighting
    float diffuseAmount = dot(directionalLightDir.xyz, input.normal);
   
    // INFO: Clamp the value between 0 and 1 so we don't get negative lighting
    diffuseAmount = saturate(diffuseAmount);
    
    // INFO: Adjust directional light colour by the diffuse amount
    float3 directionalFinal = directionalLightCol.xyz * diffuseAmount;
    
    // INFO: Final Color
    output.colour = saturate(ambientLightCol + float4(directionalFinal, 1.0f));
    
    return output;
}