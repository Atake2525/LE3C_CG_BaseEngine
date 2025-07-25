#include "Vignetting.PS.hlsl"
    
static const float PI = 3.14159265f;

struct BoxFilter
{
    bool enableBoxFilter;
    int size;
};
ConstantBuffer<BoxFilter> gBoxFilter : register(b2);

PixelShaderOutput ShadingBoxFilter(VertexShaderOutput input)
{
    
    
    PixelShaderOutput output;
    output = ShadingVignetting(input);
    
    if (gBoxFilter.enableBoxFilter)
    {
    
        const float kernel = 1.0f / (gBoxFilter.size * gBoxFilter.size);
        
        uint width, height; // 1. uvStepSizeの算出
        gTexture.GetDimensions(width, height);
        float2 uvStepSize = float2(rcp(width), rcp(height));
        float3 color = float3(0.0f, 0.0f, 0.0f);
        
        // indexの中心を算出
        int center = gBoxFilter.size - ((gBoxFilter.size - 1) / 2);
        for (int x = 0; x < gBoxFilter.size + 0; ++x)
        {
            for (int y = 0; y < gBoxFilter.size + 0; ++y)
            {
                // 3. 現在のtexcoordを算出
                float2 texcoord = input.texcoord + float2(x - center, y - center) * uvStepSize;
                // 4. 色に1/9掛けて足す
                float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                color.rgb += fetchColor * kernel;
            }
        }
    
        output.color.rgb = color;
    }
    
    return output;
}