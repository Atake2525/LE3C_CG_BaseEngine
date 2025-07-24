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
        float2 difference = float2(0.0f, 0.0f); // �c�����ꂼ��̏�ݍ��݌��ʂ��i�[����
        uint width, height; // 1. uvStepSize�̎Z�o
        gTexture.GetDimensions(width, height);
        float2 uvStepSize = float2(rcp(width), rcp(height));
        // index�̒��S���Z�o
        int center = 1 - ((1 - 1) / 2);
        // �F���P�x�ɕϊ����āA��ݍ��݂��s���Ă����B����Filter�p��kernel�ɂȂĂ���̂ŁA��邱�Ǝ��͍̂��܂ł̏�ݍ��݂Ɠ���
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
                // NDC -> View�B P^{-1}�ɂ�����x��y��zw�ɉe����^���Ȃ��̂ŉ��ł��ǂ��B�Ȃ̂ŁA�킴�킴�s���n���Ȃ��Ă��ǂ�
                // gMaterial.projectionInverse��CBuffer���g���ēn���Ă�������
                float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
                float viewZ = viewSpace.z * rcp(viewSpace.w); // �������W�n����f�J���g���W�n�֕ϊ�
                difference.x += viewZ * kPrewittHorizontalKernel[x][y];
                difference.y += viewZ * kPrewittVerticalKernel[x][y];
            }
        }
         // �ω��̒������E�F�C�g�Ƃ��č����B�E�F�C�g�̌�����@���F�X�ƍl������B���Ƃ���diffurence.x�������g���Ή������̃G�b�W�����o�����
        float weight = length(difference);
        weight = saturate(weight); // 0~1�ɂ��Ă���
        
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
        output.color.a = 1.0f;
       // output.color.rgb *= rcp(weight);
    }
    
    return output;
}