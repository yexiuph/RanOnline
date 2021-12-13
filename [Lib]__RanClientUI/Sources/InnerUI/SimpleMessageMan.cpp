#include "pch.h"
#include "SimpleMessageMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "d3dfont.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#include "UITextControl.h"
#include "../[Lib]__EngineUI/Sources/UIMan.h"
#include "DxGlobalStage.h"
#include "GameTextControl.h"
#include "SUBPATH.h"
#include "InnerInterface.h"
#include "BaseString.h"
#include "GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	float	CSimpleMessageMan::fLIFE_TIME = 4.0f;

CSimpleMessageMan::CSimpleMessageMan ()	
{
}

CSimpleMessageMan::~CSimpleMessageMan ()
{
}

void CSimpleMessageMan::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "SIMPLE_MESSAGE_MAN_TEXT" );
	pTextBox->SetFont ( pFont );
	RegisterControl ( pTextBox );
	m_pInfoDisplay = pTextBox;

	CUIControl*	pDummy = new CUIControl;
	pDummy->CreateSub ( this, "SIMPLE_MESSAGE_MAN_TEXT" );
	RegisterControl ( pDummy );
	m_pInfoDisplayDummy = pDummy;

	CString strFullPath = DxGlobalStage::GetInstance().GetAppPath();
	strFullPath += SUBPATH::GLOGIC_FILE;
	strFullPath += ID2GAMEWORD("TIP_FILE");
	LoadMessage ( strFullPath );
}

void CSimpleMessageMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fLifeTime -= fElapsedTime;

	if ( m_fLifeTime < 0.0f )
	{
		ClearText ();
	}

	if ( m_fLifeTime < -3.0f )
	{
		if ( !m_vecMESSAGE.size () )
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			return ;
		}

		const int MAX_INDEX = static_cast<int>(m_vecMESSAGE.size ());
		srand ( (UINT)time( NULL ) );
		int nIndex = rand () % MAX_INDEX;
		AddText ( m_vecMESSAGE[nIndex], NS_UITEXTCOLOR::WHITE );
	}

	const long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	//	NOTE
	//		포지션 설정
	{
		bool bOVER_ONELINE = false;
		if ( 1 < m_pInfoDisplay->GetTotalLine () ) bOVER_ONELINE = true;

		const UIRECT& rcOriginPos = m_pInfoDisplayDummy->GetGlobalPos ();

		D3DXVECTOR2 vNewPos;
		vNewPos.x = (X_RES - rcOriginPos.sizeX) / 2.0f;
		vNewPos.y = 70.0f;

		if ( bOVER_ONELINE )
		{
			vNewPos.x += 60.0f;
			m_pInfoDisplay->SetTextAlign ( TEXT_ALIGN_LEFT );
		}
		else
		{
			m_pInfoDisplay->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		}

		m_pInfoDisplay->SetGlobalPos ( vNewPos );			
	}
}

void	CSimpleMessageMan::ClearText ()
{
	m_pInfoDisplay->ClearText ();
}

void CSimpleMessageMan::AddText ( CString strMessage, D3DCOLOR dwMessageColor )
{
	m_pInfoDisplay->AddText ( strMessage, dwMessageColor );

	m_fLifeTime = fLIFE_TIME * m_pInfoDisplay->GetLineCount(0);
}

bool CSimpleMessageMan::LoadMessage ( const CString& strFileName )
{
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s( strFileName.GetString(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT );
	StringCchCat ( fname, _MAX_EXT, ext );

	std::auto_ptr<CBaseString> pBString(GLOGIC::openfile_basestring(GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
																	strFileName.GetString(), 
																	fname,
																	false,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if ( !pBString.get() )
	{
		CDebugSet::ToLogFile ( "ERROR : CSimpleMessageMan::LoadMessage(), File Open %s", fname );
		return false;
	}

	CBaseString &cBString = *pBString;

	CString strLine;
	CString strFullText;
	while ( cBString.GetNextLine(strLine) )
	{
		if ( strLine == ';' )
		{
			int nLength = strFullText.GetLength ();
			if ( 2<nLength && strFullText[nLength-2] == '\r' && strFullText[nLength-1] == '\n' )
				strFullText = strFullText.Left(nLength-2);

			nLength = strFullText.GetLength ();
			if ( nLength )
			{
				m_vecMESSAGE.push_back ( strFullText );
				strFullText.Empty ();
			}
		}
		else
		{			
			strLine += "\r\n";
			strFullText += strLine;
		}
	}

	if ( strFullText.GetLength () )	m_vecMESSAGE.push_back ( strFullText );

	return true;
}