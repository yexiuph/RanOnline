#pragma once

#include "../[Lib]__MfcEx/Sources/ExLibs/webbrowser2.h"

//--------------------------------------------------------------------
// CommonWeb : ������
//--------------------------------------------------------------------
class CCommonWeb
{
public:
	enum
	{
		HELP_ID,		// ���� �� ������
		ITEMSHOP_ID,	// �����ۘ� ������

		FIGHT_ID,		// ĳ���� ���� ���� - ����
		FENCING_ID,		// ĳ���� ���� ���� - �˵�
		ARCHERY_ID,		// ĳ���� ���� ���� - ���
		SPIRIT_ID,		// ĳ���� ���� ���� - �⿹
		//SHOP_ID = 1,	// ���� �� ������

		TOTAL_ID = 6	// ������ ����
		//TOTAL_ID = 2	// ������ ����
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