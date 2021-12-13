#include "pch.h"
#include "CollisionMap.h"
#include "CollisionMapCell.h"
#include "collision.h"

#include "DxLandMan.h"
#include "./DxFrameMesh.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCollisionMap::CCollisionMap () :
	m_pMapCells ( NULL ),
	m_pVertices ( NULL ),
	m_pAABBTreeRoot ( NULL ),
	m_nMapCells ( 0 ),
	m_nVertices ( 0 ),
	m_ppDxFrame ( NULL ),
	m_pFrameMesh ( NULL ),	
	m_pCollisionCell ( NULL )
{
}

CCollisionMap::~CCollisionMap()
{
	CleanUp();
}

void CCollisionMap::SetDxFrameMesh ( DxFrameMesh* pFrameMesh )
{
	if ( !pFrameMesh )
	{
		_ASSERT ( pFrameMesh &&
			"���� ����Ÿ FrameMesh�� �����Ͱ� �߸� ���� �Ǿ����ϴ�." );
		return ;
	}

	m_pFrameMesh = pFrameMesh;
}

BOOL CCollisionMap::DeleteCellArray ()
{
	//for ( DWORD i = 0; i < m_nMapCells; i++ )
	//{
	//	SAFE_DELETE ( m_pMapCells[i] );
	//}
	m_nMapCells = 0;
	SAFE_DELETE_ARRAY ( m_pMapCells );

	return TRUE;
}

BOOL CCollisionMap::DeleteDxFrameArray ()
{
	SAFE_DELETE_ARRAY ( m_ppDxFrame );
	return TRUE;
}

BOOL CCollisionMap::DeleteVertexArray ()
{
	m_nVertices = 0;
	SAFE_DELETE_ARRAY ( m_pVertices );
	return TRUE;
}

BOOL CCollisionMap::CreateCellArray ( DWORD nMapCells )
{
	if ( m_nMapCells || m_pMapCells )
	{
		_ASSERT ( 0 && "�ﰢ�� ����(�� ���, �ε��� ����)�ȿ� �̹� ����Ÿ�� �����մϴ�." );
		return FALSE;
	}

	m_nMapCells = nMapCells;
	m_pMapCells = new CCollisionMapCell [ m_nMapCells ];

	return TRUE;
}

BOOL CCollisionMap::AddCell ( DWORD VertA, DWORD VertB, DWORD VertC, DWORD CellID )
{
	if ( !m_nMapCells || !m_pMapCells )
	{
		_ASSERT ( 0 && "�ﰢ�� ����(�� ���, �ε��� ����)�� ���� �������� �ʾҽ��ϴ�." );
		return FALSE;
	}

	if ( m_nMapCells <= CellID )
	{
		_ASSERT ( 0 && "�ε��� ������ �Ѿ����ϴ�." );
		return FALSE;
	}

//	m_pMapCells[CellID] = new CCollisionMapCell;
	m_pMapCells[CellID].SetCellID  ( CellID );
	m_pMapCells[CellID].SetIndices ( VertA, VertB, VertC );

	return TRUE;
}

CCollisionMapCell*	CCollisionMap::GetCell( DWORD CellID )
{
	if ( !m_nMapCells || !m_pMapCells )
	{
		_ASSERT ( 0 && "�ﰢ�� ����(�� ���, �ε��� ����)�� ���� �������� �ʾҽ��ϴ�." );
		return FALSE;
	}

	if ( m_nMapCells <= CellID )
	{
		_ASSERT ( 0 && "�ε��� ������ �Ѿ����ϴ�." );
		return FALSE;
	}

	return &m_pMapCells[CellID];
}

BOOL CCollisionMap::CreateDxFrameArray ()
{
	if ( m_ppDxFrame )
	{
		_ASSERT ( 0 && "���ؽ� ���۾ȿ� �̹� ����Ÿ�� �����մϴ�." );
		return FALSE;
	}

	m_ppDxFrame = new DxFrame* [ m_nMapCells ];
	memset ( m_ppDxFrame, 0, sizeof ( DxFrame* ) * m_nMapCells );

	return TRUE;
}

BOOL CCollisionMap::SetDxFrameArray ( DxFrame** ppDxFrame )
{
	if ( !ppDxFrame )
	{
		_ASSERT ( 0 && "������ ����Ÿ�� �����ϴ�." );
		return FALSE;
	}

	memcpy ( m_ppDxFrame, ppDxFrame, sizeof ( DxFrame* ) * m_nMapCells );
	return TRUE;
}

BOOL CCollisionMap::GetDxFrameArray ( DxFrame** ppDxFrame )
{
	if ( !m_ppDxFrame )
	{
		_ASSERT ( 0 && "���ؽ� ���۾ȿ� ����Ÿ�� �������� �ʽ��ϴ�." );
		return FALSE;
	}

	memcpy ( ppDxFrame, m_ppDxFrame, sizeof ( DxFrame* ) * m_nMapCells );
	return TRUE;
}

BOOL CCollisionMap::CreateVertexArray ( DWORD nVertices )
{
	if ( m_nVertices || m_pVertices )
	{
		_ASSERT ( 0 && "���ؽ� ���۾ȿ� �̹� ����Ÿ�� �����մϴ�." );
		return FALSE;
	}

	m_nVertices = nVertices;
	m_pVertices = new D3DXVECTOR3 [ m_nVertices ];
	memset ( m_pVertices, 0, sizeof ( D3DXVECTOR3 ) * m_nVertices );

	return TRUE;
}

BOOL CCollisionMap::SetVertices ( D3DXVECTOR3* pVertices )
{
	if ( !pVertices )
	{
		_ASSERT ( 0 && "������ ����Ÿ�� �����ϴ�." );
		return FALSE;
	}

	memcpy ( m_pVertices, pVertices, sizeof ( D3DXVECTOR3 ) * m_nVertices );
	return TRUE;
}

BOOL CCollisionMap::GetVertices ( D3DXVECTOR3* pVertices )
{
	if ( !m_nVertices || !m_pVertices )
	{
		_ASSERT ( 0 && "���ؽ� ���۾ȿ� ����Ÿ�� �������� �ʽ��ϴ�." );
		return FALSE;
	}

	memcpy ( pVertices, m_pVertices, sizeof ( D3DXVECTOR3 ) * m_nVertices );
	return TRUE;
}

BOOL CCollisionMap::SetDataToMap  ( DWORD nIndices, DWORD* pIndices, DWORD nVertices, D3DXVECTOR3* pVertices,
								   DxFrame** pFrame, D3DXVECTOR3 vCenter, D3DXVECTOR3 vWidth )
{
	//	Note : �ε��� ����, ���� ��ȯ/�����ϱ�
	//
	const DWORD nTriangle = nIndices / 3;
	if ( !CreateCellArray ( nTriangle ) )
	{
		_ASSERT ( 0 && "�ε��� �� ������ ����ϱ� ���� ���۸� ��� �� ������ �߻��Ͽ����ϴ�." );
		return FALSE;
	}
	for ( DWORD i = 0; i < nTriangle; i++ )
	{			
		if ( !AddCell ( pIndices[(i * 3) + 0], pIndices[(i * 3) + 1], pIndices[(i * 3) + 2], i ) )
		{
			_ASSERT ( 0 && "���� �߰��ϴ��� ������ �߻��Ͽ����ϴ�." );
			return FALSE;
		}		
	}

	//	Note : ���ؽ� ����, ����
	if ( !CreateVertexArray ( nVertices ) )
	{
		_ASSERT ( 0 && "���ؽ� ������ ����ϱ� ���� ���۸� ��� �� ������ �߻��Ͽ����ϴ�." );
		return FALSE;
	}
    if ( !SetVertices ( pVertices ) )
	{
		_ASSERT ( 0 && "���ؽ� ������ ����ϴ��� ������ �߻��Ͽ����ϴ�." );
		return FALSE;
	}

	if ( !CreateDxFrameArray () )
	{
		_ASSERT ( 0 && "DxFrame ������ �迭������ �����Ͽ����ϴ�." );
		return FALSE;
	}
	if ( !SetDxFrameArray ( pFrame ) )
	{
		_ASSERT ( 0 && "DxFrame ������ ������ �����Ͽ����ϴ�." );
		return FALSE;
	}

	m_vTreeCenter = vCenter;
	m_Width = vWidth;

	return TRUE;
}

BOOL CCollisionMap::DelDataToMap  ()
{	
	DeleteCellArray ();
	DeleteVertexArray ();
	SAFE_DELETE_ARRAY ( m_ppDxFrame );

	return TRUE;
}

DWORD CCollisionMap::GetCellCount ()
{
	return m_nMapCells;
}

DWORD CCollisionMap::GetVertexCount()
{
	return m_nVertices;
}

D3DXVECTOR3	CCollisionMap::GetTreeCenter ()
{
	return m_vTreeCenter;
}

void	CCollisionMap::GetTreeWidth ( D3DXVECTOR3* pvWidth )
{
	if ( !pvWidth )
	{
		_ASSERT ( 0 && "NULL�� ���ɴϴ�." );
		return;
	}
	memcpy ( pvWidth, &m_Width, sizeof ( D3DXVECTOR3 ) * 3 );
}

float	CCollisionMap::GetTreeXWidth ()
{
	return m_XWidth;
}

float	CCollisionMap::GetTreeYWidth ()
{
	return m_YWidth;
}

float	CCollisionMap::GetTreeZWidth ()
{
	return m_ZWidth;
}

BOOL CCollisionMap::GetVertex ( D3DXVECTOR3* pvPoint, DWORD ID )
{
	if ( m_nVertices <= ID )
	{
		_ASSERT ( 0 && "�ε��� ������ ����ϴ�." );
		return FALSE;			
	}

	*pvPoint = m_pVertices[ID];
	return TRUE;
}

BOOL CCollisionMap::GetCellVertex ( D3DXVECTOR3* pvPoint, DWORD ID )
{
	if ( m_nMapCells <= ID )
	{
		_ASSERT ( 0 && "�ε��� ������ ����ϴ�." );
		return FALSE;			
	}

	CCollisionMapCell* const pCell = &m_pMapCells[ID];

	pvPoint[0] = m_pVertices[pCell->GetIndex(0)];
	pvPoint[1] = m_pVertices[pCell->GetIndex(1)];
	pvPoint[2] = m_pVertices[pCell->GetIndex(2)];

	return TRUE;
}

DWORD	CCollisionMap::GetUsedMemory ()
{
	return
		sizeof ( CCollisionMapCell ) * m_nMapCells +
		sizeof ( CCollisionMapCell*) * m_nMapCells + 
		sizeof ( DxFrame* ) * m_nMapCells +
		sizeof ( D3DXVECTOR3 ) * m_nVertices + 
		sizeof ( DxAABBNode ) * m_nMapCells;
}

BOOL CCollisionMap::Import( const BOOL bEdit )
{
	if ( !m_pFrameMesh->GetFrameRoot() ) return FALSE;

	DWORD			dwTriangles		=	m_pFrameMesh->GetTriangles();
	DWORD			dwVertices		=	m_pFrameMesh->GetVertices();	

	DWORD			*pIndices = new DWORD [ dwTriangles * 3 ];
	DWORD			dwIndexCount = 0;
	memset ( pIndices, 0, dwTriangles * 3 * sizeof ( DWORD ) );

	OCTREEVERTEX	*pVertices = new OCTREEVERTEX [ dwVertices ];
	DWORD			dwVertexCount = 0;
	memset ( pVertices, 0, dwVertices * sizeof ( OCTREEVERTEX ) );	
	
	DWORD nDxFrame = 0;
	DxFrame** ppDxFrame = new DxFrame *[ dwTriangles ];

	///////////////////////////////////////////////////////////////////////////////////////
	//	Note : ����Ÿ �ܾ����.
	//
	m_pFrameMesh->GetFrameRoot()->GetAllNodeData( pIndices, &dwIndexCount,
		pVertices, &dwVertexCount, ppDxFrame, &nDxFrame, m_pFrameMesh->GetFrameRoot(), true, bEdit );
	///////////////////////////////////////////////////////////////////////////////////////

	//	�����Ȳ ǥ��
	//
	ExportProgress::EndPos = dwIndexCount / 3;

	//	OCTREEVERTEX -> D3DXVECTOR3 ( Vec�� ����� )
	D3DXVECTOR3* pVecVertices = new D3DXVECTOR3 [ dwVertices ];
	for ( DWORD i = 0; i < dwVertices; i++ )
	{
		pVecVertices[i] = pVertices[i].vPos;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//	Note : ���� �浹�ʿ� �ʿ��� ����Ÿ ����
	//
	if ( !SetDataToMap ( dwIndexCount, pIndices, dwVertices, pVecVertices,
			ppDxFrame, m_pFrameMesh->GetFrameRoot()->GetTreeCenter(),
			D3DXVECTOR3(
				m_pFrameMesh->GetFrameRoot()->GetTreeXWidth(),
				m_pFrameMesh->GetFrameRoot()->GetTreeYWidth(),
				m_pFrameMesh->GetFrameRoot()->GetTreeZWidth() ) )
		)
	{		
		SAFE_DELETE_ARRAY ( ppDxFrame );
		SAFE_DELETE_ARRAY ( pVecVertices );
		SAFE_DELETE_ARRAY ( pIndices );
		SAFE_DELETE_ARRAY ( pVertices );	

		return FALSE;
	}
	////////////////////////////////////////////////////////////////////////////////////////

    SAFE_DELETE_ARRAY ( ppDxFrame );    
	SAFE_DELETE_ARRAY ( pVecVertices );
	SAFE_DELETE_ARRAY ( pIndices );
	SAFE_DELETE_ARRAY ( pVertices );	
	
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//	Note : ���� ������� �ʴ� �κ�, �۾��ؾ���
	if ( FAILED ( MakeAABBTree () ) )
	{
		_ASSERT(0);
		return FALSE;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	return TRUE;
}

DxFrame* const CCollisionMap::GetDxFrame ( DWORD CellID )
{
    return m_ppDxFrame[CellID];
}

HRESULT CCollisionMap::LoseAABBTree ()
{
	SAFE_DELETE ( m_pAABBTreeRoot );
	return S_OK;
}

BOOL CCollisionMap::CleanUp ()
{	
	DelDataToMap ();
	LoseAABBTree ();

	m_CollisionContainer.CleanUp ();
	m_pCollisionCell = NULL;

	return TRUE;
}

unsigned int WINAPI CCollisionMap::ImportThread(
	LPVOID pParam )
{
	CCollisionMap* const pCollisionMap = (CCollisionMap*)pParam;

	if ( pCollisionMap )
	{
		pCollisionMap->CleanUp();
		pCollisionMap->Import( FALSE );
	}

	return 0;
}

BOOL CCollisionMap::SaveFile ( CSerialFile& SFile )
{
	SFile.WriteBuffer ( &m_nMapCells, sizeof ( DWORD ) );
	for ( DWORD i = 0; i < m_nMapCells; i++ )
	{
		m_pMapCells[i].SaveFile ( SFile );
	}

	if( m_nVertices )
	{
		SFile.WriteBuffer ( &m_nVertices, sizeof ( DWORD ) );
		SFile.WriteBuffer ( m_pVertices, sizeof ( D3DXVECTOR3 ) * m_nVertices );
	}

	if ( m_pAABBTreeRoot ) m_pAABBTreeRoot->SaveFile ( SFile );

	return TRUE;
}

BOOL CCollisionMap::LoadFile ( CSerialFile& SFile )
{	
	DWORD nMapCells = 0;
	SFile.ReadBuffer ( &nMapCells, sizeof ( DWORD ) );

	CreateCellArray ( nMapCells );	
	for ( DWORD i = 0; i < nMapCells; i++ )
	{
//		m_pMapCells[i] = new CCollisionMapCell;
		m_pMapCells[i].LoadFile ( SFile );
	}

	DWORD nVertices = 0;
	SFile.ReadBuffer ( &nVertices, sizeof ( DWORD ) );
	CreateVertexArray ( nVertices );
	SFile.ReadBuffer ( m_pVertices, sizeof ( D3DXVECTOR3 ) * nVertices );

	m_pAABBTreeRoot = new DxAABBNode;
	m_pAABBTreeRoot->LoadFile ( SFile );

	return TRUE;
}
