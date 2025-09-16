#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CParts_Character : public CPartObject
{
public:
	typedef struct tagPartObjectDesc : public PARTOBJECT_DESC {
		_wstring strModelName;
	} CHARACTER_PART_DESC;

protected:
	CParts_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CParts_Character(const CParts_Character& rhs);
	virtual ~CParts_Character() = default;

public:
	_float				Get_AnimProgress();
	virtual void		Set_AnimProgress(_float fProgress);
	virtual _wstring	Get_CurrentAnim();
	virtual void		Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	virtual _bool		Play_Animation(_float fTimeDelta);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	_wstring m_strModelName = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END