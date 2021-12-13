///////////////////////////////////////////////////////////////////////
//	UIGroup 최저 기반 클래스
//
//	최초 작성자 : 서보건
//	이후 수정자 : 성기엽
//	로그
//		[2003.8.6] - 리팩토링
//					 Access 권한 변경
//
///////////////////////////////////////////////////////////////////////
#pragma	once

#include "UIControl.h"
#include "UIControlContainer.h"
#include "UIMessageQueue.h"

class CUIGroup : public CUIControl
{
public:
	CUIGroup ();
	virtual	~CUIGroup ();

protected:
	CUIControlContainer	m_ControlContainer; // 컨트롤을 등록, 관리한다.
	//CUIMessageQueue		m_ActionMsgQ;		// 메세지 큐

public:
	BOOL RegisterControl( CUIControl* pNewControl ); // 컨트롤을 그룹에 등록한다.
	CUIControl*	FindControl( UIGUID ControlID ); // 컨트롤을 찾는다.

	virtual BOOL IsExclusiveControl();
	virtual void ResetExclusiveControl();
	virtual void SetExclusiveControl();

protected:
	BOOL m_bClickedFoldControl;
	INT m_nCurFoldGroupID;
	BOOL m_bAllExclusiveControl;

	VOID InitFoldGroup();
	BOOL IsClickedFoldControl( CUIControl * pControl );
	VOID SetClickedFoldControl( CUIControl * pControl );
	VOID SetFoldGroup( CUIControl * pControl, INT nGroupID );
	
public:
	virtual	void AlignSubControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );
	virtual void ReSizeControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) {}
	virtual void ResetMessageEx();
	virtual void RemoveAll() { m_ControlContainer.RemoveAll(); } // MEMO : 그룹에 속한 모든 컨트롤을 삭제한다.
	virtual bool DeleteControl ( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	// 상속된 펑션 - 재정의
	virtual	void SetDiffuse(D3DCOLOR _diffuse);
	
	virtual void SetGlobalPos(const UIRECT& Pos);
	virtual void SetGlobalPos(const D3DXVECTOR2& vPos);

	virtual void SetTransparentOption ( BOOL bTransparency );

	virtual	BOOL SetFadeIn ();
	virtual	BOOL SetFadeOut();

public:
	// 기본 펑션
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

//protected:
//	// 그룹안에 있는 컨트롤에 대한 보임/숨김 메세지를 큐에서 가져온다음 전송한다.
//	void UpdateActionMsgQ();
//	// 그룹안에 있는 컨트롤이 보임/숨김에 대한 메세지만 처리한다.
//	// 하지만 메세지 처리와 액션 처리가 따로 있는건 좀 문제가 있어보인다.
//	virtual	void TranslateActionMsgQ( UIGUID ControlID, DWORD dwMsg );
};
