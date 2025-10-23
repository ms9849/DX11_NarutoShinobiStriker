#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture, g_Texture_Skill;

float   g_Alpha;
float   g_ProgressRate;
int     g_iProgressBarTextureNum;
float   g_MaxSkillCoolDown;
float   g_SkillCoolDown;
float   g_CurrentHP;
float   g_MaxHP;
float   g_iWinSizeX;
float   g_iWinSizeY;

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
    
    /* Out.vPosition.xy => 시야각에 있는 점들을 90에 맞춰준다 */ 
    /* Out.vPosition.z => n~f사이에 있는 점들의 z를 0 ~ f로 바꿔준다. */ 
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
     
    return Out;
}

PS_OUT PS_MASK(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.r < 0.5)
        discard;
    
    return Out;
}

PS_OUT PS_ENEMY_HPBAR(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
       
    if (Out.vColor.r == 0 && Out.vColor.g == 0 && Out.vColor.b == 0)
        discard;
    else if (g_CurrentHP / g_MaxHP > In.vTexcoord.x)
    {
        Out.vColor.r = 1;
        Out.vColor.gb = 0;
    }
    else
        Out.vColor.rgb = 0;
    
    return Out;
}

PS_OUT PS_ProgressBar(PS_IN In)
{
    PS_OUT Out;
  
    /* 체력바 */
    if(g_iProgressBarTextureNum == 2)
    {        
        Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
        
        if (In.vTexcoord.x < g_ProgressRate)
        {
            Out.vColor = float4(0.0f, 0.9f, 0.f, Out.vColor.a);
        }
    }
    
    else
    {
        if (In.vTexcoord.x < g_ProgressRate)
            Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
        else
            discard;   
    }
    
    return Out;
}


PS_OUT PS_FadeInOut(PS_IN In)
{
    PS_OUT Out;
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    /* g_Alpha값 입력받아서 세팅해줄 수 있게 해야 한다. */
    Out.vColor.a *= g_Alpha;

    return Out;
}

PS_OUT PS_Skill(PS_IN In)
{
    PS_OUT Out;

    float2 vCenter = float2(0.5, 0.5);
    float fRadius = 0.4f;

    float2 vDiff = In.vTexcoord - vCenter;
    float fDist = length(vDiff);

    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    // 스킬 아이콘 영역 안일 때만 처리
    if (fDist <= fRadius)
    {
        float fScale = 1.2f;

        float2 vTextureCenter = float2(0.5, 0.5);
        float2 vUV = vTextureCenter + vDiff * fScale;

        Out.vColor = g_Texture_Skill.Sample(DefaultSampler, vUV);

        float fTop = vCenter.y + fRadius; 
        float fBottom = vCenter.y - fRadius;

        float fRatio = g_SkillCoolDown / g_MaxSkillCoolDown;

        float fCut = lerp(fBottom, fTop, fRatio);

        if (In.vTexcoord.y >= fCut)
        {
            Out.vColor.rgb *= 0.2f;
            Out.vColor.a = 1.f;
        }
    }

    return Out;
}
PS_OUT PS_SpecialSkill(PS_IN In)
{
    PS_OUT Out;
    
    float fCenterY = 0.55f;
    float fRadius = 0.3f;
    
    float2 vCenter = float2(0.2, fCenterY);
    float2 vTextureCenter = float2(0.5, 0.5);
    float fRatio = 4.0f;
    
    float2 vDiff = In.vTexcoord - vCenter;
    vDiff.x *= fRatio;
    
    float fDist = length(vDiff);
    
    /* 스킬 아이콘 그려짐 */
    if (fDist <= fRadius)
    {
        float fScale = 1.8f;
        // 중심 0.5 0.5에서 떨어진 거리 구하고, 그만큼 스케일링 해줌.
        float2 vUV = vTextureCenter + vDiff * fScale;

        Out.vColor = g_Texture_Skill.Sample(DefaultSampler, vUV);
        
        float fTop = fCenterY + fRadius; // 아이콘 위쪽 Y
        float fBottom = fCenterY - fRadius; // 아이콘 아래쪽 Y

        float fRatio = g_SkillCoolDown / g_MaxSkillCoolDown; // 0~1
        float fCut = lerp(fBottom, fTop, fRatio);

        /* 스킬 아이콘이 표현되는 최소 최대 y의 texcoord 구한뒤, fRatio를 이용하여 */
        /* 보간을 통해 어느 지점부터 밝게 표현할 것인지 설정 */
        if (In.vTexcoord.y >= fCut)
        {
            Out.vColor *= 0.2f;
            Out.vColor.a = 1.f;
        }
    }
    else
        Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    // RGB 평균값 (밝기)
    float fLuminance = dot(Out.vColor.rgb, float3(0.299, 0.587, 0.114));

    // 너무 어두운 부분은 버림
    if (fLuminance < 0.2f)
        discard;
    
    Out.vColor.rgb += float3(0.5f, 0.5f, 0.5f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_MASK
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK();
    }

    pass UI_ENEMYHPBAR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ENEMY_HPBAR();
    }

    pass UI_ProgressBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ProgressBar();
    }

    pass UI_FadeInOut
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FadeInOut();
    }

    pass UI_Skill
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Skill();
    }

    pass UI_SpecialSkill
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SpecialSkill();
    }

    pass Outfit_BackGround
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SwordTrail
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }
}