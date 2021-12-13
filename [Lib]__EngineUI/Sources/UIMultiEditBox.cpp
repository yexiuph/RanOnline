#include "pch.h"
#include ".\uimultieditbox.h"

#include "UIEditBox.h"
#include "UIKeyCheck.h"
#include "UITextUtil.h"
#include "DXInputString.h" //../[Lib]__Engine/Sources/Common

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const INT CUIMultiEditBox::nDEFAULT_INDEX = 1;

CUIMultiEditBox::CUIMultiEditBox(void)
	: m_nCurIndex( nDEFAULT_INDEX )
	, m_pEditBox( NULL )
	, m_bNativeMode( false )
{
}

CUIMultiEditBox::~CUIMultiEditBox(void)
{
}

//void CUIMultiEditBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
//{
//	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
//
//	ASSERT( m_pEditBox );
//	if( m_pEditBox->IsLimited() ) GoLineDown();
//}

void CUIMultiEditBox::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( CHECK_MOUSE_IN ( dwMsg ) && CHECK_LB_DOWN_LIKE ( dwMsg ) )
	{
		CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
		if( pEditBox )
		{
			m_nCurIndex = pEditBox->GetTabIndex ();

			EndEdit ();
			SetEditBox ( pEditBox );
			BeginEdit ();
		}
	}

	CUIControl* pParent = GetTopParent();
	if( !pParent ) pParent = this;	//	만약 이클래스가 최상위 컨트롤인 경우
	BOOL bFocus = pParent->IsFocusControl();

	if( bFocus )
	{
		if( DXInputString::GetInstance().CheckWideCaret() ) return;

		if( UIKeyCheck::GetInstance()->Check( DIK_DOWN, DXKEY_DOWN ) )			
		{
			GoLineDown();
		}
		else if( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN )
				|| UIKeyCheck::GetInstance()->Check( DIK_NUMPADENTER, DXKEY_DOWN ))
		{
			if( DXInputString::GetInstance().CheckEnterKeyDown() )
			{
				GoLineDown();
			}
		}
		else if( UIKeyCheck::GetInstance()->Check( DIK_UP, DXKEY_DOWN ) )		
		{
			GoLineUp();
		}
		//else if( UIKeyCheck::GetInstance()->Check( DIK_BACKSPACE, DXKEY_DOWN ) )
		//{
		//	if( m_pEditBox->IsEmpty() ) GoLineUp();
		//}
	}
}

void CUIMultiEditBox::CreateEditBox ( UIGUID WndID, char* szEditBox, char* szCarrat,
								   BOOL bCarratUseRender, DWORD dwCarratDiffuse, CD3DFontPar* pFont )
{
	GASSERT( szEditBox && szCarrat && pFont );

	int nCount = m_ControlContainer.size ();

	CUIEditBox* pEditBox = new CUIEditBox;
	pEditBox->CreateSub ( this, szEditBox, UI_FLAG_DEFAULT, WndID );
	pEditBox->CreateCarrat ( szCarrat, bCarratUseRender, dwCarratDiffuse );
	pEditBox->SetFont ( pFont );
	pEditBox->SetTabIndex ( nDEFAULT_INDEX + nCount );
	{
		const UIRECT & uiRect = pEditBox->GetLocalPos();
		INT nLIMITCHAR = static_cast<INT>( uiRect.sizeX ) / 6; // MEMO : 영어 한글자가 약 6픽셀
		--nLIMITCHAR; // MEMO : 마지막 1픽셀은 제거
		pEditBox->SetLimitInput ( nLIMITCHAR );
	}
	RegisterControl ( pEditBox );
}

void CUIMultiEditBox::Init()
{
	m_nCurIndex = nDEFAULT_INDEX;
	CUIControlContainer::UICONTROL_LIST_CITER citer = m_ControlContainer.GetControl().begin();
	CUIControl * pControl = (*citer);	//	HEAD임
	SetEditBox( (CUIEditBox*)pControl );
}

void CUIMultiEditBox::BeginEdit ()
{
	m_pEditBox->BeginEdit ();

	if( m_bNativeMode != m_pEditBox->IsMODE_NATIVE() )
	{
		m_pEditBox->DoMODE_TOGGLE();
	}
}

void CUIMultiEditBox::EndEdit ()
{
	if( !m_pEditBox ) return ; // MEMO : BeginEdit() 호출저에는 m_pEditBox는 NULL이다.
	m_pEditBox->EndEdit ();

	m_bNativeMode = m_pEditBox->IsMODE_NATIVE();
}

void CUIMultiEditBox::ClearEdit()
{
	const CUIControlContainer::UICONTROL_LIST & list	= m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer		= list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end	= list.end ();

	for ( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox = (CUIEditBox*)(*citer);
		if( !pEditBox ) continue;

		pEditBox->ClearEdit();
	}
}

void CUIMultiEditBox::SetText( CString & strText )
{
	CString strToken;
	INT nCurPos(0);

	const CUIControlContainer::UICONTROL_LIST & list	= m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer		= list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end	= list.end ();

	for ( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox = (CUIEditBox*)(*citer);
		if( !pEditBox ) continue;

		strToken = strText.Tokenize( "\r\n", nCurPos );
		if( strToken != "")	pEditBox->SetEditString( strToken );	
		else				return ;
	}
}

INT CUIMultiEditBox::GetText( CString & strText )
{
	const CUIControlContainer::UICONTROL_LIST & list	= m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer		= list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end	= list.end ();

	for ( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox = (CUIEditBox*)(*citer);
		if( !pEditBox ) continue;

		const CString & strTemp = pEditBox->GetEditString();
		if( strTemp.IsEmpty() ) strText += strTemp + " \r\n";
		else					strText += strTemp + "\r\n";
	}

	strText.TrimRight( " \r\n" );

	return strText.GetLength();
}

void CUIMultiEditBox::GoLineUp()
{
	INT nCount = m_ControlContainer.size ();	
	if( nCount < 2 )					return ; //	컨트롤이 1개일때는 아무런 처리가 필요없다.
	if( m_nCurIndex == nDEFAULT_INDEX ) return ;

	--m_nCurIndex;

	EndEdit();	//	구 컨트롤, 에디트 해제	
	if ( !FindIndexControl() ) return ; // 새 컨트롤, 에디트 지정
	BeginEdit();
}

void CUIMultiEditBox::GoLineDown()
{
	INT nCount = m_ControlContainer.size ();	
	if( nCount < 2 )			return ; // 컨트롤이 1개일때는 아무런 처리가 필요없다.
	if( m_nCurIndex == nCount )	return ;

	++m_nCurIndex;

	EndEdit();	//	구 컨트롤, 에디트 해제	
	if ( !FindIndexControl() ) return ; // 새 컨트롤, 에디트 지정
	BeginEdit();
}

BOOL CUIMultiEditBox::FindIndexControl()
{
	const CUIControlContainer::UICONTROL_LIST & list	= m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer		= list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end	= list.end ();

	for ( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox = (CUIEditBox*)(*citer);
		if( !pEditBox ) continue;

		if ( pEditBox->GetTabIndex() == m_nCurIndex )
		{
			SetEditBox( pEditBox );	
			return TRUE;
		}
	}

	return FALSE;
}

void CUIMultiEditBox::SetLimitInput( int nLimitInput )
{
	const CUIControlContainer::UICONTROL_LIST & list	= m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer		= list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end	= list.end ();

	for ( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox = (CUIEditBox*)(*citer);
		if( !pEditBox ) continue;

		pEditBox->SetLimitInput( nLimitInput );
	}
}