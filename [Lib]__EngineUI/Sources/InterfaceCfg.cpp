#include "pch.h"
#include "interfacecfg.h"
#include "StringFile.h"
#include "STRINGUTILS.h"
#include "StlFunctions.h"
#include "InterfaceCfgXml.h"
#include "RANPARAM.h" //../[Lib]__MfcEx/Sources
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInterfaceCfg& CInterfaceCfg::GetInstance()
{
	static CInterfaceCfg Instance;
	return Instance;
}

CInterfaceCfg::CInterfaceCfg(void)
{
	memset( m_szPath, 0, sizeof(TCHAR)*MAX_PATH );

	memset( m_szTextureName, 0, sizeof(TCHAR)*STRING_NUM_128 );
	memset( m_szFlipTextureName, 0, sizeof(TCHAR)*STRING_NUM_128 );
}

CInterfaceCfg::~CInterfaceCfg(void)
{
	if ( !m_TextMap.IsEmpty() )
		m_TextMap.RemoveAll();

	std::for_each( m_mapUICfg.begin(), m_mapUICfg.end(), std_afunc::DeleteMapObject() );
	m_mapUICfg.clear();
}

BOOL CInterfaceCfg::LoadText( const TCHAR * filename )
{
	CString strFileName;
	strFileName = GetPath();
	strFileName += filename;

	if( RANPARAM::bXML_USE == TRUE )
	{
		CInterfaceCfgXml interfaceCfgXml( RANPARAM::strCountry );
		return interfaceCfgXml.Parse( m_mapUICfg, strFileName.GetString() );
	}
	else
	{
		CStringFile StrFile;
		if ( !StrFile.Open ( strFileName.GetString() ) )	return FALSE;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( _T("\t") );

		CString strLine;
		CStringArray StrArray;

		while ( StrFile.GetNextLine(strLine) )
		{
			STRUTIL::StringSeparate( strLine, StrArray );

			if( StrArray.GetCount() <= 0 ) continue;

			CString StrMark = StrArray.GetAt(0);

			if ( StrMark.GetAt(0)=='/' && StrMark.GetAt(1)=='/' )	continue;

			int i= (int)StrArray.GetSize() - 1;
			CString StrValue;
			while ( i )
			{
				StrValue+=StrArray.GetAt(StrArray.GetSize()-i );
				StrValue+='\t';
				i--;
			}
			// = StrArray.GetAt(1);

			m_TextMap.SetAt(StrMark , StrValue);
		}	

		return TRUE;
	}
}

void	CInterfaceCfg::Reset()
{
	m_rcControlPos = m_rcTexturePos = m_rcFlipTexturePos =UIRECT(0,0,0,0);

	StringCchCopy( m_szTextureName, STRING_NUM_128, _T("") );
	StringCchCopy( m_szFlipTextureName, STRING_NUM_128, _T("") );
}

CString	CInterfaceCfg::GetText( const TCHAR * keyvalue)
{
	CString strText;
	if ( m_TextMap.Lookup(keyvalue, strText) )	return strText;
	
	return NULL;
}

BOOL CInterfaceCfg::ArrangeInfo( const TCHAR *keyvalue, INTERFACE_CFG & uiCfg )
{
	ITER_MAP_UI_CFG iter = m_mapUICfg.find( TSTRING(keyvalue) );
	if( iter != m_mapUICfg.end() )
	{
		INTERFACE_CFG * pCfg = (*iter).second;
		uiCfg = *pCfg;

		return TRUE;
	}
	else
	{
		CDebugSet::ToLogFile ( _T("[INTERFACE] %s not found"), keyvalue );
		return FALSE;
	}
}

BOOL CInterfaceCfg::ArrangeInfo( const TCHAR *keyvalue )
{
	CString Info = GetText(keyvalue);
	CStringArray	StrArray;

	float	X_Size, Y_Size;
	float	left,top,sizeX,sizeY;
	float	fMicroX, fMicroY;

	if ( Info.GetLength () )
	{
		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( _T(" ") );
		STRUTIL::RegisterSeparator ( _T("\t") );

		STRUTIL::StringSeparate ( Info, StrArray );

		if( StrArray.GetSize() == 5 )
		{
			CString TempString( StrArray.GetAt(0) );
			StringCchCopy( m_szTextureName, STRING_NUM_128, TempString.GetString() );

			TempString = StrArray.GetAt(1);	left = (float)_tstof(TempString.GetString() );
			TempString = StrArray.GetAt(2);	top  = (float)_tstof(TempString.GetString() );
			TempString = StrArray.GetAt(3);	sizeX= (float)_tstof(TempString.GetString() );
			TempString = StrArray.GetAt(4);	sizeY= (float)_tstof(TempString.GetString() );

			m_rcControlPos = UIRECT( left, top, sizeX, sizeY);

			X_Size = 1; Y_Size = 1; left = 0; top = 0; sizeX = 1; sizeY = 1;

			return TRUE;
		}
		
		if ( StrArray.GetSize() != 11 ) return FALSE;
		
		CString TempString( StrArray.GetAt(0) );
		StringCchCopy( m_szTextureName, STRING_NUM_128, TempString.GetString() );
		
		//TempString = StrArray.GetAt(1);	X_Size = (float)_tstof(TempString.GetString() );
		//TempString = StrArray.GetAt(2);	Y_Size = (float)_tstof(TempString.GetString() );

		TempString = StrArray.GetAt(1);	left = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(2);	top  = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(3);	sizeX= (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(4);	sizeY= (float)_tstof(TempString.GetString() );
		
		m_rcControlPos = UIRECT( left, top, sizeX, sizeY);

		TempString = StrArray.GetAt(5);	X_Size = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(6);	Y_Size = (float)_tstof(TempString.GetString() );

		TempString = StrArray.GetAt(7);	left = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(8);	top  = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(9);	sizeX= (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(10);sizeY= (float)_tstof(TempString.GetString() );	

		fMicroX = 0.25f/X_Size;
		fMicroY = 0.25f/Y_Size;

		m_rcTexturePos = UIRECT( left/X_Size+fMicroX, top/Y_Size+fMicroY, sizeX/X_Size, sizeY/Y_Size );

		return TRUE;
	}
	else
	{
		CDebugSet::ToLogFile ( _T("[INTERFACE] %s not found"), keyvalue );
		return FALSE;
	}
}

BOOL CInterfaceCfg::ArrangeFlipInfo(/*const*/ TCHAR *keyvalue)
{
	CString Info = GetText(keyvalue);
	CStringArray	StrArray;

	float	X_Size, Y_Size;
	float	left,top,sizeX,sizeY;

	if ( Info.GetLength () )
	{
		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( _T(" ") );
		STRUTIL::RegisterSeparator ( _T("\t") );

		STRUTIL::StringSeparate ( Info, StrArray );
		
		if ( StrArray.GetSize() != 7 ) return FALSE;

		CString TempString = StrArray.GetAt(0);
		StringCchCopy( m_szFlipTextureName, STRING_NUM_128, TempString.GetString() );
		
		TempString = StrArray.GetAt(1);	X_Size = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(2);	Y_Size = (float)_tstof(TempString.GetString() );

		TempString = StrArray.GetAt(3);	left = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(4);	top  = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(5);	sizeX= (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(6);	sizeY= (float)_tstof(TempString.GetString() );
		
		m_rcFlipTexturePos = UIRECT( left/X_Size, top/Y_Size, sizeX/X_Size, sizeY/Y_Size );

		return TRUE;
	}
	else
	{
		CDebugSet::ToLogFile ( _T("[INTERFACE] %s not found"), keyvalue );
		return FALSE;
	}	
}

BOOL CInterfaceCfg::ArrangeExInfo(/*const*/ TCHAR *keyvalue)
{
	CString Info = GetText(keyvalue);
	CStringArray	StrArray;

	float	left,top,sizeX,sizeY;

	if ( Info.GetLength () )
	{
		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( _T(" ") );
		STRUTIL::RegisterSeparator ( _T("\t") );

		STRUTIL::StringSeparate ( Info, StrArray );
		
		if ( StrArray.GetSize() != 4 ) return FALSE;

		CString TempString = StrArray.GetAt(0);	left = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(1);	top  = (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(2);	sizeX= (float)_tstof(TempString.GetString() );
		TempString = StrArray.GetAt(3);	sizeY= (float)_tstof(TempString.GetString() );
		
		m_rcExPos = UIRECT( left, top, sizeX, sizeY );
	}
	else
	{
		CDebugSet::ToLogFile ( _T("[INTERFACE] %s not found"), keyvalue );
	}
	return TRUE;
}

TCHAR* CInterfaceCfg::GetTextureName()
{
	if( !_tcscmp(m_szTextureName, _T("NULL") ) )	
		return NULL;

	return m_szTextureName;
}

TCHAR* CInterfaceCfg::GetFlipTextureName()
{
	if( !_tcscmp(m_szFlipTextureName, _T("NULL") ) )	
		return NULL;

	return m_szFlipTextureName;
}

CString CInterfaceCfg::GetText(CString keyvalue)
{
	CString strText;
	if ( m_TextMap.Lookup(keyvalue.GetString(),strText) )
		return strText;

	return NULL;
}

BOOL CInterfaceCfg::GetText( const TCHAR *keyvalue, CString &Text)
{
	if( m_TextMap.Lookup(keyvalue, Text ) )
		return TRUE;

	return FALSE;
}

BOOL CInterfaceCfg::GetText(CString keyvalue, CString &Text)
{
	if( m_TextMap.Lookup(keyvalue.GetString(), Text ) )	
		return TRUE;

	return FALSE;
}