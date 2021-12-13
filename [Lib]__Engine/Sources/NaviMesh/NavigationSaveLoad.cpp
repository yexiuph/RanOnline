#include "pch.h"

#include "NavigationCell.h"
#include "NavigationMesh.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void	NavigationMesh::SaveFile ( CSerialFile &SFile )
{	
	SFile << m_nNaviVertex;
	if ( m_nNaviVertex )
	{
		SFile.WriteBuffer ( m_pNaviVertex, sizeof ( D3DXVECTOR3	) * m_nNaviVertex );
	}

    int	DACellCount = 0;
	DACellCount  = TotalCells();
	SFile << DACellCount ;
	for ( int i = 0; i < DACellCount ; i++ )
	{
		NavigationCell* pCell = m_pDACell[i];
		pCell->SaveFile ( SFile );
	}	

	for ( int i = 0; i < DACellCount ; i++ )
	{
		NavigationCell* pCell = m_pDACell[i];
		//	<--	Link를 저장하기 위함
		for ( int i = 0; i < 3; i++ )
		{
			NavigationCell* LinkCell = pCell->Link(i);
			if ( LinkCell )
			{
				SFile << BOOL ( TRUE );

				DWORD	LinkID = LinkCell->CellID();
				SFile << LinkID;
			}
			else
			{
				SFile << BOOL ( FALSE );
			}
		}
		//	-->	Link를 저장하기 위함
	}
}

void	NavigationMesh::LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SFile >> m_nNaviVertex;
	if ( m_nNaviVertex )
	{
        m_pNaviVertex = new D3DXVECTOR3 [ m_nNaviVertex ];
		SFile.ReadBuffer ( m_pNaviVertex, sizeof ( D3DXVECTOR3	) * m_nNaviVertex );
	}
	
	DWORD	dwDACellCount = 0;
	SFile >> dwDACellCount;
	if ( dwDACellCount )
	{
		m_pDACell = new NavigationCell* [ dwDACellCount ];
	}
	
	for ( DWORD i = 0; i < dwDACellCount; i++ )
	{
		NavigationCell* NewCell = new NavigationCell;		
		NewCell->LoadFile ( SFile );

		m_pDACell[i] = NewCell;
		m_CellArray.push_back(NewCell);
	}	

	BOOL	bExist = FALSE;
	for ( DWORD i = 0; i < dwDACellCount; i++ )
	{
		NavigationCell* pCell = m_pDACell[i];
		//	<--	Link를 저장하기 위함
		for ( int i = 0; i < 3; i++ )
		{
			SFile >> bExist;			
			if ( bExist )
			{
				DWORD	LinkID;
				SFile >> LinkID;
				pCell->SetLink ( NavigationCell::CELL_SIDE(i), GetCell ( LinkID ) );
			}
			else
			{
				pCell->SetLink ( NavigationCell::CELL_SIDE(i), NULL );
			}
		}
		//	-->	Link를 저장하기 위함
	}

	MakeAABBTree( );

//	<--	Export된 데이타 로드했을때, 네비게이션 메쉬 볼 수 있도록
//		해주는 코드임
//	-->
	if ( pd3dDevice )
	{
		//CreateVBIB ( pd3dDevice );
	}
}

void	NavigationCell::SaveFile ( CSerialFile &SFile )
{
    SFile << m_CellID;
	SFile.WriteBuffer ( m_Vertex, sizeof ( DWORD ) * 3 );
	SFile.WriteBuffer ( m_Side, sizeof ( Line2D ) * 3 );
	SFile.WriteBuffer ( &m_CellPlane, sizeof ( Plane ) );
	SFile.WriteBuffer ( &m_CenterPoint, sizeof ( D3DXVECTOR3 ) );
    SFile.WriteBuffer ( m_WallMidpoint, sizeof ( D3DXVECTOR3 ) * 3 );
	SFile.WriteBuffer ( m_WallDistance, sizeof ( float ) * 3 );	
//	SFile.WriteBuffer ( &m_Normal, sizeof ( D3DXVECTOR3 ) );
}

void	NavigationCell::LoadFile ( CSerialFile &SFile )
{
    SFile >> m_CellID;
	SFile.ReadBuffer ( m_Vertex, sizeof ( DWORD ) * 3 );
	SFile.ReadBuffer ( m_Side, sizeof ( Line2D ) * 3 );
	SFile.ReadBuffer ( &m_CellPlane, sizeof ( Plane ) );
	SFile.ReadBuffer ( &m_CenterPoint, sizeof ( D3DXVECTOR3 ) );
    SFile.ReadBuffer ( m_WallMidpoint, sizeof ( D3DXVECTOR3 ) * 3 );
	SFile.ReadBuffer ( m_WallDistance, sizeof ( float ) * 3 );	
//	SFile.ReadBuffer ( &m_Normal, sizeof ( D3DXVECTOR3 ) );
}