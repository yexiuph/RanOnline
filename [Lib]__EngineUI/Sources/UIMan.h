#pragma	once

#include "UIControlContainer.h"
#include "UIFocusContainer.h"
#include "UIMessageQueue.h"
#include "PositionKeeper.h"

class CUIControl;

class CUIMan
{
	static CString m_strPath; // GUI 패스
	static long	m_lResolution; // MEMO : 해상도

	CPositionKeeper		m_PositionKeeper;
	CUIControlContainer	m_ControlContainer;
	//CUIMessageQueue	m_ActionMsgQ;
	CUIFocusContainer	m_BottomList;
	CUIFocusContainer	m_FocusList;
	CUIFocusContainer	m_TopList;

public:
	static BOOL m_bBIGFONT;

private:
	//BOOL m_bFirstControl;
	BOOL m_bMouseInControl;

	BOOL m_bExclusiveControl;			// MEMO : 독점 컨트롤의 존재 여부
	CUIControl * m_pExclusiveControl;	// MEMO : 독점 컨트롤의 컨트롤 포인터

	//BOOL m_bFocusControl;				// MEMO : 포커스 컨트롤의 존재 여부
	CUIControl * m_pFocusControl;		// MEMO : 포커스 컨트롤의 컨트롤 포인터

protected:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	CUIMan ();
	virtual	~CUIMan ();

public:
	static const CString & GetPath() 			{ return m_strPath; }
	static void SetPath( const char * szPath )	{ GASSERT( szPath ); m_strPath = szPath; }

	static void SetResolution( long lResolution )	{ m_lResolution = lResolution; }
	static long GetResolution()						{ return m_lResolution; }

public:
	CUIControl * GetFocusControl ();
	//CUIControl * GetFocusControl();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg ) {}

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();		
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
	virtual HRESULT FinalCleanup ();

public:
	void SetMouseInControl( BOOL bMouseInControl )	{ m_bMouseInControl = bMouseInControl; }
	BOOL IsMouseInControl()							{ return m_bMouseInControl; }

	BOOL IsExclusiveControl()						{ return m_bExclusiveControl; }
	void ResetExclusiveControl();

private:
	BOOL	UpdateList( CUIFocusContainer* pList, BOOL bFocusList, float fElapsedTime );
	HRESULT	RenderList( CUIFocusContainer* pList, LPDIRECT3DDEVICEQ pd3dDevice );
	
public:
	CUIControlContainer* GetControlContainer()	{ return &m_ControlContainer; }
	CUIFocusContainer* GetBottomList()			{ return &m_BottomList; }
	CUIFocusContainer* GetFocusList()			{ return &m_FocusList; }
	CUIFocusContainer* GetTopList()				{ return &m_TopList; }
	CPositionKeeper* GetPositionKeeper()		{ return &m_PositionKeeper; }

public:
	void ShowGroupBottom( UIGUID ControlID, bool bMakeMsg = false );
	void ShowGroupFocus( UIGUID ControlID, bool bMakeMsg = false  );
	void ShowGroupTop( UIGUID ControlID, bool bMakeMsg = false  );
	void HideGroup( UIGUID ControlID, bool bMakeMsg = false  );
	BOOL IsVisibleGroup( UIGUID ControlID );

public:
	BOOL RegisterControl ( CUIControl* pNewControl, bool bPosisionKeep = false );
	CUIControl*	FindControl ( UIGUID ControlID );
	BOOL DeleteControl ( UIGUID ControlID );

private:
	void DeleteAllControl ();

public:
    void MOVE_DEFAULT_POSITION ();

//public:
//	void PostUIMessage ( UIGUID cID, DWORD dwMsg );
//
//protected:
//	void UpdateActionMsgQ ();	
//	void TranslateActionMsgQ ( UIGUID ControlID, DWORD dwMsg );

private:
	void SetControlPosition();
	void MaintainControlPosition();
};