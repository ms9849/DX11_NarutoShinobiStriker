#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DissolveTexture;
texture2D g_DiffuseTexture, g_MaskTexture, g_NoiseTexture;
vector g_vMainColor, g_vSubColor;
vector g_vCamPosition;
int g_iNumWidth, g_iNumHeight, g_iCurrentIdx;


struct VS_IN
{
    float3 vPosition : POSITION;
    
    row_major float4x4 TransformMatrix : WORLD;
    
    float3 vRotation : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float2 vDeltaAcc : TEXCOORD2;
    float3 vDirection : TEXCOORD3;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float  fSize : PSIZE;
    
    float3 vRotation : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float2 vDeltaAcc : TEXCOORD2;
    float3 vDirection : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    /* 로컬 좌표를 의미하는 vPosition */
    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
    
    /* 월드 좌표를 의미하는 vPosition */
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fSize = length(In.TransformMatrix._11_12_13);
    
    Out.vRotation = In.vRotation;
    Out.vLifeTime = In.vLifeTime;
    Out.vDeltaAcc = In.vDeltaAcc;
    Out.vDirection = In.vDirection;
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float  fSize : PSIZE;
    
    float3 vRotation : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float2 vDeltaAcc : TEXCOORD2;
    float3 vDirection : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float2 vNoiseTexcoord : TEXCOORD2;
};

//GS_MAIN(triangle GS_INIn[3])
//GS_MAIN(line GS_IN In[2])

/* 점이니까 무조건 한개만 들어온다는 뜻으로, In[1]. */
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    /* 수명따라 줄어들게 ㄱㄱ*/
    float fScaleFactor = (1 - In[0].vLifeTime.x / In[0].vLifeTime.y);
    
    float3 vLook = normalize(In[0].vDirection.xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].fSize * fScaleFactor * 2.f;
    
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * fScaleFactor * 0.5f;
   
    vLook = vRight;
    vRight = normalize(cross(vUp, vLook)) * In[0].fSize * fScaleFactor * 2.f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_NONUV(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = normalize(In[0].vDirection.xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].fSize;
    
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize;
   
    vLook = vRight;
    vRight = normalize(cross(vUp, vLook)) * In[0].fSize ;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}


[maxvertexcount(6)]
void GS_BillBoard(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].fSize * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

[maxvertexcount(6)]
void GS_SPRITE(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].fSize * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    // 스프라이트 시트 인덱싱
    int iFrameX = g_iCurrentIdx % g_iNumWidth;
    int iFrameY = g_iCurrentIdx / g_iNumWidth;

    float2 fBaseUV = float2(iFrameX / (float) g_iNumWidth, iFrameY / (float) g_iNumHeight);
    float2 fCellSize = float2(1.0f / g_iNumWidth, 1.0f / g_iNumHeight);

    // 사각형 꼭짓점
    float3 vPos = In[0].vPosition.xyz;

    Out[0].vPosition = mul(float4(vPos + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = fBaseUV + float2(0.0f, 0.0f) * fCellSize;
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vNoiseTexcoord = float2(0.0f, 0.0f);
    
    Out[1].vPosition = mul(float4(vPos - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = fBaseUV + float2(1.0f, 0.0f) * fCellSize;
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vNoiseTexcoord = float2(1.0f, 0.0f);
    
    Out[2].vPosition = mul(float4(vPos - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = fBaseUV + float2(1.0f, 1.0f) * fCellSize;
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vNoiseTexcoord = float2(1.0f, 1.0f);
    
    Out[3].vPosition = mul(float4(vPos + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = fBaseUV + float2(0.0f, 1.0f) * fCellSize;
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vNoiseTexcoord = float2(0.0f, 1.0f);

    // 두 삼각형으로 출력
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float2 vNoiseTexcoord : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor *= g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    if (Out.vColor.a >= 0.6f)
        Out.vColor.rgba = g_vMainColor;
    else
        Out.vColor.rgb = g_vSubColor;
    
    if (Out.vColor.a < 0.2f)
        discard;
    
    return Out;
}

PS_OUT PS_FadeOut(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor *= g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    if(Out.vColor.a >= 0.6f)
        Out.vColor.rgb = g_vMainColor;
    else
        Out.vColor.rgb = g_vSubColor;
    
    float fDissolveColor = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    float fAlpha = 1.0f;
    float DissolveThreshold = (In.vLifeTime.x / In.vLifeTime.y);
    fAlpha = smoothstep(DissolveThreshold - 0.05f, DissolveThreshold + 0.05f, fDissolveColor);
    Out.vColor.a *= fAlpha;
    
    if (Out.vColor.a < 0.2f)
        discard;
    
    return Out;
}

PS_OUT PS_FloatAndDrop(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    // mask 샘플 (같은 UV 사용)
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);  

    
    /* 디졸브 수행 */ 
    
    float fDissolveColor = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    float fAlpha = 1.0f;
    float DissolveThreshold = (In.vLifeTime.x / In.vLifeTime.y);
    fAlpha = smoothstep(DissolveThreshold - 0.05f, DissolveThreshold + 0.05f, fDissolveColor);
    Diffuse *= fAlpha;
    
    /* 마스킹 수행 */
    Diffuse.a *= MaskSample.r;

    if (Diffuse.a < 0.1f)
        Diffuse.a = 0;
    else if (Diffuse.a < 0.6f)
        Diffuse.rgb = g_vMainColor.xyz;
    else
        Diffuse.rgb = g_vSubColor.xyz;

    Out.vColor = Diffuse;
    
    if(Out.vColor.a < 0.2f)
        discard;
 
    return Out;
}

PS_OUT PS_SPRITE_PARTICLE(PS_IN In)
{
    PS_OUT Out;
    
    // diffuse 샘플
    float4 Diffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // mask 샘플 (같은 UV 사용)
    float4 MaskSample = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
    
     
    float fDissolveValue = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseTexcoord).r;
    
    float DissolveThreshold = (In.vLifeTime.x / In.vLifeTime.y);
    float fAlpha = smoothstep(DissolveThreshold - 0.1f, DissolveThreshold + 0.1f, fDissolveValue);
    
    Diffuse.a *= fAlpha;
    
    Diffuse.a *= MaskSample.r;

    if (Diffuse.a < 0.1f)
        Diffuse.a = 0;
    else if (Diffuse.a < 0.6f)
        Diffuse.rgb = g_vMainColor.xyz;
    else
        Diffuse.rgb = g_vSubColor.xyz;

    Out.vColor = Diffuse;
 
    if (Out.vColor.a == 0.f)
        discard;
    
    return Out;
}

technique11 DefaultTechnique
{
    // Idx 0
    pass Explosion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // Idx 1
    pass Drop
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BillBoard();
        PixelShader = compile ps_5_0 PS_FadeOut();
    }
    // Idx 2
    pass FloatAndDrop
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BillBoard();
        PixelShader = compile ps_5_0 PS_FloatAndDrop();
    }
    // Idx 3
    pass ExplosionFloat
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BillBoard();
        PixelShader = compile ps_5_0 PS_FadeOut();
    }
    // Idx 4
    pass ExplosionNonUV
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BillBoard();
        PixelShader = compile ps_5_0 PS_FadeOut();
    }
    // Idx 5
    pass SpriteParticle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SPRITE();
        PixelShader = compile ps_5_0 PS_SPRITE_PARTICLE();
    }
}