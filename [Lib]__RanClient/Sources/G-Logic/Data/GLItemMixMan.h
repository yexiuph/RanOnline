#pragma once

#include <string>

#include "./GLItem.h"

typedef std::map<std::string,ITEM_MIX>		ITEMMIX_STRMAP;
typedef std::map<DWORD,ITEM_MIX>			ITEMMIX_KEYMAP;

typedef ITEMMIX_STRMAP::iterator			ITEMMIX_STRMAP_ITER;
typedef ITEMMIX_KEYMAP::iterator			ITEMMIX_KEYMAP_ITER;

class GLItemMixMan
{
public:
	enum
	{
		VERSION	= 0x0100,		
	};

public:

	ITEMMIX_STRMAP	m_mapStrItemMix;
	ITEMMIX_KEYMAP	m_mapKeyItemMix;

	std::string				m_strPATH;

public:
	void SetPath ( std::string strPATH )	{ m_strPATH = strPATH; }
	std::string GetPath ()					{ return m_strPATH.c_str(); }

protected:
	bool	m_bServer;

public:
	HRESULT LoadFile ( const char* szFile, BOOL bServer );

	void insert ( ITEM_MIX& sItemMix );

	void SortMeterialItem( ITEM_MIX& sItemMix );	//	재료아이템 정렬
	void SortInvenItem ( SINVENITEM* pInvenItem );

	const ITEM_MIX* GetItemMix( const ITEM_MIX& sItemMix );
	const ITEM_MIX* GetItemMix( DWORD dwKey );

public:
	GLItemMixMan ();

public:
	~GLItemMixMan ();
	void CleanUp();

public:
	static GLItemMixMan& GetInstance();
};

class ITEMMIX_DATA_CMP
{
public:
	bool operator() ( const ITEMMIX_DATA &lvalue, const ITEMMIX_DATA &rvalue )
	{
		if ( lvalue.sNID.dwID < rvalue.sNID.dwID ) return true;
		else if ( lvalue.sNID.dwID == rvalue.sNID.dwID ) 
		{	
			if ( lvalue.nNum < rvalue.nNum ) return true;
		}
		
		return false;
	}
};

class ITEMMIX_INVEN_CMP
{
public:
	bool operator() ( const SINVENITEM &lvalue, const SINVENITEM &rvalue )
	{
		if ( lvalue.sItemCustom.sNativeID.dwID < rvalue.sItemCustom.sNativeID.dwID ) return true;
		else if ( lvalue.sItemCustom.sNativeID.dwID == rvalue.sItemCustom.sNativeID.dwID ) 
		{	
			if ( lvalue.sItemCustom.wTurnNum < rvalue.sItemCustom.wTurnNum ) return true;
		}
		
		return false;
	}
};

