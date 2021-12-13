#include "pch.h"
#include "UIEditBox.h"
#include "DXInputString.h"
#include "BasicCarrat.h"
#include "d3dfont.h"
#include "UITextUtil.h"
#include "UIRenderQueue.h"
#include "RANPARAM.h" //../[Lib]__MfcEx/Sources

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int	CUIEditBox::nDEFAULT_LIMIT_INPUT = 256;
const float	CUIEditBox::fLINE_END_GAP = 2.0f;
const int	CUIEditBox::nNOT_USE_MONEY_FORMAT = 0;

UIGUID		CUIEditBox::DEBUG_BEGIN_ID = NO_ID;
UIGUID		CUIEditBox::DEBUG_END_ID = NO_ID;

CUIEditBox::CUIEditBox ()
	: m_bModeNative ( true )
	, m_bBeginEdit ( false )
	, m_bLimited( false )
	, m_nLimitInput ( nDEFAULT_LIMIT_INPUT )

	, m_pFont ( NULL )
	, m_pCarrat ( NULL )

	, m_nTabIndex ( 0 )
	, m_bHide ( FALSE )
	, m_bUseNumber ( false )
	, m_nMoneyUNIT ( nNOT_USE_MONEY_FORMAT )
	, m_nAlign ( EDIT_LEFT_ALIGN )
	, m_dwTextColor ( NS_UITEXTUTIL::ENABLE )
	, m_bVALID_INPUT ( true )
	, m_bWideCarret ( false ) 

	, m_bMinusSign( TRUE )
	, m_bKeyInput( TRUE )

	, m_xViewPos( 0 )
	, m_xInsertPos( 0 )
	, m_xInsertPosR( 0 )
	, m_fCarratPos( 0.0f )
{
}

CUIEditBox::~CUIEditBox ()
{
}

void CUIEditBox::CreateCarrat ( char* szCarrat, BOOL bUseRender, DWORD dwDiffuse )
{
	m_pCarrat = new CBasicCarrat;
	m_pCarrat->CreateSub ( this, szCarrat );
	m_pCarrat->SetUseRender ( bUseRender );
	m_pCarrat->SetVisibleSingle( FALSE );
	RegisterControl ( m_pCarrat );

	m_fCaretSizeX = m_pCarrat->GetLocalPos().sizeX;
	m_dwCarratDiffuse = dwDiffuse;
}

void CUIEditBox::SetFont ( CD3DFontPar* pFont )
{
	if ( !pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다." );
		return;
	}
	m_pFont = pFont;
}

void CUIEditBox::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CUIEditBox::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CUIEditBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return;
	if ( !m_pFont )		 return;
	if ( !m_pCarrat )	 return;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsBegin () )
	{
		//CUIControl* pParent = GetTopParent ();
		//BOOL bFocus = pParent->IsFocusControl();
		//if ( !bFocus )
		//{
		//	EndEdit ();
		//	return;
		//}

		if ( !DXInputString::GetInstance().IsOn () )
		{
			EndEdit ();
			return;
		}

		SetMODE_NATIVE( DXInputString::GetInstance().IsNativeMode() ? true : false );

		switch( RANPARAM::emSERVICE_TYPE )
		{
		case EMSERVICE_INDONESIA:
		case EMSERVICE_MALAYSIA_EN:
		case EMSERVICE_PHILIPPINES:
//		case EMSERVICE_VIETNAM:
			if( IsMODE_NATIVE() )
			{
				DXInputString::GetInstance().ConverseNativeToEng();
			}
			break;
		};

		if( !m_bKeyInput )
			DXInputString::GetInstance().ConverseFullToHalf();

		CString strInput = DXInputString::GetInstance().GetString();
		int xInsertPos = DXInputString::GetInstance().GetInsertPos();

		//CDebugSet::ToView ( 1, 16, "[Origin]%s", strInput );

		// 	문자입력 이외에 관계없는 키가 눌러진 경우, 모두 무시
		m_bVALID_INPUT = true;
		if( !m_bMinusSign && ( strInput[0] == '-' ) ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LMENU ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RCONTROL ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RMENU ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( !m_bVALID_INPUT )
		{
			SET_STRING_BUFFER ( m_strText );
			SetInsertPos();
			return;
		}

		//	입력 조합창 위치 갱신
		const UIRECT& rcGlobalPos = GetGlobalPos ();
		CRect rtEditBox ( (int)rcGlobalPos.left, (int)rcGlobalPos.top, (int)rcGlobalPos.right, (int)rcGlobalPos.bottom ); 
		DXInputString::GetInstance().Move ( rtEditBox );

		//	입력한계를 넘은 경우
		if ( m_nLimitInput < strInput.GetLength () )
		{
			m_bLimited = true;

			SET_STRING_BUFFER ( m_strText );
			SetInsertPos();
			return;	
		}
		else
		{
			m_bLimited = false;
		}

		// 입력 받은 문자가 유효하므로 m_xInsertPos 갱신한다
		m_xInsertPos = xInsertPos;
		m_xInsertPosR = m_xInsertPos;

		//	숫자만 입력 허용
		if( IsUSE_NUMBER() && !strInput.IsEmpty() )
		{
			// 앞쪽 문자와 0을 제거한다.
			while( TRUE )
			{
				if( strInput.IsEmpty() || (_istdigit( strInput[0] ) ) ) //&& strInput[0] != '0') )
					break;

				strInput.Delete( 0, 1 );
			}

			// 뒷쪽 문자들을 제거한다.
			int strLength = strInput.GetLength();
			for ( int i = 0; i < strLength; ++i )
			{
				if( !_istdigit( strInput[i] ) )
				{
					strInput.Delete( i, strLength - i );
					break;
				}
			}

			SET_STRING_BUFFER( strInput );
			SetInsertPos();

			//CDebugSet::ToView ( 1, 17, "[bNumber]%s", strInput );
		}

		m_strRenderText = m_strText = strInput;

		//CDebugSet::ToView ( 1, 18, "[m_strText]%s", m_strText );
		//CDebugSet::ToView ( 1, 19, "[m_strRenderText]%s", m_strRenderText );
	}

	//	입력값 가리기
	if ( IsHide () )
	{
		const char cAsterisk = '*';
		std::string strAsterisk ( m_strRenderText.GetLength (), cAsterisk );
		m_strRenderText = strAsterisk.c_str ();
	}

	//	돈 단위 찍기
	if ( m_nMoneyUNIT && !m_strText.IsEmpty() )
	{
		LONGLONG lnMoney = (LONGLONG) _ttoi64( m_strText.GetString() );
		if( lnMoney > 0)
		{
			m_strRenderText = NS_UITEXTUTIL::MAKE_MONEY_FORMAT ( lnMoney, m_nMoneyUNIT, m_strUNIT_SYMBOL );

			if( DXInputString::m_bCaratMove )
			{
				// 삽입되는 심벌에 맞춰서 m_xInsertPosR 갱신
				int iLen = m_strText.GetLength();

				for( int i=iLen-m_nMoneyUNIT; i>0; i-=m_nMoneyUNIT )
				{
					if( i < m_xInsertPos )
						m_xInsertPosR += m_strUNIT_SYMBOL.GetLength();
				}
			}
		}
	}

	UpdateRenderText();
	UpdatePosition();

	// 텍스쳐에 글자를 렌더링하는 방식으로 변경 ( 준혁 )
	m_pFont->PushText( (TCHAR*)m_strRenderTextR.GetString() );
}

HRESULT CUIEditBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	CUIRenderQueue::Get()->RenderImmediately();

	// 텍스쳐에 글자를 렌더링하는 방식으로 변경 ( 준혁 )
	// 확장캐럿일 경우 글자 반전 처리 
	if( IsBegin() && m_bWideCarret &&m_pCarrat->IsRender() )
	{
		int nSel = (int)(( m_pCarrat->GetGlobalPos().left - m_vRenderPos.x) / 6);// 캐럿이 선택한 글자 위치(뒷자리)
		CString strRenderTextL = m_strRenderTextR.Left( nSel ) ;
		CString strRenderText  = m_strRenderTextR.Mid( nSel, 2 ) ; // 반전 글자

		SIZE TextSize;
		m_pFont->GetTextExtent ( strRenderTextL.GetString(), TextSize );
		D3DXVECTOR2 vRenderPos = m_vRenderPos;		
		vRenderPos.x = vRenderPos.x + (float)TextSize.cx;

		hr = m_pFont->DrawText( m_vRenderPos.x, m_vRenderPos.y, m_dwTextColor, m_strRenderTextR.GetString(), 0L, FALSE );	
		m_pFont->DrawText( vRenderPos.x, vRenderPos.y, NS_UITEXTUTIL::BLACK , strRenderText.GetString(), 0L, FALSE );	

		if ( FAILED ( hr ) ) return hr;
		
		return S_OK;
	}

	hr = m_pFont->DrawText( m_vRenderPos.x, m_vRenderPos.y, m_dwTextColor, m_strRenderTextR.GetString(), 0L, FALSE );	

	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void CUIEditBox::UpdateRenderText()	// 준혁
{
	m_strRenderTextR = m_strRenderText;

	if( DXInputString::m_bCaratMove == FALSE )
		return;

	const int lEditWidth = (int)( GetGlobalPos().sizeX - fLINE_END_GAP );
	int iLen = m_strRenderTextR.GetLength();

	CString strTemp;
	SIZE sizeTemp;

	// m_strRenserText ( 멀티바이트 ) <= 동기화 => m_xViewPos, m_xInsertPosR ( 유니코드 )
	int iViewPosM = m_xViewPos;
	int iInsertPosRM = m_xInsertPosR;
	if( m_pFont )
	{
		WCHAR wTemp[ 1024 ];
		m_pFont->MultiToWide( m_strRenderTextR.GetString(), iLen, wTemp, 1022 );
		CStringW strTempW = wTemp;

		iViewPosM = WideToMultiPos( &strTempW, m_xViewPos );
		iInsertPosRM = WideToMultiPos( &strTempW, m_xInsertPosR );
	}

	// m_xInsertPosR 가 m_xViewPos 보다 앞에 있다면 보정
	if( iInsertPosRM < iViewPosM )
	{
		iViewPosM = iInsertPosRM;

		// 페이징
		for( int i=0; i<7; ++i )
		{
			if( iViewPosM <= 0 )
				break;
			if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM - 1 ] ) )
				iViewPosM -= 2;
			else
				iViewPosM -= 1;
		}

		iViewPosM = max( iViewPosM, 0 );
	}

	// 출력범위 ( m_xViewPos 부터 lEditWidth 만큼 ) 안에 m_xInsertPosR 가 없다면 보정
	if( iInsertPosRM - iViewPosM > 0 )
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );

		if( sizeTemp.cx > lEditWidth )
		{
			while( sizeTemp.cx > lEditWidth )
			{
				if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM ] ) )
					iViewPosM += 2;
				else
					iViewPosM += 1;
				strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
				m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );
			}

			// 페이징
			int iTemp = iInsertPosRM;
			for( int i=0; i<7; ++i )
			{
				if( iTemp >= iLen )
					break;
				if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM ] ) )
				{
					iViewPosM += 2;
					iTemp += 2;
				}
				else
				{
					iViewPosM += 1;
					iTemp += 1;
				}
			}
		}
	}

	// 보정된 위치 iViewPosM ( 멀티 바이트 ) <= 동기화 => m_xViewPos ( 유니코드 )
	m_xViewPos = MultiToWidePos( &m_strRenderTextR, iViewPosM );

	// 캐럿 위치 계산
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent ( strTemp.GetString(), sizeTemp );
		m_fCarratPos = (float)sizeTemp.cx;
	}

	// m_xViewPos 부터 시작해서 끝까지 길이 계산 후 lEditWidth 안에 들어올때까지 자른다
	if( iLen > 0 )
	{
		m_strRenderTextR = m_strRenderTextR.Right( iLen - iViewPosM );
		m_pFont->GetTextExtent( m_strRenderTextR.GetString(), sizeTemp );
		iLen = m_strRenderTextR.GetLength();

		while( sizeTemp.cx > lEditWidth )
		{
			if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iLen-1 ] ) )
				m_strRenderTextR = m_strRenderTextR.Left( iLen-2 );
			else
				m_strRenderTextR = m_strRenderTextR.Left( iLen-1 );
			m_pFont->GetTextExtent( m_strRenderTextR.GetString(), sizeTemp );
			iLen = m_strRenderTextR.GetLength();
		}
	}
}

void CUIEditBox::UpdatePosition ()
{
	if( DXInputString::m_bCaratMove )
	{
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strRenderTextR.GetString(), TextSize );

		//	글자 뿌릴 위치
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left;
			switch ( m_nAlign )
			{
			case EDIT_CENTER_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				break;

			case EDIT_RIGHT_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.right - (float)TextSize.cx;
				break;
			}
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	캐럿 뿌릴 위치
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + m_fCarratPos;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}
	else
	{
		const long lEditWidth = (long)(GetGlobalPos().sizeX - fLINE_END_GAP);

		//	문자열이 에디트박스를 넘어설 경우 앞부분 잘라내기
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strRenderTextR.GetString(), TextSize );

		if ( lEditWidth < TextSize.cx )
		{
			int nStrLength = m_strRenderTextR.GetLength ();
			while ( lEditWidth < TextSize.cx && nStrLength )
			{
				int nCut = nStrLength - 1;		
				if ( NS_UITEXTUTIL::IsDBCS ( m_strRenderTextR[0] ) )	nCut--;

				m_strRenderTextR = m_strRenderTextR.Right ( nCut );

				nStrLength = m_strRenderTextR.GetLength ();
				m_pFont->GetTextExtent ( m_strRenderTextR.GetString (), TextSize );
			}
		}

		//	글자 뿌릴 위치
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left;
			switch ( m_nAlign )
			{
			case EDIT_CENTER_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				break;

			case EDIT_RIGHT_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.right - (float)TextSize.cx;
				break;
			}
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	캐럿 뿌릴 위치
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + (float)TextSize.cx;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}

	{ // 한국어에서 문자 조합일때 캐럭을 확장한다.
		UIRECT rcCaretOldPos = m_pCarrat->GetGlobalPos();
		UIRECT rcCaretNewPos(rcCaretOldPos);

		if( DXInputString::GetInstance().CheckWideCaret() )
		{
			m_bWideCarret = true;
			rcCaretNewPos.sizeX = 12; // 폰트 9, 돋움체에서 글자 하나의 사이즈
			if( !DXInputString::m_bCaratMove ) rcCaretNewPos.left -= 12;
		}
		else
		{
			m_bWideCarret = false;
			rcCaretNewPos.sizeX = m_fCaretSizeX;
		}

		m_pCarrat->SetGlobalPos( rcCaretNewPos );
		m_pCarrat->AlignSubControl( rcCaretOldPos, rcCaretNewPos );
	}
}

void CUIEditBox::GetInsertPos()
{
	m_xInsertPos = DXInputString::GetInstance().GetInsertPos();		
	m_xInsertPosR = m_xInsertPos;
}

void CUIEditBox::SetInsertPos()
{
	m_xInsertPos = DXInputString::GetInstance().SetInsertPos( m_xInsertPos );
	m_xInsertPosR = m_xInsertPos;
}

int CUIEditBox::MultiToWidePos( CString* strMulti, int iMultiPos )
{
	if( m_pFont )
	{
		CString strTemp = strMulti->Left( iMultiPos );
		WCHAR wTemp[ 512 ]={0};
		return m_pFont->MultiToWide( strTemp.GetString(), strTemp.GetLength(), wTemp, 510 );
	}
	return 0;
}

int CUIEditBox::WideToMultiPos( CStringW* strWide, int iWidePos )
{
	if( m_pFont )
	{
		CStringW strTemp = strWide->Left( iWidePos );
		TCHAR cTemp[ 1024 ]={0};
		return m_pFont->WideToMulti( strTemp.GetString(), strTemp.GetLength(), cTemp, 1022 );
	}
	return 0;
}

void CUIEditBox::FLUSH_STRING_BUFFER ()
{
	DXInputString::GetInstance().SetString ();
}

void CUIEditBox::SET_STRING_BUFFER ( const char* pStr )
{
	FLUSH_STRING_BUFFER ();	
	DXInputString::GetInstance().SetString ( pStr );
}

bool CUIEditBox::BeginEdit ()
{
	if ( m_bBeginEdit ) return true;

	if ( DXInputString::GetInstance().IsOn () )	return false;

	//	만약, Native 모드일 경우 'Native'로 전환
	if ( IsMODE_NATIVE() )
	{
		DXInputString::GetInstance().ConverseEngToNative();
	}

	m_bBeginEdit = true;

	SET_STRING_BUFFER ( m_strText );
	DXInputString::GetInstance().OnInput ();
	SetInsertPos();

	m_pCarrat->SetVisibleSingle ( TRUE );
	if ( m_pCarrat->IsUseRender () )
	{
		m_pCarrat->SetDiffuse ( m_dwCarratDiffuse );
	}

	return true;
}

bool CUIEditBox::EndEdit ()
{	
	if ( !m_bBeginEdit ) return false;

	m_pCarrat->SetVisibleSingle ( FALSE );

	//	만약, Native 모드일경우 영문으로 '무조건' 전환
	if( IsMODE_NATIVE() )
	{
		DXInputString::GetInstance().ConverseNativeToEng ();
	}

	m_bBeginEdit = false;

	DXInputString::GetInstance().OffInput ();
	FLUSH_STRING_BUFFER ();

	return true;
}

void CUIEditBox::ClearEdit ()
{
	FLUSH_STRING_BUFFER ();
	m_strText.Empty ();
	m_strRenderText.Empty ();

	m_xViewPos = 0;
	m_xInsertPos = 0;
	m_xInsertPosR = 0;
}

void CUIEditBox::SetEditString ( const CString& strInput )
{
	if( m_strText == strInput.Left( m_nLimitInput ) ) return;
	m_strText = strInput.Left( m_nLimitInput );
	m_strRenderText = m_strText;

	m_xViewPos = 0;

	// 현재 에디트 중인 컨트롤만 즉시 문장을 치환한다.
	// 그렇지 않은 것의 경우, 실제 전환이 일어났을 때 하는 것이 좋다.
	if ( IsBegin () )
	{
		SET_STRING_BUFFER ( m_strText );
		GetInsertPos();
	}
	else
	{
		m_xInsertPos = MultiToWidePos( &m_strText, m_strText.GetLength() );
		m_xInsertPosR = m_xInsertPos;
	}
}

bool CUIEditBox::SET_MONEY_FORMAT ( int nMoneyUNIT, const CString& strUNIT_SYMBOL )
{	
	if ( nMoneyUNIT < 0 ) return false;
	if ( 0 < nMoneyUNIT && strUNIT_SYMBOL.IsEmpty() ) return false;

	m_nMoneyUNIT = nMoneyUNIT;
	m_strUNIT_SYMBOL = strUNIT_SYMBOL;

	//	돈 모양을 쓰려면, 무조건 숫자 입력으로 바꾼다.
	DoUSE_NUMBER( nMoneyUNIT ? true : false );

	return true;
}

void CUIEditBox::DoMODE_TOGGLE ()
{
	SET_STRING_BUFFER ( m_strText );
	SetInsertPos();

	if ( IsMODE_NATIVE() )
	{
		DXInputString::GetInstance().ConverseNativeToEng ();
		SetMODE_NATIVE( false );
	}
	else
	{
		DXInputString::GetInstance().ConverseEngToNative ();
		SetMODE_NATIVE( true );
	}
}