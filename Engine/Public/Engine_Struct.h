#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE			hInstance;
		HWND				hWnd;
		unsigned int		iNumLevels;
		unsigned int		iWinSizeX, iWinSizeY;
		WINMODE				eWindowMode;
	} ENGINE_DESC;

	typedef struct tagLightDesc
	{
		LIGHT			eType;
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;
	}LIGHT_DESC;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3				vPosition;
		XMFLOAT2				vTexCoord;
		static constexpr unsigned int iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXPOSTEX;

	typedef struct tagVertexPositionNormalTexCoor
	{
		XMFLOAT3				vPosition;
		XMFLOAT3				vNormal;
		XMFLOAT2				vTexCoord;
		static constexpr unsigned int iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXNORTEX;

	typedef struct tagVertexPosition
	{
		XMFLOAT3			vPosition;

		static constexpr unsigned int					iNumElements = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

	}VTXPOS;

	/* Pos,Nor,Tangent,TexCoord */
	/* */
	typedef struct tagVertexMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT2			vTexcoord;

		static constexpr unsigned int					iNumElements = { 4 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXMESH;

	typedef struct tagVertexAnimationMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT2			vTexcoord;

		/* 이 정점이 어떤 뼈들의 상태를 받아야하는가? */
		XMUINT4				vBlendIndex;
		/* 각 뼈의 상태가 어떤 비율로 적용되야할지? */
		XMFLOAT4			vBlendWeight;

		static constexpr unsigned int					iNumElements = { 6 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	} VTXANIMMESH;

	typedef struct tagVertexPosTexInstanceParticleDesc
	{
		static constexpr unsigned int					iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
	}VTX_POSTEX_INSTANCE_PARTICLE;

	/* 모델 인스턴스용 데이터 */
	typedef struct tagVertexInstance_Model
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

	}VTX_INSTANCE_MODEL;

	/* 파티클 인스턴스용 정점 데이터 */
	typedef struct tagVertexInstance_Particle
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

		XMFLOAT2			vLifeTime;
	}VTX_INSTANCE_PARTICLE;
	/* 
	Channel에서 사용하는 키프레임 별 Scale, Rotation, Translation.
	추후 선형 보간을 통해 키프레임 사이의 애니메이션을 직접 만들어줘야 한다.
	*/
	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;
		float			fTrackPosition;

	}KEYFRAME;

	/*
	$$$$$$$$\ $$\   $$\ $$$$$$$\   $$$$$$\  $$$$$$$\ $$$$$$$$\ 
	$$  _____|$$ |  $$ |$$  __$$\ $$  __$$\ $$  __$$\\__$$  __|
	$$ |      \$$\ $$  |$$ |  $$ |$$ /  $$ |$$ |  $$ |  $$ |   
	$$$$$\     \$$$$  / $$$$$$$  |$$ |  $$ |$$$$$$$  |  $$ |   
	$$  __|    $$  $$<  $$  ____/ $$ |  $$ |$$  __$$<   $$ |   
	$$ |      $$  /\$$\ $$ |      $$ |  $$ |$$ |  $$ |  $$ |   
	$$$$$$$$\ $$ /  $$ |$$ |       $$$$$$  |$$ |  $$ |  $$ |   
	\________|\__|  \__|\__|       \______/ \__|  \__|  \__|                           
	*/

	/* 머테리얼 하나의 정보. */
	typedef struct tagExportMaterialDesc {
		unsigned int iNumSRVs[AI_TEXTURE_TYPE_MAX];
	} EXPORT_MATERIAL_DESC;


	/*
	$$$$$$\ $$\      $$\ $$$$$$$\   $$$$$$\  $$$$$$$\ $$$$$$$$\ 
	\_$$  _|$$$\    $$$ |$$  __$$\ $$  __$$\ $$  __$$\\__$$  __|
	  $$ |  $$$$\  $$$$ |$$ |  $$ |$$ /  $$ |$$ |  $$ |  $$ |   
	  $$ |  $$\$$\$$ $$ |$$$$$$$  |$$ |  $$ |$$$$$$$  |  $$ |   
	  $$ |  $$ \$$$  $$ |$$  ____/ $$ |  $$ |$$  __$$<   $$ |   
	  $$ |  $$ |\$  /$$ |$$ |      $$ |  $$ |$$ |  $$ |  $$ |   
	$$$$$$\ $$ | \_/ $$ |$$ |       $$$$$$  |$$ |  $$ |  $$ |   
	\______|\__|     \__|\__|       \______/ \__|  \__|  \__|   
	*/

	/* 머테리얼 하나의 읽어들일 정보. */
	typedef struct tagImportMaterialDesc {
		unsigned int iNumSRVs[AI_TEXTURE_TYPE_MAX];
		char szTexturePath[AI_TEXTURE_TYPE_MAX][MAX_PATH];
	} IMPORT_MATERIAL_DESC;

}

#endif // Engine_Struct_h__
