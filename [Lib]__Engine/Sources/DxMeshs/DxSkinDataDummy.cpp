#include "pch.h"
#include "DxSkinCharData.h"
#include "./GLogic.h"
#include "./StringUtils.h"
#include "./stlFunctions.h"
#include <algorithm>

#include "DxSkinDataDummy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxSkinDataDummy::DxSkinDataDummy(void)
{
	CleanUp ();
}

DxSkinDataDummy::~DxSkinDataDummy(void)
{
	CleanUp ();
}

void DxSkinDataDummy::CleanUp ()
{
	m_strFileName.clear();
	
	m_fScale = 1.0f;

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_strPIECE[i].clear();
	}

	std::for_each ( m_vecANIMINFO.begin(), m_vecANIMINFO.end(), std_afunc::DeleteObject() );
	m_vecANIMINFO.clear();

	m_strSkeleton.clear();
}

BOOL DxSkinDataDummy::LOAD_0100 ( basestream &SFile )
{
	DWORD dwPieceSize;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE )		return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )	continue;
		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	char szSkeleton[MAX_PATH] = "";
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*MAX_PATH );
	m_strSkeleton = szSkeleton;

	//	Note :에니메이션 읽기.
	char szAniName[ACF_SZNAME] = "";

	DWORD dwLenght;
	SFile >> dwLenght;

	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile.ReadBuffer ( szAniName, sizeof(char)*ACF_SZNAME );

		CString strCfgFile;
		CString strTEMP = szAniName;
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";
		
		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

BOOL DxSkinDataDummy::LOAD_0101 ( basestream &SFile )
{
	DWORD dwPieceSize;

	SFile >> m_fScale;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE )		return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )	continue;
		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	//
	SFile >> m_strSkeleton;

	//	Note :에니메이션 읽기.
	//
	DWORD dwLenght;
	SFile >> dwLenght;

	std::string strANIFILE;
	CString strCfgFile, strTEMP;
	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile >> strANIFILE;

		strTEMP = strANIFILE.c_str();
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";
		
		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

//	Note : 스킨 파일을 읽음.
//		BOOL DxSkinDataDummy::LoadFile () 에서도 같은 구조로 로딩을 행하므로 이 로드/새이브 부분이 바뀔시
//		병행해서 같이 수정이 필요하다.
//
BOOL DxSkinDataDummy::LoadFile ( const char* szFile )
{
	char szPathName[MAX_PATH] = {0};
	StringCchCopy( szPathName, MAX_PATH, DxSkinCharDataContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );


	// C++ 20 Standards Update : Auto PTR -> Unqiue Ptr - YeXiuPH
	std::unique_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bENGLIB_ZIPFILE, 
																	GLOGIC::strSKINOBJ_ZIPFILE.c_str(),
																	szPathName, 
																	szFile ) );

	if ( !pBStream.get() )
		return FALSE;

	basestream &SFile = *pBStream;

	DWORD dwVer;
	SFile >> dwVer;

	switch ( dwVer )
	{
	case 0x0100:
		LOAD_0100(SFile);
		break;

	case 0x0101:
	case 0x0102:
		LOAD_0101(SFile);
		break;

	default:
		return FALSE;
	};

	//	Note : 스킨 케릭터 이를 설정.
	//
	m_strFileName = szFile;

	return TRUE;
}

SANIMCONINFO* DxSkinDataDummy::FindAniInfo ( const char* szName )
{
	size_t nSIZE = m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		if ( !strcmp(m_vecANIMINFO[n]->m_szName,szName) )	return m_vecANIMINFO[n];
	}

	return NULL;
}

