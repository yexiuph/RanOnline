#include "pch.h"
#include "./StlFunctions.h"

#include "./StringUtils.h"
#include "./StringFile.h"

#include "./RENDERPARAM.h"
#include "DxSkinMesh9_CPU.h"
#include "DxSkinMesh9_NORMAL.h"
#include "DxSkinMesh9_HLSL.h"

#include "DxSkinMeshMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DxSkinMeshMan& DxSkinMeshMan::GetInstance()
{
	static DxSkinMeshMan Instance;
	return Instance;
}

DxSkinMeshMan::DxSkinMeshMan(void)
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxSkinMeshMan::~DxSkinMeshMan(void)
{
}

DxSkinMesh9* DxSkinMeshMan::FindSkinMesh ( const char* szSkinMesh )
{
	SKINMESHMAP_ITER iter = m_SkinMeshMap.find ( std::string(szSkinMesh) );
	if ( iter!=m_SkinMeshMap.end() )	return (*iter).second;

	return NULL;
}

DxSkinMesh9* DxSkinMeshMan::LoadSkinMesh( const char* _szSkinMesh, const char* _szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
	HRESULT hr;
	DxSkinMesh9* pSkinMesh;

	GASSERT(_szSkinMesh);
	GASSERT(_szSkeleton);

	CString strFileName(_szSkinMesh);
	strFileName.MakeLower();

	CString strSkeleton(_szSkeleton);
	strSkeleton.MakeLower();

	pSkinMesh = FindSkinMesh ( strFileName.GetString() );
	if ( pSkinMesh )
	{
		pSkinMesh->AddRef();
		return pSkinMesh;
	}

	// Note : 모드를 확인 후 작업한다.
	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
		pSkinMesh = new DxSkinMesh9_CPU;
		break;
	case EMCRT_NORMAL:
		pSkinMesh = new DxSkinMesh9_NORMAL;
		break;
	case EMCRT_VERTEX:
	case EMCRT_PIXEL:
		pSkinMesh = new DxSkinMesh9_HLSL;
		break;
	};

	//	TODO::
	hr = pSkinMesh->OnCreateSkin( pd3dDevice, strFileName.GetString(), strSkeleton.GetString(), bThread );
	if ( FAILED(hr) )
	{
		delete pSkinMesh;
		return NULL;
	}

	//	Note : 리스트에 삽입.
	//
	m_SkinMeshMap.insert ( std::make_pair(std::string(strFileName.GetString()),pSkinMesh) );

	return pSkinMesh;
}

BOOL DxSkinMeshMan::PreLoad ( const char* szSkinList, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
	CString strPath;
	strPath = GetPath();
	strPath += szSkinList;	

	CStringFile StrFile;
	if ( !StrFile.Open ( strPath.GetString() ) )	return FALSE;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "\t" );
	STRUTIL::RegisterSeparator ( " " );
	STRUTIL::RegisterSeparator ( "," );

	CString strLine;
	while ( StrFile.GetNextLine(strLine) )
	{
		if ( strLine.GetLength() == 0 )		continue;

		CStringArray *pStrArray = new CStringArray;
		STRUTIL::StringSeparate ( strLine, *pStrArray );

		if ( pStrArray->GetSize() != 2 )
		{
			SAFE_DELETE(pStrArray);
			continue;
		}

		CString strSkin = pStrArray->GetAt(0);
		CString strBone = pStrArray->GetAt(1);

		strSkin.MakeLower();
		strBone.MakeLower();

		DxSkinMesh9* pSkinMesh = LoadSkinMesh( strSkin.GetString(), strBone.GetString(), pd3dDevice, FALSE );
		if ( !pSkinMesh )
		{
			CString strMessage;
			strMessage.Format (_T("DxSkinMeshMan::PreLoad() [%s] LoadSkinMesh Failed."), strSkin);
			MessageBox (NULL, strMessage, _T("ERROR"), MB_OK );

			SAFE_DELETE(pStrArray);
			continue;
		}

		m_PreLoadSkinMeshMap.insert ( std::make_pair(std::string(strSkin.GetString()),pSkinMesh) );

		SAFE_DELETE(pStrArray);
	};

	return TRUE;
}

HRESULT DxSkinMeshMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxSkinMeshMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxSkinMeshMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{

	return S_OK;
}

HRESULT DxSkinMeshMan::DeleteDeviceObjects()
{
	SKINMESHMAP_ITER iter = m_SkinMeshMap.begin();
	SKINMESHMAP_ITER iter_end = m_SkinMeshMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->OnSkinDestroyDevice ();
		(*iter).second->OnSkinCleanUp ();
	}

	std::for_each ( m_SkinMeshMap.begin(), m_SkinMeshMap.end(), std_afunc::DeleteMapObject() );
	m_SkinMeshMap.clear();

	return S_OK;
}

HRESULT DxSkinMeshMan::CleanUp ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	InvalidateDeviceObjects ( pd3dDevice );
	DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxSkinMeshMan::DoInterimClean ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SKINMESHLIST listInterimClean;
	
	//	Note : 제거할 리스트 작성.
	//
	{
		SKINMESHMAP_ITER iter = m_SkinMeshMap.begin();
		SKINMESHMAP_ITER iter_end = m_SkinMeshMap.end();
		for ( ; iter!=iter_end; ++iter )
		{
			//	Note : 사전 로딩 리스트에 있는 파일 제외.
			//
			SKINMESHMAP_ITER found = m_PreLoadSkinMeshMap.find ( (*iter).first );
			if ( found!=m_PreLoadSkinMeshMap.end() )	continue;

			//	제거 리스트에 등록.
			listInterimClean.push_back ( (*iter).second );
		}
	}

	//	Note : 데이터 정리.
	//
	{
		SKINMESHLIST_ITER iter = listInterimClean.begin();
		SKINMESHLIST_ITER iter_end = listInterimClean.end();
		for ( ; iter!=iter_end; ++iter )
		{
			//DxSkinMesh9* pSkinMesh = (*iter);
			std::string strname = (*iter)->GetSkinFileName();

			//	데이터 삭제.
			(*iter)->OnSkinDestroyDevice ();
			(*iter)->OnSkinCleanUp ();
			//SAFE_DELETE(pSkinMesh);

			//	로드 파일 리스트에서 제거.
			SKINMESHMAP_ITER iter_del = m_SkinMeshMap.find ( strname );
			if ( iter_del!=m_SkinMeshMap.end() )	m_SkinMeshMap.erase ( iter_del );
		}
	}

	return S_OK;
}
