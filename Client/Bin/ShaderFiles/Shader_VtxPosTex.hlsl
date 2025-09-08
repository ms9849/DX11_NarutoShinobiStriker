
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture, g_Texture_Skill;

float   g_Alpha;
float   g_ProgressRate;
int     g_iProgressBarTextureNum;
float   g_MaxSkillCoolDown;
float   g_SkillCoolDown;

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

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
    
    if (Out.vColor.a < 0.3)
       discard;
    
    return Out;
}

PS_OUT PS_MASK(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.r < 0.3)
        discard;
    
    return Out;
}

PS_OUT PS_ProgressBar(PS_IN In)
{
    PS_OUT Out;
    
    /* 필살기 게이지 */
    if(g_iProgressBarTextureNum == 2)
    {
        float fSlope = 0.05f;
        
        Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
        
        if (In.vTexcoord.x < g_ProgressRate - fSlope * (In.vTexcoord.y - 0.5f))
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
    float fDist = length(In.vTexcoord - vCenter);
    
    /* 스킬 아이콘 그려짐 */
    if (fDist <= 0.4)
    {
        float fScale = 1.2;
        // 중심 0.5 0.5에서 떨어진 거리 구하고, 그만큼 스케일링 해줌.
        float2 vUV = vCenter + (In.vTexcoord - vCenter) * fScale;

        Out.vColor = g_Texture_Skill.Sample(DefaultSampler, vUV);
        
        if (0.9f - (g_SkillCoolDown / g_MaxSkillCoolDown) * 0.8f >= In.vTexcoord.y)
        {
            Out.vColor *= 0.2f;
            Out.vColor.a = 1.f;
        }

    }
    
    else
        Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_SpecialSkill(PS_IN In)
{
    PS_OUT Out;
    float2 vCenter = float2(0.2, 0.55);
    float2 vTextureCenter = float2(0.5, 0.5);
    float fRatio = 4.0f;
    
    float2 vDiff = In.vTexcoord - vCenter;
    vDiff.x *= fRatio;
    
    float fDist = length(vDiff);
    /* 스킬 아이콘 그려짐 */
    if (fDist <= 0.30)
    {
        float fScale = 1.8f;
        // 중심 0.5 0.5에서 떨어진 거리 구하고, 그만큼 스케일링 해줌.
        float2 vUV = vTextureCenter + vDiff * fScale;

        Out.vColor = g_Texture_Skill.Sample(DefaultSampler, vUV);
    }
    else
        Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass UI
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_MASK
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MASK();
    }

    pass UI_ProgressBar
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_ProgressBar();
    }

    pass UI_FadeInOut
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_FadeInOut();
    }

    pass UI_Skill
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Skill();
    }

    pass UI_SpecialSkill
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_SpecialSkill();
    }
}