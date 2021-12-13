#pragma	once

#include "UIDataType.h"
#include "UIControlDefine.h"
#include "UIControlMessage.h"
#include "UIDebugSet.h"

const int VERTEX_SIZE = 4;
const int INDEX_SIZE = 6;
const int TEXTURE_NAME_SIZE = 64;

struct INTERFACE_CFG
{
	CString strTextureName;
	UIRECT	rcControlPos;
	UIRECT	rcTexturePos;

	WORD	dwAlignFlag;

	INTERFACE_CFG()
		: dwAlignFlag(0)
	{
	}
};

class CUIControl
{
#ifdef	__UI_DEGUG__
private:
	std::string m_strControlName;

public:
	void SetControlNameEx( const char* strControlName )	{ m_strControlName = strControlName; }
#endif	//	__UI_DEGUG__

public:
	void SetUseRender( BOOL bUseRender )	{ m_bUseRender = bUseRender; }
	BOOL IsUseRender()						{ return m_bUseRender; }
	BOOL IsNoRender()						{ return !m_bVisible || !m_bUseRender; }

public:
	enum
	{
		INVISIBLE_MODE	= 0,
		FADEIN_MODE		= 1,
		FADEOUT_MODE	= 2,
		VISIBLE_MODE	= 3
	};

public:
	CUIControl ();
	virtual ~CUIControl();

public:
	void Create ( UIGUID WndID,	const char* szControlKeyword, WORD wAlignFlag = UI_FLAG_DEFAULT );
	void CreateSub ( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );

	// Note : 아래 UI 생성 함수는 XML로 부터 정렬 플래그를 가져온다.
	//
	void CreateEx ( UIGUID WndID,	const char* szControlKeyword );
	void CreateSubEx ( CUIControl* pParent, const char* szControlKeyword, UIGUID WndID = NO_ID );

public:
	UIRECT AlignMainControl( const UIRECT& rcPos );
	virtual	void AlignSubControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );
	virtual void ReSizeControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );

public:
	void SetTexturePos( const UIRECT& rcTexPos );
	void SetTexturePos( int wIndex, const D3DXVECTOR2& vPos );
	void SetTexture( LPDIRECT3DTEXTUREQ pTexture )	{ m_pTexture = pTexture; }

	const UIRECT& GetTexturePos() const			{ return m_rcTexurePos; }

	void SetFadeTime( float fFadeLimitTime )	{ m_fFadeLimitTime = fFadeLimitTime; }
	float GetFadeElapsedTime()					{ return m_fFadeElapsedTime; }

public:
	void	SetBeginAlpha ( DWORD dwAlpha )		{ m_dwBeginAlpha = dwAlpha; }

public:
	void SetGlobalPos( int wIndex, const D3DXVECTOR2& vPos ) { m_UIVertex[wIndex] = vPos; }
	virtual void SetGlobalPos( const UIRECT& rcPos );
	virtual void SetGlobalPos( const D3DXVECTOR2& vPos );
	
	void SetLocalPos( const UIRECT& rcLocalPosNew ) { m_rcLocalPos = rcLocalPosNew;	}
	void SetLocalPos( const D3DXVECTOR2& vPos );

	const UIRECT& GetGlobalPos() const		{ return m_rcGlobalPos; }
	const UIRECT& GetLocalPos () const		{ return m_rcLocalPos; }

public:
	void SetProtectSizePoint ();
	void SetProtectSizePoint ( const D3DXVECTOR2 vSIZE );
	const D3DXVECTOR2& GetProtectSize() const { return m_vProtectSize; }

private:
	void CheckProtectSize ();
	void CheckBoundary ();

public:
	const D3DCOLOR&	GetDiffuse() const		{ return m_UIVertex[0].GetDiffuse(); }
	virtual void SetDiffuse(D3DCOLOR _diffuse);

public:
	virtual void SetTransparentOption ( BOOL bTransparency );
	BOOL GetTransparentOption()				{ return m_bTransparency; }

public:
	void SetTextureName( const CString& strTextureName );
	const CString& GetTextureName() const	{ return m_strTextureName; }

public:
	void SetParent( CUIControl* pParent )	{ m_pParent = pParent; }
	CUIControl* GetParent()					{ return m_pParent; }
	CUIControl*	GetTopParent();

public:
	HRESULT	LoadTexture ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UnLoadTexture ();

public:
	// 메세지 관련 함수
	DWORD GetMessageEx ()					{ return m_dwMsg; }
	void AddMessageEx ( DWORD dwMsg )		{ m_dwMsg |= dwMsg; }
	void SetMessageEx ( DWORD dwMsg )		{ m_dwMsg = dwMsg; }
	virtual void ResetMessageEx ()			{ m_dwMsg = 0; }

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	BOOL IsVisible()				{ return m_bVisible; }

	virtual	BOOL SetFadeIn ();
	virtual	BOOL SetFadeOut();
	virtual	BOOL SetFade ( BOOL bFadeIn );

	BOOL	IsFading();	
	float	GetVisibleRate()		{ return m_fVisibleRate; }

	virtual void Reset();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) {}

public:
	virtual HRESULT OneTimeSceneInit()											{ return S_OK; }
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects()									{ return S_OK; }
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects();							
	virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );			
	virtual HRESULT FrameMove(LPDIRECT3DDEVICEQ pd3dDevice,float fElapsedTime)	{ return S_OK; }
	virtual HRESULT FinalCleanup()												{ return S_OK; }

public:
	inline BOOL	MouseUpdate( int PosX, int PosY );

protected:
	void	FadeUpdate(float fElapsedTime);

private:
	void	SetVertexPos();
	UIRECT	GetAlignPos ( UIRECT Pos );

public:
	void	SetDiffuseAlpha ( WORD wAlpha );
	void	SetVertexPos( float z );

public:
	void SetAlignFlag( WORD wFlag )		{ m_wAlignFlag = wFlag; CHECK_ALIGN_FLAG( m_wAlignFlag ); }
	void AddAlignFlag( WORD wFlag )		{ m_wAlignFlag |= wFlag; CHECK_ALIGN_FLAG( m_wAlignFlag ); }
	void ResetAlignFlag()				{ m_wAlignFlag = UI_FLAG_DEFAULT;	}
	WORD GetAlignFlag()					{ return m_wAlignFlag; }

public:
	// 마우스 메세지를 받을 필요없는 컨트롤에 SetNoUpdate(true)를 세팅한다.
	void SetNoUpdate( BOOL bNoUpdate )	{ m_bNO_UPDATE = bNoUpdate; }
	BOOL IsNoUpdate()					{ return !m_bVisible || m_bNO_UPDATE; }

public:
	void SetWndID( UIGUID WndID )		{ m_WndID = WndID; }
	UIGUID GetWndID()					{ return m_WndID; }

protected:
	UIRECT	m_rcGlobalPos;	
	UIRECT	m_rcLocalPos;
	D3DXVECTOR2	m_vProtectSize;
	UIRECT	m_rcTexurePos;

	UIVERTEX	m_UIVertex[VERTEX_SIZE];

	LPDIRECT3DTEXTUREQ	m_pTexture;
	CString				m_strTextureName;

	float	m_fFadeLimitTime;
	float	m_fFadeElapsedTime;

	BOOL	m_bTransparency;	
	float	m_fVisibleRate;

	BOOL	m_bUseRender;
	BOOL	m_bCheckProtectSize;

	DWORD	m_dwMsg;		//	메시지
	BOOL	m_bNO_UPDATE;

private:
	UIGUID	m_WndID;		//	컨트롤 ID,Unique ID
	CUIControl*	m_pParent;
	WORD	m_wAlignFlag;
	BOOL	m_bVisible;
	int		m_VisibleState;
	DWORD	m_dwBeginAlpha;

protected:
	BOOL m_bExclusiveControl; // MEMO : 단독 컨트롤

public:
	virtual void SetExclusiveControl()				{ m_bExclusiveControl = TRUE; }
	virtual void ResetExclusiveControl()	{ m_bExclusiveControl = FALSE; }
	BOOL IsExclusiveSelfControl()			{ return m_bExclusiveControl; }
	virtual BOOL IsExclusiveControl()		{ return m_bExclusiveControl; }

protected:
	BOOL m_bFocusControl; // MEMO : 포커스 컨트롤

public:
	virtual void SetFocusControl()			{ m_bFocusControl = TRUE; }
	virtual void ResetFocusControl()		{ m_bFocusControl = FALSE; }
	virtual BOOL IsFocusControl()			{ return m_bFocusControl; }

protected:
	INT m_nFoldID; // MEMO : 중복된 컨트롤의 그룹, 0이면 그룹이 아니다.

public:
	VOID SetFoldID( INT nID )				{ GASSERT( nID > 0 ); m_nFoldID = nID; }
	INT GetFoldID()							{ return m_nFoldID; }
};
