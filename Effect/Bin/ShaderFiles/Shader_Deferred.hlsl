#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
texture2D g_Texture;
vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vCamPosition;

texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_FinalShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_ShadowTexture;

texture2D g_BlurTexture;
texture2D g_BlurSmallTexture;
texture2D g_BlurXTexture;
texture2D g_BlurSmallXTexture;

texture2D g_LightLampTexture;
texture2D g_OutlineTexture;
texture2D g_StaticShadowTexture;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vBackBuffer : SV_TARGET0;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    float fNdotL = dot(normalize(g_vLightDir) * -1.f, vNormal);
    fNdotL = ceil(fNdotL * 6.f) / 6.f;

    if(fNdotL <= 0.2f)
        fNdotL = 0.05f;
    else
        fNdotL = 1.f;

    Out.vShade = g_vLightDiffuse * saturate(max(fNdotL, 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
  
    vector vPosition;
    
    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);
    
    /* 로컬위치 * 월드   */
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
    
    vector vPosition;
    
    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);
    
    /* 로컬위치 * 월드   */
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    vector vLightDir = vPosition - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    float fNdotL = dot(normalize(vLightDir) * -1.f, vNormal);
    
    Out.vShade = fAtt * (g_vLightDiffuse * saturate(max(fNdotL, 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
 
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    
    Out.vSpecular = fAtt * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f));
    
    return Out;
}

/*
float g_fWeights[25] =
{
    0.0001, 0.0003, 0.0008, 0.0021, 0.0052, 0.0131, 0.0316, 0.0750,
    0.155, 0.295, 0.440, 0.560, 0.590,
    0.560, 0.440, 0.295, 0.155, 0.0750, 0.0316, 0.0131, 0.0052,
    0.0021, 0.0008, 0.0003, 0.0001
};
*/


/*
float g_fWeights[37] =
{
    0.056135, 0.076621, 0.102740, 0.135335, 0.175131, 0.222635, 0.278037, 0.341108, 0.411112,
    0.486752, 0.566154, 0.646905, 0.726149, 0.800737, 0.867428, 0.923116, 0.965069, 0.991151,
    1.000000, 0.991151, 0.965069, 0.923116, 0.867428, 0.800737, 0.726149, 0.646905, 0.566154,
    0.486752, 0.411112, 0.341108, 0.278037, 0.222635, 0.175131, 0.135335, 0.102740, 0.076621,
    0.056135
};
*/

/*
툰 셰이딩을 기본으로 적용한다. 
인자 줘서 설정 가능하게 하는게 이상적이겠지만 일단은 제외...
*/

/*
float g_fWeights[41] =
{
    0.01688, 0.02513, 0.03666, 0.05239, 0.07337, 0.10067, 0.13534, 0.17826, 0.23007, 0.29092,
    0.36045, 0.43756, 0.52045, 0.60653, 0.69257, 0.77484, 0.84937, 0.91225, 0.96001, 0.98985,
    1.0,
    0.98985, 0.96001, 0.91225, 0.84937, 0.77484, 0.69257, 0.60653, 0.52045, 0.43756, 0.36045,
    0.29092, 0.23007, 0.17826, 0.13534, 0.10067, 0.07337, 0.05239, 0.03666, 0.02513, 0.01688
};
*/


float g_fWeights[21] =
{
    0.5520, 0.6065, 0.6669, 0.7318, 0.7993, 0.8676, 0.9353, 0.9999, 1.0561, 1.0902, 1.0,
    1.0902, 1.0561, 0.9999, 0.9353, 0.8676, 0.7993, 0.7318, 0.6669, 0.6065, 0.5520
};


float g_fSmallWeights[13] =
{
    0.39965, 0.4338, 0.46765, 0.49995, 0.52805, 0.5451, 0.5,
    0.5451, 0.52805, 0.49995, 0.46765, 0.4338, 0.39965
};

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(ShadeSampler, In.vTexcoord);
    
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
   
    Out.vBackBuffer = vDiffuse * vShade; //+ vSpecular;
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
    
    vector vPosition;
    
    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);
    
    /* 로컬위치 * 월드   */
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    vPosition = mul(vPosition, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
    
    /* -1, 1 -> 0, 0  */
    /* 1, -1 -> 1, 1  */
    
    float2 vTexcoord;
    
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    vector vStaticShadowDepth = g_StaticShadowTexture.Sample(DefaultSampler, vTexcoord);
    bool IsStatic = false;
    
    if (vPosition.w - 0.2f > vStaticShadowDepth.x * 3000.0f)
    {
        Out.vBackBuffer.rgb *= 0.4f;
        Out.vBackBuffer.a *= 0.5f;
        IsStatic = true;
    }
    
    if(false == IsStatic)
    {
        int iCount = 0;
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                vector vShadowDepth = g_ShadowTexture.Sample(DefaultSampler, vTexcoord + float2( i* 1 / 1280.f, j *1 / 720.f) * 0.1f);

                if (vPosition.w - 0.2f > vShadowDepth.x * 3000.0f)
                    iCount++;
            }
        }
        
        Out.vBackBuffer.rgb *= lerp(1.0f, 0.4f, iCount / 9.0f);
        Out.vBackBuffer.a *= 0.5f;
    }
    
    /* 블러 후처리 */
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    for (int i = -10; i <= 10; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + i / 720.f;
        
        vColor += g_fWeights[i + 10] * g_BlurXTexture.Sample(ClampSampler, vTexcoord);
    }
  
    Out.vBackBuffer += vColor / 8.f;
    
    /* 약한 블러 후처리 */
    vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    for (int i = -6; i <= 6; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + i / 720.f;
        
        vColor += g_fSmallWeights[i + 6] * g_BlurSmallXTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBackBuffer += vColor / 6.5f;
    
    
    Out.vBackBuffer *= g_OutlineTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

struct PS_OUT_BLUR_X
{
    float4 vBlurX : SV_TARGET0;
};


PS_OUT_BLUR_X PS_MAIN_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -10; i <= 10; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / 1280.f;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 10] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor / 12.5f;
    
    return Out;
}

PS_OUT_BLUR_X PS_MAIN_SMALL_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -6; i <= 6; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / 1280.f;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fSmallWeights[i + 6] * g_BlurSmallTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor / 10.f;
    
    return Out;
}

float Laplacian_Mask[9] =
{
    -1, -1, -1,
    -1, 8, -1,
    -1, -1, -1,
};

float PixelsX[9] = { 
    -1.f, 0.f, 1.f,
    -1.f, 0.f, 1.f,
    -1.f, 0.f, 1.f
};
float PixelsY[9] = { 
    -1.f, -1.f, -1.f,
    0.f, 0.f, 0.f,
    1.f, 1.f, 1.f
};

PS_OUT_BACKBUFFER PS_MAIN_Outline(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
  
    Out.vBackBuffer = float4(0.f, 0.f, 0.f, 0.f);
    
    for (int iIdxY = 0; iIdxY < 3; ++iIdxY)
    {
        for (int iIdxX = 0; iIdxX < 3; ++iIdxX)
        {
            Out.vBackBuffer += Laplacian_Mask[iIdxY * 3 + iIdxX] * (g_NormalTexture.Sample(ShadeSampler, In.vTexcoord +
            float2(PixelsX[iIdxY * 3 + iIdxX] * 1.1f / 1280.f, PixelsY[iIdxY * 3 + iIdxX] * 1.1f / 720.f)));
        }
    }
    
    /* 
    float3(0.3f, 0.59f, 0.11f)) 가 가지는 의미
    -> 인간이 느끼는 색상 별 예민도. 이 값으로 내적하면 그레이스케일 형태의 (rgb가 같은)
    색이 뽑혀나온다고 한다.
    */
    float GrayColor = 1 - dot(Out.vBackBuffer.rgb, float3(0.3f, 0.59f, 0.11f));
    //float GrayColor = 1 - length(Out.vBackBuffer.rgb);
    
    /*
    다 살리면 너무 민감하니까 성분이 약한 부분은 바로 죽여주기.
    */
    if(GrayColor > 0.5f)
        GrayColor = 1.f;
    else
        GrayColor = 0.f;
    
    Out.vBackBuffer = float4(GrayColor, GrayColor, GrayColor, 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_X();
    }

    pass Outline
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Outline();
    }

    pass Blur_Small_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SMALL_X();
    }
}