#include "pch.h"
#include "BasicTextBox.h"
#include "UIRenderQueue.h"
#include "DxFontMan.h" //../[Lib]__Engine/Sources/DxCommon

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

long CBasicTextBox::GetLongestLine ()
{
	long lLongestLine = 0l; // MEMO

	int nTextBox = (int)m_TextBox.size ();

	for ( int x = 0; x < nTextBox; x++ )
	{
		STEXTPART& sTEXTPART = m_TextBox[x];
		DEQUETEXTLINE& dequeLine = sTEXTPART.dequeLine;

		for ( int i = 0; i < (int) dequeLine.size (); ++i )
			lLongestLine = max ( lLongestLine, dequeLine[i].strSize.cx );
	}

	return lLongestLine;
}

void	CBasicTextBox::SetTextDataEx ( int nIndex, DWORD dwData )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return ;
	}
	
	m_TextBox[nIndex].m_dwDataEx = dwData;
}

DWORD	CBasicTextBox::GetTextDataEx ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return UINT_MAX;
	}
	
	return (DWORD)m_TextBox[nIndex].m_dwDataEx;
}

void CBasicTextBox::SetTextData ( int nIndex, DWORD_PTR dwData )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return ;
	}
	
	m_TextBox[nIndex].m_dwData = dwData;	
}

DWORD_PTR CBasicTextBox::GetTextData ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		return UINT_MAX;
	}
	
	return (DWORD_PTR)m_TextBox[nIndex].m_dwData;
}

void CBasicTextBox::SetOverColor( int nIndex, D3DCOLOR TextColor )
{
	m_nOverIndex = nIndex;
	m_dwOverColor = TextColor;
}

void	CBasicTextBox::SetTextColor ( int nIndex, D3DCOLOR dwTextColor )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return;
	}

	STEXTPART& sTEXTPART = m_TextBox[nIndex];
	sTEXTPART.m_dwColor = dwTextColor;
}

D3DCOLOR	CBasicTextBox::GetTextColor ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return UINT_MAX;
	}

	const STEXTPART& sTEXTPART = m_TextBox[nIndex];
	return sTEXTPART.m_dwColor;
}

void	CBasicTextBox::SetUseTextColor ( int nIndex, BOOL bUseTextColor )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return ;
	}

	STEXTPART& sTEXTPART = m_TextBox[nIndex];
	sTEXTPART.m_bUseTextColor = bUseTextColor;
}

BOOL	CBasicTextBox::IsUseTextColor ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return FALSE;
	}

	const STEXTPART& sTEXTPART = m_TextBox[nIndex];
	return sTEXTPART.m_bUseTextColor;
}

int		CBasicTextBox::GetLineCount ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return 0;
	}

	return (int)m_TextBox[nIndex].dequeLine.size ();
}

void	CBasicTextBox::PopFrontTextLine ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return ;
	}

	m_TextBox[nIndex].dequeLine.pop_front ();
}

void CBasicTextBox::SetCurLine ( int nCurLine )
{
	if ( nCurLine < nDEFAULT_STARTLINE )	nCurLine = nDEFAULT_STARTLINE;
	
	if ( m_nVisibleLine < m_nTotalLine )
	{
		const int nEndStartIndex = m_nTotalLine - m_nVisibleLine;
		if ( nEndStartIndex < nCurLine )	nCurLine = nEndStartIndex;
	}

	if ( nCurLine == m_nCurLine ) return ;

	m_nCurLine = nCurLine;
	
	ReCalcRenderPos ();
}

CString CBasicTextBox::GetText ( int nIndex )
{
	const int nSize = (int)m_TextBox.size ();

	if ( nSize < 1 )
	{
		GASSERT ( 0 && "텍스트박스가 비어 있습니다." );
		return NULL;
	}

	if ( nIndex < 0 || nSize <= nIndex )
	{
		GASSERT ( 0 && "잘못된 인덱스입니다." );
		return NULL;
	}	

	static CString strCombine;
	strCombine.Empty ();

	STEXTPART& sTEXTPART = m_TextBox[nIndex];
	DEQUETEXTLINE& dequeTextLine = sTEXTPART.dequeLine;
	const int nLineSize = (int)dequeTextLine.size();
	for ( int i = 0; i < nLineSize; i++ )
	{
		STEXTLINE& sTextLine = dequeTextLine[i];

		VECTORTEXTWORD& vectorWord = sTextLine.vectorWord;
		const int nWordSize = (int)vectorWord.size ();
		for ( int j = 0; j < nWordSize; j++ )
		{
			STEXTWORD& sTextWord = vectorWord[j];
			strCombine += sTextWord.strWord;
		}
	}

	return strCombine;
}


void CBasicTextBox::AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	CUIGroup::AlignSubControl ( rcParentOldPos, rcParentNewPos );

	ReCalcRenderPos ();
}

void  CBasicTextBox::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );

	ReCalcRenderPos ();
}

void CBasicTextBox::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );
	
	ReCalcRenderPos ();	
}

void CBasicTextBox::SetFont ( CD3DFontPar* pFont )
{
	if ( !pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다." );
		return ;
	}

	if ( m_pFont )
	{
		GASSERT ( pFont && "폰트를 교체할 경우, 텍스트박스를 벗어나 출력하는 문제가 발생할 수 있습니다." );
		return ;
	}

	m_pFont = pFont;
	m_pFont->GetTextExtent ( " ", BLANKSIZE );
}

void CBasicTextBox::SetTextAlign ( int nAlign )
{
	m_nAlign = nAlign;

	ReCalcRenderPos ();
}

void CBasicTextBox::SetLineInterval ( const float& fLineInterval )
{
	m_fLineInterval = fLineInterval;

	ReCalcRenderPos ();
}

void CBasicTextBox::SetPartInterval ( const float& fPartInterval )
{
	m_fPartInterval = fPartInterval;

	ReCalcRenderPos ();
}

void CBasicTextBox::ClearText ()
{
	m_TextBox.clear ();
	m_nTotalLine = 0;
	m_nCurLine = nDEFAULT_STARTLINE;

	m_nImageGenID = BASE_IMAGE_CONTROLID;
	for ( int i = 0; i < (int)m_ImageList.size(); ++i )
	{
		DeleteControl ( m_ImageList[i], 0 );
	}
	m_ImageList.clear ();
}

void CBasicTextBox::SetTextImage ( int nIndex, CString strKeyword )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return;
	}

	m_nImageGenID++;
	if ( BASE_IMAGE_CONTROLID_END <= m_nImageGenID ) m_nImageGenID = BASE_IMAGE_CONTROLID;

	CUIControl* pImage = new CUIControl;
	pImage->CreateSub ( this, strKeyword.GetString(), UI_FLAG_DEFAULT, m_nImageGenID );
	pImage->InitDeviceObjects ( m_pd3dDevice );	
	pImage->RestoreDeviceObjects ( m_pd3dDevice );
	RegisterControl ( pImage );

	m_ImageList.push_back ( m_nImageGenID );

	ReCalcRenderPos ();
}

HRESULT CBasicTextBox::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	ReCalcRenderPos ();

	return S_OK;
}

bool CBasicTextBox::GetPartInfo ( const int nIndex, UIRECT& rcPart )
{
	if ( nIndex < 0 || GetCount () <= nIndex ) return false;

	const STEXTPART& sPart = m_TextBox[nIndex];
	rcPart = sPart.m_rcPart;

	return true;
}

const CBasicTextBox::STEXTPART& CBasicTextBox::GetTextPart ( const int& nIndex )
{
	if ( nIndex < 0 || GetCount () <= nIndex )
	{
		static STEXTPART sDUMMY;
		return sDUMMY;
	}

	return m_TextBox[nIndex];
}