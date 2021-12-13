#include "pch.h"
#include "SkillWindowToTray.h"
#include "SkillImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD CSkillWindowToTray::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB ( 200, 255, 255, 255 );
const float CSkillWindowToTray::fDEFAULT_MOUSE_INTERPOLIATION = 20.0f;

CSkillWindowToTray::CSkillWindowToTray () :
	m_pSkillImage ( NULL ),
	m_sNativeID ( NATIVEID_NULL () ),
	m_bUseSnap ( FALSE )
{
}

CSkillWindowToTray::~CSkillWindowToTray ()
{
}

void CSkillWindowToTray::CreateSubControl ()
{
	CSkillImage* pSkillImage = new CSkillImage;
	pSkillImage->CreateSub ( this, "SKILL_WINDOW_TO_TARY_IMAGE" );
	pSkillImage->CreateSubControl ();
	pSkillImage->SetUseRender ( TRUE );
	pSkillImage->SetVisibleSingle ( FALSE );
	RegisterControl ( pSkillImage );
    m_pSkillImage = pSkillImage;

	ResetSkill ();
}

void CSkillWindowToTray::SetSkill ( SNATIVEID sNativeID )
{
	if ( sNativeID == NATIVEID_NULL () )
	{
		GASSERT ( 0 && "무효화한 ID가 설정되려고 합니다." );
		return ;
	}

	m_sNativeID = sNativeID;	
	m_pSkillImage->SetSkill ( m_sNativeID );
	m_pSkillImage->SetVisibleSingle ( TRUE );
	m_pSkillImage->SetDiffuse ( dwDEFAULT_TRANSPARENCY );
}

void  CSkillWindowToTray::ResetSkill ()
{
	m_pSkillImage->ResetSkill ();
	m_pSkillImage->SetVisibleSingle ( FALSE );
	m_sNativeID = NATIVEID_NULL ();
}

void CSkillWindowToTray::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	
	if ( !m_pSkillImage ) return ;

	if ( m_sNativeID != NATIVEID_NULL () )
	{		
		if ( !IsUseSnap () )
		{
			const D3DXVECTOR2 vImagePos ( x - fDEFAULT_MOUSE_INTERPOLIATION, y - fDEFAULT_MOUSE_INTERPOLIATION );
			SetGlobalPos ( vImagePos );
		}		
	}
}