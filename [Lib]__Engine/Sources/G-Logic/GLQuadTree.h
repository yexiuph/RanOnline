#pragma once

#include "GLList.h"

enum EMDIVISION
{
	//QUAD_DIV_00	= 1,
	QUAD_DIV_01		= 2,	//	4
	QUAD_DIV_02		= 4,	//	16
	QUAD_DIV_03		= 8,	//	64
	QUAD_DIV_04		= 16,	//	256
	QUAD_DIV_05		= 32,	//	1024
	QUAD_DIV_06		= 64,	//	4096
	QUAD_DIV_07		= 128,	//	16384
	QUAD_DIV_08		= 256,	//	65536
	QUAD_DIV_09		= 512,	//	262144
	QUAD_DIV_10		= 1024,	//	1048576

	QUAD_DEFAULT_DIV =QUAD_DIV_04,
};

enum SQUADNODEFLAG
{
	QUAD_LEAF	= 0x0001,
};

inline int TOOVERNUMBER ( float fValue )
{
	int nValue;
	nValue = (int) fValue;
	if ( fValue >= 0.0f )	
	{
		if ( fValue != (float)nValue )	nValue += +1;
	}

	return nValue;
}

inline int TODOWNNUMBER ( float fValue )
{
	int nValue;
	nValue = (int) fValue;
	if ( fValue < 0.0f )	
	{
		if ( fValue != (float)nValue )	nValue += -1;
	}

	return nValue;
}


template<class TYPE >
struct SQUADNODE
{
	DWORD dwFlag;
	int	nMaxX, nMaxZ;
	int	nMinX, nMinZ;

	DWORD				dwCID;
	TYPE*				pData;

	SQUADNODE*			pParent;	//	부모 노드.

	SQUADNODE*			pNode11;	//	자식노드들.
	SQUADNODE*			pNode12;
	SQUADNODE*			pNode21;
	SQUADNODE*			pNode22;

	SQUADNODE*			pFindNext;	//	검색시에 사용되는 노드.

	SQUADNODE () :
		dwFlag(NULL),
		nMaxX(0),
		nMaxZ(0),
		nMinX(0),
		nMinZ(0),
		dwCID(-1),
		pData(0),
		pParent(NULL),
		pNode11(NULL),
		pNode12(NULL),
		pNode21(NULL),
		pNode22(NULL),
		pFindNext(NULL)
	{
	}
	~SQUADNODE ()
	{
		SAFE_DELETE(pData);

		SAFE_DELETE(pNode11);
		SAFE_DELETE(pNode12);
		SAFE_DELETE(pNode21);
		SAFE_DELETE(pNode22);
	}

	BOOL IsWithInThisNode ( int nX, int nZ )
	{
		return ( nMinX<=nX && nX<nMaxX && nMinZ<=nZ && nZ<nMaxZ );
	}
};

struct BOUDRECT
{
	int nMaxX;
	int nMaxZ;
	int nMinX;
	int nMinZ;
	
	BOUDRECT () :
		nMaxX(0),
		nMaxZ(0),
		nMinX(0),
		nMinZ(0)
	{
	}

	BOUDRECT ( int MaX, int MaZ, int MiX, int MiZ )
	{
		nMaxX = MaX;
		nMaxZ = MaZ;
		nMinX = MiX;
		nMinZ = MiZ;
	}

	BOOL IsWithIn ( int nX, int nZ )
	{
		return ( nMinX<=nX && nX<nMaxX && nMinZ<=nZ && nZ<nMaxZ );
	}
};
typedef BOUDRECT* PBOUDRECT;

template<class TYPE >
class GLQuadTree
{
protected:
	float	m_fsrcMaxX, m_fsrcMaxZ;
	float	m_fsrcMinX, m_fsrcMinZ;

protected:
	int		m_nMaxX;
	int		m_nMaxZ;
	int		m_nMinX;
	int		m_nMinZ;

	int		m_nDivSizeX;
	int		m_nDivSizeZ;

	int		m_nAxisX;
	int		m_nAxisZ;

	EMDIVISION			m_emDivision;
	SQUADNODE<TYPE>*	m_pRootNode;

	DWORD				m_dwCellCount;
	SQUADNODE<TYPE>**	m_pNodeArray;

protected:
	HRESULT MakeTree ( int nMaxX, int nMaxZ, int nMinX, int nMinZ,
						int nDivNum, SQUADNODE<TYPE>** ppNode );
public:
	HRESULT SetState ( float fMaxX, float fMaxZ, float fMinX, float fMinZ, EMDIVISION emDiv );
	
	int GetMaxX ()		{ return m_nMaxX; }
	int GetMaxZ ()		{ return m_nMaxZ; }
	int GetMinX ()		{ return m_nMinX; }
	int GetMinZ ()		{ return m_nMinZ; }

	int GetSizeX ()		{ return m_nMaxX-m_nMinX; }
	int GetSizeZ ()		{ return m_nMaxZ-m_nMinZ; }

	int	GetDivSizeX ()	{ return m_nDivSizeX; }
	int	GetDivSizeZ ()	{ return m_nDivSizeZ; }

	int GetAxisX ()		{ return m_nAxisX; }
	int GetAxisZ ()		{ return m_nAxisZ; }

	DWORD GetCellNum ()	{ return m_dwCellCount; }

public:
	HRESULT MakeTree ();

public:
	//	Note : x, z 지점이 속하는 노드를 리턴함. pNode는 탐색 초기 지점 ( pRoot )
	//
	void FindNodes ( BOUDRECT &Rect, SQUADNODE<TYPE>* pNode, SQUADNODE<TYPE>** pFindList );
	SQUADNODE<TYPE>* FindNode ( int nX, int nZ )	{ return FindNode ( nX, nZ, m_pRootNode ); }
	SQUADNODE<TYPE>* FindNode ( int nX, int nZ, SQUADNODE<TYPE>* pNode );
	SQUADNODE<TYPE>* GetRootNode ()	{ return m_pRootNode; }
	
	SQUADNODE<TYPE>* GetNode ( int nIndex )
	{
		//GASSERT(m_dwCellCount>(DWORD)nIndex&&"인덱스가 셀사이즈를 초과하였습니다.");
		if ( nIndex < 0 || m_dwCellCount <= (DWORD)nIndex )
		{
			CDebugSet::ToLogFile( "m_dwCellCount < 0 || m_dwCellCount > (DWORD)nIndex" );
			return NULL;
		}

		return m_pNodeArray[nIndex];
	}

public:
	GLQuadTree(void);
	~GLQuadTree(void);
};

template<class TYPE>
GLQuadTree<TYPE>::GLQuadTree(void) :
	m_fsrcMaxX(0.0f),
	m_fsrcMaxZ(0.0f),
	m_fsrcMinX(0.0f),
	m_fsrcMinZ(0.0f),

	m_nMaxX(0),
	m_nMaxZ(0),
	m_nMinX(0),
	m_nMinZ(0),
	m_nDivSizeX(0),
	m_nDivSizeZ(0),
	m_nAxisX(0),
	m_nAxisZ(0),
	m_pRootNode(NULL),
	m_pNodeArray(NULL)
{
}

template<class TYPE>
GLQuadTree<TYPE>::~GLQuadTree(void)
{
	SAFE_DELETE(m_pRootNode);
	SAFE_DELETE_ARRAY(m_pNodeArray);
}

template<class TYPE>
HRESULT GLQuadTree<TYPE>::SetState ( float fMaxX, float fMaxZ, float fMinX, float fMinZ, EMDIVISION emDiv )
{
	if ( fMaxX <= fMinX || fMaxZ <= fMinZ )		return E_FAIL;

	m_fsrcMaxX = fMaxX;
	m_fsrcMaxZ = fMaxZ;
	m_fsrcMinX = fMinX;
	m_fsrcMinZ = fMinZ;

	m_nMaxX = TOOVERNUMBER ( fMaxX );
	m_nMaxZ = TOOVERNUMBER ( fMaxZ );

	m_nMinX = TODOWNNUMBER ( fMinX );
	m_nMinZ = TODOWNNUMBER ( fMinZ );

	m_emDivision = emDiv;

	m_nAxisX = ( m_nMaxX - m_nMinX ) / 2 + m_nMinX ;
	m_nAxisZ = ( m_nMaxZ - m_nMinZ ) / 2 + m_nMinZ;

	float fSizeX = float ( m_nMaxX - m_nMinX );
	float fSizeZ = float ( m_nMaxZ - m_nMinZ );

	float fDivSizeX = fSizeX / float(emDiv);
	float fDivSizeZ = fSizeZ / float(emDiv);

	if ( fDivSizeX > fDivSizeZ )	fDivSizeZ = fDivSizeX;
	else							fDivSizeX = fDivSizeZ;

	m_nDivSizeX = TOOVERNUMBER ( fDivSizeX );
	m_nDivSizeZ = TOOVERNUMBER ( fDivSizeZ );

	int nSizeX = m_nDivSizeX * emDiv;
	int nSizeZ = m_nDivSizeZ * emDiv;

	m_nMaxX = m_nAxisX + m_nDivSizeX * emDiv/2;
	m_nMaxZ = m_nAxisZ + m_nDivSizeZ * emDiv/2;
	
	m_nMinX = m_nAxisX - m_nDivSizeX * emDiv/2;
	m_nMinZ = m_nAxisZ - m_nDivSizeZ * emDiv/2;

	m_nAxisX = ( m_nMaxX - m_nMinX ) / 2 + m_nMinX ;
	m_nAxisZ = ( m_nMaxZ - m_nMinZ ) / 2 + m_nMinZ;

	SAFE_DELETE_ARRAY(m_pNodeArray);
	m_pNodeArray = new SQUADNODE<TYPE>*[m_emDivision*m_emDivision];

	return S_OK;
}

template<class TYPE>
HRESULT GLQuadTree<TYPE>::MakeTree ()
{
	SAFE_DELETE(m_pRootNode);

	m_dwCellCount = 0;
	MakeTree ( m_nMaxX, m_nMaxZ, m_nMinX, m_nMinZ, (int)m_emDivision, &m_pRootNode );
	
	return S_OK;
}

template<class TYPE>
HRESULT GLQuadTree<TYPE>::MakeTree ( int nMaxX, int nMaxZ, int nMinX, int nMinZ,
							  int nDivNum, SQUADNODE<TYPE>** ppNode )
{
	HRESULT hr = S_OK;

	*ppNode = new SQUADNODE<TYPE>;
	(*ppNode)->nMaxX = nMaxX;
	(*ppNode)->nMaxZ = nMaxZ;
	(*ppNode)->nMinX = nMinX;
	(*ppNode)->nMinZ = nMinZ;

	if ( nDivNum == 1 )
	{
		(*ppNode)->dwFlag |= QUAD_LEAF;
		(*ppNode)->pData = new TYPE;
		(*ppNode)->dwCID = m_dwCellCount;

		m_pNodeArray[m_dwCellCount] = (*ppNode);
		m_dwCellCount++;

		return S_OK;
	}

	nDivNum /= 2;

	int nDivSX = (nMaxX-nMinX) / 2;
	int nDivSZ = (nMaxZ-nMinZ) / 2;

	int nMidX = nMinX + nDivSX;
	int nMidZ = nMinZ + nDivSZ;

	//	Note : 사각형 분할 표.
	//
	//		---------------------------------------------
	//		+				+				+
	//		(nMaxX,nMaxZ)	(nMidX,nMaxZ)	(nMinX,nMaxZ)
	//				[11]			[12]
	//
	//		+				+				+
	//		(nMaxX,nMidZ)	(nMidX,nMidZ)	(nMinX,nMidZ)
	//				[21]			[22]
	//
	//		+				+				+
	//		(nMaxX,nMinZ)	(nMidX,nMinZ)	(nMinX,nMinZ)
	//		---------------------------------------------
	//

	MakeTree ( nMaxX, nMaxZ, nMidX, nMidZ, nDivNum, &(*ppNode)->pNode11 );
	(*ppNode)->pNode11->pParent = (*ppNode);

	MakeTree ( nMidX, nMaxZ, nMinX, nMidZ, nDivNum, &(*ppNode)->pNode12 );
	(*ppNode)->pNode12->pParent = (*ppNode);

	MakeTree ( nMaxX, nMidZ, nMidX, nMinZ, nDivNum, &(*ppNode)->pNode21 );
	(*ppNode)->pNode21->pParent = (*ppNode);

	MakeTree ( nMidX, nMidZ, nMinX, nMinZ, nDivNum, &(*ppNode)->pNode22 );
	(*ppNode)->pNode22->pParent = (*ppNode);

	return S_OK;
}

template<class TYPE>
SQUADNODE<TYPE>* GLQuadTree<TYPE>::FindNode ( int nX, int nZ, SQUADNODE<TYPE>* pNode )
{
	if ( pNode->nMinX <= nX && nX < pNode->nMaxX &&
		pNode->nMinZ <= nZ && nZ < pNode->nMaxZ )
	{
		if ( pNode->dwFlag&QUAD_LEAF )	return pNode;

		SQUADNODE<TYPE>* pFinded = NULL;
		pFinded = FindNode ( nX, nZ, pNode->pNode11 );
		if ( pFinded ) return pFinded;

		pFinded = FindNode ( nX, nZ, pNode->pNode12 );
		if ( pFinded ) return pFinded;

		pFinded = FindNode ( nX, nZ, pNode->pNode21 );
		if ( pFinded ) return pFinded;

		pFinded = FindNode ( nX, nZ, pNode->pNode22 );
		if ( pFinded ) return pFinded;
	}

	return NULL;
}

template<class TYPE>
void GLQuadTree<TYPE>::FindNodes ( BOUDRECT &Rect, SQUADNODE<TYPE>* pNode, SQUADNODE<TYPE>** ppFindList )
{
	if ( pNode->nMaxX<Rect.nMinX && pNode->nMinX<Rect.nMinX )
		return;	// back 부분이 없으므로 충돌하지 않음.

	if ( pNode->nMaxX>Rect.nMaxX && pNode->nMinX>Rect.nMaxX )
		return;	// back 부분이 없으므로 충돌하지 않음.

	if ( pNode->nMaxZ<Rect.nMinZ && pNode->nMinZ<Rect.nMinZ )
		return;	// back 부분이 없으므로 충돌하지 않음.

	if ( pNode->nMaxZ>Rect.nMaxZ && pNode->nMinZ>Rect.nMaxZ )
		return;	// back 부분이 없으므로 충돌하지 않음.

	if ( pNode->dwFlag&QUAD_LEAF )
	{
		pNode->pFindNext = (*ppFindList);
		(*ppFindList) = pNode;

		return;
	}
	
	FindNodes ( Rect, pNode->pNode11, &(*ppFindList) );
	FindNodes ( Rect, pNode->pNode12, &(*ppFindList) );
	FindNodes ( Rect, pNode->pNode21, &(*ppFindList) );
	FindNodes ( Rect, pNode->pNode22, &(*ppFindList) );
}
