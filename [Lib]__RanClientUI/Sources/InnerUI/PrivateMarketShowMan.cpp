#include "pch.h"
#include "PrivateMarketShowMan.h"
#include "UITextControl.h"
#include "BasicVarTextBox.h"
#include "PrivateMarketShow.h"
#include "GLGaeaClient.h"
#include "DxViewPort.h"
#include "InnerInterface.h"
#include "GameTextControl.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPrivateMarketShowMan::CPrivateMarketShowMan ()
	: m_CONTROL_NEWID( 0 )
{
}

CPrivateMarketShowMan::~CPrivateMarketShowMan ()
{
}

HRESULT	CPrivateMarketShowMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

void CPrivateMarketShowMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{		
	//CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_bFIRST_TOUCH = true;

	CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( !IsVisible () ) return ;

	//	의도된 것임
	//	업데이트 순서를 '역'으로 진행함, 즉 rbegin부터 시작해서 rend로 감
	CUIControlContainer::UICONTROL_LIST list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_RITER riter = list.rbegin ();
	CUIControlContainer::UICONTROL_LIST_RITER riter_end = list.rend ();
	for ( ; riter != riter_end; ++riter )
	{
		CUIControl* pControl = (*riter);

		//	보이지 않는 컨트롤은 업데이트 하지 않는다.
		if ( !pControl->IsVisible () )	continue;

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		UIGUID cID = pControl->GetWndID ();	
		DWORD dwMsg = pControl->GetMessageEx ();
		
		if ( CHECK_KEYFOCUSED ( dwMsg ) )	CUIControl::AddMessageEx ( UIMSG_KEY_FOCUSED );
		//if ( CHECK_FOCUSED ( dwMsg ) )	CUIControl::AddMessageEx ( UIMSG_FOCUSED );
		if ( CHECK_TOPARENT ( dwMsg ) )		CUIControl::AddMessageEx ( CUT_LOW24BIT ( dwMsg ) );

		if ( bFirstControl )
		{
			NS_UIDEBUGSET::BlockBegin ();
			if ( dwMsg && ( cID < FORCE_ID ) ) TranslateUIMessage ( cID, dwMsg );
			NS_UIDEBUGSET::BlockEnd ();
		}
	}

	//UpdateActionMsgQ();
}

void	CPrivateMarketShowMan::ADD_MARKET ( const CROWREN &sDISP )
{
	const DWORD dwGaeaID = sDISP.m_dwID;		

	PRIVATE_MARKET_SHOW_MAP_ITER found = m_mapPrivateMarketShow.find ( dwGaeaID );
	if ( found==m_mapPrivateMarketShow.end() )
	{
		const D3DCOLOR cCOLOR = NS_UITEXTCOLOR::DEFAULT;
		const D3DXVECTOR3 &vPOS = sDISP.m_vPOS;

		//	Note : 추가
		//	학교, 파티 구분 작업해야함
		CString strTemp;

		
		CString strName;
		CString strMarket;
		CPrivateMarketShow* pVarTextBox = MAKE_PRIVATEMARKET ();
		strTemp.Format ( "%s%s", ID2GAMEWORD("PRIVATE_MARKET"), sDISP.m_szNAME );
		pVarTextBox->AddTextNoSplit ( strTemp, UINT_MAX );
		pVarTextBox->RePosControl ( 0, 0 );
		pVarTextBox->ClearText ();

		strName.Format( _T( "%s" ), sDISP.m_szNAME );
		strMarket.Format( _T( "%s" ), sDISP.m_szPMARKET );
		pVarTextBox->SetNameString( strName );
		pVarTextBox->SetMarketString( strMarket );

		GLCharacter* pChar = GLGaeaClient::GetInstance().GetCharacter();
		if( pChar->IsMarketClick( pVarTextBox->GetGaeaID() ) )
		{
			pVarTextBox->ClearText();
			int nIndex = pVarTextBox->AddText ( (char*)ID2GAMEWORD("PRIVATE_MARKET"), NS_UITEXTCOLOR::AQUA );			
			pVarTextBox->AddString ( nIndex, pVarTextBox->GetNameString(), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			pVarTextBox->AddTextNoSplit ( pVarTextBox->GetMarketString(), UINT_MAX );	
			pVarTextBox->RePosControl ( 0, 0 );
			pVarTextBox->SetGaeaID ( dwGaeaID );
			UPDATE_MARKET_POS ( pVarTextBox, sDISP );
		}
		else
		{
			int nIndex = pVarTextBox->AddText ( (char*)ID2GAMEWORD("PRIVATE_MARKET"), NS_UITEXTCOLOR::CHARTREUSE );			
			pVarTextBox->AddString ( nIndex, sDISP.m_szNAME, NS_UITEXTCOLOR::GOLD );
			pVarTextBox->AddTextNoSplit ( sDISP.m_szPMARKET, UINT_MAX );
			pVarTextBox->RePosControl ( 0, 0 );
			pVarTextBox->SetGaeaID ( dwGaeaID );
			UPDATE_MARKET_POS ( pVarTextBox, sDISP );
		}

		m_mapPrivateMarketShow.insert ( std::make_pair(dwGaeaID,pVarTextBox) );
	}
	else
	{			
		CPrivateMarketShow* pVarTextBox = (*found).second;
		UPDATE_MARKET_POS ( pVarTextBox, sDISP );
	}
}

void	CPrivateMarketShowMan::DEL_MARKET ( DWORD dwGaeaID )
{
	PRIVATE_MARKET_SHOW_MAP_ITER found = m_mapPrivateMarketShow.find ( dwGaeaID );
	if ( found!=m_mapPrivateMarketShow.end() )
	{
		CPrivateMarketShow* pVarTextBox = (*found).second;
		
		m_PrivateMarketPool.ReleaseNonInit( pVarTextBox );
		DeleteControl ( pVarTextBox->GetWndID (), 0 );
		m_mapPrivateMarketShow.erase ( found );
	}
}

void	CPrivateMarketShowMan::UPDATE_MARKET ( const CROWREN &sDISP )
{
	PRIVATE_MARKET_SHOW_MAP_ITER found = m_mapPrivateMarketShow.find ( sDISP.m_dwID );
	if ( found!=m_mapPrivateMarketShow.end() )
	{
		CPrivateMarketShow* pVarTextBox = (*found).second;
		
		UPDATE_MARKET_POS ( pVarTextBox, sDISP );
	}
}

void	CPrivateMarketShowMan::UPDATE_MARKET_POS ( CPrivateMarketShow* pVarTextBox, const CROWREN& sDISP )
{
	if ( !pVarTextBox ) return ;

	D3DXVECTOR3 vScreen;

	if( !IS_VISIBLE_NAME( sDISP, vScreen ) ) 
	{
		pVarTextBox->SetVisibleSingle ( FALSE );
	}
	else
	{
		const UIRECT& rcOriginPos = pVarTextBox->GetGlobalPos ();

		D3DXVECTOR2 vPos;
		vPos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ));
		vPos.y = vScreen.y - rcOriginPos.sizeY;

		pVarTextBox->SetGlobalPos ( vPos );
		pVarTextBox->SetVisibleSingle ( TRUE );
	}
}

CPrivateMarketShow*	CPrivateMarketShowMan::MAKE_PRIVATEMARKET ()
{
	CPrivateMarketShow* pInfo = m_PrivateMarketPool.New();

	if( !pInfo->IsMemPool() )
	{
		if ( BASE_CONTROLID_END <= m_CONTROL_NEWID ) m_CONTROL_NEWID = BASE_CONTROLID;
		else m_CONTROL_NEWID += BASE_CONTROLID;

		pInfo->SetMemPool();
		pInfo->CreateSub ( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE, m_CONTROL_NEWID );
		pInfo->CreateSubControl ();
		pInfo->SetTextAlign ( TEXT_ALIGN_LEFT );
		pInfo->SetLineInterval ( 4.0f );
		pInfo->InitDeviceObjects ( m_pd3dDevice );
		pInfo->RestoreDeviceObjects ( m_pd3dDevice );
		pInfo->SetBlockMouseTracking ( true );
	}

	RegisterControl ( pInfo );
	return pInfo;
}

void CPrivateMarketShowMan::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( CInnerInterface::GetInstance().IsVisibleGroup ( PRIVATE_MARKET_WINDOW ) )	return ;
	if ( CInnerInterface::GetInstance().IsVisibleGroup ( STORAGE_WINDOW ) )			return ;
	if ( CInnerInterface::GetInstance().IsVisibleGroup ( CLUB_STORAGE_WINDOW ) )	return ;
	if ( UIKeyCheck::GetInstance ()->CheckSimple ( DIK_LMENU, DXKEY_PRESSED ) )		return ;

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		if ( !m_bFIRST_TOUCH ) return ;
		m_bFIRST_TOUCH = false;

		CPrivateMarketShow* pPMarketShow = (CPrivateMarketShow*) FindControl ( ControlID );
		if ( !pPMarketShow ) return ;

		pPMarketShow->SetUseOverColor ( true );
		pPMarketShow->SetOverColor ( NS_UITEXTCOLOR::LIGHTSKYBLUE );

		if ( UIMSG_LB_DUP & dwMsg || UIMSG_LB_UP & dwMsg || UIMSG_LB_DOWN & dwMsg || UIMSG_LB_PRESEED & dwMsg )
		{
			AddMessageEx ( UIMSG_MOUSEIN_MARKET_ADVERTISE_LBDNUP );
		}

		if ( UIMSG_LB_UP & dwMsg )
		{
			GLCharacter* pChar = GLGaeaClient::GetInstance().GetCharacter();
			if( !pChar->IsMarketClick( pPMarketShow->GetGaeaID() ) )
			{
				pChar->InsertMarketClick( pPMarketShow->GetGaeaID() );
				pPMarketShow->ClearText();
				int nIndex = pPMarketShow->AddText ( (char*)ID2GAMEWORD("PRIVATE_MARKET"), NS_UITEXTCOLOR::AQUA );			
				pPMarketShow->AddString ( nIndex, pPMarketShow->GetNameString(), NS_UITEXTCOLOR::LIGHTSKYBLUE );
				pPMarketShow->AddTextNoSplit ( pPMarketShow->GetMarketString(), UINT_MAX );			
			}

			CInnerInterface::GetInstance().SetPrivateMarketOpen ( false, pPMarketShow->GetGaeaID () );			
		}
	}
}

bool	CPrivateMarketShowMan::IS_VISIBLE_NAME ( const CROWREN& sDISP, D3DXVECTOR3 & vScreen )
{
	D3DXVECTOR3 * vPos = GLGaeaClient::GetInstance().FindCharHeadPos( sDISP.m_szNAME );
	vScreen = DxViewPort::GetInstance().ComputeVec3Project ( vPos, NULL );

	long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD(lResolution) - 30;
	WORD Y_RES = LOWORD(lResolution) - 30;

	if ( (vScreen.x<40) || (vScreen.y<40) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
	{
		return false;
	}

	return true;
}

bool CPrivateMarketShowMan::DeleteControl( UIGUID ControlID, int refactoring )
{
	return m_ControlContainer.EraseControl ( ControlID, FALSE );
}

void CPrivateMarketShowMan::RemoveAll()
{
	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER iter = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER iter_end = list.end ();

	for ( ; iter != iter_end; ++iter )
	{
		m_PrivateMarketPool.ReleaseNonInit ( (CPrivateMarketShow*) (*iter) );
	}

	m_ControlContainer.RemoveAll ( false );
	m_mapPrivateMarketShow.clear();
}