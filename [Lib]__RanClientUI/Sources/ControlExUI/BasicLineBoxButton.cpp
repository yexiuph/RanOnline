#include "pch.h"
#include "./BasicLineBoxButton.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicLineBoxButton::CBasicLineBoxButton ()
{
}

CBasicLineBoxButton::~CBasicLineBoxButton ()
{
}

void CBasicLineBoxButton::CreateBaseButton ( char* szBaseFrame, char* szBoxButton, char* szBoxButtonFlip )
{	
	CreateButtonImage ( szBoxButton, szBoxButtonFlip );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_BUTTON", "BASIC_LINE_BOX_RIGHT_BUTTON", "BASIC_LINE_BOX_TOP_BUTTON", "BASIC_LINE_BOX_BOTTOM_BUTTON" );

	WORD wAlignFlagBack = GetAlignFlag ();

	//	�簢 �ڽ� ����������
	SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );

    CUIControl TempControl;
	TempControl.Create ( 1, szBaseFrame );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	//	��ư�� �׵θ��� �ܺ��� ���⿡ ���� ����� ������ �Ǵ� ���� ���´�.
	//	ó������ ��ư�� ũ�⸸ŭ ����� ���̴� ���� ������,
	//	�� ���Ŀ��� �׷� '������'�� �ʿ䰡 ����.
	//	����, �÷��׸� ��� �⺻���� �����ع�����.
	ResetAlignFlagLine ();

	SetAlignFlag ( wAlignFlagBack );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CBasicLineBoxButton::CreateButtonImage ( char* szBoxButton, char* szBoxButtonFlip )
{
	//	���⼭�� ��ư�� ���� �̹����μ��� ���Ҹ��� �����մϴ�.
	//	���������� '��ư'���μ��� �ǹ̸� �����°���
	//	�� Ŭ����(CBasicLineBoxButton)�� �ν��Ͻ��Դϴ�.
	//	����, ���콺�� Ŭ���Ǿ������� ���� �޽����� �ܺο��� ó���ǵ��� �ϱ� ������,
	//	Ư���� ��Ʈ�� ID�� �ο����� �ʽ��ϴ�.
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szBoxButton );
	pButton->CreateFlip ( szBoxButtonFlip, CBasicButton::CLICK_FLIP );	
	RegisterControl ( pButton );	
}