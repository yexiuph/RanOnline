#include "pch.h"
#include "BasicQuickSkillSlot.h"

#include "GLGaeaClient.h"
#include "SkillImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicQuickSkillSlot::CBasicQuickSkillSlot () :
	m_pSkillImage ( NULL ),
	m_pSkillMouseOver ( NULL ),
	m_pd3dDevice ( NULL )
{
}

CBasicQuickSkillSlot::~CBasicQuickSkillSlot ()
{
}

void  CBasicQuickSkillSlot::CreateSubControl ()
{
	CreateSkillImage ();
	CreateMouseOver ();
}

void CBasicQuickSkillSlot::CreateSkillImage ()
{
	CSkillImage* pSkillImage = new CSkillImage;
	pSkillImage->CreateSub ( this, "QUICK_SKILL_IMAGE" );
	pSkillImage->CreateSubControl ();
	pSkillImage->SetUseRender ( TRUE );
	pSkillImage->SetVisibleSingle ( FALSE );
    RegisterControl ( pSkillImage );
	m_pSkillImage = pSkillImage;
}

void CBasicQuickSkillSlot::SetUseSkillImageTwinkle( bool bUse )
{
	if( !m_pSkillImage)
	{
		GASSERT( 0 && "CBasicQuickSkillSlot::SetUseSkillImageTwinkle(), m_pSkillImage is NULL." );
		return ;
	}

	m_pSkillImage->SetUseTwinkle( bUse );
}

void	CBasicQuickSkillSlot::UpdateSlot ( SNATIVEID sNativeID )
{	
	if ( !m_pSkillImage )
	{
		GASSERT ( 0 && "스킬 이미지가 만들어지지 않았습니다."  );
		return ;
	}

	if ( sNativeID != NATIVEID_NULL() )
	{
		m_pSkillImage->SetSkill ( sNativeID );
		m_pSkillImage->SetVisibleSingle ( TRUE );
		m_pSkillImage->SetSkillProcess ( sNativeID );
	}
	else
	{
		m_pSkillImage->SetVisibleSingle ( FALSE );
		m_pSkillImage->ResetSkill ();
	}	
}

void CBasicQuickSkillSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SNATIVEID sNativeID = GLGaeaClient::GetInstance().GetCharacter()->GetskillRunSlot ();
    UpdateSlot ( sNativeID );
}

void CBasicQuickSkillSlot::CreateMouseOver ()
{
	CUIControl* pSkillMouseOver = new CUIControl;
	pSkillMouseOver->CreateSub ( this, "QUICK_SKILL_MOUSE_OVER", UI_FLAG_DEFAULT, QUICK_SKILL_MOUSE_OVER );
	pSkillMouseOver->SetVisibleSingle ( TRUE );
    RegisterControl ( pSkillMouseOver );
	m_pSkillMouseOver = pSkillMouseOver;
}

UIRECT  CBasicQuickSkillSlot::GetAbsPosSkillImage ()
{	
	if ( !m_pSkillImage )
	{
		GASSERT ( 0 && "널입니다." );
		return UIRECT ();
	}
	
	return m_pSkillImage->GetGlobalPos ();	
}