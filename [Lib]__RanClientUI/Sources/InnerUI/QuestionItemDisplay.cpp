#include "pch.h"
#include "QuestionItemDisplay.h"
#include "QuestionItemType.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"
#include "DxViewPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CQuestionItemDisplay::CQuestionItemDisplay ()
{
}

CQuestionItemDisplay::~CQuestionItemDisplay ()
{
}

void CQuestionItemDisplay::CreateSubControl ()
{
	CString strKeyword[MAXTYPE] = 
	{
		"QUESTION_ITEM_SPEED_UP",
		"QUESTION_ITEM_CRAZY_TIME",
		"QUESTION_ITEM_POWER_UP",
		"QUESTION_ITEM_EXP_TIME",

		"QUESTION_ITEM_EXP_GET",
		"QUESTION_ITEM_LUCKY",
		"QUESTION_ITEM_BOMB",
		"QUESTION_ITEM_OHNO",

		"QUESTION_ITEM_SPEED_UP_M",
		"QUESTION_ITEM_MADNESS",
		"QUESTION_ITEM_ATTACK_UP_M",
		"QUESTION_ITEM_HEAL"
	};

	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_pQI_TYPE[i] = new CQuestionItemType;
		m_pQI_TYPE[i]->CreateSub ( this, strKeyword[i].GetString(), UI_FLAG_DEFAULT, SPEED_UP + i );
		m_pQI_TYPE[i]->CreateSubControl ( strKeyword[i] );
		m_pQI_TYPE[i]->SetVisibleSingle ( FALSE );
		m_pQI_TYPE[i]->SetUseRender ( FALSE );
		m_pQI_TYPE[i]->STOP ();
		m_pQI_TYPE[i]->RESET ();		
		RegisterControl ( m_pQI_TYPE[i] );

        m_QI_TYPE_KEEP[i] = new CUIControl;
		m_QI_TYPE_KEEP[i]->CreateSub ( this, strKeyword[i].GetString() );
		m_QI_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
		RegisterControl ( m_QI_TYPE_KEEP[i] );
	}

	m_pPositionControl = new CUIControl;
	m_pPositionControl->CreateSub ( this, "QUESTION_ITEM_POSITION" );
	m_pPositionControl->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pPositionControl );
}

bool	CQuestionItemDisplay::START ( UIGUID cID )
{
	if ( cID < SPEED_UP || HEAL < cID ) return false;
	
	int nIndex = cID - SPEED_UP;
	m_pQI_TYPE[nIndex]->SetVisibleSingle ( TRUE );
	m_pQI_TYPE[nIndex]->START ();

	return true;
}

bool	CQuestionItemDisplay::RESET ( UIGUID cID )
{
	if ( cID < SPEED_UP || HEAL < cID ) return false;

	int nIndex = cID - SPEED_UP;	
	m_pQI_TYPE[nIndex]->RESET ();

    return true;
}

bool	CQuestionItemDisplay::STOP ( UIGUID cID )
{
	if ( cID < SPEED_UP || HEAL < cID ) return false;

	int nIndex = cID - SPEED_UP;	
	m_pQI_TYPE[nIndex]->STOP ();
	m_pQI_TYPE[nIndex]->SetVisibleSingle ( FALSE );

	return true;
}

void CQuestionItemDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	D3DXVECTOR3 vPos = GLGaeaClient::GetInstance().GetCharacter()->GetPosBodyHeight();

	static D3DXVECTOR3 vScreenBack;
	D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

	// 마우스 움직임에 흔들림을 보정한다.
	if( abs( vScreenBack.x - vScreen.x ) < 1.0f )
	{
		vScreen.x = vScreenBack.x;
	}
	
	bool bPLAYING( false );
	bool bKEEPING( false );

	for ( int i = 0; i < MAXTYPE; ++i )
	{
		const UIRECT& rcOriginPos = m_pQI_TYPE[i]->GetGlobalPos ();

		D3DXVECTOR2 vPos;
		vPos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ));
		vPos.y = m_pPositionControl->GetGlobalPos().top;

		if ( m_pQI_TYPE[i]->ISPLAYING () )
		{
			m_pQI_TYPE[i]->SetGlobalPos ( vPos );

			bPLAYING = true;
		}
		else
		{
			STOP ( SPEED_UP + i );
		}

		if ( m_QI_TYPE_KEEP[i]->IsVisible () )
		{
			m_QI_TYPE_KEEP[i]->SetGlobalPos ( vPos );

			bKEEPING = true;
		}
	}

	vScreenBack = vScreen;

	if ( !bPLAYING && !bKEEPING ) CInnerInterface::GetInstance().HideGroup ( GetWndID () );
}

void CQuestionItemDisplay::ALLSTOP ()
{
	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_pQI_TYPE[i]->STOP ();
		m_pQI_TYPE[i]->RESET ();
		m_pQI_TYPE[i]->SetVisibleSingle ( FALSE );

		m_QI_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
	}

	CInnerInterface::GetInstance().HideGroup ( GetWndID () );
}

bool CQuestionItemDisplay::KEEP_START ( UIGUID cID )
{
	if ( cID < SPEED_UP || HEAL < cID ) return false;

	int nIndex = cID - SPEED_UP;	

	m_QI_TYPE_KEEP[nIndex]->SetVisibleSingle ( TRUE );
	m_QI_TYPE_KEEP[nIndex]->SetDiffuseAlpha ( 50 );

	return true;
}

void CQuestionItemDisplay::KEEP_STOP ()
{
	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_QI_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
	}
}