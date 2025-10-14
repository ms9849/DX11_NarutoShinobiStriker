
/* 재질들 임시로 세팅 */
vector g_vMtrlDiffuse = 1.f;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;

/* 디폴트 샘플러 세팅 */
sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

/* Rasterizer 디폴트 세팅 */
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
};

/* Rasterizer 와이어 프레임 */
RasterizerState RS_Wireframe
{
    FillMode = Wireframe;
};

/* Rasterizer 컬 모드 Front. 주로 스카이박스에 쓰임. */
RasterizerState RS_Cull_Front
{
    CullMode = Front;
};

RasterizerState RS_Cull_None
{
    CullMode = None;
};

/* DepthStencil 디폴트 세팅 */
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

/* DepthStencil 깊이  테스트 끄기. 마찬가지로 주로 스카이박스에 쓰임. */
DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

/* 블렌더 끄기. 어떤 렌더타겟에 블렌드 스테이트를 넘겨줄 것인지 설정. */
BlendState BS_None
{
    BlendEnable[0] = false;
};

/* 블렌더 켜기. 어떤 렌더타겟에 블렌드 스테이트를 넘겨줄 것인지 설정. */
BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};
