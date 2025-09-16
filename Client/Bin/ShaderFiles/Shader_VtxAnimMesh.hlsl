#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPosition;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* 메시다 ㅇ영향을 주는 뼈들의 집합*/
matrix g_BoneMatrices[512];

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};


/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float2 vTexcoord : TEXCOORD0;
    
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    /* 스키닝 */
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    /* Out.vPosition.xy => 시야각에 있는 점들을 90에 맞춰준다 */ 
    /* Out.vPosition.z => n~f사이에 있는 점들의 z를 0 ~ f로 바꿔준다. */   
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    /* 정점의 월드 위치를 받아온다. */
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);

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
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    /* 
    빛의 광원. 노멀에 -1 곱해서 방향이 반대가 됐다. 
    따라서 둘이 이루는 각도가 0일때 최대, 90일때가 최소가 된다. (cos 함수와 유사함) 
    -> cos 함수와 유사하므로, 두 벡터의 크기를 1로 설정한 뒤, 내적 계산을 통해 유도한 
    cos 값을 실제 명암으로 사용할 수 있다.
    */
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    
    // reflect 함수로 반사광 구해줌.
    /*
    반사하는 과정 ->
    1. 빛의 방향광을 노멀벡터에 내적시켜 스칼라값을 구해낸다. ( 방향광의 스칼라를 구해냄. )
    2. 구해낸 스칼라값을 노멀벡터에 곱한다. ( 이 과정에서 방향광의 벡터 중 노멀과 평행한 성분만 뽑아낸다.)
    3. 2번 과정에서 뽑아낸 벡터를 2번 곱한뒤, 원래 방향광과 더한다.
    4. 반사 벡터가 구해진다.
    
    2번이 아니라 한번만 더한다면 슬라이드 벡터를 구해낼 수 있다. (평행한 성분을 빼버린, 노멀벡터와 90도를 이루는 벡터)
    */
    vector vReflect = normalize(reflect(normalize(g_vLightDir), In.vNormal));
    
    // 카메라의 look 벡터를 반대 방향으로 돌린다.
    vector vLook = In.vWorldPos - g_vCamPosition;
    
    // 내적해서 0보다 큰 각도(0~180도) 구하기.
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), 50.f);
    
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
       
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}