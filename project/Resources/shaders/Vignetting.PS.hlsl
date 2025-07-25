#include "Grayscale.PS.hlsl"

struct Vignette
{
    int enableVignette;
    float intensity; //!< 輝度
    float scale;
};
ConstantBuffer<Vignette> gVigentte : register(b1);

PixelShaderOutput ShadingVignetting(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output = ShadingGrayscale(input); 
    //output.color = gTexture.Sample(gSampler, input.texcoord);
    
    if (gVigentte.enableVignette)
    {
         // 周囲を0に、中心になるほど明るくなる様に計算で調整
        float2 correct = input.texcoord * (1.0f - input.texcoord.yx * 1.0f);
        // correctだけで計算すると中心の最大値が0.0625で暗すぎるのでscaleで調整。この例では16倍して１にしている
        float vignette = correct.x * correct.y * gVigentte.intensity;
        // とりあえず0.8乗でそれっぽくしてみた
        vignette = saturate(pow(vignette, gVigentte.scale));
        // 係数として乗算
        output.color.rgb *= vignette;
    }
    
    return output;
    
}

