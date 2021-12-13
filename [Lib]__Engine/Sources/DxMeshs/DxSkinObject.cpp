#include "pch.h"

#include "./SerialFile.h"
#include "DxSkinMeshMan.h"
#include "DxSkinAniMan.h"

#include "DxSkinObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char DxSkinObject::m_szPath[MAX_PATH] = "";

void DxSkinObject::SetPath ( char* szPath )
{
	StringCchCopy( m_szPath, MAX_PATH, szPath );
}

DxSkinObject::DxSkinObject() :
	m_pSkinMesh(NULL)
{
	memset( m_szSkinMesh, 0, sizeof(char)*MAX_PATH );
}

DxSkinObject::~DxSkinObject(void)
{
}

BOOL DxSkinObject::CreateSkinMesh ( const char* szFile, const char* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
	GASSERT(szFile);
	GASSERT(szSkeleton);
	GASSERT(pd3dDevice);

	if ( !DxSkinAniControl::LoadSkeleton ( szSkeleton, pd3dDevice ) )	return FALSE;

	StringCchCopy( m_szSkinMesh, MAX_PATH, szFile );
	m_pSkinMesh = DxSkinMeshMan::GetInstance().LoadSkinMesh ( m_szSkinMesh, m_strSkeleton.c_str(), pd3dDevice, bThread );
	if ( !m_pSkinMesh )		return FALSE;

	return TRUE;
}

BOOL DxSkinObject::LoadObject ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
	GASSERT(szFile);
	GASSERT(m_szPath[0]!=NULL);
	if ( !pd3dDevice )	return FALSE;

	//	Note : 앳 대이터 초기화.
	//
	DxSkinAniControl::ClearAll ();

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, m_szPath );
	StringCchCat( szPathName, MAX_PATH, szFile );

	CSerialFile SFile;
	if ( !SFile.OpenFile ( FOT_READ, szPathName ) )		return FALSE;

	SFile.ReadBuffer ( m_szSkinMesh, sizeof(char)*MAX_PATH );

	char szFILE[MAX_PATH] = "";
	SFile.ReadBuffer ( szFILE, sizeof(char)*MAX_PATH );
	m_strSkeleton = szFILE;

	m_pSkeleton = DxBoneCollector::GetInstance().Load ( m_strSkeleton.c_str(), pd3dDevice );
	m_pSkinMesh = DxSkinMeshMan::GetInstance().LoadSkinMesh ( m_szSkinMesh, m_strSkeleton.c_str(), pd3dDevice, bThread );
	if ( !m_pSkeleton || !m_pSkinMesh )	return FALSE;

	//	Note :에니메이션 읽기.
	//
	DWORD dwLenght;
	char szAniName[128] = "";

	SFile >> dwLenght;
	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile.ReadBuffer ( szAniName, sizeof(char)*128 );

		DxSkinAniControl::LoadAnimation ( szAniName, pd3dDevice );
	}

	DxSkinAniControl::ClassifyAnimation ();

	//	Note : 초기 에니메이션 설정.
	//
	DxSkinAniControl::SELECTANI ( AN_GUARD_N );

	return TRUE;
}

BOOL DxSkinObject::SaveObject ( const char* szFile )
{
	if ( !m_pSkeleton || !m_pSkinMesh )	return FALSE;

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, m_szPath );
	StringCchCat( szPathName, MAX_PATH, szFile );

	CSerialFile SFile;
	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		return FALSE;

	char szSkeleton[MAX_PATH] = "";
	StringCchCopy( szSkeleton, MAX_PATH, m_strSkeleton.c_str() );

	SFile.WriteBuffer ( m_szSkinMesh, sizeof(char)*MAX_PATH );
	SFile.WriteBuffer ( szSkeleton, sizeof(char)*MAX_PATH );

	DWORD dwLenght;

	dwLenght = 0;

	SFile << GETANIAMOUNT();
	
	for ( size_t n=0; n<m_vecAnim.size(); ++n )
	{
		SFile.WriteBuffer ( m_vecAnim[n]->pAnimCont->m_szName, sizeof(char)*128 );
	}

	return TRUE;
}

void DxSkinObject::ClearAll ()
{
	memset(m_szSkinMesh, 0, sizeof(char) * (MAX_PATH));	
	m_pSkinMesh = NULL;

	DxSkinAniControl::ClearAll ();
}

HRESULT DxSkinObject::FrameMove ( float fElapsedTime )
{
	DxSkinAniControl::FrameMove ( fElapsedTime );

	return S_OK;
}

HRESULT DxSkinObject::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot )
{
	if ( !m_pSkinMesh )	return S_FALSE;

	//	Note : Skin Animation.
	//
	DxSkinAniControl::Render ( matRot );

	//	Note : SkinMesh 그리기.
	//
	UINT cTriangles(0);
	
	//	TODO::
	//m_pSkinMesh->DrawMeshClobal ( pd3dDevice, cTriangles );

	return S_OK;
}
