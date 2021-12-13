#pragma	once

#include "../[Lib]__EngineUI/Sources/GameTextLoader.h"

//	게임 텍스트
struct SGAMEWORD
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags;
};

//	게임 텍스트
struct SGAMEINTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags;
};

//	게임 텍스트
struct SGAMEEXTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags;
};

//	서버 텍스트
struct SSERVERTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags;
};

const char * ID2GAMEWORD ( const char* szKeyword, int nIndex = 0 );
const char * ID2GAMEINTEXT ( const char* szKeyword, int nIndex = 0 );
const char * ID2GAMEEXTEXT ( const char* szKeyword, int nIndex = 0 );
const char * ID2SERVERTEXT ( const char* szKeyword, int nIndex = 0 );

class CGameTextMan
{
public:
	enum
	{		
		EM_GAME_WORD,
		EM_GAME_IN_TEXT,
		EM_GAME_EX_TEXT,
		EM_SERVER_TEXT,
	};

private:
	CGameTextMan ();
	virtual ~CGameTextMan ();

public:
	const char* GetPath ()				{ return m_szPath; }
	void SetPath( const char* szPath )	{ StringCchCopy(m_szPath,MAX_PATH,szPath); }

public:
	const CString & GetGameWord( std::string strKey, int Index = 0 );
	const CString & GetGameInText( std::string strKey, int Index = 0 );
	const CString & GetGameExText( std::string strKey, int Index = 0 );
	const CString & GetServerText( std::string strKey, int nIndex = 0 );

	const CString &	GetText( std::string strKey, int Index, int nType );
	int	GetNumber( std::string strKey, int Index, int nType );

public:
	void Clear ( int nType );
	BOOL LoadText ( const char* szFileName, int nType, BOOL bXML = FALSE );	

private:
	char m_szPath[MAX_PATH];

private:
    SGAMEWORD	m_GameWord;
	SGAMEINTEXT	m_GameInText;
	SGAMEEXTEXT	m_GameExText;
	SSERVERTEXT m_ServerText;

public:
	static CGameTextMan& GetInstance();
};