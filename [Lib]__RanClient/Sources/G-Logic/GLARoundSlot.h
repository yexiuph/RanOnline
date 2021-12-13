#ifndef GLAROUNDSLOT_H_
#define GLAROUNDSLOT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../[Lib]__Engine/Sources/NaviMesh/NavigationMesh.h"

#ifndef GLLandMan
	class GLLandMan;
#endif //GLLandMan

#ifndef GLChar
	class GLChar;
	typedef GLChar* PGLCHAR;
#endif

#ifndef GLSummonField
	class GLSummonField;
	typedef GLSummonField* PGLSUMMONFIELD;
#endif

#ifndef GLCrow
	class GLCrow;
	typedef GLCrow* PGLCROW;
#endif

//	Note : �ǰ��ݽÿ� �ڽ��� �ֺ��� ������� �ִ� ��ġ�� �ִ��� ����.
//		�ϰ� �� ��ġ�� ��ȯ.
//
class GLARoundSlot
{
public:
	enum
	{
		SLOT_SIZE	= 8,
	};

protected:
	PGLCHAR			m_pThisChar;
	PGLSUMMONFIELD	m_pThisSummon;

	DWORD		m_dwAmount; // ���� ĳ���Ϳ� �����ִ� ���� ����
	DWORD		m_ARoundSlot[SLOT_SIZE];

	bool		m_bUseSummon;

public:
	void SetChar ( PGLCHAR pThisChar );
	void SetSummon ( PGLSUMMONFIELD pThisSummon );
	void SetUseSummon ( bool bUseSummon ) { m_bUseSummon = bUseSummon; }
	D3DXVECTOR3 CalculateTracePos ( const int nDegree, const D3DXVECTOR3 vThisPos,
		const float fRange );

protected:
	BOOL ValidPosition ( NavigationMesh* pNaviMesh, const D3DXVECTOR3 &vTarPos, const DWORD dwTarCell, const D3DXVECTOR3 &vTracePos );

public:
	DWORD GetAmount ()		{ return m_dwAmount; }
	void SetTargetOptSlot (  const DWORD dwAttID, const DWORD dwSlot  );
	void ResetTargetOptSlot ( const DWORD dwAttID, const DWORD dwSlot );

	DWORD TestOptimumSlot ( PGLCROW pCrowAttackor, float fRange, NavigationMesh* pNaviMesh, D3DXVECTOR3 &vTracePos );
	BOOL ValidOptimumSlot ( NavigationMesh* pNaviMesh, const D3DXVECTOR3 &vTracePos );

	DWORD ReqOptimumSlot ( PGLCROW pCrowAttackor, float fRange, NavigationMesh* pNaviMesh, D3DXVECTOR3 &vTracePos );

	void Reset();

public:
	void Update ( GLLandMan* pLandMan );

public:
	GLARoundSlot(void);
	~GLARoundSlot(void);
};

#endif // GLAROUNDSLOT_H_