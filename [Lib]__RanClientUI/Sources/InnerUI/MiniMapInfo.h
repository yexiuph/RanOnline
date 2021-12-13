#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;

class CMiniMapInfo : public CUIGroup
{
protected:
	enum
	{
		MINIMAP_CLUBMARK = NO_ID + 1
	};

public:
	CMiniMapInfo();
	virtual ~CMiniMapInfo(void);

public:
	void CreateSubControl();

public:
	void SetMapPos( float fCurX, float fCurY, int nMiniX, int nMiniY );
	void UpdatePostion();

	CString & GetMapName() { return m_strMapName; }
	void SetMapName( const CString & strMapName ) { m_strMapName = strMapName; } 

public:
	// �������¸� �����ϰ� ����Ʈ�� ����� ����� �׸��� ����
	virtual HRESULT Render(LPDIRECT3DDEVICEQ pD3DDevice);

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	int m_nMiniPosX, m_nMiniPosY;	// �̴ϸ� ��ǥ.

private:
	// Ŭ�� ��ũ ǥ�ø� ���� ����
	DWORD	m_dwVer;
	int		m_nID;
	int		m_nServer;
	CString	m_strClubName;

private:
	CString			m_strMapName;	// �� �̸�

	CBasicTextBox*	m_pChannelTextBox;
	CBasicTextBox*	m_pMinimapTextBox;
	CUIControl*		m_pClubMark;
};