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

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )	continue;
		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	char szSkeleton[MAX_PATH] = "";
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*MAX_PATH );
	m_strSkeleton = szSkeleton;

	//	Note :���ϸ��̼� �б�.
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

	//	Note : ���� File Data �� �����Ѵ�.
	//

	return TRUE;
}

BOOL DxSkinDataDummy::LOAD_0101 ( basestream &SFile )
{
	DWORD dwPieceSize;

	SFile >> m_fScale;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE )		return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )	continue;
		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	SFile >> m_strSkeleton;

	//	Note :���ϸ��̼� �б�.
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

	//	Note : ���� File Data �� �����Ѵ�.
	//

	return TRUE;
}

//	Note : ��Ų ������ ����.
//		BOOL DxSkinDataDummy::LoadFile () ������ ���� ������ �ε��� ���ϹǷ� �� �ε�/���̺� �κ��� �ٲ��
//		�����ؼ� ���� ������ �ʿ��ϴ�.
//
BOOL DxSkinDataDummy::LoadFile ( const char* szFile )
{
	char szPathName[MAX_PATH] = {0};
	StringCchCopy( szPathName, MAX_PATH, DxSkinCharDataContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );

	std::auto_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bENGLIB_ZIPFILE, 
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

	//	Note : ��Ų �ɸ��� �̸� ����.
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

