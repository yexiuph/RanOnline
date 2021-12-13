#pragma once

#include "../[Lib]__MfcEx/Sources/ExLibs/webbrowser2.h"

//--------------------------------------------------------------------
// CommonWeb : 최준혁
//--------------------------------------------------------------------
class CCommonWeb
{
public:
	enum
	{
		HELP_ID,		// 도움말 웹 브라우저
		ITEMSHOP_ID,	// 아이템샾 브라우저

		FIGHT_ID,		// 캐릭터 생성 정보 - 격투
		FENCING_ID,		// 캐릭터 생성 정보 - 검도
		ARCHERY_ID,		// 캐릭터 생성 정보 - 양궁
		SPIRIT_ID,		// 캐릭터 생성 정보 - 기예
		//SHOP_ID = 1,	// 상점 웹 브라우저

		TOTAL_ID = 6	// 브라우저 갯수
		//TOTAL_ID = 2	// 브라우저 갯수
	};

protected:
	static CCommonWeb*	m_cpCommonWeb;

	BOOL				m_bCreate[ TOTAL_ID ];
	CWebBrowser2		m_cWebBrowser[ TOTAL_ID ];

	CWnd*				m_pWnd;
	BOOL				m_bVisible[ TOTAL_ID ];
	RECT*				m_pBound;
	BOOL*				m_pVisible;

public:
	CCommonWeb();
	~CCommonWeb();

public:
	VOID Create( CWnd* pParent, BOOL* pVisible, RECT* pBound );
	VOID Navigate( INT iID, const TCHAR* pszPath, BOOL bShow = FALSE );
	VOID Move( INT iID, INT iLeft, INT iTop, INT iRight, INT iBottom, BOOL bReDraw = TRUE, BOOL bCalRect = TRUE );

	void Refresh( INT iID );

	BOOL GetCreate( INT iID = -1 );
	VOID SetVisible( INT iID, BOOL bVisible );
	VOID SetVisible( BOOL bVisible );
	BOOL GetVisible( INT iID );

	bool IsCompleteLoad( INT iID );
	void ReSetCompleteLoad( INT iID );

public:
	static CCommonWeb* Get();
	static VOID CleanUp();
};