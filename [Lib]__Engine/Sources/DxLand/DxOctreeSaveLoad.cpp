#include "pch.h"

#include "DxOctree.h"
#include "./DxFrameMesh.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL	DxOctree::SaveFile ( CSerialFile &SFile )
{
	SFile << m_bSubDivided;
	SFile.WriteBuffer ( &m_vMax, sizeof ( D3DXVECTOR3 ) );
	SFile.WriteBuffer ( &m_vMin, sizeof ( D3DXVECTOR3 ) );

	m_DataAddress = SFile.GetfTell () + sizeof ( DWORD ) + sizeof ( DWORD );
	SFile.WriteBuffer ( &m_DataAddress, sizeof ( DWORD ) );
	SFile.WriteBuffer ( &m_DataSize, sizeof ( DWORD ) );

	//	Note : DxFrame-Tree 저장.
	//	
	//
	if ( m_pDxFrameHead )
	{
		SFile << BOOL ( TRUE );
		m_pDxFrameHead->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	long OldPos = SFile.GetfTell ();	//	현재의 포지션
	long NewPos = m_DataAddress - sizeof ( DWORD );	//	사이즈를 기록할 포지션

	m_DataSize = SFile.GetfTell () - m_DataAddress;
	SFile.SetOffSet ( NewPos );
	SFile.WriteBuffer ( &m_DataSize, sizeof ( DWORD ) );
	SFile.SetOffSet ( OldPos );
	
	//	Note : 자식-Octree m_pOctreeNodes 들을 저장.
	//	
	for ( int i = 0; i < 8; i++ )
	{
		if ( m_pOctreeNodes[i] )
		{
			SFile << BOOL ( TRUE );
			m_pOctreeNodes[i]->SaveFile ( SFile );
		}
		else
		{
			SFile << BOOL ( FALSE );			
		}
	}

	return TRUE;
}

BOOL	DxOctree::LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, BOOL bUseDynamicLoad )
{
	BOOL	bExist = FALSE;

	SFile >> m_bSubDivided;
	SFile.ReadBuffer ( &m_vMax, sizeof ( D3DXVECTOR3 ) );
	SFile.ReadBuffer ( &m_vMin, sizeof ( D3DXVECTOR3 ) );	
	SFile.ReadBuffer ( &m_DataAddress, sizeof ( DWORD ) );
	SFile.ReadBuffer ( &m_DataSize, sizeof ( DWORD ) );	

	//	에디터용이면 Dynamic Load를 쓰지 않고, 바로 로드한다.
	if ( bUseDynamicLoad )
	{
		long	CurPos = SFile.GetfTell ();
		SFile.SetOffSet ( CurPos + m_DataSize );
	}
	else
	{
		SFile >> bExist;
		if ( bExist )
		{
			m_pDxFrameHead = new DxFrame;
			m_pDxFrameHead->LoadFile ( SFile, pd3dDevice, pEffGlobalHead, m_pDxAlphaMapFrameHead );
		}
		m_bLoaded = TRUE;
	}
	
	//	자식 : m_pOctreeNodes
	//	
	for ( int i = 0; i < 8; i++ )
	{
		SFile >> bExist;
		if ( bExist )
		{		
			m_pOctreeNodes[i] = new DxOctree;
			m_pOctreeNodes[i]->LoadFile ( SFile, pd3dDevice, pEffGlobalHead, bUseDynamicLoad );
		}		
	}	

	if ( !m_bSubDivided )
	{
		OctreeDebugInfo::EndNodeCount++;
	}
	
	return TRUE;
}
	
BOOL DxOctree::DynamicLoad ( CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead )
{	
	if ( !SFile.IsOpen () )
	{
		return FALSE;
	}

	SFile.SetOffSet ( m_DataAddress );

	BOOL bExist = FALSE;
	SFile >> bExist;
	if ( bExist )
	{
		m_pDxFrameHead = new DxFrame;
		m_pDxFrameHead->LoadFile ( SFile, pd3dDevice, pEffGlobalHead, m_pDxAlphaMapFrameHead );
	}

	m_bLoaded = TRUE;

	return TRUE;
}

BOOL DxOctree::LoadBasicPos ( CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
{
	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if ( COLLISION::IsCollisionAABBToAABB(vMax,vMin,m_vMax,m_vMin) )
	{
		if ( !m_bLoaded )
		{
			BOOL bOk = DynamicLoad ( SFile, pd3dDevice, pEffGlobalHead );
			if ( !bOk )				return FALSE;
		}

		if ( !m_bSubDivided )		return TRUE;

		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->LoadBasicPos ( SFile, pd3dDevice, pEffGlobalHead, vMax, vMin );
	}

	return TRUE;
}