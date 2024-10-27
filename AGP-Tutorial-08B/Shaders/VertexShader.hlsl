#define MAX_POINT_LIGHTS 8

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

struct PointLight
{
    float4 position;
    float4 colour;
    float strength;
    bool enabled;
    float2 padding;
};

cbuffer ConstantBuffer0
{
    matrix WVP; // World View Projection matrix, used to transform the vertices into screen space
    float4 ambientLightCol; // The colour of the ambient light
    float4 directionalLightDir; // The direction of the directional light
    float4 directionalLightCol; // The colour of the directional light
    //float4 pointLightPos; // The position of the point light
    //float4 pointLightCol; // The colour of the point light
    //float pointLightStrength; // The strength of the point light
    PointLight pointLights[MAX_POINT_LIGHTS];
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
    
    // INFO: Point Lighting
    float3 pointFinal = float3(0, 0, 0);
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (!pointLights[i].enabled)
            continue;
        
        float4 pointLightDir = float4(normalize(pointLights[i].position.xyz - input.position), 0.0f);
        float pointLightDistance = length(pointLights[i].position.xyz - input.position);
        float pointLightAttenuation = pointLights[i].strength / (pointLightDistance * pointLightDistance + pointLights[i].strength);
        float pointAmount = dot(pointLightDir.xyz, input.normal) * pointLightAttenuation;
        pointAmount = saturate(pointAmount);
        pointFinal += pointLights[i].colour.xyz * pointAmount;
    }
    
    // INFO: Final Color
    output.colour = saturate(ambientLightCol + float4(directionalFinal, 1) + float4(pointFinal, 1));
    
    return output;
}