#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct Instance_Desc
	{
		_float3			vPivot;
		_float3			vCenter;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;
		_float2			vLifeTime;
		_bool			bIsLoop;
		_float4			vColor;
		_float			fDuration;
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& _rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	INSTANCE_DESC Get_Instance_Desc() const {
		return m_tInstanceData;
	}
	_uint Get_NumInstance() const {
		return m_iNumInstance;
	}
	mt19937_64* Get_RandomNumber() {
		return &m_RandomNumber;
	}
	_float* Get_mLifeTime() {
		return m_pLifeTime;
	}
	void Set_mLifeTime(_uint _iIndex, _float _fLifeTime) {
		m_pLifeTime[_iIndex] = _fLifeTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render();
	virtual HRESULT Bind_Buffers();
public:
	void Tick_Drop(_float _fTimeDelta);
	void Tick_Spread(_float _fTimeDelta);
	void Tick_Charge(_float _fTimeDelta);
	void Tick_Reset(_float _fTimeDelta);

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;



protected:
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;
	INSTANCE_DESC				m_tInstanceData;

	_bool						m_bIsFinished = { false };
	_float						m_fTimeAcc = { 0.f };

protected:
	_float*						m_pSpeed = { nullptr };
	_float*						m_pLifeTime = { nullptr };

	ID3D11Buffer*				m_pVBInstance = { nullptr };
};

END