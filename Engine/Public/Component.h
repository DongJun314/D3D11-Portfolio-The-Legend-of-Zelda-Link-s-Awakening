#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CComponent(const CComponent& _rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	_bool m_bIsCloned = { false };

protected:
	ID3D11Device*			m_pDevice  = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };
};

END