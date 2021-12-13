#include "pch.h"

#include "GLDefine.h"
#include "d3dfont.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "BasicLineBox.h"
#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"
#include "GLItemMan.h"
#include "DXInputString.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/UIEditBoxMan.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/Common/BaseString.h"

#include ".\shopitemsearchwindow.h"

#define MAX_ROLLOVER_TEXT 10


CString strLoadListFileName = "SearchItemList.txt";

CShopItemSearchWindow::CShopItemSearchWindow(void)
{
	m_pItemNameEdit        = NULL;
	m_pComboRollSearchOver = NULL;
	m_pComboBoxOpen[0]     = NULL;
	m_pComboBoxOpen[1]     = NULL;
	m_pComboBoxOpen[2]     = NULL;
	m_pComboBoxOpen[3]     = NULL;
	m_pComboRollOver[0]    = NULL;
	m_pComboRollOver[1]    = NULL;
	m_pComboRollOver[2]    = NULL;
	m_pComboRollOver[3]    = NULL;
	m_RollOverID		   = NO_ID;
	m_nSelectString        = 0;
	m_nBeforeStrLength     = 0;
	m_nPageNum		       = 0;
	m_searchTime		   = 0;
	m_bSearch			   = FALSE;
}

CShopItemSearchWindow::~CShopItemSearchWindow(void)
{
}

void CShopItemSearchWindow::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG/*D3DFONT_SHADOW | D3DFONT_ASCII*/ );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "ITEM_SHOP_SEARCH_LINEBOX1" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "ITEM_SHOP_SEARCH_LINEBOX2" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( "ITEM_SHOP_SEARCH_BGEDIT" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CBasicTextButton *pSearchButton1, *pSearchButton2;
		pSearchButton1 = CreateTextButton ( "ITEM_SHOP_SEARCH_BUTTON1", ITEMSHOPSEARCH_BUTTON1, const_cast<char*>(ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",0)) );
		pSearchButton2 = CreateTextButton ( "ITEM_SHOP_SEARCH_BUTTON2", ITEMSHOPSEARCH_BUTTON2, const_cast<char*>(ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",1)) );
		pSearchButton2->SetShortcutKey ( DIK_ESCAPE );
	}

	{
		CBasicTextBox*	  pTextStatic = NULL;
		pTextStatic = CreateStaticControl ( "ITEM_SHOP_SEARCH_STATIC1", pFont, TEXT_ALIGN_LEFT );
		pTextStatic->AddText ( ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",2), NS_UITEXTCOLOR::SILVER );

		pTextStatic = CreateStaticControl ( "ITEM_SHOP_SEARCH_STATIC2", pFont, TEXT_ALIGN_LEFT );
		pTextStatic->AddText ( ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",3), NS_UITEXTCOLOR::SILVER );

		pTextStatic = CreateStaticControl ( "ITEM_SHOP_SEARCH_STATIC3", pFont, TEXT_ALIGN_LEFT );
		pTextStatic->AddText ( ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",4), NS_UITEXTCOLOR::SILVER );

		pTextStatic = CreateStaticControl ( "ITEM_SHOP_SEARCH_STATIC4", pFont, TEXT_ALIGN_LEFT );
		pTextStatic->AddText ( ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",5), NS_UITEXTCOLOR::SILVER );

		pTextStatic = CreateStaticControl ( "ITEM_SHOP_SEARCH_STATIC5", pFont, TEXT_ALIGN_LEFT );
		pTextStatic->AddText ( ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",6), NS_UITEXTCOLOR::SILVER );

		pTextStatic = CreateStaticControl ( "ITEM_SHOP_SEARCH_STATIC6", pFont, TEXT_ALIGN_LEFT );
		pTextStatic->AddText ( ID2GAMEWORD("ITEM_SHOP_SEARCH_TEXT",7), NS_UITEXTCOLOR::SILVER );
	}

	{
		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan;
		pEditBoxMan->CreateSub ( this, "ITEM_SHOP_SEARCH_EDITMAN", UI_FLAG_DEFAULT, ITEMSHOPSEARCH_MAN );
		pEditBoxMan->CreateEditBox ( ITEMSHOPSEARCH_EDIT, "ITEM_SHOP_SEARCH_EDIT", "ITEM_SHOP_SEARCH_CARRAT", TRUE, UINT_MAX, pFont, 38 );
		RegisterControl ( pEditBoxMan );
		m_pItemNameEdit = pEditBoxMan;
	}

	{
		CBasicComboBox *pComboBox = new CBasicComboBox;
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, ITEMSHOPSEARCH_COMBO_OPEN1 );
		pComboBox->CreateBaseComboBox ( "ITEMSHOPSEARCH_COMBO_OPEN1" );			
		RegisterControl ( pComboBox );
		m_pComboBoxOpen[0] = pComboBox;
	}
	{
		CBasicComboBox *pComboBox = new CBasicComboBox;
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, ITEMSHOPSEARCH_COMBO_OPEN2 );
		pComboBox->CreateBaseComboBox ( "ITEMSHOPSEARCH_COMBO_OPEN2" );			
		RegisterControl ( pComboBox );
		m_pComboBoxOpen[1] = pComboBox;
	}
	{
		CBasicComboBox *pComboBox = new CBasicComboBox;
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, ITEMSHOPSEARCH_COMBO_OPEN3 );
		pComboBox->CreateBaseComboBox ( "ITEMSHOPSEARCH_COMBO_OPEN3" );			
		RegisterControl ( pComboBox );
		m_pComboBoxOpen[2] = pComboBox;
	}
	{
		CBasicComboBox *pComboBox = new CBasicComboBox;
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, ITEMSHOPSEARCH_COMBO_OPEN4 );
		pComboBox->CreateBaseComboBox ( "ITEMSHOPSEARCH_COMBO_OPEN4" );			
		RegisterControl ( pComboBox );
		m_pComboBoxOpen[3] = pComboBox;
	}


	{
		CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver;
		pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ITEMSHOPSEARCH_COMBO_ROLLOVER );
		pComboBoxRollOver->CreateBaseComboBoxRollOver ( "ITEMSHOPSEARCH_COMBO_ROLLOVER_SEARCH", FALSE );			
		pComboBoxRollOver->SetVisibleSingle ( FALSE );
		RegisterControl ( pComboBoxRollOver );
		m_pComboRollSearchOver = pComboBoxRollOver;
	}
	{
		CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver;
		pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ITEMSHOPSEARCH_COMBO_ROLLOVER1 );
		pComboBoxRollOver->CreateBaseComboBoxRollOver ( "ITEMSHOPSEARCH_COMBO_ROLLOVER1", FALSE );			
		pComboBoxRollOver->SetVisibleSingle ( FALSE );
		RegisterControl ( pComboBoxRollOver );
		m_pComboRollOver[0] = pComboBoxRollOver;
	}
	{
		CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver;
		pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ITEMSHOPSEARCH_COMBO_ROLLOVER2 );
		pComboBoxRollOver->CreateBaseComboBoxRollOver ( "ITEMSHOPSEARCH_COMBO_ROLLOVER2", FALSE );			
		pComboBoxRollOver->SetVisibleSingle ( FALSE );
		RegisterControl ( pComboBoxRollOver );
		m_pComboRollOver[1] = pComboBoxRollOver;
	}
	{
		CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver;
		pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ITEMSHOPSEARCH_COMBO_ROLLOVER3 );
		pComboBoxRollOver->CreateBaseComboBoxRollOver ( "ITEMSHOPSEARCH_COMBO_ROLLOVER3", FALSE );			
		pComboBoxRollOver->SetVisibleSingle ( FALSE );
		RegisterControl ( pComboBoxRollOver );
		m_pComboRollOver[2] = pComboBoxRollOver;
	}
	{
		CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver;
		pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ITEMSHOPSEARCH_COMBO_ROLLOVER4 );
		pComboBoxRollOver->CreateBaseComboBoxRollOver ( "ITEMSHOPSEARCH_COMBO_ROLLOVER4", FALSE );			
		pComboBoxRollOver->SetVisibleSingle ( FALSE );
		RegisterControl ( pComboBoxRollOver );
		m_pComboRollOver[3] = pComboBoxRollOver;
	}


	LoadSearchFile();

	
}

void CShopItemSearchWindow::LoadSearchFile()
{
	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += strLoadListFileName;

	std::auto_ptr<CBaseString> pBString(GLOGIC::openfile_basestring(GLOGIC::bGLOGIC_ZIPFILE, 
		GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
		strPath.c_str(), 
		strLoadListFileName, 
		true,
		GLOGIC::bGLOGIC_PACKFILE ) );

	if( !pBString.get() )
	{
		CDebugSet::ToLogFile ( "ERROR : GLStringTable::LOADFILE(), File Open %s", strLoadListFileName );
		return;
	}

	CBaseString &cBString = *pBString;

	CString resToken, strBUFFER;
	CString strToken("\t");

	SNATIVEID nId;

	int strMax = 0;

	while ( cBString.GetNextLine(strBUFFER) > 0 )
	{
		int curPos(0);
		resToken = strBUFFER.Tokenize(strToken.GetString(), curPos);
		m_listSrcString.push_back( resToken ); 
		
	}

}

void CShopItemSearchWindow::ReqSearch()
{
	if( m_pItemNameEdit->GetEditLength( ITEMSHOPSEARCH_EDIT ) != 0 )
	{
		GLMSG::SNETPC_PMARKET_SEARCH_ITEM NetMsg;

		StringCbCopy( NetMsg.sSearchData.szItemName, MAP_NAME_LENGTH, m_pItemNameEdit->GetEditString( ITEMSHOPSEARCH_EDIT ) );

		// 착용 부서
		int nIndex;
		nIndex = m_pComboRollOver[0]->GetSelectIndex();
		if( nIndex != 0 && nIndex != -1 )
		{
			switch ( nIndex )
			{
			case 1:
				NetMsg.sSearchData.dwReqCharClass = GLCC_FIGHTER_M;
				break;
			case 2:
				NetMsg.sSearchData.dwReqCharClass = GLCC_ARMS_M;
				break;
			case 3:
				NetMsg.sSearchData.dwReqCharClass = GLCC_SPIRIT_W;
				break;
			case 4:
				NetMsg.sSearchData.dwReqCharClass = GLCC_ARCHER_W;
				break;
			case 5:
				NetMsg.sSearchData.dwReqCharClass = GLCC_EXTREME_M;
				break;
			}
		}
		// 아이템 타입
		nIndex = m_pComboRollOver[1]->GetSelectIndex();
		if( nIndex != 0 && nIndex != -1 )
		{
			switch ( nIndex )
			{
			case 1: // 검
				NetMsg.sSearchData.dwSuitType = ITEMATT_SWORD;
				break;
			case 2: // 도
				NetMsg.sSearchData.dwSuitType = ITEMATT_SABER;
				break;
			case 3: // 활
				NetMsg.sSearchData.dwSuitType = ITEMATT_BOW;
				break;
			case 4: // 창
				NetMsg.sSearchData.dwSuitType = ITEMATT_SPEAR;
				break;

			case 5: // 단검
				NetMsg.sSearchData.dwSuitType = ITEMATT_DAGGER;
				break;
			case 6: // 몽둥이
				NetMsg.sSearchData.dwSuitType = ITEMATT_STICK;
				break;
			case 7: // 권
				NetMsg.sSearchData.dwSuitType = ITEMATT_GWON;
				break;
			case 8: // 투척
				NetMsg.sSearchData.dwSuitType = ITEMATT_THROW;
				break;
			}
		}
		// 착용 레벨
		nIndex = m_pComboRollOver[2]->GetSelectIndex();
		if( nIndex != 0 && nIndex != -1 )
		{
			NetMsg.sSearchData.wReqLevel = nIndex * 10;
		}
		// 착용 스텟
		nIndex = m_pComboRollOver[3]->GetSelectIndex();
		if( nIndex != 0 && nIndex != -1  )
		{
			switch ( nIndex )
			{
			case 1:
				NetMsg.sSearchData.sReqStats.wDex = 1;
				break;
			case 2:
				NetMsg.sSearchData.sReqStats.wPow = 1;
				break;
			case 3:
				NetMsg.sSearchData.sReqStats.wSpi = 1;
				break;
			}
		}


		m_sSearchData = NetMsg.sSearchData;

		NETSENDTOFIELD( &NetMsg );
	}
}

void CShopItemSearchWindow::ResetAll ()
{
	m_pItemNameEdit->ClearEdit ( ITEMSHOPSEARCH_EDIT );

	m_pItemNameEdit->Init ();
	m_pItemNameEdit->EndEdit ();
	m_pItemNameEdit->BeginEdit ();

	m_nSelectString  = 0;
	m_nPageNum		 = 0;
}

void	CShopItemSearchWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		ResetAll ();
		m_pComboRollSearchOver->ClearText();
		m_pComboRollOver[0]->ClearText();
		m_pComboRollOver[1]->ClearText();
		m_pComboRollOver[2]->ClearText();
		m_pComboRollOver[3]->ClearText();


		CString strTemp1, strTemp2, strTemp3;

		strTemp1 = ID2GAMEWORD( "ITEM_SHOP_SEARCH_NONE" );
		m_pComboRollOver[0]->AddText( strTemp1 );
		m_pComboRollOver[1]->AddText( strTemp1 );
		m_pComboRollOver[2]->AddText( strTemp1 );
		m_pComboRollOver[3]->AddText( strTemp1 );

		m_pComboBoxOpen[0]->SetText ( strTemp1 );
		m_pComboBoxOpen[1]->SetText ( strTemp1 );
		m_pComboBoxOpen[2]->SetText ( strTemp1 );
		m_pComboBoxOpen[3]->SetText ( strTemp1 );

		for( int i = 0; i < 7; i++ )
		{
			strTemp2 = ID2GAMEWORD ( "ITEM_SHOP_SEARCH_TYPE", i );
			m_pComboRollOver[1]->AddText ( strTemp2 );	

			if( i >= 5 ) continue;

			strTemp1 = ID2GAMEWORD ( "ITEM_SHOP_SEARCH_CLASS", i );
			m_pComboRollOver[0]->AddText ( strTemp1 );	

			if( i >= 3 ) continue;
			
			strTemp3 = ID2GAMEWORD ( "ITEM_SHOP_SEARCH_STATUS", i );
			m_pComboRollOver[3]->AddText ( strTemp3 );	
			
		}

		for( int i = 0; i < 20; i++ )
		{
			strTemp1.Format( "%d", i * 10 );
			m_pComboRollOver[2]->AddText ( strTemp1 );	
		}

		
	}
	else
	{
		m_pItemNameEdit->EndEdit ();
	}
	m_bSearch = FALSE;
}

CBasicTextButton* CShopItemSearchWindow::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );
	return pTextButton;
}

CBasicTextBox* CShopItemSearchWindow::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}



void CShopItemSearchWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_RollOverID == NO_ID )
	{
		CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}else{
		ResetMessageEx ();

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}

	if( m_bSearch )
	{
		if( m_searchTime != 0 )
		{
			CTime currentTime = GLGaeaClient::GetInstance().GetCurrentTime();
			CTimeSpan timeSpan = currentTime - m_searchTime;
			if( timeSpan.GetTotalSeconds() >= 5 ) 
			{
				m_bSearch = FALSE;
				m_searchTime = 0;
				ReqSearch();
			}
		}
	}
}

void CShopItemSearchWindow::UpdateComoboItem()
{
	if( m_vecString.size() != 0 )
	{
		m_pComboRollSearchOver->SetVisibleSingle(TRUE);
		m_pComboRollSearchOver->ClearText();
		size_t count = 0;
		for( size_t i = (m_nPageNum*MAX_ROLLOVER_TEXT); i < m_vecString.size(); i++ )
		{						
			if( m_vecString.size() > MAX_ROLLOVER_TEXT ) 
				m_pComboRollSearchOver->AddText (m_vecString[i], TRUE, TRUE );
			else				  
				m_pComboRollSearchOver->AddText (m_vecString[i], FALSE, TRUE );

			count++;
			if( count >= MAX_ROLLOVER_TEXT ) break;
		}						
	}else{
		m_pComboRollSearchOver->SetVisibleSingle(FALSE);
		m_pComboRollSearchOver->ClearText();
	}
}

void CShopItemSearchWindow::ComboInit()
{
	m_pComboRollSearchOver->SetVisibleSingle(FALSE);
	m_pComboRollSearchOver->ClearText();
	m_vecString.clear();
	m_nBeforeStrLength = 0;
	m_nSelectString = -1;
	m_nPageNum      = 0;
}

void CShopItemSearchWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
		case ITEMSHOPSEARCH_MAN:
		case ITEMSHOPSEARCH_EDIT:
			if( m_pItemNameEdit->GetEditLength( ITEMSHOPSEARCH_EDIT ) != 0 )
			{
				if( DxInputDevice::GetInstance().IsUpdatKeyState() )
				{
					if( UIKeyCheck::GetInstance()->Check ( DIK_RETURN, DXKEY_DOWN )  )
					{
//						ComboInit();
						m_pComboRollSearchOver->SetVisibleSingle(FALSE);
					}else if( UIKeyCheck::GetInstance()->Check ( DIK_PGUP, DXKEY_DOWN )  )
					{
						if( m_nPageNum > 0 ) 
						{
							// 이전 페이지로 이동
							m_nPageNum--;
							m_nSelectString = 0;

							UpdateComoboItem();

							m_pItemNameEdit->SetEditString( ITEMSHOPSEARCH_EDIT, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}
					}else if( UIKeyCheck::GetInstance()->Check ( DIK_PGDN, DXKEY_DOWN )  )
					{
						if(((m_nPageNum+1)*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()) 
						{
							m_nPageNum++;
							m_nSelectString = 0;

							UpdateComoboItem();

							m_pItemNameEdit->SetEditString( ITEMSHOPSEARCH_EDIT, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}

					}else if( UIKeyCheck::GetInstance()->Check ( DIK_UP, DXKEY_DOWN )  )
					{
						if(m_nSelectString > 0) 
						{
							m_nSelectString--;
							m_pItemNameEdit->SetEditString( ITEMSHOPSEARCH_EDIT, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}else if( m_nPageNum > 0 ) 
						{
							// 이전 페이지로 이동
							m_nPageNum--;
							m_nSelectString = MAX_ROLLOVER_TEXT-1;

							UpdateComoboItem();

							m_pItemNameEdit->SetEditString( ITEMSHOPSEARCH_EDIT, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}
					}else if( UIKeyCheck::GetInstance()->Check ( DIK_DOWN, DXKEY_DOWN )  )
					{
						if(m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()-1) 
						{
							if( m_nSelectString < MAX_ROLLOVER_TEXT-1 )
							{
								m_nSelectString++;
								m_pItemNameEdit->SetEditString( ITEMSHOPSEARCH_EDIT, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
								m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
							}else{
								// 이후 페이지로 이동
								m_nPageNum++;
								m_nSelectString = 0;

								UpdateComoboItem();

								m_pItemNameEdit->SetEditString( ITEMSHOPSEARCH_EDIT, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
								m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
							}
						}
					}else{
						CString strItemName = m_pItemNameEdit->GetEditString( ITEMSHOPSEARCH_EDIT );

						int nStrLength = strItemName.GetLength();

						//if( strItemName.GetLength() != m_nBeforeStrLength )
						{
							if( m_nBeforeStrLength < strItemName.GetLength() && m_vecString.size() != 0 )
							{

								std::vector<CString> vecString = m_vecString;

								m_vecString.clear();
								m_nSelectString = -1;
								m_nPageNum      = 0;

								for( size_t i = 0; i < vecString.size(); i++ )
								{
									if( vecString[i].Find( strItemName.GetString() ) != -1 )
									{
										m_vecString.push_back( vecString[i] );
									}
								}
							}else{
								m_vecString.clear();
								m_nSelectString = -1;
								m_nPageNum      = 0;

								SRCLIST_ITER iter = m_listSrcString.begin();

								for( ; iter != m_listSrcString.end(); ++iter )
								{
									CString string = *iter;
									if( string.Find( strItemName.GetString() ) != -1 )
									{
										m_vecString.push_back( string );
									}
								}

							}
							UpdateComoboItem();
							if( m_vecString.size() != 0 ) m_nBeforeStrLength = strItemName.GetLength();
							
						}
					}
				}				
			}else{
				ComboInit();
			}
			break;
		case ITEMSHOPSEARCH_COMBO_ROLLOVER:
			{
				if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
				{				
					DWORD dwMsg = GetMessageEx ();
					if ( dwMsg & UIMSG_LB_UP )
					{
						dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
						ResetMessageEx ();
						AddMessageEx ( dwMsg );						
					}
					break;
				}

				if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{				
					int nIndex = m_pComboRollSearchOver->GetSelectIndex ();
					if ( nIndex < 0 ) return ;
					CString strTemp = m_pComboRollSearchOver->GetSelectText ( nIndex );								
					m_pItemNameEdit->SetEditString( ITEMSHOPSEARCH_EDIT, strTemp );
					ComboInit();
				}
			}
			break;
		case ITEMSHOPSEARCH_COMBO_OPEN1:
		case ITEMSHOPSEARCH_COMBO_OPEN2:
		case ITEMSHOPSEARCH_COMBO_OPEN3:
		case ITEMSHOPSEARCH_COMBO_OPEN4:
			{
				if ( CHECK_MOUSE_IN ( dwMsg ) &&  m_pComboRollSearchOver->IsVisible() == FALSE )
				{
					if ( dwMsg & UIMSG_LB_DOWN )
					{
						for( int i = 0; i < 4; i++ )
						{
							if( ControlID - ITEMSHOPSEARCH_COMBO_OPEN1 == i )
							{
								m_pComboRollOver[i]->SetVisibleSingle(TRUE);
							}else{
								m_pComboRollOver[i]->SetVisibleSingle(FALSE);
							}
						}	
						m_RollOverID = ControlID + 4;
						m_bFirstLBUP = TRUE;
					}
				}
			}
			break;
		case ITEMSHOPSEARCH_COMBO_ROLLOVER1:
		case ITEMSHOPSEARCH_COMBO_ROLLOVER2:
		case ITEMSHOPSEARCH_COMBO_ROLLOVER3:
		case ITEMSHOPSEARCH_COMBO_ROLLOVER4:
			{
				if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
				{				
					DWORD dwMsg = GetMessageEx ();
					dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
					ResetMessageEx ();
					AddMessageEx ( dwMsg );

					break;
				}

				if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{		
					int nIndex = m_pComboRollOver[ControlID-ITEMSHOPSEARCH_COMBO_ROLLOVER1]->GetSelectIndex ();
					if ( nIndex < 0 ) return ;
					CString strTemp =  m_pComboRollOver[ControlID-ITEMSHOPSEARCH_COMBO_ROLLOVER1]->GetSelectText ( nIndex );	
					m_pComboBoxOpen[ControlID-ITEMSHOPSEARCH_COMBO_ROLLOVER1]->SetText( strTemp );
				}
			}
			break;
		case ITEMSHOPSEARCH_BUTTON1:
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{	
				if( m_bSearch )
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_SEARCHING_REQ") );
					return;
				}
				else 
				{
					m_bSearch = TRUE;
					m_searchTime = GLGaeaClient::GetInstance().GetCurrentTime();
					
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_SEARCHING_REQ") );
					return;
				}
			}
			break;

		case ITEMSHOPSEARCH_BUTTON2:
		case ET_CONTROL_BUTTON:
			if( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pItemNameEdit->EndEdit ();
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
			break;
	}
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}

