#include "pch.h"
#include "CharacterWindow.h"
#include "GLGaeaClient.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CCharacterWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();	
	BOOL bPointUsable = (sCharData.m_wStatsPoint)?TRUE:FALSE;

//	스탯 컨트롤
	SetPointControl ( bPointUsable );

//	이름,클래스	
	SetName ( sCharData );

//	경험치
	SetEXP ( sCharData );

//	Stat	
	SetPow ( sCharData );
	SetDex ( sCharData );
	SetSpi ( sCharData );
	//SetInt ( sCharData );
	SetStr ( sCharData );
	SetSta ( sCharData );
	SetPoint ( sCharData );

//	Status
	SetLevel ( sCharData );
	SetDamage ( sCharData );
	SetDefense ( sCharData );
	SetReqPa ( sCharData );
	SetReqSa ( sCharData );	
	SetReqMa ( sCharData );	
	SetHP ( sCharData );
	SetMP ( sCharData );
	SetSP ( sCharData );
	SetHitRate ( sCharData );
	SetAvoidRate ( sCharData );
	SetPremium ();

//	저항력
	SetFireResi ( sCharData );
	SetColdResi ( sCharData );
	SetElectricResi ( sCharData );
	SetPoisonResi ( sCharData );
	SetSpiritResi ( sCharData );

//	사회 성향
	SetAcademy ( sCharData );
	SetActivityPoint ( sCharData );
	SetPKState ( sCharData );
	SetDepartment ( sCharData );
	SetBright ( sCharData );
	SetGrade ( sCharData );

	{	//	제 자리 찾아가기
		const UIRECT& rcPowPos = m_pPowDiaableButton->GetGlobalPos ();
		m_pPowButton->SetGlobalPos ( D3DXVECTOR2 ( rcPowPos.left, rcPowPos.top ) );

		const UIRECT& rcDexPos = m_pDexDiaableButton->GetGlobalPos ();
		m_pDexButton->SetGlobalPos ( D3DXVECTOR2 ( rcDexPos.left, rcDexPos.top ) );

		const UIRECT& rcSpiPos = m_pSpiDiaableButton->GetGlobalPos ();
		m_pSpiButton->SetGlobalPos ( D3DXVECTOR2 ( rcSpiPos.left, rcSpiPos.top ) );

		//const UIRECT& rcIntPos = m_pIntDiaableButton->GetGlobalPos ();
		//m_pIntButton->SetGlobalPos ( D3DXVECTOR2 ( rcIntPos.left, rcIntPos.top ) );

		const UIRECT& rcStaPos = m_pStaDiaableButton->GetGlobalPos ();
		m_pStaButton->SetGlobalPos ( D3DXVECTOR2 ( rcStaPos.left, rcStaPos.top ) );

		const UIRECT& rcStrPos = m_pStrDiaableButton->GetGlobalPos ();
		m_pStrButton->SetGlobalPos ( D3DXVECTOR2 ( rcStrPos.left, rcStrPos.top ) );
	}

	{
		m_pPowText->SetUseTextColor ( 0, FALSE );
		m_pPowTextStatic->SetUseTextColor ( 0, FALSE );

		m_pDexText->SetUseTextColor ( 0, FALSE );
		m_pDexTextStatic->SetUseTextColor ( 0, FALSE );

		m_pSpiText->SetUseTextColor ( 0, FALSE );
		m_pSpiTextStatic->SetUseTextColor ( 0, FALSE );

		//m_pIntText->SetUseTextColor ( 0, FALSE );
		//m_pIntTextStatic->SetUseTextColor ( 0, FALSE );

		m_pStrText->SetUseTextColor ( 0, FALSE );
		m_pStrTextStatic->SetUseTextColor ( 0, FALSE );

		m_pStaText->SetUseTextColor ( 0, FALSE );
		m_pStaTextStatic->SetUseTextColor ( 0, FALSE );
	}

	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCharacterWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();
		SetPhoneNumber( sCharData.m_szPhoneNumber );
	}
}