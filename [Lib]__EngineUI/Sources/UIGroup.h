///////////////////////////////////////////////////////////////////////
//	UIGroup ���� ��� Ŭ����
//
//	���� �ۼ��� : ������
//	���� ������ : ���⿱
//	�α�
//		[2003.8.6] - �����丵
//					 Access ���� ����
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
	CUIControlContainer	m_ControlContainer; // ��Ʈ���� ���, �����Ѵ�.
	//CUIMessageQueue		m_ActionMsgQ;		// �޼��� ť

public:
	BOOL RegisterControl( CUIControl* pNewControl ); // ��Ʈ���� �׷쿡 ����Ѵ�.
	CUIControl*	FindControl( UIGUID ControlID ); // ��Ʈ���� ã�´�.

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
	virtual void RemoveAll() { m_ControlContainer.RemoveAll(); } // MEMO : �׷쿡 ���� ��� ��Ʈ���� �����Ѵ�.
	virtual bool DeleteControl ( UIGUID ControlID, int refactoring ); // ��Ʈ���� �����Ѵ�.

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	// ��ӵ� ��� - ������
	virtual	void SetDiffuse(D3DCOLOR _diffuse);
	
	virtual void SetGlobalPos(const UIRECT& Pos);
	virtual void SetGlobalPos(const D3DXVECTOR2& vPos);

	virtual void SetTransparentOption ( BOOL bTransparency );

	virtual	BOOL SetFadeIn ();
	virtual	BOOL SetFadeOut();

public:
	// �⺻ ���
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

//protected:
//	// �׷�ȿ� �ִ� ��Ʈ�ѿ� ���� ����/���� �޼����� ť���� �����´��� �����Ѵ�.
//	void UpdateActionMsgQ();
//	// �׷�ȿ� �ִ� ��Ʈ���� ����/���迡 ���� �޼����� ó���Ѵ�.
//	// ������ �޼��� ó���� �׼� ó���� ���� �ִ°� �� ������ �־�δ�.
//	virtual	void TranslateActionMsgQ( UIGUID ControlID, DWORD dwMsg );
};
