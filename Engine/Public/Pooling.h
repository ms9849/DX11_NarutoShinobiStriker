#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPooling : public CComponent
{
public:
	typedef struct tagPoolingDesc {
		_wstring strPoolingTag;
	} POOLING_DESC;

private:
	CPooling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPooling(const CPooling& Prototype);
	virtual ~CPooling() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	
	_bool IsUsing() {
		return m_isUsing;
	}

	void Set_Using(_bool bFlag) {
		m_isUsing = bFlag;
	}

	const _wstring& Get_PoolingTag() {
		return m_strPoolingTag;
	}

private:
	/* 
	풀링할 객체의 wstring형 태그. 해당 태그를 통해 풀링 매니저에 접근해서 풀링을 수행해준다.
	*/
	_wstring m_strPoolingTag = {};
	
	/*
	현재 해당 객체가 Object_Manager에서 돌고 있는지 
	*/
	_bool	m_isUsing = { false };
public:
	static CPooling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
