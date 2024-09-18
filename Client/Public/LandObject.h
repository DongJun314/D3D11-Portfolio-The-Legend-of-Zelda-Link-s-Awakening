#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCell;
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CLandObject abstract : public CGameObject
{
public:
	typedef struct LandObj_Desc: public GAMEOBJECT_DESC
	{
		CTransform*				pNavTransform = { nullptr };
		vector<CCell*>			vecCells;

		// GameA
		CTransform*				pGameA_NavTransform = { nullptr };
		vector<CCell*>			vecGameA_Cells;

		// GameB
		CTransform*				pBossStage_NavTransform = { nullptr };
		vector<CCell*>			vecBossStage_Cells;
	}LANDOBJ_DESC;

protected:
	CLandObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLandObject(const CLandObject& _rhs);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT SetUp_OnTerrain(class CTransform* _pTargetTransform, class CNavigation* _pNavigation);
	HRESULT SetUp_OnMiniGameA_Terrain(class CTransform* _pTargetTransform, class CNavigation* _pNavigation);
	HRESULT SetUp_OnBossStage_Terrain(class CTransform* _pTargetTransform, class CNavigation* _pNavigation);
	HRESULT SetUp_OnFirstView_Terrain(class CTransform* _pTargetTransform, class CNavigation* _pNavigation);
	HRESULT SetUp_OnFirstView_MiniGameA_Terrain(class CTransform* _pTargetTransform, class CNavigation* _pNavigation);
	HRESULT SetUp_OnFirstView_BossStage_Terrain(class CTransform* _pTargetTransform, class CNavigation* _pNavigation);

public:
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) = 0;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	vector<CCell*>		m_vecCells;
	CTransform*			m_pNavTransform = { nullptr };

	// Mini GameA
	vector<CCell*>		m_vecGameA_Cells;
	CTransform*			m_pGameA_NavTransform = { nullptr };

	// Mini GameB
	vector<CCell*>		m_vecBossStage_Cells;
	CTransform*			m_pBossStage_NavTransform = { nullptr };
};

END