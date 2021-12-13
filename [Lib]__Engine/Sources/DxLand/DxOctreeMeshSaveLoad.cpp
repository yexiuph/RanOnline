#include "pch.h"

#include "DxOctreeMesh.h"
#include "./SerialFile.h"
#include "./Collision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL	DxOctreeMesh::SaveFile ( CSerialFile &SFile )
{
	HRESULT hr(S_OK);

	SFile << m_dwFVF;

	//Note : VB
	OCTREEVERTEX	*ppbDataVB = NULL;
	SFile << m_dwNumVertices;	
	hr = m_pVB->Lock( 0, 0, (VOID**)&ppbDataVB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxOctreeMesh::SaveFile() -- m_pVB->Lock() -- Failed" );

	SFile.WriteBuffer ( ppbDataVB, sizeof ( OCTREEVERTEX ) * m_dwNumVertices );
	m_pVB->Unlock();

	//Note : IB
	WORD			*ppbDataIB = NULL;
	SFile << m_dwNumFaces;
	hr = m_pIB->Lock( 0, 0, (VOID**)&ppbDataIB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxOctreeMesh::SaveFile() -- m_pIB->Lock() -- Failed" );

	SFile.WriteBuffer ( ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
	m_pIB->Unlock();

	//Note : D3DXATTRIBUTERANGE
	SFile << m_dwAttribTableSize;
	SFile.WriteBuffer ( m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );

	if ( m_pAABBTreeRoot )
	{
		SFile << BOOL ( TRUE );
		m_pAABBTreeRoot->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	return TRUE;
}

BOOL	DxOctreeMesh::LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	BOOL	bExist = FALSE;
	BOOL	bDataOK = TRUE;

	SFile >> m_dwFVF;	
	
	OCTREEVERTEX	*ppbDataVB = NULL;
	SFile >> m_dwNumVertices;
	if( m_dwNumVertices > 0 )
	{
		ppbDataVB = new OCTREEVERTEX [ m_dwNumVertices ];
		SFile.ReadBuffer ( ppbDataVB, sizeof ( OCTREEVERTEX ) * m_dwNumVertices );	
	}
	else
	{
		bDataOK = FALSE;
		CDebugSet::ToLogFile( "DxOctreeMesh::LoadFile() -- Data Error -- VB" );
	}
	
	WORD* ppbDataIB = NULL;	
	SFile >> m_dwNumFaces;
	if( m_dwNumFaces > 0 )
	{
		ppbDataIB = new WORD [ m_dwNumFaces * 3 ];
		SFile.ReadBuffer ( ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
	}
	else
	{
		bDataOK = FALSE;
		CDebugSet::ToLogFile( "DxOctreeMesh::LoadFile() -- Data Error -- IB" );
	}

	// Note : D3DXATTRIBUTERANGE
	SFile >> m_dwAttribTableSize;
	if( m_dwAttribTableSize > 0 )
	{
		CreateAttribute ( m_dwAttribTableSize );
		SFile.ReadBuffer ( m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );
	}
	else
	{
		bDataOK = FALSE;
		CDebugSet::ToLogFile( "DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE" );
	}

	if( bDataOK )
	{
		// Note : 
		CreateDxOctreeMesh ( pd3dDevice, m_dwNumFaces, m_dwNumVertices, m_dwFVF );

		// Note : VB
		OCTREEVERTEX	*ppbDataLockVB = NULL;
		hr = m_pVB->Lock( 0, 0, (VOID**)&ppbDataLockVB, NULL );
		if( FAILED(hr) )	CDebugSet::ToLogFile( "DxOctreeMesh::LoadFile() -- m_pVB->Lock() -- Failed" );

		memcpy ( ppbDataLockVB, ppbDataVB, sizeof ( OCTREEVERTEX ) * m_dwNumVertices );			
		m_pVB->Unlock();

		// Note : IB
		WORD			*ppbDataLockIB = NULL;
		hr = m_pIB->Lock( 0, 0, (VOID**)&ppbDataLockIB, NULL );
		if( FAILED(hr) )	CDebugSet::ToLogFile( "DxOctreeMesh::LoadFile() -- m_pIB->Lock() -- Failed" );

		memcpy ( ppbDataLockIB, ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
		m_pIB->Unlock();
	}
	else
	{
		SAFE_DELETE_ARRAY( m_pAttribTable );
	}

	SAFE_DELETE_ARRAY ( ppbDataVB );
	SAFE_DELETE_ARRAY ( ppbDataIB );

	SFile >> bExist;
	if ( bExist )
	{
		m_pAABBTreeRoot = new DxAABBNode;
		m_pAABBTreeRoot->LoadFile ( SFile );
	}

	return TRUE;
}
