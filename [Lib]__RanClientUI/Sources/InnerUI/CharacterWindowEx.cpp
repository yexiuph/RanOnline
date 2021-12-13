#include "pch.h"
#include "CharacterWindow.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "GLGaeaClient.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "GLogicData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	레벨
void CCharacterWindow::SetLevel ( const GLCHARLOGIC& sCharData )
{
	const WORD& wLevel = sCharData.m_wLevel;

    CString strCombine;
	strCombine.Format ( "%d", wLevel );
	if ( m_pLevelText )
	{
		m_pLevelText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::GOLD );
	}
}

void CCharacterWindow::SetName ( const GLCHARLOGIC& sCharData )
{
	if ( m_pNameText )
	{
		m_pNameText->SetOneLineText ( sCharData.m_szName, NS_UITEXTCOLOR::ORANGE );
	}
}

//	경험치
void CCharacterWindow::SetEXP( const GLCHARLOGIC& sCharData )
{
	const LONGLONG& lNOW = sCharData.m_sExperience.lnNow;
	const LONGLONG& lMAX = sCharData.m_sExperience.lnMax;

	int nIndex = 0;
	
	if ( m_pEXPText ) m_pEXPText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%I64d", lNOW );
	if ( m_pEXPText ) nIndex = m_pEXPText->AddTextNoSplit ( strCombine, NS_UITEXTCOLOR::WHITE );

	strCombine.Format ( " / " );
	if ( m_pEXPText ) m_pEXPText->AddString ( nIndex, strCombine, NS_UITEXTCOLOR::LIGHTGREY );

	strCombine.Format ( "%I64d", lMAX );
	if ( m_pEXPText ) m_pEXPText->AddString ( nIndex, strCombine, NS_UITEXTCOLOR::WHITE );

	const float fPercent = float(lNOW) / float(lMAX);
	if ( m_pEXP ) m_pEXP->SetPercent ( fPercent );
}

//	Stat	
void CCharacterWindow::SetPow ( const GLCHARLOGIC& sCharData )
{
	m_pPowText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS.wPow );
	int nIndex = m_pPowText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wPow ) return ;
	
	strCombine.Format ( "(+%d)", sADD.wPow );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wPow ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pPowText->AddString ( nIndex, strCombine, dwColor );	
}

void CCharacterWindow::SetDex ( const GLCHARLOGIC& sCharData )
{
	m_pDexText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS.wDex );
	int nIndex = m_pDexText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wDex ) return ;

	strCombine.Format ( "(+%d)", sADD.wDex );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wDex ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pDexText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetSpi ( const GLCHARLOGIC& sCharData )
{
	m_pSpiText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS.wSpi );
	int nIndex = m_pSpiText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wSpi ) return ;

	strCombine.Format ( "(+%d)", sADD.wSpi );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wSpi ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pSpiText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetInt ( const GLCHARLOGIC& sCharData )
{
	//m_pIntText->ClearText ();

	//CString strCombine;
	//strCombine.Format ( "%d", sCharData.m_sSUMSTATS.wInt );
	//int nIndex = m_pIntText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	//SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	//SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	//if ( !sADD.wInt ) return ;

	//strCombine.Format ( "(+%d)", sADD.wInt );

	//D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	//if ( sITEM.wInt ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	//m_pIntText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetStr ( const GLCHARLOGIC& sCharData )
{
	m_pStrText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS.wStr );
	int nIndex = m_pStrText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wStr ) return ;

	strCombine.Format ( "(+%d)", sADD.wStr );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wStr ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pStrText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetSta ( const GLCHARLOGIC& sCharData )
{
	m_pStaText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS.wSta );
	int nIndex = m_pStaText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wSta ) return ;

	strCombine.Format ( "(+%d)", sADD.wSta );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wSta ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pStaText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetPointControl ( BOOL bPointUsable )
{
	//	Point
	if ( m_pPointDisable ) m_pPointDisable->SetVisibleSingle ( !bPointUsable );
	if ( m_pPointEnable ) m_pPointEnable->SetVisibleSingle ( bPointUsable );

	//	Stats Button
	if ( m_pPowButton ) m_pPowButton->SetVisibleSingle ( bPointUsable );
	if ( m_pDexButton ) m_pDexButton->SetVisibleSingle ( bPointUsable );
	if ( m_pSpiButton ) m_pSpiButton->SetVisibleSingle ( bPointUsable );

	if ( m_pStrButton ) m_pStrButton->SetVisibleSingle ( bPointUsable );
	if ( m_pStaButton ) m_pStaButton->SetVisibleSingle ( bPointUsable );

	if ( m_pPowDiaableButton ) m_pPowDiaableButton->SetVisibleSingle ( !bPointUsable );
	if ( m_pDexDiaableButton ) m_pDexDiaableButton->SetVisibleSingle ( !bPointUsable );
	if ( m_pSpiDiaableButton ) m_pSpiDiaableButton->SetVisibleSingle ( !bPointUsable );

	if ( m_pStrDiaableButton ) m_pStrDiaableButton->SetVisibleSingle ( !bPointUsable );
	if ( m_pStaDiaableButton ) m_pStaDiaableButton->SetVisibleSingle ( !bPointUsable );
}

void CCharacterWindow::SetPoint ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_wStatsPoint );
	if ( m_pPointText ) m_pPointText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::GOLD );
}

//	Status
void CCharacterWindow::SetDamage ( const GLCHARLOGIC& sCharData )
{	
	const int& nLow = sCharData.GETFORCE_LOW ();
	const int& nHigh = sCharData.GETFORCE_HIGH ();

	CString strCombine;
	strCombine.Format ( "%d~%d", nLow, nHigh );
	if ( m_pDamageText ) m_pDamageText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetDefense ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.GETDEFENSE () );
	if ( m_pDefenseText ) m_pDefenseText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetReqPa ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_wSUM_PA );
	if ( m_pReqPaText ) m_pReqPaText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetReqSa ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_wSUM_SA );
	if ( m_pReqSaText ) m_pReqSaText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetReqMa ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_wSUM_MA );
	if ( m_pReqMaText ) m_pReqMaText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetHitRate ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.GETHIT () );
	if ( m_pHitRateText ) m_pHitRateText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetHP ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d/%d", sCharData.GETHP (), sCharData.GETMAXHP () );
	if ( m_pHPText ) m_pHPText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetMP ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d/%d", sCharData.m_sMP.wNow, sCharData.m_sMP.wHigh );
	if ( m_pMPText ) m_pMPText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetSP ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d/%d", sCharData.m_sSP.wNow, sCharData.m_sSP.wHigh );
	if ( m_pSPText ) m_pSPText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetAvoidRate ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.GETAVOID () );
	if ( m_pAvoidRateText ) m_pAvoidRateText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

//	저항력
void CCharacterWindow::SetFireResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nFire );
	if ( m_pFireText ) m_pFireText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetColdResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nIce );
	if ( m_pColdText ) m_pColdText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetElectricResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nElectric );
	if ( m_pElectricText ) m_pElectricText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetPoisonResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nPoison );
	if ( m_pPoisonText ) m_pPoisonText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetSpiritResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nSpirit );
	if ( m_pSpiritText ) m_pSpiritText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

//	사회성향
//void CCharacterWindow::SetPK ( const GLCHARLOGIC& sCharData )
//{
//	CString strCombine;
//	strCombine.Format ( "%d", sCharData.m_wPK );
//	if ( m_pPKText ) m_pPKText->SetOneLineText ( strCombine );
//}

void CCharacterWindow::SetBright ( const GLCHARLOGIC& sCharData )
{
	const int nMIN = -100;
	const int nMAX = 100;
	const int nRANGE = nMAX - nMIN;

	int nPercent = sCharData.GETBRIGHTPER ();
	nPercent = -(nPercent);	//	이미지가 반대로 뒤집혀있어서.
	if ( nPercent < nMIN ) nPercent = nMIN;
	if ( nPercent > nMAX ) nPercent = nMAX;

	nPercent += nMAX;

	float fPercent = static_cast<float>(nPercent) / static_cast<float>(nRANGE);

	const UIRECT& rcFramePos = m_pBrightFrame->GetGlobalPos ();
	const UIRECT& rcThumbPos = m_pBrightSlider->GetGlobalPos ();

	float fMOVE = (rcFramePos.sizeX - rcThumbPos.sizeX) * fPercent;
	float fLeft = rcFramePos.left + fMOVE;

	m_pBrightSlider->SetGlobalPos ( D3DXVECTOR2 ( fLeft, rcThumbPos.top ) );
}

void CCharacterWindow::SetPKState ( const GLCHARLOGIC& sCharData )
{
	if ( m_pPKStateText )
	{		
		m_pPKStateText->SetOneLineText ( GLGaeaClient::GetInstance().GetCharacter()->GET_PK_NAME ().c_str(), NS_UITEXTCOLOR::WHITE );
	}
}

void CCharacterWindow::SetGrade ( const GLCHARLOGIC& sCharData )
{
	if ( m_pGradeText )
	{
//		m_pGradeText->SetOneLineText ( ID2GAMEWORD("ACADEMY_NAME",sCharData.m_wSchool) );
	}
}

void CCharacterWindow::SetActivityPoint ( const GLCHARLOGIC& sCharData )
{
	if ( m_pActivityPoint )
	{
		CString strCombine;
		strCombine.Format ( "%d", sCharData.m_nLiving );
		m_pActivityPoint->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
	}
}

void CCharacterWindow::SetAcademy ( const GLCHARLOGIC& sCharData )
{
	if ( m_pAcademy )
	{
		m_pAcademy->SetOneLineText ( ID2GAMEWORD("ACADEMY_NAME",sCharData.m_wSchool), NS_UITEXTCOLOR::WHITE );

		for ( int i = 0; i < nSCHOOL_MARK; ++i ) m_pSchoolMark[sCharData.m_wSchool]->SetVisibleSingle ( FALSE );
		m_pSchoolMark[sCharData.m_wSchool]->SetVisibleSingle ( TRUE );
	}
}

void CCharacterWindow::SetDepartment ( const GLCHARLOGIC& sCharData )
{	
	if ( m_pDepartment )
	{
		m_pDepartment->SetOneLineText ( COMMENT::CHARCLASS[CharClassToIndex(sCharData.m_emClass)].c_str(), NS_UITEXTCOLOR::WHITE );
	}
}

void CCharacterWindow::SetPremium ()
{
	if ( m_pPremiumText )
	{
		m_pPremiumTextStatic->SetVisibleSingle ( FALSE );

		bool bPREMIUM = GLGaeaClient::GetInstance().GetCharacter()->m_bPREMIUM;
		if ( bPREMIUM )
		{
			__time64_t tPREMIUM = GLGaeaClient::GetInstance().GetCharacter()->m_tPREMIUM;
			CTime cTime ( tPREMIUM );
			CString strExpireDate;
			strExpireDate.Format ( "%02d/%02d/%02d", cTime.GetYear()%2000, cTime.GetMonth(), cTime.GetDay () );
			m_pPremiumText->SetOneLineText ( strExpireDate, NS_UITEXTCOLOR::DARKORANGE );
			m_pPremiumTextStatic->SetVisibleSingle ( TRUE );
		}
	}
}

void CCharacterWindow::SetPhoneNumber( const CString & strPhoneNumber )
{
	if( m_pMobileText )
	{
		m_pMobileText->SetOneLineText( strPhoneNumber, NS_UITEXTCOLOR::SILVER );
	}
}