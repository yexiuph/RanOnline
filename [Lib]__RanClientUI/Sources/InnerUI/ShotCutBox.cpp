#include "pch.h"
#include "ShotCutBox.h"
#include "DXInputString.h"
#include "../[Lib]__EngineUI/Sources/BasicCarrat.h"
#include "d3dfont.h"
#include "../[Lib]__EngineUI/Sources/UITextUtil.h"
#include "../[Lib]__EngineUI/Sources/UIRenderQueue.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int	CShotCutBox::nDEFAULT_LIMIT_INPUT = 1;
const float	CShotCutBox::fLINE_END_GAP = 2.0f;

UIGUID		CShotCutBox::DEBUG_BEGIN_ID = NO_ID;
UIGUID		CShotCutBox::DEBUG_END_ID = NO_ID;

CShotCutBox::CShotCutBox ()
	: m_bModeNative ( false )
	, m_bBeginEdit ( false )
	, m_bLimited( false )
	, m_nLimitInput ( nDEFAULT_LIMIT_INPUT )
	, m_pFont ( NULL )
	, m_pCarrat ( NULL )
	, m_dwTextColor ( NS_UITEXTUTIL::ENABLE )
	, m_bKeyInput( TRUE )
	, m_xViewPos( 0 )
	, m_xInsertPos( 0 )
	, m_xInsertPosR( 0 )
	, m_fCarratPos( 0.0f )
{
//	DXInputString::GetInstance().m_bCaratMove = true;
}

CShotCutBox::~CShotCutBox ()
{
}

void CShotCutBox::CreateCarrat ( char* szCarrat, BOOL bUseRender, DWORD dwDiffuse )
{
	m_pCarrat = new CBasicCarrat;
	m_pCarrat->CreateSub ( this, szCarrat );
	m_pCarrat->SetUseRender ( bUseRender );
	m_pCarrat->SetVisibleSingle( FALSE );
	RegisterControl ( m_pCarrat );

	m_fCaretSizeX = m_pCarrat->GetLocalPos().sizeX;
	m_dwCarratDiffuse = dwDiffuse;
}

void CShotCutBox::SetFont ( CD3DFontPar* pFont )
{
	if ( !pFont )
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�." );
		return;
	}
	m_pFont = pFont;
}

void CShotCutBox::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CShotCutBox::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CShotCutBox::SetImpossableKey()
{
	for( int i =0; i< 12; ++i){
		m_NotSetKey[i] = DIK_F1 + i;
	}

	m_NotSetKey[12] = DIK_LCONTROL;
	m_NotSetKey[13] = DIK_LMENU;
	m_NotSetKey[14] = DIK_RCONTROL;
	m_NotSetKey[15] = DIK_RMENU;
	m_NotSetKey[16] = DIK_RSHIFT;
	m_NotSetKey[17] = DIK_LSHIFT;
	m_NotSetKey[18] = DIK_TAB;
	m_NotSetKey[19] = DIK_ESCAPE;
	m_NotSetKey[20] = DIK_RETURN;
	m_NotSetKey[21] = DIK_GRAVE;	
	m_NotSetKey[22] = DIK_Y;	
}

void CShotCutBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return;
	if ( !m_pFont )		 return;
	if ( !m_pCarrat )	 return;
	
	BOOL bInput = false;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	SetImpossableKey();

	if ( IsBegin () )
	{
		if ( !DXInputString::GetInstance().IsOn () )
		{
			EndEdit ();
			return;
		}

		SetMODE_NATIVE(false);

		DXInputString::GetInstance().ConverseNativeToEng();
		
		if( !m_bKeyInput )
			DXInputString::GetInstance().ConverseFullToHalf();

		CString strInput = DXInputString::GetInstance().GetString();
		int xInsertPos = DXInputString::GetInstance().GetInsertPos();

		strInput.MakeUpper();

		// 	�����Է� �̿ܿ� ������� Ű�� ������ ���, ��� ����
		m_bVALID_INPUT = true;

		for ( int i = 0; i< 21; ++i){
			if( DxInputDevice::GetInstance().GetKeyState ( m_NotSetKey[i] ) & DXKEY_DOWN ){
				m_bVALID_INPUT = false;
			}
		}

		if( !m_bVALID_INPUT )
		{
			SET_STRING_BUFFER ( m_strText );
			SetInsertPos();
			return;
		}

		if(strInput != ""){
            bInput = true;			
		}		

		//	�Է� ����â ��ġ ����
		const UIRECT& rcGlobalPos = GetGlobalPos ();
		CRect rtEditBox ( (int)rcGlobalPos.left, (int)rcGlobalPos.top, (int)rcGlobalPos.right, (int)rcGlobalPos.bottom ); 
		DXInputString::GetInstance().Move ( rtEditBox );

		//	�Է��Ѱ踦 ���� ���
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

		// �Է� ���� ���ڰ� ��ȿ�ϹǷ� m_xInsertPos �����Ѵ�
		m_xInsertPos = xInsertPos;
		m_xInsertPosR = m_xInsertPos;

		m_strRenderText = m_strText = strInput;

	}

	UpdateRenderText();
	UpdatePosition();

	// �ؽ��Ŀ� ���ڸ� �������ϴ� ������� ���� ( ���� )
	m_pFont->PushText( (TCHAR*)m_strRenderTextR.GetString() );

	if(bInput)	
		EndEdit();
}

HRESULT CShotCutBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	CUIRenderQueue::Get()->RenderImmediately();

	// �ؽ��Ŀ� ���ڸ� �������ϴ� ������� ���� ( ���� )
	hr = m_pFont->DrawText( m_vRenderPos.x, m_vRenderPos.y, m_dwTextColor, m_strRenderTextR.GetString(), 0L, FALSE );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void CShotCutBox::UpdateRenderText()	// ����
{
	m_strRenderTextR = m_strRenderText;

	if( DXInputString::m_bCaratMove == FALSE )
		return;

	const int lEditWidth = (int)( GetGlobalPos().sizeX - fLINE_END_GAP );
	int iLen = m_strRenderTextR.GetLength();

	CString strTemp;
	SIZE sizeTemp;

	// m_strRenserText ( ��Ƽ����Ʈ ) <= ����ȭ => m_xViewPos, m_xInsertPosR ( �����ڵ� )
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

	// m_xInsertPosR �� m_xViewPos ���� �տ� �ִٸ� ����
	if( iInsertPosRM < iViewPosM )
	{
		iViewPosM = iInsertPosRM;

		// ����¡
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

	// ��¹��� ( m_xViewPos ���� lEditWidth ��ŭ ) �ȿ� m_xInsertPosR �� ���ٸ� ����
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

			// ����¡
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

	// ������ ��ġ iViewPosM ( ��Ƽ ����Ʈ ) <= ����ȭ => m_xViewPos ( �����ڵ� )
	m_xViewPos = MultiToWidePos( &m_strRenderTextR, iViewPosM );

	// ĳ�� ��ġ ���
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent ( strTemp.GetString(), sizeTemp );
		m_fCarratPos = (float)sizeTemp.cx;
	}

	// m_xViewPos ���� �����ؼ� ������ ���� ��� �� lEditWidth �ȿ� ���ö����� �ڸ���
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

void CShotCutBox::UpdatePosition ()
{
	if( DXInputString::m_bCaratMove )
	{
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strRenderTextR.GetString(), TextSize );

		//	���� �Ѹ� ��ġ
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	ĳ�� �Ѹ� ��ġ
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + m_fCarratPos;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}
	else
	{
		const long lEditWidth = (long)(GetGlobalPos().sizeX - fLINE_END_GAP);

		//	���ڿ��� ����Ʈ�ڽ��� �Ѿ ��� �պκ� �߶󳻱�
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

		//	���� �Ѹ� ��ġ
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);		
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	ĳ�� �Ѹ� ��ġ
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + (float)TextSize.cx;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}
}

void CShotCutBox::GetInsertPos()
{
	m_xInsertPos = DXInputString::GetInstance().GetInsertPos();		
	m_xInsertPosR = m_xInsertPos;
}

void CShotCutBox::SetInsertPos()
{
	m_xInsertPos = DXInputString::GetInstance().SetInsertPos( m_xInsertPos );
	m_xInsertPosR = m_xInsertPos;
}

int CShotCutBox::MultiToWidePos( CString* strMulti, int iMultiPos )
{
	if( m_pFont )
	{
		CString strTemp = strMulti->Left( iMultiPos );
		WCHAR wTemp[ 512 ]={0};
		return m_pFont->MultiToWide( strTemp.GetString(), strTemp.GetLength(), wTemp, 510 );
	}
	return 0;
}

int CShotCutBox::WideToMultiPos( CStringW* strWide, int iWidePos )
{
	if( m_pFont )
	{
		CStringW strTemp = strWide->Left( iWidePos );
		TCHAR cTemp[ 1024 ]={0};
		return m_pFont->WideToMulti( strTemp.GetString(), strTemp.GetLength(), cTemp, 1022 );
	}
	return 0;
}

void CShotCutBox::FLUSH_STRING_BUFFER ()
{
	DXInputString::GetInstance().SetString ();
}

void CShotCutBox::SET_STRING_BUFFER ( const char* pStr )
{
	FLUSH_STRING_BUFFER ();	
	DXInputString::GetInstance().SetString ( pStr );
}

bool CShotCutBox::BeginEdit ()
{
	if ( m_bBeginEdit ) return true;

	if ( DXInputString::GetInstance().IsOn () )	return false;

	//	����, Native ����� ��� 'Native'�� ��ȯ
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
		//m_pCarrat->SetDiffuse ( m_dwCarratDiffuse );
		// ĳ�� ���İ��� �������״�.
		m_pCarrat->SetDiffuse ( D3DCOLOR_ARGB(127,255,255,255) );
	}

	return true;
}

bool CShotCutBox::EndEdit ()
{	
	if ( !m_bBeginEdit ) return false;

	m_pCarrat->SetVisibleSingle ( FALSE );

	//	����, Native ����ϰ�� �������� '������' ��ȯ
	if( IsMODE_NATIVE() )
	{
		DXInputString::GetInstance().ConverseNativeToEng ();
	}

	m_bBeginEdit = false;

	DXInputString::GetInstance().OffInput ();
	FLUSH_STRING_BUFFER ();

	return true;
}

void CShotCutBox::ClearEdit ()
{
	FLUSH_STRING_BUFFER ();
	m_strText.Empty ();
	m_strRenderText.Empty ();

	m_xViewPos = 0;
	m_xInsertPos = 0;
	m_xInsertPosR = 0;
}

void CShotCutBox::SetEditString ( const CString& strInput )
{
	m_strText = strInput.Left( m_nLimitInput );
	m_strRenderText = m_strText;

	m_xViewPos = 0;

	// ���� ����Ʈ ���� ��Ʈ�Ѹ� ��� ������ ġȯ�Ѵ�.
	// �׷��� ���� ���� ���, ���� ��ȯ�� �Ͼ�� �� �ϴ� ���� ����.
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