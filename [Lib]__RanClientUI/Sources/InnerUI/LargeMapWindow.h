#pragma	once

#include "UIWindowEx.h"
#include "GLParty.h"
#include "GLCrowRenList.h"

class	CBasicScrollBarEx;
class	GLMapAxisInfo;
struct	GLPARTY_CLIENT;
class	CBasicTextBox;

class	CLargeMapWindow : public CUIWindowEx
{
private:
	enum
	{
		LARGEMAP_SCROLLBAR_H = ET_CONTROL_NEXT,
		LARGEMAP_SCROLLBAR_V,
		LARGEMAP_IMAGE,
		LARGEMAP_RESIZE_THUMB,
		PLAYER_MARK,
		LARGEMAP_DIRECTION,
		LARGEMAP_RB_THUMB,
		LARGEMAP_LB_THUMB,
		BASE_CONTROLID,
		BASE_CONTROLID_END = BASE_CONTROLID + 1000,
	};

//private:
//	typedef	std::map<DWORD,CUIControl*>		PARTY_ONMAP_MAP;
//	typedef PARTY_ONMAP_MAP::iterator		PARTY_ONMAP_MAP_ITER;

	typedef std::vector<CUIControl*>		GUILD_POS_VECTOR;
	typedef	GUILD_POS_VECTOR::iterator		GUILD_POS_VECTOR_ITER;

public:
	CLargeMapWindow ();
	virtual	~CLargeMapWindow ();

public:
	void	Initialize();
	void	ConvIntDataFloatData();
	void	CreateSubControl();
	void	UPDATE_CHARDIR();

private:
	void	SetAutoMove( bool bAutoMove )	{ m_bAUTO_MOVE = bAutoMove; }
	void	TranslateMeg( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl );
	void	TranslateMegThumb( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void	SetVisibleSingle ( BOOL bVisible );

public:
	void	SetMapAxisInfo ( GLMapAxisInfo &sInfo, const CString& strMapName );
	void	OnLButtonUp();

	void	UPDATE_TARGET_POS( int iX, int iY );
	void	VISIBLE_TARGET_POS( BOOL bVisible = FALSE );

	void	SetDefaultPosition();

private:
	void	UPDATE_CHAR_POS ( CUIControl * pControl, const D3DXVECTOR3& vPlayerPos );
	void	UPDATE_CHAR_ROTATE ( const D3DXVECTOR3& vCharDir );

	void	UPDATE_MOUSE_POS ( const int nMouseX, const int nMouseY );

	void	UPDATE_PARTY();
	void	UPDATE_PARTY_DEL();
	void	UPDATE_PARTY_INS( GLPARTY_CLIENT * pSelfClient );

	void	UPDATE_GUILD();
	void	RESET_GUILD();
	bool	IS_GUILD_UPDATE();
	void	UPDATE_GUILD_INFO( bool bVisible );

	//void	AddPartyMemberOnMAP ( const DWORD& dwGaeaID, const D3DXVECTOR3& vPos, const int nPartyID );

private:
	GLPARTY_CLIENT*	FindSelfClient ();
	BOOL	IsSameMap ( GLPARTY_CLIENT *pSelf, GLPARTY_CLIENT *pMember );

private:
	//CUIControl*	MAKE_MARK ( const int nPartyID );

	void	CONVERT_WORLD2MAP ( const D3DXVECTOR3& vPos, int& nPosX, int& nPosY );
	void	CONVERT_MOUSE2MAP ( const int nMousePosX, const int nMousePosY, int& nPosX, int& nPosY );
	void	CONVERT_MAP2WORLD ( const int nPosX, const int nPosY, D3DXVECTOR3& vPos );
	void	DoCHARPOS_OnCENTER_InVIEW ( int nMapPosX, int nMapPosY );

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fPercentH_BACK;
	float				m_fPercentV_BACK;

	D3DXVECTOR2			m_vScrollGap;

	D3DXVECTOR2			m_vRenderSize;
	D3DXVECTOR2			m_vRenderPos;

private:
	int					m_nWORLD_START_X;
	int					m_nWORLD_START_Y;
	int					m_nWORLD_SIZE_X;
	int					m_nWORLD_SIZE_Y;

	int					m_nMAP_TEXSIZE_X;
	int					m_nMAP_TEXSIZE_Y;

	int					m_nVIEW_START_X;
	int					m_nVIEW_START_Y;
	int					m_nVIEW_SIZE_X;
	int					m_nVIEW_SIZE_Y;

	D3DXVECTOR2			m_vWORLD_START;
	D3DXVECTOR2			m_vWORLD_SIZE;
	D3DXVECTOR2			m_vMAP_TEXSIZE;

private:
	BOOL				m_bFirstGap;
	D3DXVECTOR2			m_vGap;

	int					m_PosX;
	int					m_PosY;

	D3DXVECTOR2			m_vMOUSE_BACK;

private:
	CBasicScrollBarEx*	m_pScrollBarH;
	CBasicScrollBarEx*	m_pScrollBarV;

	CUIControl*			m_pMap;	
	CUIControl*			m_pPlayerMark;

	CUIControl*			m_pMousePosBack;
	CBasicTextBox*		m_pMousePosText;

	CBasicButton*		m_pLBThumb;
	CBasicButton*		m_pRBThumb;

	CUIControl*			m_pMark[MAXPARTY];

private:
	//PARTY_ONMAP_MAP	m_mapPARTY;
	//UIGUID			m_CONTROL_NEWID;	
	GUILD_POS_VECTOR	m_vecGuild;
	bool				m_bAUTO_MOVE;

private:
	CUIControl*			m_pTargetMark;
	int					m_iTargetX;
	int					m_iTargetY;
};