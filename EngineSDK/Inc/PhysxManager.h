#pragma once

#include "Base.h"

/* CRTDBG의 NEW 키워드 방지용.. 하.... */
#ifdef new
#undef new
#endif

#define PX_PHYSX_STATIC_LIBRARY

#include "Physx/PxPhysics.h"
#include "Physx/PxPhysicsAPI.h"

#ifdef _DEBUG
#define new DBG_NEW
#endif

using namespace physx;

NS_BEGIN(Engine)

class CPhysxManager : public CBase
{
private:
	CPhysxManager();
	virtual ~CPhysxManager() = default;

public:
	HRESULT Initialize();

	void Add_GameObject_ToPhysx(class CGameObject* pGameObject);
	void Add_Geometry_ToPhysx(class CGameObject* pGameObject, class CModel* pModel);

	void Check_GeometryCollision(_float fTimeDelta);

private:
	/* 모든 Physx 모듈을 사용하려면 필요한 인스턴스. 다렉의 Device나 현재 프레임워크의 GameInstance 같은 녀석.*/
	PxFoundation* m_PxFoundation = { nullptr };

	/* Foundation을 생성하기 위해 필요한 녀석 */
	PxDefaultAllocator m_DefaultAllocator = {};
	PxDefaultErrorCallback m_DefaultErrorCallback = {};

	/* Scene 내 모든 녀석들에게 영향을 미치는 녀석이라고 한다. (중력 같은거?)*/
	PxPhysics* m_PxPhysx = { nullptr };

	/* 말그대로 씬의 개념. 액터들을 모아놓고 액터끼리의 상호작용을 수행한다. */
	PxScene* m_PxScene = { nullptr };

	/* Physx Visual Debugger를 사용하기 위해 필요한 녀석 */
	PxPvd* m_Pvd = { nullptr };
	/* CPU 멀티 스레딩을 사용하기 위한 녀석 */
	PxDefaultCpuDispatcher* m_PxDispatcher = { nullptr };
	PxMaterial* m_pTestMaterial = { nullptr };
	
	/* 
	물리 영향을 받는 Dynamic 한 녀석들,
	반대로 static은 벽이나 건물 등 물리적인 영향을 
	받지 않을때 사용한다고 한다.
	*/
	vector<pair<class CGameObject*, PxRigidDynamic*>> m_DynamicActors = {};
	/*
	구워놓은 충돌용 메시들 보관용. 추후 순회해서 
	객체들과의 충돌 감지를 수행해야 한다.
	*/
	vector<PxTriangleMeshGeometry*> m_Geometries = {};
	vector<PxTriangleMesh*> m_TriangleMeshes = {};
public:
	static CPhysxManager* Create();
	virtual void Free() override;
};

NS_END