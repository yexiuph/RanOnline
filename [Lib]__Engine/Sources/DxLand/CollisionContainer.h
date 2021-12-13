#ifndef	__C_COLLISION_CONTAINER__
#define	__C_COLLISION_CONTAINER__

#include "./CList.h"

struct	DxFrame;

struct	SCollisionCell
{
	DWORD		dwID;
	DxFrame*	pFrame;
	D3DXVECTOR3	vCollision;

	SCollisionCell*	pNext;
};

class	CCollisionContainer
{
public:
    CCollisionContainer ();
	~CCollisionContainer();

public:
	BOOL	CleanUp ();

	BOOL	AddCell ( SCollisionCell& Cell );
	SCollisionCell*	const GetHead ();

public:
	void	SetCollisionVertex ( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1 );
	void	GetCollisionVertex ( D3DXVECTOR3& v0, D3DXVECTOR3& v1 );
    
	D3DXVECTOR3	GetVertexV0 ();
	D3DXVECTOR3 GetVertexV1 ();

	void	SetVertexV0 ( D3DXVECTOR3 v0 );
	void	SetVertexV1 ( D3DXVECTOR3 v1 );

    SCollisionCell*	GetMinDistFromV0 ( const DxFrame* pFrame = NULL );

	int		GetCount ();

private:
	int		m_NodeCount;

private:
	SCollisionCell*	m_pHead;

private:
	D3DXVECTOR3	m_V0;
	D3DXVECTOR3	m_V1;
};

#endif	//	__C_COLLISION_CONTAINER__