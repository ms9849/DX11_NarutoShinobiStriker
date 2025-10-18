#include "Engine_Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D   g_DiffuseTexture;
texture2D   g_MaskTexture;
texture2D   g_NoiseTexture;

float4 g_vMainColor, g_vSubColor;
float       g_fDeltaU, g_fDeltaV;
float       g_fLifeTime, g_fLifeTimeAcc;
int         g_iNumWidth, g_iNumHeight;
int         g_iCurrentIdx;

/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float2 vTexcoord : TEXCOORD0;
    float2 vNoiseTexCoord : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vNoiseTexCoord : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vNoiseTexCoord = In.vTexcoord;
    Out.vTexcoord = In.vTexcoord;
    Out.vTexcoord.x = In.vTexcoord.x / g_iNumWidth + (1.0f / g_iNumWidth) * (g_iCurrentIdx % g_iNumWidth);
    Out.vTexcoord.y = In.vTexcoord.y / g_iNumHeight + (1.0f / g_iNumHeight) * (g_iCurrentIdx / g_iNumWidth);
    
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    
    return Out;
}

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vNoiseTexCoord : TEXCOORD1;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_DISSOLVE(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // mask 샘플 (같은 UV 사용)
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
   
    float NoiseValue = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseTexCoord).r;
   
    if (g_fLifeTimeAcc / g_fLifeTime >= 0.5f)
    {
        float fAlpha = 1.0f;
        float DissolveThreshold = (g_fLifeTimeAcc / g_fLifeTime - 0.5f) * 2.f; // 0 ~ 1
        fAlpha = smoothstep(DissolveThreshold - 0.1f, DissolveThreshold + 0.1f, NoiseValue);
        Diffuse.a *= fAlpha;
    }

    /* 마스킹 수행 */
    Diffuse.a *= MaskSample.r;

    if (Diffuse.a < 0.1f)
        Diffuse.a = 0;
    else if (Diffuse.a < 0.6f)
        Diffuse.rgba = g_vMainColor.xyzw;
    else
        Diffuse.rgba = g_vSubColor.xyzw;
    
    Out.vDiffuse = Diffuse;
 
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_DISSOLVE_NONLIGHT(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // mask 샘플 (같은 UV 사용)
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
   
    float NoiseValue = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseTexCoord).r;
    
    float fAlpha = 1.0f;

    if (g_fLifeTimeAcc / g_fLifeTime >= 0.5f)
    {
        float DissolveThreshold = (g_fLifeTimeAcc / g_fLifeTime - 0.5f) * 2.f; // 0 ~ 1
        fAlpha = smoothstep(DissolveThreshold - 0.1f, DissolveThreshold + 0.1f, NoiseValue);
    }

    Diffuse.a *= fAlpha;

    /* 마스킹 수행 */
    Diffuse.a *= MaskSample.r;

    if (Diffuse.a < 0.1f)
        discard;
    else if (Diffuse.a < 0.6f)
        Diffuse.rgba = g_vMainColor.xyzw;
    else
        Diffuse.rgba = g_vSubColor.xyzw;
    
    Out.vDiffuse = Diffuse;
 
    return Out;
}

PS_OUT PS_DISSOLVE_IMMEDIATE(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // mask 샘플 (같은 UV 사용)
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
   
    float NoiseValue = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseTexCoord).r;
    
    float DissolveThreshold = g_fLifeTimeAcc / g_fLifeTime; // 0 ~ 1
    /* 디졸브 먼저 수행 */
    float fAlpha = smoothstep(DissolveThreshold - 0.1f, DissolveThreshold + 0.1f, NoiseValue);

    Diffuse.a *= fAlpha;
 
    /* 흑백 이미지라 rgb로 투명도 표현중인 것 같으니까 일단 이렇게.. */
    /* 마스킹 수행 */
    Diffuse.a *= MaskSample.r;

    if (Diffuse.a < 0.1f)
        Diffuse.a = 0;
    else if (Diffuse.a < 0.6f)
        Diffuse.rgba = g_vMainColor.xyzw;
    else
        Diffuse.rgba = g_vSubColor.xyzw;

    Out.vDiffuse = Diffuse;
 
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_DELTAUV(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    // mask 샘플 (같은 UV 사용)
    In.vTexcoord -= float2(g_fDeltaU * g_fLifeTimeAcc, g_fDeltaV * g_fLifeTimeAcc);
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Diffuse.a *= MaskSample.r;
    
    if (Diffuse.a < 0.1f)
        Diffuse.a = 0;
    else if (Diffuse.a < 0.6f)
        Diffuse.rgba = g_vMainColor.xyzw;
    else
        Diffuse.rgba = g_vSubColor.xyzw;
    
    Out.vDiffuse = Diffuse;
   
    return Out;
}

PS_OUT PS_ONLYMASK(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // mask 샘플 (같은 UV 사용)
    In.vTexcoord -= float2(g_fDeltaU * g_fLifeTimeAcc, g_fDeltaV * g_fLifeTimeAcc);
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);

    /* 마스킹 수행 */
    Diffuse.a *= MaskSample.r;

    if (Diffuse.a < 0.6f)
        Diffuse.rgba = g_vMainColor.xyzw;
    else
        Diffuse.rgba = g_vSubColor.xyzw;

    Out.vDiffuse = Diffuse;
 
    return Out;
}

PS_OUT PS_NONE(PS_IN In)
{    
    PS_OUT Out;
    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
   
    if (Out.vDiffuse.a < 0.6f)
        Out.vDiffuse.rgba = g_vMainColor.xyzw;
    else
        Out.vDiffuse.rgba = g_vSubColor.xyzw;
    
    return Out;
}

PS_OUT PS_ALL(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // mask 샘플 (같은 UV 사용)
    In.vTexcoord -= float2(g_fDeltaU * g_fLifeTimeAcc, g_fDeltaV * g_fLifeTimeAcc);
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float NoiseValue = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseTexCoord).r;
    
    float fAlpha = 1.0f;

    if (g_fLifeTimeAcc / g_fLifeTime >= 0.5f)
    {
        float DissolveThreshold = (g_fLifeTimeAcc / g_fLifeTime - 0.5f) * 2.f; // 0 ~ 1
        fAlpha = smoothstep(DissolveThreshold - 0.1f, DissolveThreshold + 0.1f, NoiseValue);
    }

    Diffuse.a *= fAlpha;

    /* 마스킹 수행 */
    Diffuse.a *= MaskSample.r;

    if (Diffuse.a < 0.01f)
        Diffuse.a = 0;
    else if (Diffuse.a < 0.6f)
        Diffuse.rgba = g_vMainColor.xyzw;
    else
        Diffuse.rgba = g_vSubColor.xyzw;

    Out.vDiffuse = Diffuse;
 
    return Out;
}

technique11 DefaultTechnique
{
    pass Dissolve //0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }

    pass DissolveImmediate //1 
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_IMMEDIATE();
    }

    pass DeltaUV //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DELTAUV();
    }

    pass NONLIGHT //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_NONLIGHT();
    }

    pass ONLYMASK //4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ONLYMASK();
    }
    pass NONE //5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NONE();
    }

    pass ALL //6
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ALL();
    }

    pass Dissolve_CULL //7
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }

    pass DissolveImmediate__CULL //8 
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_IMMEDIATE();
    }
}