#include "pch.h"
#include "NameDisplayMan.h"
#include "NameDisplay.h"
#include "DxViewPort.h"
#include "UITextControl.h"
#include "InnerInterface.h"
#include "GLCharClient.h"
#include "GLGaeaClient.h"
#include "GLPartyClient.h"
#include "TargetInfoDisplay.h"
#include "PrivateMarketShowMan.h"

#include "HeadChatDisplayMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CNameDisplayMan::CNameDisplayMan ()
	: m_nNAMETYPE( NAME_DISPLAY_DEFAULT )
	, m_dwCOUNT(0)
	, m_CONTROL_NEWID(0)
{
}

CNameDisplayMan::~CNameDisplayMan ()
{
	ClearList ();
}

CNameDisplay* CNameDisplayMan::MAKE_NAMEDISPLAY ()
{
	static UIRECT rcNameDisplay; // MEMO : 기본 로컬 포지션을 저장
	CNameDisplay* pNameDisplay = m_NameDisplayPool.New();

	if( !pNameDisplay->IsMemPool() )
	{
		if ( BASE_CONTROLID_END <= m_CONTROL_NEWID ) m_CONTROL_NEWID = BASE_CONTROLID;	
		else m_CONTROL_NEWID += BASE_CONTROLID;

		pNameDisplay->SetMemPool();
		pNameDisplay->CreateSub ( this, "NAME_DISPLAY", UI_FLAG_XSIZE | UI_FLAG_YSIZE, m_CONTROL_NEWID );
		pNameDisplay->CreateSubControl ();
		pNameDisplay->InitDeviceObjects ( m_pd3dDevice );
		pNameDisplay->RestoreDeviceObjects ( m_pd3dDevice );

		if( !rcNameDisplay.left ) rcNameDisplay = pNameDisplay->GetLocalPos();
	}
	else
	{
		pNameDisplay->AlignSubControl( pNameDisplay->GetLocalPos(), rcNameDisplay );
		pNameDisplay->SetGlobalPos( rcNameDisplay );
		pNameDisplay->INITIALIZE();
	}

	RegisterControl ( pNameDisplay );
	return pNameDisplay;
}

void CNameDisplayMan::ADD_DISP_NAME ( CROWREN &sDISP_NAME, const TCHAR * szOwnerName )
{
	if( sDISP_NAME.m_szNAME[0] == _T('\0') ) return;

	CPrivateMarketShowMan *pPRIVATE_MARKET_SHOW = CInnerInterface::GetInstance().GetPMarketShow();

	CROWRENCOPY_PAIR key = std::make_pair(sDISP_NAME.m_emCROW,sDISP_NAME.m_dwID);
	CROWRENCOPY_LIST_ITER found = m_listDISP_NAME.find ( key );
	if ( found!=m_listDISP_NAME.end() )
	{
		CNameDisplay* pDISP = (*found).second;

		// 다를 경우.
		if ( pDISP->DIFFERENT(sDISP_NAME) )
		{
			//	Note : 종전 콘트롤 삭제.
			m_NameDisplayPool.ReleaseNonInit( pDISP );
			DeleteControl ( pDISP->GetWndID(), 0 );
			m_listDISP_NAME.erase ( found );

			//	Note : 새로 생성.
			{
				CNameDisplay* pDISP_NEW = MAKE_NAMEDISPLAY ();
				
				pDISP_NEW->SET_INFO ( sDISP_NAME );
				pDISP_NEW->UPDATE ( m_dwCOUNT, sDISP_NAME.m_vPOS );

				DWORD dwNICK_COLOR(UINT_MAX);

				if ( sDISP_NAME.m_dwTYPE & TYPE_TARGET )	dwNICK_COLOR = sDISP_NAME.m_dwCOLOR;
				if ( sDISP_NAME.m_dwTYPE & TYPE_CLUBBATTLE ) dwNICK_COLOR = sDISP_NAME.m_dwCOLOR;

				if( sDISP_NAME.m_emCROW == CROW_PET || sDISP_NAME.m_emCROW == CROW_SUMMON )
				{
					if( szOwnerName )
						pDISP_NEW->SetPetOwnerName( szOwnerName, NS_UITEXTCOLOR::SILVER );
				}

				if ( sDISP_NAME.IsCLUB() )
				{
					if( sDISP_NAME.m_szNICK[0] != _T('\0') )
					{
						pDISP_NEW->SetClubName ( sDISP_NAME.m_szNICK, dwNICK_COLOR );
					}
					else
					{
						pDISP_NEW->SetClubName ( " ", dwNICK_COLOR );
					}
				}

				pDISP_NEW->SetName ( sDISP_NAME.m_szNAME, sDISP_NAME.m_dwCOLOR, sDISP_NAME.m_wSCHOOL, sDISP_NAME.m_emPARTY );
				UPDATE_NAME_POS ( pDISP_NEW, sDISP_NAME );	

				m_listDISP_NAME.insert ( std::make_pair(key,pDISP_NEW) );

				//	Note : 상점 윈도우 생성.
				if ( pPRIVATE_MARKET_SHOW && pDISP_NEW->IsPMARKET() )
				{
					pPRIVATE_MARKET_SHOW->ADD_MARKET ( sDISP_NAME );
				}
				else
				{
					pPRIVATE_MARKET_SHOW->DEL_MARKET ( pDISP->GETCTRLID() );
				}
			}
		}
		else
		{
			pDISP->UPDATE ( m_dwCOUNT, sDISP_NAME.m_vPOS );

			//	Note : 상점 윈도우 생성.
			if ( pPRIVATE_MARKET_SHOW && pDISP->IsPMARKET() )
			{
				pPRIVATE_MARKET_SHOW->UPDATE_MARKET ( sDISP_NAME );
			}
			else
			{
				UPDATE_NAME_POS ( pDISP, sDISP_NAME );
			}
		}
	}
	else
	{
		CNameDisplay* pDISP_NEW = MAKE_NAMEDISPLAY ();
		
		pDISP_NEW->SET_INFO ( sDISP_NAME );
		pDISP_NEW->UPDATE ( m_dwCOUNT, sDISP_NAME.m_vPOS );

		DWORD dwNICK_COLOR(UINT_MAX);

		if ( sDISP_NAME.m_dwTYPE & TYPE_TARGET )	dwNICK_COLOR = sDISP_NAME.m_dwCOLOR;
		if ( sDISP_NAME.m_dwTYPE & TYPE_CLUBBATTLE ) dwNICK_COLOR = sDISP_NAME.m_dwCOLOR;

		if( sDISP_NAME.m_emCROW == CROW_PET || sDISP_NAME.m_emCROW == CROW_SUMMON )
		{
			if( szOwnerName )
				pDISP_NEW->SetPetOwnerName( szOwnerName, NS_UITEXTCOLOR::SILVER );
		}

		if ( sDISP_NAME.IsCLUB() )
		{
			if( sDISP_NAME.m_szNICK[0] != _T('\0') )
			{
				pDISP_NEW->SetClubName ( sDISP_NAME.m_szNICK, dwNICK_COLOR );
			}
			else
			{
				pDISP_NEW->SetClubName ( " ", dwNICK_COLOR );
			}
		}

		pDISP_NEW->SetName ( sDISP_NAME.m_szNAME, sDISP_NAME.m_dwCOLOR, sDISP_NAME.m_wSCHOOL, sDISP_NAME.m_emPARTY );
		UPDATE_NAME_POS ( pDISP_NEW, sDISP_NAME );	

		m_listDISP_NAME.insert ( std::make_pair(key,pDISP_NEW) );

		//	Note : 상점 윈도우 생성.
		if ( pPRIVATE_MARKET_SHOW && pDISP_NEW->IsPMARKET() )
		{
			pPRIVATE_MARKET_SHOW->ADD_MARKET ( sDISP_NAME );
		}
	}
}

void CNameDisplayMan::UpdateHold ( DWORD _dwCOUNT )
{
	CPrivateMarketShowMan *pPRIVATE_MARKET_SHOW = CInnerInterface::GetInstance().GetPMarketShow();

	CROWRENCOPY_LIST_ITER pos = m_listDISP_NAME.begin();
	CROWRENCOPY_LIST_ITER end = m_listDISP_NAME.end();
	while ( pos!=end )
	{
		CROWRENCOPY_LIST_ITER cur = pos++;
		CNameDisplay *pDISP = (*cur).second;

		if ( pDISP->GETCOUNT()!=_dwCOUNT )
		{
			//	Note : 상점 윈도우 생성.
			if ( pPRIVATE_MARKET_SHOW && pDISP->IsPMARKET() )
			{
				pPRIVATE_MARKET_SHOW->DEL_MARKET ( pDISP->GETCTRLID() );
			}

			m_NameDisplayPool.ReleaseNonInit( pDISP );
			DeleteControl ( pDISP->GetWndID(), 0 );
			m_listDISP_NAME.erase ( cur );
			continue;
		}

		BOOL bVISIBLE = FALSE;
		EMCROW emCROW = pDISP->GETCROW();
		switch ( m_nNAMETYPE )
		{
		case NAME_DISPLAY_ALL:	
		case NAME_DISPLAY_MOBCHAR:
			bVISIBLE = TRUE;
			break;

		case NAME_DISPLAY_ITEMMONEY:
			if ( emCROW==CROW_ITEM||emCROW==CROW_MONEY )	bVISIBLE = TRUE;
			break;

		case NAME_DISPLAY_PARTY:
		case NAME_DISPLAY_PARTY_CONFT:
		case NAME_DISPLAY_CLUB_CONFT:
			if ( pDISP->IsRELAY() )							bVISIBLE = TRUE;
			break;
		};

		//	 적대관계만...
		if ( pDISP->IsTARGET() )							bVISIBLE = TRUE;
		if ( pDISP->IsClubTARGET() )						bVISIBLE = TRUE;

		if ( emCROW==CROW_NPC )								bVISIBLE = TRUE;
		if ( !pDISP->GET_DISP() )							bVISIBLE = FALSE;

		pDISP->SetVisibleSingle ( bVISIBLE );
	}
}

void CNameDisplayMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//	의도적으로 호출하지 않음
	//CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	
	//	NOTE : 등록된 이름 
	//
	UpdateHold ( m_dwCOUNT );

	//	채팅 출력시
	//	기본 정보 출력 생략
	CHeadChatDisplayMan* pHeadChatDisplayMan = CInnerInterface::GetInstance().GetHeadChatDisplayMan ();
	if ( pHeadChatDisplayMan )
	{
		CHeadChatDisplayMan::CHATMAP & pChatMap = pHeadChatDisplayMan->GetChatMap ();
		CHeadChatDisplayMan::CHATMAP_ITER iter = pChatMap.begin();
		CHeadChatDisplayMan::CHATMAP_ITER iter_end = pChatMap.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLCOPY *pCOPY = GLGaeaClient::GetInstance().GetCopyActor ( std::string((*iter).first) );
			if ( !pCOPY )							continue;

			CROWRENCOPY_LIST_ITER iter = m_listDISP_NAME.find ( CROWRENCOPY_PAIR(pCOPY->GetCrow(),pCOPY->GetCtrlID()) );
			if ( iter!=m_listDISP_NAME.end() )		(*iter).second->SetVisibleSingle ( FALSE );
		}
	}
}

void CNameDisplayMan::ClearList ()
{
	CPrivateMarketShowMan *pPRIVATE_MARKET_SHOW = CInnerInterface::GetInstance().GetPMarketShow();
	if ( pPRIVATE_MARKET_SHOW )		pPRIVATE_MARKET_SHOW->RemoveAll();

	m_listDISP_NAME.clear();
	m_ControlContainer.RemoveAll ();
}

HRESULT CNameDisplayMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

bool CNameDisplayMan::IS_VISIBLE_NAME ( const CROWREN& sDIS_NAME, D3DXVECTOR3 & vScreen )
{
	D3DXVECTOR3 vPOS = sDIS_NAME.m_vPOS;
	vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPOS, NULL );

	long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD(lResolution) - 30;
	WORD Y_RES = LOWORD(lResolution) - 30;

	if ( (vScreen.x<40) || (vScreen.y<40) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
	{
		return false;
	}

	return true;
}

void CNameDisplayMan::UPDATE_NAME_POS ( CNameDisplay* pNameDisplay, const CROWREN& sDIS_NAME )
{
	if ( !pNameDisplay )	return;

	if ( pNameDisplay->IsPMARKET() )
	{
		pNameDisplay->SET_DISP ( false );
		return;
	}

	D3DXVECTOR3 vScreen;

	if( !IS_VISIBLE_NAME( sDIS_NAME, vScreen ) )
	{
		pNameDisplay->SET_DISP ( false );
	}
	else
	{
		const UIRECT& rcOriginPos = pNameDisplay->GetGlobalPos ();

		D3DXVECTOR2 vpos;
		vpos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f )); // MEMO
		vpos.y = floor(vScreen.y - rcOriginPos.sizeY);

		pNameDisplay->SET_DISP ( true );

		//if ( rcOriginPos.left!=vpos.x || rcOriginPos.top!=vpos.y )
		
		if( abs( rcOriginPos.left - vpos.x ) > 1 || abs( rcOriginPos.top - vpos.y ) > 1 )
		{
			pNameDisplay->SetGlobalPos( vpos );
		}
	}
}

void CNameDisplayMan::VisibleTargetInfoCtrl ( EMCROW emCROW, DWORD dwCtrID, BOOL bVISIBLE )
{
	CROWRENCOPY_LIST_ITER iter = m_listDISP_NAME.find ( CROWRENCOPY_PAIR(emCROW,dwCtrID) );
	if ( iter!=m_listDISP_NAME.end() )
	{
		CROWRENCOPY_PAIR sKEY = (*iter).first; // MEMO
		CNameDisplay* pDISP = (*iter).second;
		
		pDISP->SetVisibleSingle ( bVISIBLE );
	}
}

const CROWREN* CNameDisplayMan::GET_DISP_INFO ( EMCROW emCROW, DWORD dwCtrID )
{
	CROWRENCOPY_LIST_ITER iter = m_listDISP_NAME.find ( CROWRENCOPY_PAIR(emCROW,dwCtrID) );
	if ( iter==m_listDISP_NAME.end() )		return NULL;

	CROWRENCOPY_PAIR sKEY = (*iter).first; // MEMO
	CNameDisplay* pDISP = (*iter).second;
	return pDISP->GET_INFO();
}

bool CNameDisplayMan::DeleteControl( UIGUID ControlID, int refactoring )
{
	return m_ControlContainer.EraseControl( ControlID, FALSE );
}