#include "pch.h"
#include "./GLLandMan.h"
#include "./GLItem.h"
#include "./GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL GLLandMan::LoadWldFile ( const char* szWldFile, bool bLandGateLoad )
{
	HRESULT hr;

	m_sLevelHead.m_strWldFile = szWldFile;

	char szPathName[MAX_PATH] = {0};
	StringCchCopy ( szPathName, MAX_PATH, DxLandMan::GetPath() );
	StringCchCat ( szPathName, MAX_PATH, m_sLevelHead.m_strWldFile.c_str() );

	char szFileType[FILETYPESIZE];
	DWORD dwVersion;

	BOOL bExist;
	CSerialFile SFile;
	if ( hr = SFile.OpenFile( FOT_READ, szPathName ) )
	{
		//	Note : 파일 형식 버전확인.
		//
		SFile.GetFileType( szFileType, FILETYPESIZE, dwVersion );

		if ( strcmp(DxLandMan::FILEMARK,szFileType) )
		{
			CString Str;
			Str.Format ( "[%s]  Wld file Head Error.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		if ( dwVersion > DxLandMan::VERSION )
		{
			CString Str;
			Str.Format ( "[%s] Wld file unknown version.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		SLAND_FILEMARK sLandMark;

		//	Note : .wld 파일의 맵ID는 사용하지 않음.
		SNATIVEID sMapID;
		SFile >> sMapID.dwID;

		//	Note : .wld 파일의 멥이름은 사용하지 않음.
		char szMapName[MAXLANDNAME];
		SFile.ReadBuffer ( szMapName, sizeof(char)*MAXLANDNAME );

		sLandMark.LoadSet ( SFile );

		//	Note : 내비개이션 메시 읽기.
		//
		SFile.SetOffSet ( sLandMark.dwNAVI_MARK );
		SFile >> bExist;
		if ( bExist )
		{
			m_pNaviMesh = new NavigationMesh;
			m_pNaviMesh->LoadFile ( SFile );
		}

		//	Note : 출입구 설정 읽기.
		//
		if ( sLandMark.dwGATE_MARK && bLandGateLoad )
		{
			SFile.SetOffSet ( sLandMark.dwGATE_MARK );
			m_cLandGateMan.Load ( SFile );
		}

		//	Note : 날씨 설정 읽기.
		//
		if ( sLandMark.dwWEATHER_MARK )
		{
			SFile.SetOffSet ( sLandMark.dwWEATHER_MARK );

			DxWeatherMan::LoadSetProp ( SFile, m_sWeatherProp );
		}
	}

	return TRUE;
}

//	Note : 파일 구조가 변경시에는 GLAGLandMan의 LoadFile() 함수의 파일 읽기 구조 또한 검토 하여야 함.
//
BOOL GLLandMan::LoadFile ( const char *szFile )
{
	GASSERT(szFile);
	HRESULT	hr;

	//	Note : Map 데이터 리셋.
	ReSetMap ();

	BOOL bOk(FALSE);
	bOk = GLLevelFile::LoadFile ( szFile, FALSE, m_pd3dDevice );
	if ( !bOk )			return FALSE;

	bOk = LoadWldFile ( m_sLevelHead.m_strWldFile.c_str(), GLLevelFile::GetFileVer()<=VERSION_BEFORE_GATE );
	if ( !bOk )
	{
		CString strTemp = m_sLevelHead.m_strWldFile.c_str();
		strTemp += " : GLLandMan::LoadWldFile() Load Faile.";
		MessageBox ( NULL, strTemp, "ERROR", MB_OK );
		
		return FALSE;
	}

	//	Note : Map 대이터 초기화.
	hr = SetMap ();
	if ( FAILED(hr) )	return FALSE;

	if ( m_pd3dDevice )
	{
		hr = GLLevelFile::InitDeviceObjects ( m_pd3dDevice );
		if ( FAILED(hr) )	return hr;

		hr = GLLevelFile::RestoreDeviceObjects ( m_pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	SMAPNODE* pMAP_NODE = GLGaeaServer::GetInstance().FindMapNode ( m_sMapID );
	if ( pMAP_NODE )
	{
		m_bPKZone	   = pMAP_NODE->bPKZone;
		m_bFreePK	   = pMAP_NODE->bFreePK;
		m_bItemDrop	   = pMAP_NODE->bItemDrop;
		m_bPetActivity = pMAP_NODE->bPetActivity;
		m_bDECEXP      = pMAP_NODE->bDECEXP;
		m_bVehicleActivity = pMAP_NODE->bVehicleActivity;
		m_bClubBattleZone = pMAP_NODE->bClubBattleZone;
	}

	return TRUE;
}

BOOL GLLandMan::LoadFileForInstantMap( GLLevelFile *pDestData, GLLevelFile *pSrcData )
{
	HRESULT	hr;
	BOOL bOk(FALSE);

	if( pSrcData == NULL ) return FALSE;



	*pDestData = *pSrcData;

	bOk = LoadWldFile ( m_sLevelHead.m_strWldFile.c_str(), GLLevelFile::GetFileVer()<=VERSION_BEFORE_GATE );
	if ( !bOk )
	{
		CString strTemp = m_sLevelHead.m_strWldFile.c_str();
		strTemp += " : GLLandMan::LoadWldFile() Load Faile.";
		MessageBox ( NULL, strTemp, "ERROR", MB_OK );

		return FALSE;
	}

	//	Note : Map 대이터 초기화.
	hr = SetMap ();
	if ( FAILED(hr) )	return FALSE;

	if ( m_pd3dDevice )
	{
		hr = GLLevelFile::InitDeviceObjects ( m_pd3dDevice );
		if ( FAILED(hr) )	return hr;

		hr = GLLevelFile::RestoreDeviceObjects ( m_pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	SMAPNODE* pMAP_NODE = GLGaeaServer::GetInstance().FindMapNode ( m_sMapID );
	if ( pMAP_NODE )
	{
		m_bPKZone	   = pMAP_NODE->bPKZone;
		m_bFreePK	   = pMAP_NODE->bFreePK;
		m_bItemDrop	   = pMAP_NODE->bItemDrop;
		m_bPetActivity = pMAP_NODE->bPetActivity;
		m_bDECEXP      = pMAP_NODE->bDECEXP;
		m_bVehicleActivity = pMAP_NODE->bVehicleActivity;
		m_bClubBattleZone = pMAP_NODE->bClubBattleZone;
	}

	m_fInstantMapDeleteTime = 0.0f;

	return TRUE;
}