#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* 레퍼런스 카운트를 증가시킨다. */
	/* 증가시킨 이후의 레퍼런스 카운트를 반환한다.*/
	_uint AddRef();

	/* 레퍼런스 카운트를 감소시키거나 삭제한다.*/
	/* 감소시키기 이전의 레퍼런스 카운트를 반환한다. */
	_uint Release();

public:
	virtual void Free() = 0;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_uint m_iRefCnt = { 0 };
};

END