#include "pch.h"
#include "BasicTextBox.h"
#include "UIRenderQueue.h"
#include "DxFontMan.h" //../[Lib]__Engine/Sources/DxCommon

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	float	CBasicTextBox::fDEFAULT_LINE_INTERVAL = 2.0f;
const	float	CBasicTextBox::fDEFAULT_PART_INTERVAL = 2.0f;
const	int		CBasicTextBox::nDEFAULT_STARTLINE = 0;
const	int		CBasicTextBox::nDEFAULT_VISIBLE_LINE = 1;
const	int		CBasicTextBox::nDEFAULT_LINE_LIMIT = 100;

CBasicTextBox::CBasicTextBox () :
	m_pFont ( NULL ),
	m_fLineInterval ( fDEFAULT_LINE_INTERVAL ),
	m_nCurLine ( nDEFAULT_STARTLINE ),
	m_nVisibleLine ( nDEFAULT_VISIBLE_LINE ),
	m_nAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP ),
	m_nLimitLine ( nDEFAULT_LINE_LIMIT ),
	m_nTotalLine ( 0 ),
	m_bUseOverColor ( FALSE ),
	m_nImageGenID ( BASE_IMAGE_CONTROLID ),
	m_pd3dDevice(NULL),
	m_fPartInterval ( fDEFAULT_PART_INTERVAL )
{
}

CBasicTextBox::~CBasicTextBox ()
{
}

HRESULT CBasicTextBox::RenderWord ( STEXTWORD& sTextWord )
{
	const float fPosX = sTextWord.x;
	const float fPosY = sTextWord.y;
	CString& strWord = sTextWord.strWord;
	D3DCOLOR dwColor = sTextWord.dwColor;

	if ( m_bRenderTextColor )
		dwColor = m_dwRenderTextColor;

	bool bOverIndex(m_nRenderPartIndex==m_nOverIndex);	

	if( bOverIndex && m_bUseOverColor )
		dwColor = m_dwOverColor;	

	//	페이드 인/아웃
	D3DXCOLOR TempColor( dwColor );
	TempColor.a *= GetVisibleRate ();

	// 텍스쳐에 글자를 렌더링하는 방식으로 변경 ( 준혁 )
	return m_pFont->DrawText( fPosX, fPosY, TempColor, (TCHAR*)strWord.GetString(), 0L, FALSE );
}

HRESULT CBasicTextBox::RenderLine ( VECTORTEXTWORD& vectorTextWord )
{
	HRESULT hr;

	const int nTextWord = (int)vectorTextWord.size ();

	for ( int i = 0; i < nTextWord; ++i )
	{
		hr = RenderWord ( vectorTextWord[i] );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CBasicTextBox::RenderPart ( DEQUETEXTLINE& dequeTextLine )
{
	HRESULT hr = S_OK;

	const int nTextLine = (int)dequeTextLine.size ();

	for ( ; m_nRenderLineIndex < nTextLine; ++m_nRenderLineIndex )
	{
		hr = RenderLine ( dequeTextLine[m_nRenderLineIndex].vectorWord );
		if ( FAILED ( hr ) ) return hr;

		++m_nRenderdLine;

		if ( m_nVisibleLine == m_nRenderdLine ) break;
	}

	return S_OK;
}

HRESULT	 CBasicTextBox::RenderText ()
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
		return E_FAIL;
	}

	HRESULT hr;

	const int nTextSize = (int)m_TextBox.size ();
	if ( nTextSize < 1 ) return S_FALSE;

    m_nRenderdLine = 0;
	m_nRenderPartIndex = m_nPartIndex;
	m_nRenderLineIndex = m_nLineIndex;

	for ( ; m_nRenderPartIndex < nTextSize; ++m_nRenderPartIndex )
	{
		m_bRenderTextColor = m_TextBox[m_nRenderPartIndex].m_bUseTextColor;
		m_dwRenderTextColor = m_TextBox[m_nRenderPartIndex].m_dwColor;

		hr = RenderPart ( m_TextBox[m_nRenderPartIndex].dequeLine );
		if ( FAILED ( hr ) ) return hr;

		m_nRenderLineIndex = 0;
        
		if ( m_nVisibleLine <= m_nRenderdLine ) break;
	}

	return S_OK;
}

HRESULT CBasicTextBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( !IsVisible () ) return S_OK;

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	CUIRenderQueue::Get()->RenderImmediately();
	


	hr = RenderText ();
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

int CBasicTextBox::AddOneLineText( const CString& strText, bool bUseEllipsis /* = FALSE */, const D3DCOLOR& TextColor /* = NS_UITEXTUTIL::ENABLE */ )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
		return 0;
	}
	
    float fWidth = GetLocalPos().sizeX;

	if( bUseEllipsis ) fWidth -= 20.0f;

	//	문자열 자르기
	NS_UITEXTUTIL::ClearCRLFText ();
	NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF ( strText, fWidth, m_pFont );

	int nSplitLine = (int)MLText.size ();

	STEXTPART sTextPart;

	//	데이타로 집어넣기
	STEXTLINE sTextLine;

	SIZE strSize;

	if( nSplitLine >= 2 && bUseEllipsis ) 
		MLText[0].strLine += "...";

	m_pFont->GetTextExtent ( MLText[0].strLine.GetString(), strSize );

	sTextLine.strSize = strSize;
	sTextLine.bLASTLINE = MLText[0].bLASTLINE;
	if ( m_nAlign == TEXT_ALIGN_BOTH_X )
	{				
		NS_UITEXTUTIL::ClearWordText ();
		NS_UITEXTUTIL::STRINGVECTOR& WordText = NS_UITEXTUTIL::GetWordText ( MLText[0].strLine );

		for ( int k = 0; k < (int)WordText.size(); ++k )
		{
			STEXTWORD sTextWord;

			sTextWord.strWord = WordText[k];
			sTextWord.dwColor = TextColor;

			if ( WordText[k].GetLength () )
			{
				m_pFont->GetTextExtent ( WordText[k].GetString(), strSize );
				m_pFont->PushText( WordText[k].GetString() );
			}
			else
			{
				strSize.cx = 0;
				strSize.cy = BLANKSIZE.cy;
			}

			sTextWord.strSize = strSize;

			sTextLine.vectorWord.push_back ( sTextWord );
		}

		m_nTotalLine++;
		sTextPart.dequeLine.push_back ( sTextLine );
	}
	else
	{
		STEXTWORD sTextWord;
		sTextWord.strWord = MLText[0].strLine;
		sTextWord.dwColor = TextColor;

		sTextWord.strSize = strSize;

		if( MLText[0].strLine.GetLength() )
			m_pFont->PushText( MLText[0].strLine.GetString() );

		sTextLine.vectorWord.push_back ( sTextWord );
		sTextPart.dequeLine.push_back ( sTextLine );

		m_nTotalLine++;
	}	

	m_TextBox.push_back ( sTextPart );	

	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

int	CBasicTextBox::AddText ( const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
		return 0;
	}

	//	문자열 자르기
	NS_UITEXTUTIL::ClearCRLFText ();
	NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF ( strText, GetLocalPos().sizeX, m_pFont );

	int nSplitLine = (int)MLText.size ();
	
	STEXTPART sTextPart;

	//	데이타로 집어넣기
	for ( int i = 0; i < nSplitLine; ++i )
	{		
		STEXTLINE sTextLine;

		if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength () )
		{
			//	마지막라인이 빈 경우.
		}
		else
		{
			SIZE strSize;
			m_pFont->GetTextExtent ( MLText[i].strLine.GetString(), strSize );

			sTextLine.strSize = strSize;
			sTextLine.bLASTLINE = MLText[i].bLASTLINE;
			if ( m_nAlign == TEXT_ALIGN_BOTH_X )
			{				
				NS_UITEXTUTIL::ClearWordText ();
				NS_UITEXTUTIL::STRINGVECTOR& WordText = NS_UITEXTUTIL::GetWordText ( MLText[i].strLine );

				for ( int k = 0; k < (int)WordText.size(); ++k )
				{
					STEXTWORD sTextWord;

					sTextWord.strWord = WordText[k];
					sTextWord.dwColor = TextColor;

					if ( WordText[k].GetLength () )
					{
						m_pFont->GetTextExtent ( WordText[k].GetString(), strSize );
						m_pFont->PushText( WordText[k].GetString() );
					}
					else
					{
						strSize.cx = 0;
						strSize.cy = BLANKSIZE.cy;
					}

					sTextWord.strSize = strSize;

					sTextLine.vectorWord.push_back ( sTextWord );
				}
				
				m_nTotalLine++;
				sTextPart.dequeLine.push_back ( sTextLine );
			}
			else
			{
				STEXTWORD sTextWord;
				sTextWord.strWord = MLText[i].strLine;
				sTextWord.dwColor = TextColor;

				sTextWord.strSize = strSize;

				if( MLText[i].strLine.GetLength() )
					m_pFont->PushText( MLText[i].strLine.GetString() );

				sTextLine.vectorWord.push_back ( sTextWord );
				sTextPart.dequeLine.push_back ( sTextLine );

				m_nTotalLine++;
			}
		}
	}	

	m_TextBox.push_back ( sTextPart );

	//	제한 라인 범위가 넘을 경우, 잘라냄
	for ( ; m_nLimitLine < m_nTotalLine; --m_nTotalLine )
	{		
		STEXTPART& sDelTextPart = m_TextBox.front ();
		DEQUETEXTLINE& dequeLine = sDelTextPart.dequeLine;

		dequeLine.pop_front ();

		//	만약, 문단에 실제 글이 없을 경우,
		//	문단을 날린다.
		if ( dequeLine.size () < 1 )
		{
			if ( m_ImageList.size () )
			{
				DeleteControl ( m_ImageList[0], 0 );
				m_ImageList.pop_front ();
			}
			m_TextBox.pop_front ();
		}
	}
	
	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

int CBasicTextBox::AddTextWithCRLF ( const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
		return 0;
	}

	//	문자열 자르기
	NS_UITEXTUTIL::ClearCRLFText ();
	NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithCRLF ( strText, GetLocalPos().sizeX, m_pFont );

	int nSplitLine = (int)MLText.size ();

	STEXTPART sTextPart;

	//	데이타로 집어넣기
	for ( int i = 0; i < nSplitLine; ++i )
	{		
		STEXTLINE sTextLine;

		if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength () )
		{
			//	마지막라인이 빈 경우.
		}
		else
		{
			SIZE strSize;
			m_pFont->GetTextExtent ( MLText[i].strLine.GetString(), strSize );

			sTextLine.strSize = strSize;
			sTextLine.bLASTLINE = MLText[i].bLASTLINE;
			if ( m_nAlign == TEXT_ALIGN_BOTH_X )
			{				
				NS_UITEXTUTIL::ClearWordText ();
				NS_UITEXTUTIL::STRINGVECTOR& WordText = NS_UITEXTUTIL::GetWordText ( MLText[i].strLine );

				for ( int k = 0; k < (int)WordText.size(); ++k )
				{
					STEXTWORD sTextWord;

					sTextWord.strWord = WordText[k];
					sTextWord.dwColor = TextColor;

					if ( WordText[k].GetLength () )
					{
						m_pFont->GetTextExtent ( WordText[k].GetString(), strSize );
						m_pFont->PushText( WordText[k].GetString() );
					}
					else
					{
						strSize.cx = 0;
						strSize.cy = BLANKSIZE.cy;
					}

					sTextWord.strSize = strSize;

					sTextLine.vectorWord.push_back ( sTextWord );
				}

				m_nTotalLine++;
				sTextPart.dequeLine.push_back ( sTextLine );
			}
			else
			{
				STEXTWORD sTextWord;
				sTextWord.strWord = MLText[i].strLine;
				sTextWord.dwColor = TextColor;

				sTextWord.strSize = strSize;

				if( MLText[i].strLine.GetLength() )
					m_pFont->PushText( MLText[i].strLine.GetString() );

				sTextLine.vectorWord.push_back ( sTextWord );
				sTextPart.dequeLine.push_back ( sTextLine );

				m_nTotalLine++;
			}
		}
	}	

	m_TextBox.push_back ( sTextPart );

	//	제한 라인 범위가 넘을 경우, 잘라냄
	for ( ; m_nLimitLine < m_nTotalLine; --m_nTotalLine )
	{		
		STEXTPART& sDelTextPart = m_TextBox.front ();
		DEQUETEXTLINE& dequeLine = sDelTextPart.dequeLine;

		dequeLine.pop_front ();

		//	만약, 문단에 실제 글이 없을 경우,
		//	문단을 날린다.
		if ( dequeLine.size () < 1 )
		{
			if ( m_ImageList.size () )
			{
				DeleteControl ( m_ImageList[0], 0 );
				m_ImageList.pop_front ();
			}
			m_TextBox.pop_front ();
		}
	}

	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

int	CBasicTextBox::AddString ( int nIndex, const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
		return -1;
	}

	if ( (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "인덱스가 범위를 넘어섭니다." );
		return -1;
	}

	if ( !strText.GetLength () )
	{
		GASSERT ( 0 && "문자열이 없습니다." );
		return -1;
	}

	CString strTextCopy = strText;

	STEXTPART& sTextPart = m_TextBox[nIndex];	

	{
		STEXTLINE& sTextLine = sTextPart.dequeLine.back ();

		//	기존 문자열 마지막 라인에 글 붙이기		
		float fOldSize = (float) sTextLine.strSize.cx;

		//	기존의 문자열에 붙일 만큼만 자르기
		NS_UITEXTUTIL::ClearCRLFText ();
		NS_UITEXTUTIL::MULTILINETEXT& MLText =
			NS_UITEXTUTIL::GetMLTextWithoutCRLF ( strTextCopy, GetLocalPos().sizeX - fOldSize, m_pFont );

		if ( MLText.size () )
		{
			STEXTWORD sTextWord;
			sTextWord.strWord = MLText[0].strLine;
			sTextWord.dwColor = TextColor;
			m_pFont->GetTextExtent ( sTextWord.strWord.GetString(), sTextWord.strSize );

			if( MLText[0].strLine.GetLength() )
				m_pFont->PushText( MLText[0].strLine.GetString() );

			sTextLine.vectorWord.push_back ( sTextWord );
			sTextLine.strSize.cx += sTextWord.strSize.cx;
			sTextLine.strSize.cy = sTextWord.strSize.cy;

			int nCutPos = strTextCopy.GetLength () - sTextWord.strWord.GetLength ();
			strTextCopy = strTextCopy.Right ( nCutPos );
		}		
	}

	if ( strTextCopy.GetLength () )
	{
		//	기존의 문자열에 붙일 만큼만 자르기
		NS_UITEXTUTIL::ClearCRLFText ();
		NS_UITEXTUTIL::MULTILINETEXT& MLText =
			NS_UITEXTUTIL::GetMLTextWithoutCRLF ( strTextCopy, GetLocalPos().sizeX, m_pFont );

		int nSplitLine = (int)MLText.size ();
		
		//	데이타로 집어넣기
		for ( int i = 0; i < nSplitLine; ++i )
		{		
			STEXTLINE sTextLine;
			STEXTWORD sTextWord;

			sTextWord.strWord = MLText[i].strLine;
			sTextWord.dwColor = TextColor;
			
			if ( (i - 1 == nSplitLine) && !sTextWord.strWord.GetLength () )
			{
				//	마지막라인이 빈 경우.
			}
			else
			{
				m_pFont->GetTextExtent ( sTextWord.strWord.GetString(), sTextWord.strSize );

				if( MLText[i].strLine.GetLength() )
					m_pFont->PushText( MLText[i].strLine.GetString() );

				//	아직은 word가 별 의미를 가지지 못한다.
				//	따라서 항상 1개만 존재한다고 가정하고, 0번 인덱스를 사용한다고
				//	약속한다.
				sTextLine.vectorWord.push_back ( sTextWord );

				sTextLine.strSize.cx = sTextWord.strSize.cx;
				sTextLine.strSize.cy = sTextWord.strSize.cy;
				sTextPart.dequeLine.push_back ( sTextLine );

				m_nTotalLine++;

				//	제한 라인이 사용되고, 걸리는 경우
	//			if ( m_nLimitLine && m_nLimitLine == i )	break;
			}
		}	
	}

//	m_TextBox.push_back ( sTextString );

	//	제한 라인 범위가 넘을 경우, 잘라냄
	for ( ; m_nLimitLine < m_nTotalLine; --m_nTotalLine )
	{		
		sTextPart = m_TextBox.front ();
		DEQUETEXTLINE& dequeLine = sTextPart.dequeLine;

		dequeLine.pop_front ();

		//	만약, 문단에 실제 글이 없을 경우,
		//	문단을 날린다.
		if ( dequeLine.size () < 1 )
		{
			if ( m_ImageList.size () )
			{
				DeleteControl ( m_ImageList[0], 0 );
				m_ImageList.pop_front ();
			}
			m_TextBox.pop_front ();
		}
	}

	ReCalcRenderPos ();

	return (int) m_TextBox.size () - 1;
}

void	CBasicTextBox::SetText ( const CString& strText, const D3DCOLOR& TextColor )
{
	if(  1 == GetCount () )
	{

		if( strText == GetText( 0 ) ) return;
	}

	ClearText ();
	AddText ( strText, TextColor );
}

void CBasicTextBox::SetOneLineText ( const CString& strText, const D3DCOLOR& TextColor )
{
	if ( 1 == GetCount () )
	{
		CString& strOldText = GetText ( 0 );
		if ( strOldText == strText ) return ;
	}

	ClearText ();
	AddText ( strText, TextColor );
}

int CBasicTextBox::SetTextNoSplit ( const CString& strText, const D3DCOLOR& TextColor )
{
	if(  1 == GetCount () )
	{
		if( strText == GetText( 0 ) ) return -1;
	}

	ClearText ();	
	return AddTextNoSplit( strText, TextColor );
}

int CBasicTextBox::AddTextNoSplit ( const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
		return 0;
	}

	STEXTPART sTEXTPART;
	STEXTLINE sTEXTLINE;

	{
		STEXTWORD sTEXTWORD;
		sTEXTWORD.strWord = strText;
		sTEXTWORD.dwColor = TextColor;
		m_pFont->GetTextExtent ( sTEXTWORD.strWord.GetString(), sTEXTWORD.strSize );

		if( strText.GetLength() )
			m_pFont->PushText( strText.GetString() );

		sTEXTLINE.vectorWord.push_back ( sTEXTWORD );

		sTEXTLINE.strSize.cx = sTEXTWORD.strSize.cx;
		sTEXTLINE.strSize.cy = sTEXTWORD.strSize.cy;
	}

	sTEXTPART.dequeLine.push_back ( sTEXTLINE );
	m_TextBox.push_back ( sTEXTPART );

	m_nTotalLine++;

	//	제한 라인 범위가 넘을 경우, 잘라냄
	for ( ; m_nLimitLine < m_nTotalLine; --m_nTotalLine )
	{		
		sTEXTPART = m_TextBox.front ();
		DEQUETEXTLINE& dequeLine = sTEXTPART.dequeLine;

		dequeLine.pop_front ();

		//	만약, 문단에 실제 글이 없을 경우,
		//	문단을 날린다.
		if ( dequeLine.size () < 1 )
		{
			if ( m_ImageList.size () )
			{
				DeleteControl ( m_ImageList[0], 0 );
				m_ImageList.pop_front ();
			}
			m_TextBox.pop_front ();
		}
	}
    
	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

void CBasicTextBox::CalcMaxHEIGHT ( float* pHEIGHT, int* pVISIBLELINE )
{
	float& fTextHeight = *pHEIGHT;
	int& nVisibleLine = *pVISIBLELINE;

	const UIRECT& rcGlobalPos = GetGlobalPos ();

	//	초기화
	fTextHeight = 0.0f;
	nVisibleLine = 0;

	int nPartSize = (int) m_TextBox.size ();
	for ( int i = nPartSize-1; 0 <= i; --i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int) sDequeTextLine.size ();

		//	라인들 중 최대값을 뽑아옴
		for ( int j = nLineCount-1; 0 <= j; --j )
		{
			STEXTLINE& sTextLine = sDequeTextLine[j];

			//	한 라인 높이
			float fLINE_HEIGHT = (float)sTextLine.strSize.cy;

			//	한 라인 뿌릴 수 있는가?
			if ( rcGlobalPos.sizeY <= (fTextHeight+fLINE_HEIGHT) )
			{
				if ( !nVisibleLine )
				{
//					CDebugSet::ToLogFile ( "[%s] 사이즈가 너무 작습니다.", GetControlName() );

					//	강제로 한 라인은 보여지게 설정함
					nVisibleLine = 1;
				}

				return ;
			}

			fTextHeight += fLINE_HEIGHT;

			//	보여지는 최대 줄 수
			++nVisibleLine;

			//	행 간격
			if ( j != 0 )
			{
				float fLINE_GAP = GetLineInterval ();
				fTextHeight += fLINE_GAP;
			}
		}			

		//	문단 간격
		if ( i != 0 )
		{
			float fPART_GAP = GetPartInterval ();
			fTextHeight += fPART_GAP;
		}
	}
}

void CBasicTextBox::CalcMaxHEIGHT ( float* pHEIGHT, int* pVISIBLELINE, int nPartIndexCOPY, int nLineIndexCOPY )
{
	float& fTextHeight = *pHEIGHT;
	int& nVisibleLine = *pVISIBLELINE;

	const UIRECT& rcGlobalPos = GetGlobalPos ();

	//	초기화
	fTextHeight = 0.0f;
	nVisibleLine = 0;

	int nPartSize = (int) m_TextBox.size ();
	for ( int i = nPartIndexCOPY; i < nPartSize; ++i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int) sDequeTextLine.size ();

		//	라인들 중 최대값을 뽑아옴
		for ( int j = nLineIndexCOPY; j < nLineCount; ++j )
		{
			STEXTLINE& sTextLine = sDequeTextLine[j];

			//	한 라인 높이
			float fLINE_HEIGHT = (float)sTextLine.strSize.cy;

			//	한 라인 뿌릴 수 있는가?
			if ( rcGlobalPos.sizeY < (fTextHeight+fLINE_HEIGHT) )
			{
				if ( !nVisibleLine )
				{
//					CDebugSet::ToLogFile ( "[%s] 사이즈가 너무 작습니다.", GetControlName() );

					//	강제로 한 라인은 보여지게 설정함
					nVisibleLine = 1;
				}

				return ;
			}

			fTextHeight += fLINE_HEIGHT;

			//	보여지는 최대 줄 수
			++nVisibleLine;

			//	행 간격
			if ( (j+1) != nLineCount )
			{
				float fLINE_GAP = GetLineInterval ();
				fTextHeight += fLINE_GAP;
			}
		}			

		//	문단 간격
		if ( (i+1) != nPartSize )
		{
			float fPART_GAP = GetPartInterval ();
			fTextHeight += fPART_GAP;
		}

		nLineIndexCOPY = 0;
	}
}

float CBasicTextBox::CalcMaxHEIGHT ( int nBeginLine, int nEndLine )
{
	if ( nBeginLine < 0 ) return 0.0f;	
	if ( nEndLine < nBeginLine ) return 0.0f;

	//	초기화
	float fTextHeight = 0.0f;
	int nBEGIN_PART = 0;
	int nBEGIN_LINE = 0;
	int nAllLineCountEnd = nEndLine - nBeginLine;
	int nAllLineCount = 0;

	if ( !FindStartIndex ( nBEGIN_PART, nBEGIN_LINE, nBeginLine ) ) return 0.0f;

	int nPartSize = (int) m_TextBox.size ();
	for ( int i = nBEGIN_PART; i < nPartSize; ++i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int) sDequeTextLine.size ();

		//	라인들 중 최대값을 뽑아옴
		for ( int j = nBEGIN_LINE; j < nLineCount; ++j )
		{
			STEXTLINE& sTextLine = sDequeTextLine[j];

			//	한 라인 높이			
			fTextHeight += (float)sTextLine.strSize.cy;

			//	라인 계산
			++nAllLineCount;
			if ( nAllLineCountEnd < nAllLineCount ) return fTextHeight;

			//	행 간격
			if ( (j+1) != nLineCount ) fTextHeight += m_fLineInterval;
		}	

		//	문단 간격
		if ( (i+1) != nPartSize ) fTextHeight += m_fPartInterval;

		nBEGIN_LINE = 0;
	}

	return fTextHeight;
}

void CBasicTextBox::ReCalcRenderPos ()
{	
	const UIRECT& rcGlobalPos = GetGlobalPos ();

	//	뿌릴 문자열 위치 확인	
	if ( !(m_nAlign & TEXT_ALIGN_BOTTOM) )
	{
		if ( !FindStartIndex ( m_nPartIndex, m_nLineIndex, m_nCurLine ) ) return ;
	}

	//	아이콘 리셋
	for ( int nImage = 0; nImage < (int)m_ImageList.size (); ++nImage )
	{
		CUIControl* pImage = FindControl ( m_ImageList[nImage] );
		if ( pImage ) pImage->SetVisibleSingle ( FALSE );
	}

	const int nVisibleLineBack = m_nVisibleLine;

	//	뿌릴 최대 Y 사이즈 계산
	float fTextHeight = 0.0f;
	if ( m_nAlign & TEXT_ALIGN_BOTTOM )
	{
		CalcMaxHEIGHT ( &fTextHeight, &m_nVisibleLine );
		int nEND_LINE = m_nTotalLine - m_nVisibleLine;
		if ( nEND_LINE < m_nCurLine ) m_nCurLine = nEND_LINE;
		if ( !FindStartIndex ( m_nPartIndex, m_nLineIndex, m_nCurLine ) ) return ;
	}
	else
	{
		CalcMaxHEIGHT ( &fTextHeight, &m_nVisibleLine, m_nPartIndex, m_nLineIndex );
	}

	//	복사본
	int nPartIndex = m_nPartIndex;
	int nLineIndex = m_nLineIndex;

	//	X축 기본 포지션
	float fLEFT = (float)ceil(rcGlobalPos.left);

	//	Y축 기본 포지션
	float fTOP = (float)ceil(rcGlobalPos.top);			
	if ( m_nAlign & TEXT_ALIGN_TOP )
		;
	else if ( m_nAlign & TEXT_ALIGN_BOTTOM )
		fTOP = (float)ceil(rcGlobalPos.bottom - fTextHeight);
	else if ( m_nAlign & TEXT_ALIGN_CENTER_Y )
		fTOP = (float)ceil(rcGlobalPos.top + (rcGlobalPos.sizeY-fTextHeight) * 0.5f);
    
	bool bBREAK = false;
	int nCurrentLine = 0;
	int nPartSize = (int) m_TextBox.size ();
	float fTEXT_POS_Y = fTOP;
	for ( int i = nPartIndex; i < nPartSize; ++i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		UIRECT& rcPart = sTextPart.m_rcPart;
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int)sDequeTextLine.size ();

		D3DXVECTOR2 vSIZE ( 0.0f, 0.0f );
		D3DXVECTOR2 vBEGIN( 0.0f, 0.0f );

		//	NOTE
		//		새로운 라인인가?
		//		문장이 엔터 또는 널을 만나 끝난 문장인가?		
		bool bNEWLINE = true;
		for ( int j = nLineIndex; j < nLineCount; ++j )
		{
			++nCurrentLine;
			if ( m_nVisibleLine < nCurrentLine )
			{
				bBREAK = true;
				break;
			}

			STEXTLINE& sTextLine = sDequeTextLine[j];
			float fSizeX = (float) sTextLine.strSize.cx;
			float fSizeY = (float) sTextLine.strSize.cy;

			//	X 시작
			float fPosX = fLEFT;
			if ( m_nAlign & TEXT_ALIGN_LEFT  )
				;
			else if ( m_nAlign & TEXT_ALIGN_RIGHT )
				fPosX = (float)ceil(rcGlobalPos.right - fSizeX);
			else if ( m_nAlign & TEXT_ALIGN_CENTER_X )
				fPosX = (float)ceil(fLEFT + (rcGlobalPos.sizeX - fSizeX) / 2.0f);            
			
			//	Y 시작
			float fPosY = fTEXT_POS_Y;

			fTEXT_POS_Y += fSizeY;				//	글이 있는 줄
			if ( j != (nLineCount-1) ) fTEXT_POS_Y += GetLineInterval();	//	줄간

			//	파트의 첫번째 줄에서 아이콘 출력
			if ( j == 0 )
			{
				if ( i < (int) m_ImageList.size () )
				{
					CUIControl* pImage = FindControl ( m_ImageList[i] );
					if ( pImage )
					{
						const UIRECT& rcImagePos = pImage->GetGlobalPos ();
						pImage->SetGlobalPos (D3DXVECTOR2(fPosX-rcImagePos.sizeX-5.0f,fPosY));
						pImage->SetVisibleSingle ( TRUE );					
					}
				}
			}


			VECTORTEXTWORD& vectorTextWord = sTextLine.vectorWord;
			if ( m_nAlign & TEXT_ALIGN_BOTH_X && 1 < vectorTextWord.size() )
			{
				long lGAP = 0;
				long OneGap = 0;
				int nBlankCount = (int)vectorTextWord.size() - 1;
				if ( !nBlankCount ) nBlankCount = 1;

				//	NOTE
				//		의도적으로 첫칸을 띄운것이 아닌데,
				//		첫칸이 빈경우, 출력하지 않는다.
				if ( !vectorTextWord[0].strWord.GetLength() && !bNEWLINE )
				{
					lGAP += BLANKSIZE.cx;
					nBlankCount--;
					if ( nBlankCount == 0 ) nBlankCount = 1;
				}

				//	NOTE
				//		마지막 칸에 빈칸이 들어 있을 경우 출력하지 않는다.				
				if ( !vectorTextWord[vectorTextWord.size()-1].strWord.GetLength() )
				{
					lGAP += BLANKSIZE.cx;
					nBlankCount--;
					if ( nBlankCount == 0 ) nBlankCount = 1;
				}

				//	NOTE
				//		문장이 출력되고 남은 부분의 여백에 대해서
				//		얼마만큼으로 나눠 각각의 공백에 적용시킬것인지에 대해
				//		계산한다.
				lGAP += (long(ceil(rcGlobalPos.sizeX)) - sTextLine.strSize.cx);				
				OneGap = (long)ceil(float(lGAP) / float(nBlankCount));

				for ( int k = 0; k < (int) vectorTextWord.size (); ++k )
				{
					STEXTWORD& sTextWord = vectorTextWord[k];

					if ( k == 0 && j == nLineIndex )
					{
						vBEGIN.x = fPosX;
						vBEGIN.y = fPosY;
					}

					//	NOTE
					//		의도적으로 첫칸을 띄운것이 아닌데,
					//		첫칸이 빈 경우, 무시한다.
					if ( k == 0 && !bNEWLINE )
					{
						if ( !sTextWord.strWord.GetLength () )
						{
							continue;
						}
					}

					//	NOTE
					//		마지막 칸이 빈 경우 출력하지 않는다.
					bool bLINE_END = (k == (vectorTextWord.size () - 1));
					if ( bLINE_END )
					{
						if ( !sTextWord.strWord.GetLength () )
						{
							continue;
						}
					}
					
					sTextWord.x = fPosX;
					sTextWord.y = fPosY;

					fPosX += sTextWord.strSize.cx;
					fPosX += BLANKSIZE.cx;

					//	NOTE
					//		실질적으로 CR/LF를 만나 개행되려고 하는가?
					//		bLASTLINE이 true인 경우, 문장이 박스에 꽉차지 않고
					//		종료된다는 것을 의미한다.
					if ( !sTextLine.bLASTLINE )
					{
						if ( 0 < lGAP )
						{
							//	만약, 마지막 줄이면 '갭'을 다 소비한다.
							if ( bLINE_END )
							{
								fPosX += lGAP;
								lGAP = 0;
							}
							else
							{
								if ( OneGap < lGAP )
								{
									fPosX += OneGap;
									lGAP -= OneGap;
								}
								else
								{
									fPosX += lGAP;
									lGAP = 0;
								}
							}
						}

						bNEWLINE = false;
					}
					else
					{
						bNEWLINE = true;
					}
				}
			}
			else
			{
				for ( int k = 0; k < (int) vectorTextWord.size (); ++k )
				{
					STEXTWORD& sTextWord = vectorTextWord[k];
					sTextWord.x = fPosX;
					sTextWord.y = fPosY;

					if ( k == 0 && j == nLineIndex )
					{
						vBEGIN.x = fPosX;
						vBEGIN.y = fPosY;
					}

					fPosX += sTextWord.strSize.cx;
				}
			}

			vSIZE.x = max(sTextLine.strSize.cx, vSIZE.x);
			vSIZE.y += fSizeY;

			if ( j != (nLineCount-1) )
			{
				vSIZE.y += m_fLineInterval;
			}
		}

		if ( bBREAK ) return;

		if ( i != (nPartSize-1) ) fTEXT_POS_Y += GetPartInterval ();	//	파트 인터벌

		rcPart = UIRECT ( vBEGIN.x, vBEGIN.y, vSIZE.x, vSIZE.y );

		nLineIndex = 0;
	}	
}

BOOL CBasicTextBox::FindStartIndex ( int& nPartIndex, int& nLineIndex, const int nCurLine )
{
	if ( m_nTotalLine < 1 )	return FALSE;

	const int nPartSize = (int)m_TextBox.size ();
    
	int nCurLineAmount = 0;
	for ( int nPartIterIndex = 0; nPartIterIndex < nPartSize; ++nPartIterIndex )
	{
		const STEXTPART& sTextPart = m_TextBox[nPartIterIndex];

        const int nLineSize = (int)sTextPart.dequeLine.size ();
		nCurLineAmount += nLineSize;

		if ( nCurLine < nCurLineAmount )
		{
			nPartIndex = nPartIterIndex;			
			nLineIndex = nCurLine - (nCurLineAmount - nLineSize);

			return TRUE;
		}
	}

	return FALSE;
}

HRESULT CBasicTextBox::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}