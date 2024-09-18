#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPartObject abstract : public CGameObject
{
public:
	typedef struct
	{
		CTransform* pParentTransfrom;
	}PARTOBJ_DESC;

protected:
	CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPartObject(const CPartObject& _rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) = 0;
	virtual void Free() override;



protected:
	_float4x4 m_WorldMatrix;
	CTransform* m_pParentTransform = { nullptr };
};

END