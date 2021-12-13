#include "pch.h"

#include "./PetSkillSlot.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "PetSkillImage.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPetSkillSlot::CPetSkillSlot () :
	m_pNotLearnImage ( NULL ),
	m_pSkillImage ( NULL ),
	m_pTextBox ( NULL ),
	m_sNativeID ( NATIVEID_NULL () ),
	m_bLearnSkill ( FALSE )
{
}

CPetSkillSlot::~CPetSkillSlot ()
{
}

void CPetSkillSlot::CreateSubControl ( SNATIVEID sNativeID )
{
	D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	int	nTextAlign = TEXT_ALIGN_LEFT;

	CreateControl ( "SKILL_SLOT_BACK_SKILL_IMAGE" );
    CreateControl ( "SKILL_SLOT_BACK_SKILL_TEXT_L" );	
	CreateControl ( "SKILL_SLOT_BACK_SKILL_TEXT_M" );
	CreateControl ( "SKILL_SLOT_BACK_SKILL_TEXT_R" );

	if ( sNativeID == NATIVEID_NULL () )
	{
//		GASSERT ( 0 && "무효화한 ID 입니다. 확인하십시오." );
	}
	else
	{
		CPetSkillImage* pSkillImage = CreateSkillImage ();
		pSkillImage->SetSkill ( sNativeID );
		m_pSkillImage = pSkillImage;		

		m_pNotLearnImage = CreateControl ( "SKILL_SLOT_IMAGE_NOTLEARN" );

		m_pTextBox = CreateStaticControl ( "SKILL_SLOT_TEXT", pFont, dwColor, nTextAlign );			
		m_pTextBox->SetLineInterval ( 4.0f );
	}

	m_sNativeID = sNativeID;
}

CPetSkillImage* CPetSkillSlot::CreateSkillImage ()
{
	CPetSkillImage* pSkillImage = new CPetSkillImage;
	pSkillImage->CreateSub ( this, "SKILL_SLOT_IMAGE", UI_FLAG_DEFAULT, SKILL_SLOT_IMAGE );
	pSkillImage->CreateSubControl ();
	pSkillImage->SetUseRender ( TRUE );
    RegisterControl ( pSkillImage );

	return pSkillImage;
}

CUIControl*	CPetSkillSlot::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

CBasicTextBox* CPetSkillSlot::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CPetSkillSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_sNativeID == NATIVEID_NULL () )
	{
//		GASSERT ( 0 && "무효화한 ID 입니다. 확인하십시오." );
		return ;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sNativeID.wMainID, m_sNativeID.wSubID );
	if ( !pSkill )
	{
		GASSERT ( 0 && "찾을 수 없는 스킬입니다." );
		return ;
	}

	m_bLearnSkill = FALSE;
	if ( m_pTextBox ) 
	{
		BOOL bLearned = GLGaeaClient::GetInstance().GetPetClient()->ISLEARNED_SKILL( m_sNativeID );

		if ( bLearned )	//	배운 스킬이 있을 때
		{
			m_bLearnSkill = TRUE;

			m_strLine1.Format ( "[%s]%s.%d", pSkill->GetName(), (char*)ID2GAMEWORD ( "SKILL_SLOT", 0 ), pSkill->m_sBASIC.dwGRADE );
			if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) m_strLine1 += (char*)ID2GAMEWORD ( "SKILL_SLOT", 1 );

			m_pTextBox->ClearText ();
			m_pTextBox->AddTextNoSplit ( m_strLine1, NS_UITEXTUTIL::ENABLE );
		}		
		else	//	배운 스킬이 없을 경우
		{
			m_pTextBox->ClearText ();

			m_strLine1.Format ( "[%s]%s.%d", pSkill->GetName(), (char*)ID2GAMEWORD ( "SKILL_SLOT", 0 ), pSkill->m_sBASIC.dwGRADE );
			if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) m_strLine1 += (char*)ID2GAMEWORD ( "SKILL_SLOT", 1 );
			m_pTextBox->AddTextNoSplit ( m_strLine1, NS_UITEXTUTIL::ENABLE );
		}
	}

	//	배운 스킬
	if ( m_pNotLearnImage ) m_pNotLearnImage->SetVisibleSingle ( !m_bLearnSkill );
	
	if ( m_bLearnSkill )
	{
		if ( m_pSkillImage ) m_pSkillImage->SetSkillProcess ( m_sNativeID );
	}
}