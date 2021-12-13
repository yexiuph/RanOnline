#pragma once

#include <map>
#include "uicontrol.h"

#define STRING_NUM_128	128

typedef std::map<TSTRING, INTERFACE_CFG*>					MAP_UI_CFG;
typedef std::map<TSTRING, INTERFACE_CFG*>::iterator			ITER_MAP_UI_CFG;
typedef std::map<TSTRING, INTERFACE_CFG*>::const_iterator	CITER_MAP_UI_CFG;

class CInterfaceCfg
{
private:
	CMapStringToString  m_TextMap;
	TCHAR m_szPath[MAX_PATH];

	UIRECT	m_rcControlPos;
	UIRECT	m_rcTexturePos;
	UIRECT	m_rcFlipTexturePos;
	UIRECT	m_rcExPos;

	TCHAR	m_szTextureName[ STRING_NUM_128 ];
	TCHAR	m_szFlipTextureName[ STRING_NUM_128 ];

	MAP_UI_CFG m_mapUICfg;

protected:
	CInterfaceCfg(void);

public:
	virtual ~CInterfaceCfg(void);

public:
	BOOL ArrangeInfo( const TCHAR *keyvalue, INTERFACE_CFG & uiCfg );
	BOOL ArrangeInfo( const TCHAR *keyvalue );
	BOOL ArrangeFlipInfo(/*const*/ TCHAR *keyvalue);
	BOOL ArrangeExInfo(/*const*/ TCHAR *keyvalue);

	BOOL LoadText( const TCHAR * filename );
	CString GetText( const TCHAR *keyvalue );
	CString GetText( CString keyvalue );
	BOOL GetText( const TCHAR *keyvalue, CString &Text );
	BOOL GetText( CString keyvalue, CString &Text );   
	
	//UIRECT GetPosition(TCHAR *keyvalue);
	//UIRECT GetTexturePosition(TCHAR *keyvalue);

	TCHAR* GetTextureName(TCHAR *keyvalue);
	
	//UIRECT GetFlipTexturePosition(TCHAR *keyvalue);
	TCHAR* GetFlipTextureName(TCHAR *keyvalue);

	UIRECT GetPosition()						{ return m_rcControlPos; }
	UIRECT GetTexturePosition()					{ return m_rcTexturePos; }
	TCHAR* GetTextureName();
	
	UIRECT GetFlipTexturePosition()				{ return m_rcFlipTexturePos; }
	TCHAR* GetFlipTextureName();

	UIRECT GetExPosition()						{ return m_rcExPos; }
	void Reset();

	TCHAR* GetPath ()				{ return m_szPath; }
	void SetPath( TCHAR* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }

public:
	static CInterfaceCfg& GetInstance();
};