#include "Bounding.h"

CBounding::CBounding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CBounding::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
