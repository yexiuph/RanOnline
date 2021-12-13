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

	//	사각 박스 재조절위함
	SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );

    CUIControl TempControl;
	TempControl.Create ( 1, szBaseFrame );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	//	버튼에 테두리가 외부의 영향에 의해 사이즈가 재조절 되는 것을 막는다.
	//	처음에는 버튼의 크기만큼 사이즈를 늘이는 것이 맞지만,
	//	그 이후에는 그런 '스케일'이 필요가 없다.
	//	따라서, 플래그를 모두 기본으로 설정해버린다.
	ResetAlignFlagLine ();

	SetAlignFlag ( wAlignFlagBack );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CBasicLineBoxButton::CreateButtonImage ( char* szBoxButton, char* szBoxButtonFlip )
{
	//	여기서의 버튼은 단지 이미지로서의 역할만을 수행합니다.
	//	실질적으로 '버튼'으로서의 의미를 가지는것은
	//	이 클래스(CBasicLineBoxButton)의 인스턴스입니다.
	//	따라서, 마우스가 클릭되었는지에 대한 메시지는 외부에서 처리되도록 하기 때문에,
	//	특별히 컨트롤 ID를 부여하지 않습니다.
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szBoxButton );
	pButton->CreateFlip ( szBoxButtonFlip, CBasicButton::CLICK_FLIP );	
	RegisterControl ( pButton );	
}