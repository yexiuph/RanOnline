#include "pch.h"
#include "HelpDataMan.h"
#include "SerialFile.h"
#include "GLogicData.h"
#include "GLogic.h"
#include "StringUtils.h"
#include "StringFile.h"
#include <afxdlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::string;

CHelpDataMan& CHelpDataMan::GetInstance()
{
	static CHelpDataMan Instance;
	return Instance;
}

const WORD SHELPNODE::VER = 0x0001;
const INT LEVEL = 3;

BOOL SHELPNODE::SaveFile ( CSerialFile& SFile )
{
	{
		HELPNODE_SIZE nSize = strTitle.size () + 1;
		SFile.WriteBuffer ( &nSize, sizeof ( HELPNODE_SIZE ) );
		SFile.WriteBuffer ( strTitle.c_str (), (DWORD) (sizeof ( char ) * nSize) );
	}

	{
		HELPNODE_SIZE nSize = strContents.size () + 1;
		SFile.WriteBuffer ( &nSize, sizeof ( HELPNODE_SIZE ) );
		SFile.WriteBuffer ( strContents.c_str (), (DWORD) (sizeof ( char ) * nSize) );
	}
	
	{
		BOOL bResult = FALSE;
		HELPNODE_SIZE nSize = listSubNode.size ();
		SFile.WriteBuffer ( &nSize, (DWORD)sizeof ( HELPNODE_SIZE ) );

		HELPNODE_LIST_ITER iter = listSubNode.begin ();
		HELPNODE_LIST_ITER iter_end = listSubNode.end ();
		for ( ; iter != iter_end; ++iter )
		{
			bResult = (*iter)->SaveFile ( SFile );
			if ( !bResult ) return FALSE;
		}
	}

	return TRUE;
}

BOOL SHELPNODE::LoadFile ( basestream& SFile )
{
	{
		HELPNODE_SIZE nSize = 0;
		SFile.ReadBuffer ( &nSize, sizeof ( HELPNODE_SIZE ) );

		char* pszTemp = new char[nSize];
		SFile.ReadBuffer ( pszTemp, (DWORD)(sizeof ( char ) * nSize) );
		strTitle = pszTemp;
        delete [] pszTemp;
	}

	{
		HELPNODE_SIZE nSize = 0;
		SFile.ReadBuffer ( &nSize, sizeof ( HELPNODE_SIZE ) );

		char* pszTemp = new char[nSize];
		SFile.ReadBuffer ( pszTemp, (DWORD)(sizeof ( char ) * nSize) );
		strContents = pszTemp;
        delete [] pszTemp;
	}
	
	{
		BOOL bResult = FALSE;
		HELPNODE_SIZE nSize = 0;
		SFile.ReadBuffer ( &nSize, (DWORD)sizeof ( HELPNODE_SIZE ) );
		for ( HELPNODE_SIZE i = 0; i != nSize; ++i )
		{
			SHELPNODE* pNode = new SHELPNODE;
			bResult = pNode->LoadFile ( SFile );
			if ( !bResult )
			{
				delete pNode;
				return FALSE;
			}

			listSubNode.push_back ( pNode );
		}
	}

	return TRUE;
}

VOID SHELPNODE::SaveCsvFile( std::fstream &SFile, INT &iIndex, INT iDepth )
{
	SFile << ++iIndex << "\t";
	SFile << iDepth << "\t";
	SFile << strTitle << "\t";

	std::string strTemp;
	for( std::string::iterator ci = strContents.begin(); ci != strContents.end(); ++ci )
	{
		if( ( *ci ) == 10 )
			continue;
		if( ( *ci ) == 13 )
			strTemp.push_back( (char)7 );
		else
            strTemp.push_back( *ci );
	}

	if( strTemp.empty() )
		SFile << " " << std::endl;
	else
        SFile << strTemp << std::endl;

	HELPNODE_LIST_ITER iter = listSubNode.begin();
	HELPNODE_LIST_ITER iter_end = listSubNode.end();
	for( ; iter != iter_end; ++iter )
		(*iter)->SaveCsvFile( SFile, iIndex, iDepth+1 );
}

VOID SHELPNODE::LoadCsvFile( CStringArray &StrArray )
{
	strTitle = StrArray[2];

	std::string strTemp = StrArray[3];
	for( std::string::iterator ci = strTemp.begin(); ci != strTemp.end(); ++ci )
	{
		if( ( *ci ) == 7 )
		{
			strContents.push_back( (char)13 );
			strContents.push_back( (char)10 );
		}
		else
		{
			strContents.push_back( *ci );
		}
	}
}

VOID SHELPNODE::AddNode( INT iDepth, SHELPNODE* pNode )
{
	if( !iDepth )
	{
		listSubNode.push_back( pNode );
	}
	else
	{
		HELPNODE_LIST::reverse_iterator ei = listSubNode.rbegin();
		( *ei )->AddNode( iDepth-1, pNode );
	}
}

CHelpDataMan::CHelpDataMan ()
{
}

CHelpDataMan::~CHelpDataMan ()
{
	RemoveAll ();
}

void	CHelpDataMan::RemoveAll ()
{	
	if ( m_listHelp.empty () ) return ;

	HELPNODE_LIST_ITER iter = m_listHelp.begin ();
	HELPNODE_LIST_ITER iter_end = m_listHelp.end ();
	for ( ; iter != iter_end; ++iter )
	{
		delete (*iter);
	}

	m_listHelp.clear ();
}

HELPNODE_LIST&	CHelpDataMan::GetHelpDataMan ()
{
	return m_listHelp;
}

BOOL	CHelpDataMan::AddNode ( SHELPNODE* pParent, SHELPNODE* pNode, std::string strPrevTitle )
{
	if ( !pNode )
	{
		GASSERT ( 0 && "노드가 널입니다." );
		return FALSE;
	}

	if ( pParent )	//	서브 노드
	{
		if ( strPrevTitle.empty () )
		{
			pParent->listSubNode.push_back ( pNode );
			return TRUE;
		}

		//	불변식 :
		//		strPreTitle 다음 노드에 pNode를 삽입한다.
		HELPNODE_LIST_ITER iter = pParent->listSubNode.begin ();
		HELPNODE_LIST_ITER iter_end = pParent->listSubNode.end ();
		for ( ; iter != iter_end; ++iter )
		{
			SHELPNODE* pNode = (*iter);
			if ( pNode->strTitle == strPrevTitle )
			{
				//	insert는 현재 위치에 삽입한다.
				//	그러나, 삽입 되어야 할 곳은 현재의 다음이다.
				++iter;

				pParent->listSubNode.insert ( iter, pNode );
				return TRUE;
			}
		}

		return FALSE;
	}
	else			//	메인
	{
		if ( strPrevTitle.empty () )
		{
			m_listHelp.push_back ( pNode );
			return TRUE;
		}

		//	불변식 :
		//		strPreTitle 다음 노드에 pNode를 삽입한다.
		HELPNODE_LIST_ITER iter = m_listHelp.begin ();
		HELPNODE_LIST_ITER iter_end = m_listHelp.end ();
		for ( ; iter != iter_end; ++iter )
		{
			SHELPNODE* pNode = (*iter);
			if ( pNode->strTitle == strPrevTitle )
			{
				//	insert는 현재 위치에 삽입한다.
				//	그러나, 삽입 되어야 할 곳은 현재의 다음이다.
				++iter;

				pParent->listSubNode.insert ( iter, pNode );
				return TRUE;
			}
		}

		return FALSE;
	}
}

BOOL	CHelpDataMan::DelNode ( SHELPNODE* pParent, std::string strTitle )
{
	if ( pParent )	//	서브
	{
		//	불변식 :
		//		strPreTitle 다음 노드에 pNode를 삽입한다.
		HELPNODE_LIST_ITER iter = pParent->listSubNode.begin ();
		HELPNODE_LIST_ITER iter_end = pParent->listSubNode.end ();
		for ( ; iter != iter_end; ++iter )
		{
			SHELPNODE* pNode = (*iter);
			if ( pNode->strTitle == strTitle )
			{
				delete pNode;
				pParent->listSubNode.erase ( iter );
				return TRUE;
			}
		}

		return FALSE;
	}
	else	//	메인
	{

		//	불변식 :
		//		strPreTitle 다음 노드에 pNode를 삽입한다.
		HELPNODE_LIST_ITER iter = m_listHelp.begin ();
		HELPNODE_LIST_ITER iter_end = m_listHelp.end ();
		for ( ; iter != iter_end; ++iter )
		{
			SHELPNODE* pNode = (*iter);
			if ( pNode->strTitle == strTitle )
			{
				delete pNode;
				m_listHelp.erase ( iter );
				return TRUE;
			}
		}

		return FALSE;
	}
}

SHELPNODE*	CHelpDataMan::FindNode ( SHELPNODE* pParent, string strTitle )
{
	if ( pParent )
	{
		HELPNODE_LIST_ITER iter = pParent->listSubNode.begin ();
		HELPNODE_LIST_ITER iter_end = pParent->listSubNode.end ();
		for ( ; iter != iter_end; ++iter )
		{
			SHELPNODE* pNode = (*iter);
			if ( pNode->strTitle == strTitle )
			{
				return pNode;
			}
		}
	}
	else
	{
		HELPNODE_LIST_ITER iter = m_listHelp.begin ();
		HELPNODE_LIST_ITER iter_end = m_listHelp.end ();
		for ( ; iter != iter_end; ++iter )
		{
			SHELPNODE* pNode = (*iter);
			if ( pNode->strTitle == strTitle )
			{
				return pNode;
			}
		}
	}

	return NULL;
}

BOOL	CHelpDataMan::ChangeNode ( SHELPNODE* pParent, std::string strTitleA, std::string strTitleB )
{
	if ( pParent )
	{
		HELPNODE_LIST_ITER Aiter = pParent->listSubNode.begin ();
		HELPNODE_LIST_ITER Aiter_end = pParent->listSubNode.end ();
		for ( ; Aiter != Aiter_end; ++Aiter )
		{
			SHELPNODE* pNode = (*Aiter);
			if ( pNode->strTitle == strTitleA )
			{
				break;
			}
		}

		HELPNODE_LIST_ITER Biter = pParent->listSubNode.begin ();
		HELPNODE_LIST_ITER Biter_end = pParent->listSubNode.end ();
		for ( ; Biter != Biter_end; ++Biter )
		{
			SHELPNODE* pNode = (*Biter);
			if ( pNode->strTitle == strTitleB )
			{
				break;
			}
		}

		SHELPNODE* pTemp = (*Aiter);
		(*Aiter) = (*Biter);
		(*Biter) = pTemp;

		return TRUE;
	}
	else
	{
		HELPNODE_LIST_ITER Aiter = m_listHelp.begin ();
		HELPNODE_LIST_ITER Aiter_end = m_listHelp.end ();
		for ( ; Aiter != Aiter_end; ++Aiter )
		{
			SHELPNODE* pNode = (*Aiter);
			if ( pNode->strTitle == strTitleA )
			{
				break;
			}
		}

		HELPNODE_LIST_ITER Biter = m_listHelp.begin ();
		HELPNODE_LIST_ITER Biter_end = m_listHelp.end ();
		for ( ; Biter != Biter_end; ++Biter )
		{
			SHELPNODE* pNode = (*Biter);
			if ( pNode->strTitle == strTitleB )
			{
				break;
			}
		}

		SHELPNODE* pTemp = (*Aiter);
		(*Aiter) = (*Biter);
		(*Biter) = pTemp;

		return TRUE;
	}
}

BOOL	CHelpDataMan::SaveFile ( CString strFileName )
{
	BOOL bResult = FALSE;

	CSerialFile SFile( FOT_WRITE, strFileName.GetString () );
	if ( SFile.IsOpen () )
	{
		SFile.WriteBuffer ( &SHELPNODE::VER, (DWORD)sizeof ( WORD ) );

		HELPNODE_SIZE nSize = m_listHelp.size ();
		SFile.WriteBuffer ( &nSize, (DWORD)sizeof ( HELPNODE_SIZE ) );

		HELPNODE_LIST_ITER iter = m_listHelp.begin ();
		HELPNODE_LIST_ITER iter_end = m_listHelp.end ();        
		for ( ; iter != iter_end; ++iter )
		{
			bResult = (*iter)->SaveFile ( SFile );
			if ( !bResult ) return FALSE;
		}

		SFile.CloseFile ();

		return TRUE;
	}

	return FALSE;
}

BOOL	CHelpDataMan::LoadFile ( CString strFileName )
{	
	RemoveAll ();

	BOOL bResult = FALSE;

	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s( strFileName.GetString(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT );
	StringCchCat ( fname, _MAX_FNAME, ext );

	std::auto_ptr<basestream> pBStream(GLOGIC::openfile_basestream(	GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strGLOGIC_ZIPFILE.c_str(),
                                                                    strFileName.GetString(), 
																	fname,
																	false,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if ( !pBStream.get() )
		return FALSE;

	basestream &SFile = *pBStream;

	if ( SFile.IsOpen () )
	{
		WORD wVer = 0;
		SFile.ReadBuffer ( &wVer, (DWORD)sizeof ( WORD ) );

		HELPNODE_SIZE nSize = 0;
		SFile.ReadBuffer ( &nSize, (DWORD)sizeof ( HELPNODE_SIZE ) );

		for ( HELPNODE_SIZE i = 0; i < nSize; i++ )
		{
			SHELPNODE* pNewNode = new SHELPNODE;

			bResult = pNewNode->LoadFile ( SFile );
			if ( !bResult )
			{
				GASSERT ( 0 && "오류 발생" );
				return FALSE;
			}

			m_listHelp.push_back ( pNewNode );
		}

		SFile.CloseFile ();

		return TRUE;
	}

	return FALSE;
}

HRESULT CHelpDataMan::SaveCsvFile( CWnd* pWnd )
{
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if( dlg.DoModal() != IDOK )
		return E_FAIL;

	std::fstream streamFILE;
	streamFILE.open( dlg.GetPathName().GetString(), std::ios_base::out );

	INT iIndex = 0;

	HELPNODE_LIST_ITER iter = m_listHelp.begin();
	HELPNODE_LIST_ITER iter_end = m_listHelp.end();
	for( ; iter != iter_end; ++iter )
		(*iter)->SaveCsvFile( streamFILE, iIndex, 0 );

	streamFILE.close();

	return S_OK;
}

HRESULT CHelpDataMan::LoadCsvFile( CWnd* pWnd )
{
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return E_FAIL;

	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return E_FAIL;

	RemoveAll();

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "\t" );

	CString strLine;
	CStringArray StrArray;

	int iLine = 1;
	char szError[ 256 ];

	while( StrFile.GetNextLine( strLine ) )
	{
		++iLine;

		STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
		if( iCount < 4 )
		{
			StringCchPrintf( szError, 256, "%d Line = include blank column, next item load failed", iLine );
			MessageBox( pWnd->GetSafeHwnd(), _T(szError), _T("Fail"), MB_OK );
			return E_FAIL;
		}

		SHELPNODE* pNode = new SHELPNODE;
		pNode->LoadCsvFile( StrArray );

		INT iDepth = (INT)atoi( StrArray[1] );
		if( !iDepth )
		{
			m_listHelp.push_back( pNode );
		}
		else
		{
			HELPNODE_LIST::reverse_iterator ei = m_listHelp.rbegin();
			( *ei )->AddNode( iDepth-1, pNode );
		}
	}

	return S_OK;
}