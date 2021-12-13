#include "pch.h"
#include "./GLAGLandMan.h"
#include "./GLAgentServer.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLAGLandMan::GLAGLandMan(void) :
	m_dwClubMapID(UINT_MAX),
	m_bGuidBattleMap(false),
	m_bClubDeathMatchMap(false)
{
}

GLAGLandMan::~GLAGLandMan(void)
{
	CleanUp ();
}

BOOL GLAGLandMan::LoadWldFile ( const char* szWldFile, bool bLandGateLoad )
{
	HRESULT hr;

	m_sLevelHead.m_strWldFile = szWldFile;

	char szPathName[MAX_PATH] = {0};
	StringCchCopy ( szPathName, MAX_PATH, DxLandMan::GetPath() );
	StringCchCat ( szPathName, MAX_PATH, m_sLevelHead.m_strWldFile.c_str() );

	char szFileType[FILETYPESIZE];
	DWORD dwVersion;

	CSerialFile SFile;
	if ( hr = SFile.OpenFile ( FOT_READ, szPathName ) )
	{
		//	Note : 파일 형식 버전확인.
		//
		SFile.GetFileType( szFileType, FILETYPESIZE, dwVersion );

		if ( strcmp(DxLandMan::FILEMARK,szFileType) )
		{
			CString Str;
			Str.Format ( "[%s]  Map File Head Error.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		if ( dwVersion > DxLandMan::VERSION )
		{
			CString Str;
			Str.Format ( "[%s]  Map File unknown version.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		SLAND_FILEMARK sLandMark;

		//	Note : .wld 파일의 맵ID는 사용하지 않음.
		SNATIVEID sMapID;
		SFile >> sMapID.dwID;

		char szMapName[MAXLANDNAME];
		SFile.ReadBuffer ( szMapName, sizeof(char)*MAXLANDNAME );

		sLandMark.LoadSet ( SFile );

		//	Note : 출입구 설정 읽기.
		//
		if ( sLandMark.dwGATE_MARK && bLandGateLoad )
		{
			SFile.SetOffSet ( sLandMark.dwGATE_MARK );
			m_cLandGateMan.Load ( SFile );
		}
	}

	return TRUE;
}

BOOL GLAGLandMan::LoadFile ( const char *szFile )
{
	GASSERT(szFile);

	BOOL bOk(FALSE);
	bOk = GLLevelFile::LoadFile ( szFile, TRUE, NULL );
	if ( !bOk )			return FALSE;

	bOk = LoadWldFile ( m_sLevelHead.m_strWldFile.c_str(), GLLevelFile::GetFileVer()<=VERSION_BEFORE_GATE );
	if ( !bOk )			return FALSE;

	return TRUE;
}

void GLAGLandMan::CleanUp ()
{
	m_setPC.clear();
	m_dwClubMapID = UINT_MAX;
	m_bGuidBattleMap = false;
	m_bClubDeathMatchMap = false;

	m_cLandGateMan.CleanUp ();
}

bool GLAGLandMan::DropPC ( DWORD dwID )
{
	m_setPC.insert ( dwID );

	return true;
}

bool GLAGLandMan::DropOutPC ( DWORD dwID )
{
	SETPC_ITER pos = m_setPC.find ( dwID );
	if ( pos==m_setPC.end() )	return false;

	m_setPC.erase ( pos );

	return true;
}

void GLAGLandMan::SENDTOALLCLIENT ( LPVOID nmg )
{
	PGLCHARAG pCHAR = NULL;
	SETPC_ITER pos = m_setPC.begin();
	SETPC_ITER end = m_setPC.end();
	for ( ; pos!=end; ++pos )
	{
		pCHAR = GLAgentServer::GetInstance().GetChar ( (*pos) );
		if ( !pCHAR )	
		{
			continue;
		}
		else
		{
			GLAgentServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, nmg );
		}
	}
}
