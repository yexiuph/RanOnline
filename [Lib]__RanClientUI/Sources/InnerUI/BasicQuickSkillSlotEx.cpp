#include "pch.h"
#include "BasicQuickSkillSlotEx.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GLGaeaClient.h"
#include "UITextControl.h"
#include "SkillTrayTab.h"
#include "InnerInterface.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CBasicQuickSkillSlotEx::nNOT_INIT = -1;

CBasicQuickSkillSlotEx::CBasicQuickSkillSlotEx ()
	: m_nNumber ( nNOT_INIT )
	, m_pNumberText ( NULL )
	, m_nTabIndex(0)
{
}

CBasicQuickSkillSlotEx::~CBasicQuickSkillSlotEx ()
{
}

void CBasicQuickSkillSlotEx::CreateSubControl ( INT nTabIndex )
{
	GASSERT( 0 <= nTabIndex && nTabIndex < MAX_TAB_INDEX );
	m_nTabIndex = nTabIndex;

	CBasicQuickSkillSlot::CreateSubControl ();
	if ( m_pSkillMouseOver ) m_pSkillMouseOver->SetVisibleSingle ( FALSE );
}

void CBasicQuickSkillSlotEx::CreateNumberText ( CD3DFontPar* pFont8, int nNumber )
{
	m_nNumber = nNumber;

	int nDisplayNumber = nNumber + 1;
	if ( nDisplayNumber == QUICK_SKILL_SLOT_MAX )
	{
		nDisplayNumber = 0;
	}

	CString strNumber;	
	strNumber = CInnerInterface::GetInstance().GetdwKeyToString(RANPARAM::SkillSlot[nNumber]);

	CBasicTextBox* pNumberText = new CBasicTextBox;
	pNumberText->CreateSub ( this, "QUICK_SKILL_SLOT_NUMBER" );
    pNumberText->SetFont ( pFont8 );
	pNumberText->SetTextAlign ( TEXT_ALIGN_LEFT );
	pNumberText->SetOneLineText ( strNumber, NS_UITEXTCOLOR::WHITE );
	pNumberText->SetControlNameEx ( "스킬 넘버 텍스트박스" );
	RegisterControl ( pNumberText );
	m_pNumberText = pNumberText;
}

void CBasicQuickSkillSlotEx::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//	바로 상위 ( CBasicQuickSkillSlot::Update() )를 부르지 않은 것은 의도된 것입니다.
	//	상위의 Update는 다른 용도로 사용되기 때문에, 호출할경우
	//	다른 의미로 동작하기 때문에, 건너뛰고 그 상위 클래스를 호출합니다.
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    int nNumber = m_nNumber;
	GASSERT ( 0 <= nNumber && nNumber < EMSKILLQUICK_SIZE && "범위를 벗어납니다." );

	INT nG_Index = m_nTabIndex * QUICK_SKILL_SLOT_MAX;
	nG_Index += nNumber;

	SNATIVEID sNativeID = GLGaeaClient::GetInstance().GetCharacter()->m_sSKILLQUICK[nG_Index];
	UpdateSlot ( sNativeID );

	BOOL bSlotEmpty = (sNativeID == NATIVEID_NULL ());
	DWORD dwMsg = GetMessageEx ();
	if ( m_pSkillMouseOver)
	{
		if ( !bSlotEmpty && CHECK_MOUSE_IN ( dwMsg ) && bFirstControl )
		{
			m_pSkillMouseOver->SetVisibleSingle ( TRUE );
		}
		else
		{
			m_pSkillMouseOver->SetVisibleSingle ( FALSE );
		}
	}
}

void CBasicQuickSkillSlotEx::SetShotcutText( CString strTemp )
{
	m_pNumberText->SetText( strTemp, NS_UITEXTCOLOR::WHITE );
}