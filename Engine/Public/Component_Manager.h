#pragma once

#include "Mesh.h"
#include "Model.h"
#include "Shader.h"	
#include "Texture.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBuffer_Cell.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Particle_Point.h"

#pragma region FOR_COLLIDER
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#pragma endregion

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint _iNumLevels);
	HRESULT Add_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag, class CComponent* _pPrototype);
	class CComponent* Clone_Component(_uint _iLevelIndex, const wstring& _strPrototypeTag, void* _pArg);
	void Clear(_uint _iLevelIndex);
	class CComponent* Find_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint _iNumLevels);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_uint m_iNumLevels = { 0 };
	map<const wstring, class CComponent*>* m_pMapPrototypes = { nullptr };
	typedef map<const wstring, class CComponent*> PROTOTYPES;
};

END
