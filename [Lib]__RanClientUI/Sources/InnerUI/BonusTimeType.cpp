#include "pch.h"
#include "BonusTimeType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBonusTimeType::CBonusTimeType () :
    m_bPLAY ( false )
{
}

CBonusTimeType::~CBonusTimeType ()
{
}

void CBonusTimeType::CreateSubControl ( CString strKeyword )
{
	float fBEGIN_TIME[XCONTROL_SIZE] = { 0.0f, 0.1f, 0.2f, 3.1f, 3.2f  };
	float fEND_TIME[XCONTROL_SIZE] = { 0.2f, 0.3f, 3.2f, 3.3f, 3.4f };
	float fRATIO_ARRAY[XCONTROL_SIZE] = { 2.4f, 1.7f, 1.0f, 0.7f, 0.5f };

	const UIRECT& rcParentLocalPos = GetLocalPos ();

	for ( int nIndex = 0; nIndex < XCONTROL_SIZE; ++nIndex )
	{
		m_pXControl[nIndex] = new CUIControl;
		m_pXControl[nIndex]->CreateSub ( this, strKeyword.GetString(), UI_FLAG_DEFAULT, X1CONTROL + nIndex );
		m_pXControl[nIndex]->SetVisibleSingle ( FALSE );
		m_pXControl[nIndex]->SetTransparentOption ( TRUE );
		RegisterControl ( m_pXControl[nIndex] );

		const UIRECT& rcLocalPos = m_pXControl[nIndex]->GetLocalPos ();
		const float fRATIO = fRATIO_ARRAY[nIndex];
		float fSIZEX = rcLocalPos.sizeX*fRATIO;
		float fSIZEY = rcLocalPos.sizeY*fRATIO;
		float fLEFT	= (rcParentLocalPos.sizeX - fSIZEX)/2.0f;
		float fTOP	= (rcParentLocalPos.sizeY - fSIZEY)/2.0f;

		m_pXControl[nIndex]->SetLocalPos ( UIRECT(fLEFT,fTOP,fSIZEX,fSIZEY) );
		m_pXControl[nIndex]->SetGlobalPos( UIRECT(fLEFT,fTOP,fSIZEX,fSIZEY) );

		m_fBEGIN_TIME[nIndex] = fBEGIN_TIME[nIndex];
		m_fEND_TIME[nIndex] = fEND_TIME[nIndex];
	}
}

void	CBonusTimeType::START()
{
	RESET ();
	m_bPLAY = true;
}

void	CBonusTimeType::RESET()
{
	m_fPlayedTime = 0.0f;

	for ( int nIndex = 0; nIndex < XCONTROL_SIZE; ++nIndex )
	{
		m_pXControl[nIndex]->SetVisibleSingle ( FALSE );
	}
}

void CBonusTimeType::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( ISPLAYING() )
	{
		m_fPlayedTime += fElapsedTime;

		bool bIsVISIBLE = false;
		for ( int nIndex = 0; nIndex < XCONTROL_SIZE; ++nIndex )
		{
			if ( m_fBEGIN_TIME[nIndex] <= m_fPlayedTime && m_fPlayedTime <= m_fEND_TIME[nIndex] )
			{
				if ( !m_pXControl[nIndex]->IsVisible() ) m_pXControl[nIndex]->SetVisibleSingle ( TRUE );				
				if ( nIndex != (XCONTROL_SIZE-1) ) m_pXControl[nIndex]->SetDiffuseAlpha ( 100 );
			}
			else if ( m_fEND_TIME[nIndex] < m_fPlayedTime )
			{				
				m_pXControl[nIndex]->SetFadeOut ();
				if ( nIndex != (XCONTROL_SIZE-1) )
				{
					m_pXControl[nIndex]->SetBeginAlpha ( 100 );
				}
			}

			if ( m_pXControl[nIndex]->IsVisible () ) bIsVISIBLE = true;
			if ( m_fPlayedTime < m_fBEGIN_TIME[nIndex] ) bIsVISIBLE = true;	//	아직 시작도 되지 않은 것이면...
		}	

		if ( !bIsVISIBLE )
		{
			STOP ();
		}
	}
}
