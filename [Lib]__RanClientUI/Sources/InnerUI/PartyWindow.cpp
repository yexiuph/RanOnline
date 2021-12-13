#include "pch.h"
#include "PartyWindow.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "PartySlot.h"
#include "MiniMap.h"
#include "InnerInterface.h"

#include "GLGaeaClient.h"
#include "GLPartyClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPartyWindow::CPartyWindow () :
	m_pConditionTextBox ( NULL )
{
	for ( int i = 0; i < MAXPARTY; i++ )
	{
		m_pPartySlot[i] = NULL;
	}
}

CPartyWindow::~CPartyWindow ()
{
}

void CPartyWindow::CreateSubControl ()
{	
	int nTextAlign = TEXT_ALIGN_LEFT;
	D3DCOLOR dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = NULL;

	{	//	�⺻ �ٴ� ���
		CreateControl ( "PARTY_WINDOW_MASTER_STATIC_BACK" );
		pTextBox = CreateStaticControl ( "PARTY_WINDOW_MASTER_STATIC", pFont, dwFontColor, nTextAlign );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "PARTY_WINDOW_MASTER_MEMBER_STATIC", 0 ) );

		CreateControl ( "PARTY_WINDOW_MEMBER_STATIC_BACK" );
		pTextBox = CreateStaticControl ( "PARTY_WINDOW_MEMBER_STATIC", pFont, dwFontColor, nTextAlign );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "PARTY_WINDOW_MASTER_MEMBER_STATIC", 1 ) );
	}

	{
		pTextBox = CreateStaticControl ( "PARTY_SLOT_CONDITION_TEXT", pFont, dwFontColor, nTextAlign );
		m_pConditionTextBox = pTextBox;
		//m_pConditionTextBox->AddText ( "- ���� �κ� ��� ������ �й�", NS_UITEXTCOLOR::PARTYNAME );
		//m_pConditionTextBox->AddText ( "- ���� ���³��� ���ڹ�� �� �й�", NS_UITEXTCOLOR::PRIVATE );
	}

	CString strMemberKeyword[MAXPARTY] = 
	{
		"PARTY_SLOT_MEMBER0",
		"PARTY_SLOT_MEMBER1",
		"PARTY_SLOT_MEMBER2",
		"PARTY_SLOT_MEMBER3",
		"PARTY_SLOT_MEMBER4",
		"PARTY_SLOT_MEMBER5",
		"PARTY_SLOT_MEMBER6",
		"PARTY_SLOT_MEMBER7",
	};

	for ( int i = 0; i < MAXPARTY; i++ )
	{
		CPartySlot* pPartySlot = new CPartySlot;
		pPartySlot->CreateSub ( this, strMemberKeyword[i].GetString (), UI_FLAG_DEFAULT, PARTY_SLOT_MEMBER0 + i );
		pPartySlot->CreateSubControl ();
		RegisterControl ( pPartySlot );
		m_pPartySlot[i] = pPartySlot;
	}

//	m_pPartySlot[3]->SetSameMap ( FALSE );
}

void CPartyWindow::SetSlotMember ( CPartySlot* pPartySlot, GLPARTY_CLIENT *pMember, BOOL bSameMap, BOOL bQuitEnable, BOOL bAuthEnable )
{
	if ( !pPartySlot )
	{
		GASSERT ( 0 && "��Ƽ ������ �����ϴ�. - ����" );
		return ;
	}
	if ( !pMember )
	{
		GASSERT ( 0 && "����� �����ϴ�. - ����" );
		return ;
	}

	if ( bSameMap )	//	���� ���ΰ�?
	{
		int nPosX(0), nPosY(0);
		PLANDMANCLIENT pLandClient = GLGaeaClient::GetInstance().GetActiveMap();
		GLMapAxisInfo &sMapAxisInfo = pLandClient->GetMapAxisInfo();
		sMapAxisInfo.Convert2MapPos ( pMember->m_vPos.x, pMember->m_vPos.z, nPosX, nPosY );

		CString strPos;
		strPos.Format ( "%d/%d", nPosX, nPosY );

		pPartySlot->SetPlayerPos ( strPos );

		float fHPPercent = 0.0f;
		if ( pMember->m_sHP.wMax )
			fHPPercent = float(pMember->m_sHP.wNow) / float(pMember->m_sHP.wMax);
		pPartySlot->SetHP ( fHPPercent );
	}
	else
	{
		const CString strMapName = GLGaeaClient::GetInstance().GetMapName ( pMember->m_sMapID );
		pPartySlot->SetMapName ( strMapName );
	}
	pPartySlot->SetSameMap ( bSameMap );


	const int nClassType = CharClassToIndex ( pMember->m_emClass );	
	const CString strName = pMember->m_szName;

	pPartySlot->SetClass ( nClassType );
	pPartySlot->SetPlayerName ( strName );		
	pPartySlot->SetVisibleQuitButton ( bQuitEnable );	
	pPartySlot->SetVisibleAuthButton( bAuthEnable );

	pPartySlot->SetVisibleSingle ( TRUE );
}

void CPartyWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		
	//	��� ������ �ʵ��� ����
	for ( int i = 0; i < MAXPARTY; i++ ) m_pPartySlot[i]->SetVisibleSingle ( FALSE );
	if ( m_pConditionTextBox ) m_pConditionTextBox->SetVisibleSingle ( FALSE );

	//	��Ƽ ����Ʈ ����
	//	
	GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	������
	if ( pMaster )
	{
		GLPARTY_CLIENT *pSelf = FindSelfClient ();
		//	NOTE
		//		����, �ڽ��� ã�� ���ϸ�
		//		�� �Ʒ����� �۾��� �ƹ��͵� �������� �ʽ��ϴ�.
		if ( !pSelf ) return ;

		const BOOL bMaster = pMaster->ISONESELF ();
		SetSlotMember ( m_pPartySlot[0], pMaster, IsSameMap ( pSelf, pMaster ), bMaster, FALSE );
		m_pPartySlot[0]->SetMaster ( bMaster );

		DWORD nMEMBER_NUM = GLPartyClient::GetInstance().GetMemberNum();
		if( nMEMBER_NUM > 1 )
		{
			nMEMBER_NUM -= 1; // Note : �����ʹ� ����
			for ( DWORD i = 0; i < nMEMBER_NUM; i++ )
			{
				GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().GetMember ( i );			
				if ( pMember )
				{				
					const BOOL bONESELF = pMember->ISONESELF ();
					const BOOL bEnableQuitButton = (bMaster || bONESELF);

					//	�����Ͱ� �ƴ� 'Ż��'��� �޽����� ����
					m_pPartySlot[i+1]->SetMaster ( FALSE );

					SetSlotMember ( m_pPartySlot[i+1], pMember, IsSameMap ( pSelf, pMember ), bEnableQuitButton, bMaster );
				}
			}
		}

		if ( m_pConditionTextBox )
		{
			const SPARTY_OPT& sPartyOption = GLPartyClient::GetInstance().GetOption ();

			if ( m_sPartyOption != sPartyOption )
			{
				m_pConditionTextBox->ClearText ();

				CString strCombine;
				CString strLeft, strRight;

				strLeft = ID2GAMEWORD("PARTY_MODAL_CONDITION",0);
				strRight = ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION",sPartyOption.emGET_ITEM);
				strCombine.Format ( "%s : %s", strLeft, strRight );
				m_pConditionTextBox->AddText ( strCombine, NS_UITEXTCOLOR::PARTYNAME );

				strLeft = ID2GAMEWORD("PARTY_MODAL_CONDITION",1);
				strRight = ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION",sPartyOption.emGET_MONEY);
				strCombine.Format ( "%s : %s", strLeft, strRight );
				m_pConditionTextBox->AddText ( strCombine, NS_UITEXTCOLOR::PRIVATE );				

				m_sPartyOption = sPartyOption;
			}

			m_pConditionTextBox->SetVisibleSingle ( TRUE );
		}
	}	
}


BOOL CPartyWindow::IsSameMap ( GLPARTY_CLIENT *pSelf, GLPARTY_CLIENT *pMember )
{
	if ( !pSelf || !pMember )
	{
		GASSERT ( 0 && "�߸��� ����Ÿ�Դϴ�." );
		return FALSE;
	}
	
	return pSelf->m_sMapID.dwID == pMember->m_sMapID.dwID;
}

GLPARTY_CLIENT*	CPartyWindow::FindSelfClient ()
{
	//	�����Ͱ� �ƴϸ�, �ڽĵ��� ã�ƺ���.
	GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	������
	if ( pMaster && pMaster->ISONESELF() )
	{
		return pMaster;
	}
	else
	{
		//	�ڱ�ã��
		for ( int i = 0; i < 7; i++ )
		{
			GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().GetMember( i );
			if ( pMember && pMember->ISONESELF () )
			{
				return pMember;				
			}
		}
	}
//	GASSERT ( 0 && "�ڽ��� ã�� �� �����ϴ�." );
	return NULL;
}

void CPartyWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( PARTY_WINDOW );
			}
		}
		break;
	case PARTY_SLOT_MEMBER0:
		{
			//	NOTE
			//		��Ƽ ��ü
			if ( dwMsg & UIMSG_MOUSEIN_LBUP_EVENT )
			{
				GLPartyClient::GetInstance().Dissolve ();
			}

			//	NOTE
			//		��
			if ( dwMsg & UIMSG_MOUSEIN_RBUP_EVENT )
			{
				GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	������
				if ( pMaster )
				{
					STARGETID sTARID(CROW_PC,pMaster->m_dwGaeaID);
					GLGaeaClient::GetInstance().GetCharacter ()->ReqSkillReaction(sTARID);
				}
			}
		}
		break;

	case PARTY_SLOT_MEMBER1:
	case PARTY_SLOT_MEMBER2:
	case PARTY_SLOT_MEMBER3:
	case PARTY_SLOT_MEMBER4:
	case PARTY_SLOT_MEMBER5:
	case PARTY_SLOT_MEMBER6:
	case PARTY_SLOT_MEMBER7:
		{
			//	NOTE
			//		����
			if ( dwMsg & UIMSG_MOUSEIN_LBUP_EVENT )
			{
				int nIndex = ControlID - PARTY_SLOT_MEMBER1;
				GLPartyClient::GetInstance().Secede ( nIndex );
			}

			if( dwMsg & UIMSG_MOUSEIN_LBUP_AUTH )
			{
				int nIndex = ControlID - PARTY_SLOT_MEMBER1;
				GLPartyClient::GetInstance().Authority( nIndex );
			}

			//	NOTE
			//		��
			if ( dwMsg & UIMSG_MOUSEIN_RBUP_EVENT )
			{
				int nIndex = ControlID - PARTY_SLOT_MEMBER1;

				GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	������
				if ( pMaster )
				{
					GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().GetMember( nIndex );
					if ( pMember )
					{
						STARGETID sTARID(CROW_PC,pMember->m_dwGaeaID);
						GLGaeaClient::GetInstance().GetCharacter ()->ReqSkillReaction(sTARID);
					}
				}
			}
		}
		break;
	}
}

void CPartyWindow::SetPartyInfo ( DWORD dwPartyID, DWORD dwMasterID )
{
	m_dwPartyID = dwPartyID;
	m_dwMasterID = dwMasterID;
}

DWORD CPartyWindow::GetPartyID ()
{
	return m_dwPartyID;
}

DWORD CPartyWindow::GetMasterID()
{
	return m_dwMasterID;
}

