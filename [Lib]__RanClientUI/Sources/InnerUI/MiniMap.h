#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CBasicTextBox;
class CBasicButton;
class GLMapAxisInfo;
class CBasicProgressBar;
class CMiniMapInfo;

const DWORD UIMSG_MOUSEIN_FULLSCREENBUTTON = UIMSG_USER1;

class CMiniMap : public CUIGroup
{
private:
static	const	int	nTIME_MINUTE;

protected:
	enum
	{
		MINIMAP_FULLSCREEN = NO_ID + 1,
	};

public:
	CMiniMap();
	virtual ~CMiniMap(void);

protected:
	void	RotateMap();
	void	SetMapProperty();						// �� ��ǥ �Ӽ� �� ����
	void	UpdateMapTexturePos();	

public:
	void	CreateSubControl ();

private:
	void	CreateMap( char *MapPos);
	void	CreateUserMark(char *keyvalue);
	void	CreateZoomButton(char* InKeyvalue, char *InFlipvalue, char *OutKeyvalue, char*OutFlipvalue);
	void	CreateTextBox(char *x_keyvalue, char *y_keyvalue);

protected:
	//	�� ���� ���� �Լ�
	void	SetMapData ( const char *szMiniMapTex,UIRECT tPos );
	void	SetMapSize ( int size_x, int size_y, int start_x, int start_y );

public:
	BOOL	LoadMapInfo(char *szMapInfo);
	void	SetMapAxisInfo ( GLMapAxisInfo &sInfo, const CString & strMapName );

	void	CalcMapPos ( int cur_x, int cur_y, int& MapX, int& MapY );

	void	SetMapInfo ( int size_x, int size_y, const char *szMapName, int start_x, int start_y,UIRECT tMapPos );

	void	UpdateClubTime( float fCLUBBATTLETime );
	void	StartClubTime();
	void	EndClubTime();

public:
	virtual HRESULT Render(LPDIRECT3DDEVICEQ pD3DDevice); // �������¸� �����ϰ� ����Ʈ�� ����� ����� �׸��� ����	

public:
	void UpdateTime ( float fElapsedTime );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );


private:
	CUIControl*	m_pMap;
	CUIControl* m_pUserMark;
    CBasicTextBox*	m_pXPos;
	CBasicTextBox*	m_pYPos;
	CBasicTextBox*	m_pMiniMapName;
	CBasicButton*	m_pZoomOut;
	CBasicButton*	m_pZoomIn;

	CUIControl*		m_pTime;
	CBasicTextBox*	m_pTimeText;
	CBasicTextBox*	m_pServerTimeText;
	CUIControl*		m_pDirection;
	
	CUIControl*		m_pArrowCountBack;
	CBasicTextBox*	m_pArrowCountText;

private:
	int			m_nMap_X, m_nMap_Y;					//	��ü �� ������
	int			m_nMapSize;							//	�ʻ�����
	int			m_nStart_X, m_nStart_Y;				//	�� ���� ��ǥ

	int			m_nMin_X, m_nMin_Y;					//	�������� �� ��ǥ


	UIRECT		m_rcTexture_Pos;					//	�ؽ��� ��ǥ

private:
	CBasicProgressBar* m_pPK_ATTACK;
	CBasicProgressBar* m_pPK_DEFENSE;

	CUIControl*		m_pFullScreenButton;
	CUIControl*		m_pFullScreenButtonDummy;
	CMiniMapInfo*	m_pMiniMapInfoDummy;
	CMiniMapInfo*	m_pMiniMapInfoLeftDummy;

	CMiniMapInfo * m_pMimiMapInfo; // �� ���� ��Ʈ�� ������

private:
	bool	m_bTEST;

private:
	WORD m_wAmmoCount;
	CBasicTextBox*	m_pAmmoText;
	CBasicTextBox*	m_pClubTimeText;

	CUIControl* m_pCenterPoint;
};