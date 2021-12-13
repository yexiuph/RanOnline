// RequirePage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "RequirePage.h"
#include "SheetWithTab.h"
#include "DlgQuest.h"

#include "EtcFunction.h"

#include "GLDefine.h"

#include "GLGaeaServer.h"
#include "GLLevelFile.h"
#include "GLItem.h"
#include "GLItemMan.h"
#include "GLSkill.h"
#include "GLQuest.h"
#include "GLQuestMan.h"
#include ".\requirepage.h"

// CRequirePage dialog

IMPLEMENT_DYNAMIC(CRequirePage, CPropertyPage)
CRequirePage::CRequirePage()
	: CPropertyPage(CRequirePage::IDD)
	, m_bInit (FALSE)
	, m_pSheetTab (NULL)
{
}

CRequirePage::~CRequirePage()
{
}

void CRequirePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRequirePage, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_PENET_LEVEL_SIGN, OnCbnSelchangeComboPenetLevelSign)
	ON_BN_CLICKED(IDC_BUTTON_PENET_ITEM, OnBnClickedButtonPenetItem)
	ON_BN_CLICKED(IDC_BUTTON_PENET_ITEM_DEL, OnBnClickedButtonPenetItemDel)
	ON_BN_CLICKED(IDC_BUTTON_PENET_SKILL, OnBnClickedButtonPenetSkill)
	ON_BN_CLICKED(IDC_BUTTON_PENET_SKILL_DEL, OnBnClickedButtonPenetSkillDel)
	ON_BN_CLICKED(IDC_BUTTON_PENET_COMP_QUEST, OnBnClickedButtonPenetCompQuest)
	ON_BN_CLICKED(IDC_BUTTON_PENET_COMP_QUEST_DEL, OnBnClickedButtonPenetCompQuestDel)
	ON_BN_CLICKED(IDC_BUTTON_PENET_PROG_QUEST, OnBnClickedButtonPenetProgQuest)
	ON_BN_CLICKED(IDC_BUTTON_PENET_PROG_QUEST_DEL, OnBnClickedButtonPenetProgQuestDel)
	ON_BN_CLICKED(IDC_BUTTON_PENET_SAVE, OnBnClickedButtonPenetSave)
END_MESSAGE_MAP()


// CRequirePage message handlers
BOOL CRequirePage::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWin_Combo_Init ( this, IDC_COMBO_PENET_LEVEL_SIGN, COMMENT::CDT_SIGN, 7 );
	SetWin_Combo_Init ( this, IDC_COMBO_PENET_LP_SIGN, COMMENT::CDT_SIGN, 7 );
	SetWin_Combo_Init ( this, IDC_COMBO_PENET_ATTR_SIGN, COMMENT::CDT_SIGN, 7 );

	m_bInit = TRUE;

	UpdatePage();

	return TRUE;
}

void CRequirePage::OnShowWindow ( BOOL bShow, UINT nStatus )
{
	if ( bShow )
		UpdatePage();
}

void CRequirePage::SetReqItem ( SNATIVEID NativeID )
{
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	pGLLandMan->GetLevelRequire()->m_sItemID = NativeID;
}

void CRequirePage::SetReqSkill ( SNATIVEID NativeID )
{
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	pGLLandMan->GetLevelRequire()->m_sSkillID = NativeID;
}

void CRequirePage::UpdatePage()
{
	if ( m_bInit )
	{
		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

		SetWin_Check ( this, IDC_CHECK_PENET_PARTY, pREQUIRE->m_bPartyMbr );

		SetWin_Num_int ( this, IDC_EDIT_PENET_LEVEL_CON1, pREQUIRE->m_wLevel );
		SetWin_Num_int ( this, IDC_EDIT_PENET_LEVEL_CON2, pREQUIRE->m_wLevel2 );

		std::string strLevelSign = COMMENT::CDT_SIGN[pREQUIRE->m_signLevel];
		SetWin_Combo_Sel ( this,IDC_COMBO_PENET_LEVEL_SIGN , strLevelSign );

		if ( pREQUIRE->m_signLevel == 6 )
			SetWin_Enable ( this, IDC_EDIT_PENET_LEVEL_CON2, TRUE );
		else
			SetWin_Enable ( this, IDC_EDIT_PENET_LEVEL_CON2, FALSE );

		SetWin_Num_int ( this, IDC_EDIT_PENET_ITEM_MID, pREQUIRE->m_sItemID.wMainID );
		SetWin_Num_int ( this, IDC_EDIT_PENET_ITEM_SID, pREQUIRE->m_sItemID.wSubID );

		SITEM *pItem = GLItemMan::GetInstance().GetItem ( pREQUIRE->m_sItemID );
		CString strItemName("");

		if ( pItem )
		{
			strItemName = pItem->GetName();
		}
		else
		{
			strItemName = "None";
		}

		SetWin_Text ( this, IDC_EDIT_PENET_ITEM_NAME, strItemName.GetString() );

		SetWin_Num_int ( this, IDC_EDIT_PENET_SKILL_MID, pREQUIRE->m_sSkillID.wMainID );
		SetWin_Num_int ( this, IDC_EDIT_PENET_SKILL_SID, pREQUIRE->m_sSkillID.wSubID );

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pREQUIRE->m_sSkillID );
		CString strSkillName("");

		if ( pSkill )
		{
			strSkillName = pSkill->GetName();
		}
		else
		{
			strSkillName = "None";
		}

		SetWin_Text ( this, IDC_EDIT_PENET_SKILL_NAME, strSkillName.GetString() );

		GLQUEST* pQuestCom = GLQuestMan::GetInstance().Find ( pREQUIRE->m_sComQuestID.dwID );
		CString strQuest("");

		if ( pQuestCom )
		{
			strQuest.Format ( "[%d] %s", pREQUIRE->m_sComQuestID.dwID, pQuestCom->GetTITLE() );
		}
		else
		{
			strQuest = "None";
		}

		SetWin_Text ( this, IDC_EDIT_PENET_COMP_QUEST_NAME, strQuest.GetString() );

		GLQUEST* pQuestAct = GLQuestMan::GetInstance().Find ( pREQUIRE->m_sActQuestID.dwID );

		if ( pQuestAct )
		{
			strQuest.Format ( "[%d] %s", pREQUIRE->m_sActQuestID.dwID, pQuestAct->GetTITLE() );
		}
		else
		{
			strQuest = "None";
		}

		SetWin_Text ( this, IDC_EDIT_PENET_PROG_QUEST_NAME, strQuest.GetString() );
		
		SetWin_Num_int ( this, IDC_EDIT_PENET_LP_VAL, pREQUIRE->m_nLiving );

		std::string strLivingSign = COMMENT::CDT_SIGN[pREQUIRE->m_signLiving];
		SetWin_Combo_Sel ( this,IDC_COMBO_PENET_LP_SIGN , strLivingSign );

		SetWin_Num_int ( this, IDC_EDIT_PENET_ATTR_VAL, pREQUIRE->m_nBright );

		std::string strBrightSign = COMMENT::CDT_SIGN[pREQUIRE->m_signBright];
		SetWin_Combo_Sel ( this,IDC_COMBO_PENET_ATTR_SIGN , strBrightSign );
	}
}

void CRequirePage::InversePage()
{
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	pREQUIRE->m_bPartyMbr = GetWin_Check ( this, IDC_CHECK_PENET_PARTY ) != FALSE;

	pREQUIRE->m_wLevel = (WORD)GetWin_Num_int ( this, IDC_EDIT_PENET_LEVEL_CON1 );
	pREQUIRE->m_signLevel = (EMCDT_SIGN)GetWin_Combo_Sel ( this, IDC_COMBO_PENET_LEVEL_SIGN );
	if ( pREQUIRE->m_signLevel == 6 )
		pREQUIRE->m_wLevel2 = (WORD)GetWin_Num_int ( this, IDC_EDIT_PENET_LEVEL_CON2 );
	
	pREQUIRE->m_sItemID.wMainID = (WORD)GetWin_Num_int ( this, IDC_EDIT_PENET_ITEM_MID );
	pREQUIRE->m_sItemID.wSubID = (WORD)GetWin_Num_int ( this, IDC_EDIT_PENET_ITEM_SID );

	pREQUIRE->m_sSkillID.wMainID = (WORD)GetWin_Num_int ( this, IDC_EDIT_PENET_SKILL_MID );
	pREQUIRE->m_sSkillID.wSubID = (WORD)GetWin_Num_int ( this, IDC_EDIT_PENET_SKILL_SID );

	pREQUIRE->m_nLiving = GetWin_Num_int ( this, IDC_EDIT_PENET_LP_VAL );
	pREQUIRE->m_signLiving = (EMCDT_SIGN)GetWin_Combo_Sel ( this, IDC_COMBO_PENET_LP_SIGN );

	pREQUIRE->m_nBright = GetWin_Num_int ( this, IDC_EDIT_PENET_ATTR_VAL );
	pREQUIRE->m_signBright = (EMCDT_SIGN)GetWin_Combo_Sel ( this, IDC_COMBO_PENET_ATTR_SIGN );
}

void CRequirePage::OnCbnSelchangeComboPenetLevelSign()
{
	// TODO: Add your control notification handler code here
	if ( GetWin_Combo_Sel ( this, IDC_COMBO_PENET_LEVEL_SIGN ) == 6 )
		SetWin_Enable ( this, IDC_EDIT_PENET_LEVEL_CON2, TRUE );
	else
		SetWin_Enable ( this, IDC_EDIT_PENET_LEVEL_CON2, FALSE );
}

void CRequirePage::OnBnClickedButtonPenetItem()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	m_pSheetTab->ActiveItemTreePage ( (int)REQUIREPAGE, pREQUIRE->m_sItemID );
}

void CRequirePage::OnBnClickedButtonPenetItemDel()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	SNATIVEID sID( false );
	pREQUIRE->m_sItemID = sID;

	UpdatePage();
}

void CRequirePage::OnBnClickedButtonPenetSkill()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	m_pSheetTab->ActiveSkillTreePage ( (int)REQUIREPAGE, pREQUIRE->m_sSkillID );
}

void CRequirePage::OnBnClickedButtonPenetSkillDel()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	SNATIVEID sID( false );
	pREQUIRE->m_sSkillID = sID;

	UpdatePage();
}

void CRequirePage::OnBnClickedButtonPenetCompQuest()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	CDlgQuest dlgQuest;

	if ( dlgQuest.DoModal() == IDOK )
	{
		pREQUIRE->m_sComQuestID.dwID = dlgQuest.m_dwQuestID;
	}

	UpdatePage();
}

void CRequirePage::OnBnClickedButtonPenetCompQuestDel()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	SNATIVEID sID( false );
	pREQUIRE->m_sComQuestID.dwID = sID.dwID;

	UpdatePage();
}

void CRequirePage::OnBnClickedButtonPenetProgQuest()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	CDlgQuest dlgQuest;

	if ( dlgQuest.DoModal() == IDOK )
	{
		pREQUIRE->m_sActQuestID.dwID = dlgQuest.m_dwQuestID;
	}

	UpdatePage();
}

void CRequirePage::OnBnClickedButtonPenetProgQuestDel()
{
	// TODO: Add your control notification handler code here
	InversePage();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = pGLLandMan->GetLevelRequire();

	SNATIVEID sID( false );
	pREQUIRE->m_sActQuestID.dwID = sID.dwID;

	UpdatePage();
}

void CRequirePage::OnBnClickedButtonPenetSave()
{
	// TODO: Add your control notification handler code here
	InversePage();
	m_pSheetTab->SetActivePage ( (int)REQUIREPAGE );
}
