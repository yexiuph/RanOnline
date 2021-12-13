#include "pch.h"
#include "./GLItemMixMan.h"
#include "./GLogicData.h"
#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int nKeysize = (sizeof( DWORD )+sizeof( BYTE ) ) * ITEMMIX_ITEMNUM * 2;

GLItemMixMan& GLItemMixMan::GetInstance()
{
	static GLItemMixMan Instance;
	return Instance;
}


GLItemMixMan::GLItemMixMan () 
: m_bServer ( false )
{
}

GLItemMixMan::~GLItemMixMan ()
{
	CleanUp();
}

HRESULT GLItemMixMan::LoadFile ( const char* szFile, BOOL bServer )
{
	//	Note : 이전 정보 삭제.
	CleanUp();

	m_bServer = bServer;

	std::string strLIST_FILE = m_strPATH + szFile;

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if( !cFILE.open( strLIST_FILE, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLItemMixMan::LoadFile(), File Open %s", szFile );
		return false;
	}

	INT nItemMixKeySize(0);

	cFILE.getflag( "ITEM_MIX_INFO", "ItemMixKeySize", 0, 1, nItemMixKeySize );

	DWORD dwNUM = cFILE.GetKeySize( "ITEM_MIX_LIST", "ITEM_MIX" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		ITEM_MIX	sItemMix;

		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 0, nItemMixKeySize, sItemMix.dwKey );


		for ( int j = 0; j < ITEMMIX_ITEMNUM; ++j )
		{
			cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", (j*3)+1, nItemMixKeySize, sItemMix.sMeterialItem[j].sNID.wMainID );
			cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", (j*3)+2, nItemMixKeySize, sItemMix.sMeterialItem[j].sNID.wSubID );		
			cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", (j*3)+3, nItemMixKeySize, sItemMix.sMeterialItem[j].nNum );
		}

		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 16, nItemMixKeySize, sItemMix.sResultItem.sNID.wMainID );
		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 17, nItemMixKeySize, sItemMix.sResultItem.sNID.wSubID );
		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 18, nItemMixKeySize, sItemMix.sResultItem.nNum );

		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 19, nItemMixKeySize, sItemMix.dwRate );
		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 20, nItemMixKeySize, sItemMix.dwPrice );

		insert( sItemMix );
	}

	return S_OK;
}

void GLItemMixMan::CleanUp()
{
	m_mapStrItemMix.clear();
	m_mapKeyItemMix.clear();

	m_bServer = false;
}

void GLItemMixMan::insert ( ITEM_MIX& sItemMix )
{
	if ( m_bServer )
	{
		SortMeterialItem( sItemMix );
		m_mapKeyItemMix.insert( std::make_pair( sItemMix.dwKey, sItemMix ));
	}
	else
	{
		
		TCHAR szKey[nKeysize+1] = {0};

		SortMeterialItem( sItemMix );

		for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
		{
			_stprintf_s( szKey, "%s%0.8x%0.2x", szKey, sItemMix.sMeterialItem[i].sNID.dwID, sItemMix.sMeterialItem[i].nNum );
		}

		std::string strKey = szKey;

		m_mapStrItemMix.insert( std::make_pair( strKey, sItemMix ));		
		
	}

	return;
}

const ITEM_MIX* GLItemMixMan::GetItemMix( const ITEM_MIX& sItemMix )
{
	if ( m_bServer )	return NULL;

	TCHAR szKey[nKeysize+1] = {0};


	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
	{
		_stprintf_s( szKey, "%s%0.8x%0.2x", szKey, sItemMix.sMeterialItem[i].sNID.dwID, sItemMix.sMeterialItem[i].nNum );
	}

	std::string strKey = szKey;

	ITEMMIX_STRMAP_ITER pos_beg = m_mapStrItemMix.find( strKey );
	ITEMMIX_STRMAP_ITER pos_end = m_mapStrItemMix.end();
	if ( pos_beg == pos_end )
	{
		return NULL;
	}

	return &(*pos_beg).second;
}

const ITEM_MIX* GLItemMixMan::GetItemMix( DWORD dwKey )
{
	if ( !m_bServer ) return NULL;
	
	ITEMMIX_KEYMAP_ITER pos = m_mapKeyItemMix.find(dwKey);
	if ( pos==m_mapKeyItemMix.end() )		return NULL;

	return &(*pos).second;
}

void GLItemMixMan::SortMeterialItem( ITEM_MIX& sItemMix )
{
	std::vector< ITEMMIX_DATA >	vecTemp;
	for( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		vecTemp.push_back( sItemMix.sMeterialItem[i] );
	}

	std::sort( vecTemp.begin(), vecTemp.end(), ITEMMIX_DATA_CMP() );

	std::vector< ITEMMIX_DATA >::iterator pos = vecTemp.begin();
	for( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		sItemMix.sMeterialItem[i] = (ITEMMIX_DATA)(*pos);
		pos++;
	}
}

void GLItemMixMan::SortInvenItem ( SINVENITEM* pInvenItem )
{
	std::vector< SINVENITEM >	vecTemp;
	for( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		vecTemp.push_back( pInvenItem[i] );
	}

	std::sort( vecTemp.begin(), vecTemp.end(), ITEMMIX_INVEN_CMP() );

	std::vector< SINVENITEM >::iterator pos = vecTemp.begin();
	for( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		pInvenItem[i] = (SINVENITEM)(*pos);
		pos++;
	}

	return;
}
