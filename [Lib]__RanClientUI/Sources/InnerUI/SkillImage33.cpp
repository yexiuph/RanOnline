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

	//	���� �����Ӱ� ICONID��
	//	�ؽ��� �̸��� ���ٴ� ����
	//	�Ʒ��� �۾��� �� �̻� ������ �ʿ䰡 ���ٴ� ���� �ǹ��Ѵ�.
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