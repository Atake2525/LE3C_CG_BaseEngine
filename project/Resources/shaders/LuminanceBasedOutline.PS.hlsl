#include "GaussianFilter.PS.hlsl"

struct LuminanceBasedOutline
{
    int enableOutline;
    float pow;
};
ConstantBuffer<LuminanceBasedOutline> gOutline : register(b5);

static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

float Luminance(float3 v)
{
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    output = ShadingGaussianFilter(input);
    
    if (gOutline.enableOutline)
    {
        float2 difference = float2(0.0f, 0.0f); // 縦横それぞれの畳み込み結果を格納する
        uint width, height; // 1. uvStepSizeの算出
        gTexture.GetDimensions(width, height);
        float2 uvStepSize = float2(rcp(width), rcp(height));
        // indexの中心を算出
        int center = 1 - ((1 - 1) / 2);
        // 色を輝度に変換して、畳み込みを行っていく。微分Filter用のkernelになているので、やること自体は今までの畳み込みと同じ
        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                float2 texcoord = input.texcoord + float2(x - center, y - center) * uvStepSize;
                //float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                //float luminance = Luminance(fetchColor);
                //difference.x += luminance * kPrewittHorizontalKernel[x][y];
                //difference.y += luminance * kPrewittVerticalKernel[x][y];
                float ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
                // NDC -> View。 P^{-1}においてxとyはzwに影響を与えないので何でも良い。なので、わざわざ行列を渡さなくても良い
                // gMaterial.projectionInverseはCBufferを使って渡しておくこと
                float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
                float viewZ = viewSpace.z * rcp(viewSpace.w); // 同次座標系からデカルト座標系へ変換
                difference.x += viewZ * kPrewittHorizontalKernel[x][y];
                difference.y += viewZ * kPrewittVerticalKernel[x][y];
            }
        }
         // 変化の長さをウェイトとして合成。ウェイトの決定方法も色々と考えられる。たとえばdiffurence.xだけを使えば横方向のエッジが検出される
        float weight = length(difference);
        weight = saturate(weight); // 0~1にしておく
        
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
        output.color.a = 1.0f;
       // output.color.rgb *= rcp(weight);
    }
    
    return output;
}