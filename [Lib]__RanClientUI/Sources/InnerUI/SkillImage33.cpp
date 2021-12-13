#include "pch.h"
#include "SkillImage33.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "GLGaeaClient.h"
#include "SkillFunc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSkillImage33::CSkillImage33 ()
{
}

CSkillImage33::~CSkillImage33 ()
{
}

void CSkillImage33::SetSkill ( SNATIVEID sNativeID )
{
	if ( sNativeID == NATIVEID_NULL () )
	{
		return ;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID.wMainID, sNativeID.wSubID );
	if ( !pSkill )
	{
		return ;
	}

	SNATIVEID sICONINDEX = pSkill->m_sEXT_DATA.sICONINDEX;
	CString	  strTextureName = pSkill->m_sEXT_DATA.strICONFILE.c_str();	

	//	이전 프레임과 ICONID와
	//	텍스쳐 이름이 같다는 것은
	//	아래의 작업을 더 이상 진행할 필요가 없다는 것을 의미한다.
	if ( (sICONINDEX == m_sICONINDEX) &&
		(strTextureName == CUIControl::GetTextureName()) )
	{
		return ;
	}
	
	CUIControl::SetTexturePos ( NS_SKILL::GetIconTexurePosEx ( sICONINDEX ) );
	CUIControl::SetTextureName ( strTextureName.GetString () );
	m_sICONINDEX = sICONINDEX;

	if ( m_pd3dDevice ) CUIControl::InitDeviceObjects ( m_pd3dDevice );
}

void CSkillImage33::CreateProgressBar ()
{
	CBasicProgressBar* pSkillProcess = new CBasicProgressBar;
	pSkillProcess->CreateSub ( this, "SKILL_IMAGE_PROGRESS33" );
	pSkillProcess->CreateOverImage ( "SKILL_IMAGE_PROGRESS33_OVER" );
	pSkillProcess->SetType ( CBasicProgressBar::VERTICAL );
	pSkillProcess->SetOverImageUseRender ( TRUE );
	pSkillProcess->SetOverImageDiffuse ( NS_SKILL::RECHARGECOLOR );	
	RegisterControl ( pSkillProcess );
	m_pSkillProcess = pSkillProcess;
}