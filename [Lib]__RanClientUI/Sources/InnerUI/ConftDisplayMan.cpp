#include "pch.h"
#include "ConftDisplayMan.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "InnerInterface.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	float	CConftDisplayMan::fCHAR_HEIGHT = 100.0f;

//	초단위입니다.
const	float	CConftDisplayMan::fNUMBER_FADETIME = 0.5f;
const	float	CConftDisplayMan::fFIGHT_FADETIME = 0.75f;
const	float	CConftDisplayMan::fRESULT_FADETIME = 1.35f;
const	float	CConftDisplayMan::fGROUP_HIDE_DEAY = 0.1f;
const	int		CConftDisplayMan::nBUFFER = 1;


CConftDisplayMan::CConftDisplayMan ()
{
	for ( int i = 0; i < MAX_NUMBER_COUNT; ++i )
	{
		m_pNumber[i] = NULL;
	}
	for ( int i = 0; i < MAX_RESULT; ++i )
	{
		m_pResult[i] = NULL;
	}
}

CConftDisplayMan::~CConftDisplayMan ()
{
}

void CConftDisplayMan::CreateSubControl ()
{
	//	숫자들
	{
		CString strKeyword[MAX_NUMBER_COUNT] = 
		{
			"CONFT_NUMBER_COUNT0",
			"CONFT_NUMBER_COUNT1",
			"CONFT_NUMBER_COUNT2",
			"CONFT_NUMBER_COUNT3",
			"CONFT_NUMBER_COUNT4",
			"CONFT_NUMBER_COUNT5",
		};

		for ( int i = 0; i < MAX_NUMBER_COUNT; ++i )
		{
			m_pNumber[i] = CreateControl ( strKeyword[i].GetString () );
			m_pNumber[i]->SetVisibleSingle ( FALSE );
			m_pNumber[i]->SetTransparentOption ( TRUE );

			std::string strTemp = ID2GAMEWORD ( "CONFT_WAV_BEGIN", i );
			DxSoundLib::GetInstance()->CreateSound ( strTemp, strTemp, SFX_SOUND );
		}
	}

	//	Win
	{
		CString strKeyword[MAX_RESULT] = 
		{
			"CONFT_WIN",
			"CONFT_LOSE",
			"CONFT_DRAW"
		};

		for ( int i = 0; i < MAX_RESULT; ++i )
		{
			m_pResult[i] = CreateControl ( strKeyword[i].GetString () );
			m_pResult[i]->SetVisibleSingle ( FALSE );
			m_pResult[i]->SetTransparentOption ( TRUE );

			std::string strTemp = ID2GAMEWORD ( "CONFT_WAV_END", i );
			DxSoundLib::GetInstance()->CreateSound ( strTemp, strTemp, SFX_SOUND );
		}
	}
}

HRESULT CConftDisplayMan::FinalCleanup ()
{
	HRESULT hr = S_OK;
	hr = CUIGroup::DeleteDeviceObjects ();
	if ( FAILED ( hr ) ) GASSERT ( 0 && "DeleteDeviceObject Failed" );

	for ( int i = 0; i < MAX_NUMBER_COUNT; ++i )
	{
		std::string strTemp = ID2GAMEWORD ( "CONFT_WAV_BEGIN", i );
		DxSoundLib::GetInstance()->ReleaseSound ( strTemp );
	}

	for ( int i = 0; i < MAX_RESULT; ++i )
	{
		std::string strTemp = ID2GAMEWORD ( "CONFT_WAV_END", i );
		DxSoundLib::GetInstance()->ReleaseSound ( strTemp );
	}

	return S_OK;
}

void  CConftDisplayMan::SetHideAll ()
{
	for ( int i = 0; i < MAX_RESULT; ++i )
	{
		m_pResult[i]->SetVisibleSingle ( FALSE );
	}
	for ( int i = 0; i < MAX_NUMBER_COUNT; ++i )
	{
		m_pNumber[i]->SetVisibleSingle ( FALSE );
	}
}

void CConftDisplayMan::SetFightBegin ( const int nCount )
{
	if ( nCount < 0 || MAX_NUMBER_COUNT <= nCount )
	{
		GASSERT ( 0 && "인덱스가 범위를 넘어갑니다." );
		return ;
	}

	SetHideAll ();

	m_pNumber[nCount]->SetVisibleSingle ( TRUE );

	long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	UIRECT rcControlPos = m_pNumber[nCount]->GetLocalPos ();	
	rcControlPos.left = (float(X_RES) - rcControlPos.sizeX)/2.f;
	rcControlPos.top = (float(Y_RES) - rcControlPos.sizeY)/2.f - fCHAR_HEIGHT;
	m_pNumber[nCount]->SetLocalPos ( rcControlPos );
	m_pNumber[nCount]->SetGlobalPos ( rcControlPos );

	if ( nCount )
	{
		m_pNumber[nCount]->SetFadeTime ( fNUMBER_FADETIME );
		m_fFadeTime = fNUMBER_FADETIME + fGROUP_HIDE_DEAY;
	}
	else
	{
		m_pNumber[nCount]->SetFadeTime ( fFIGHT_FADETIME );
		m_fFadeTime = fFIGHT_FADETIME + fGROUP_HIDE_DEAY;
	}
	m_pNumber[nCount]->SetFadeOut ();

	std::string strTemp = ID2GAMEWORD ( "CONFT_WAV_BEGIN", nCount );
	DxSoundLib::GetInstance()->StopSound ( strTemp );
	DxSoundLib::GetInstance()->PlaySound ( strTemp );
}

void CConftDisplayMan::SetFightEnd ( const int nResult )
{
	SetHideAll ();

	CUIControl* pControl = NULL;
	if ( nResult < 0 || MAX_RESULT <= nResult )
	{
		GASSERT ( 0 && "인덱스가 범위를 벗어납니다." );
		return ;
	}

	pControl = m_pResult[nResult];
	pControl->SetVisibleSingle ( TRUE );

	long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	UIRECT rcControlPos = pControl->GetLocalPos ();	
	rcControlPos.left = (float(X_RES) - rcControlPos.sizeX)/2.f;
	rcControlPos.top = (float(Y_RES) - rcControlPos.sizeY)/2.f - fCHAR_HEIGHT;
	pControl->SetLocalPos ( rcControlPos );
	pControl->SetGlobalPos ( rcControlPos );

	pControl->SetFadeTime ( fRESULT_FADETIME );
	pControl->SetFadeOut ();

	m_fFadeTime = fRESULT_FADETIME + fGROUP_HIDE_DEAY;


	std::string strTemp = ID2GAMEWORD ( "CONFT_WAV_END", nResult );
	DxSoundLib::GetInstance()->StopSound ( strTemp );
	DxSoundLib::GetInstance()->PlaySound ( strTemp );
}

void CConftDisplayMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	if ( 0.0f < m_fFadeTime )
	{
		m_fFadeTime -= fElapsedTime;		
	}
	else
	{
		CInnerInterface::GetInstance().HideGroup ( GetWndID () );
	}
}

CUIControl*	CConftDisplayMan::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

float	CConftDisplayMan::GetFadeTime ()
{
	return m_fFadeTime;
}