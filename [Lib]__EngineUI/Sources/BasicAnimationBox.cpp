#include "pch.h"
#include "BasicAnimationBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicAnimationBox::CBasicAnimationBox()
	:m_bPause(FALSE)
	,m_bMoveDir(TRUE)
	,m_dwAniMode(NORMAL_ANI)
	,m_dwRenderPic(0)
	,m_fAniTime(0.0f)
	,m_fEndTime(0.0f)
	,m_TotElapsedTime(0.0f)
	,m_fLastRenderTime(0.0f)
	,m_bLoop(TRUE)
	,m_pBackImage(NULL)
{
}

CBasicAnimationBox::~CBasicAnimationBox()
{
}

void CBasicAnimationBox::Reset()
{
	CUIControl::Reset();
	m_bPause = FALSE;
	m_TotElapsedTime = 0.0f;
	m_dwRenderPic = 0;
	m_fLastRenderTime = 0.0f;
}

void CBasicAnimationBox::InitAnimateFrame()
{
	Reset();

	m_bEndAnimate = FALSE; 

	for( int index=0;index<signed(m_pAniControl.size());index++)
	{		
		m_pAniControl[index]->SetVisibleSingle(FALSE);
	}

	m_pAniControl[m_dwRenderPic]->SetVisibleSingle(TRUE);
	if ( m_pBackImage ) m_pBackImage->SetVisibleSingle( TRUE );
}

void CBasicAnimationBox::CreateAni( const char *keyvalue, DWORD AniType )
{
	m_pAniControl.clear();

	CUIControl* pAniControl = new CUIControl;
	pAniControl->CreateSub ( this, keyvalue );
	pAniControl->SetVisibleSingle ( TRUE );
	RegisterControl ( pAniControl );

	m_pAniControl.push_back(pAniControl);

	m_dwAniMode = AniType;
}

void CBasicAnimationBox::AddAni (const char* keyvalue)
{
	CUIControl* pAniControl = new CUIControl;
	pAniControl->CreateSub ( this, keyvalue );
	pAniControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pAniControl );

	m_pAniControl.push_back(pAniControl);
}

void CBasicAnimationBox::SetBackImage( const char* keyvalue )
{
	CUIControl* pUIControl = new CUIControl;
	pUIControl->CreateSub ( this, keyvalue );
	pUIControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pUIControl );

	m_pBackImage = pUIControl;
}

void CBasicAnimationBox::SetAniTime ( float fTime )
{
	m_fAniTime = fTime;
}

float CBasicAnimationBox::GetAniTime ()
{
	return m_fAniTime;
}

void CBasicAnimationBox::SetEndTime ( float fTime )
{
	m_fEndTime = fTime;
}

float CBasicAnimationBox::GetEndTime ()
{
	return m_fEndTime;
}


void CBasicAnimationBox::SetAniType ( DWORD dwAni)
{
	m_dwAniMode = dwAni;
}

DWORD CBasicAnimationBox::GetAniType ()
{
	return m_dwAniMode;
}

void CBasicAnimationBox::SetPause ( BOOL bPause)
{
	m_bPause = bPause;
}

void CBasicAnimationBox::SetAniDiffuse( D3DXCOLOR d3dColor )
{

}

void CBasicAnimationBox::TranslateMouseMessage ( DWORD dwMsg )
{	

	switch ( m_dwAniMode )
	{
	case NORMAL_ANI:
		{			
			
		}
		break;

	case PINGPONG_ANI:
		{
			
		}
		break;

	case BACK_ANI:
		{
			
		}
		break;
	}
}

void CBasicAnimationBox::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );
	
	if( bVisible )
	{
		Reset();
	}
	else
	{	
		Reset();
	}

}


void CBasicAnimationBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( m_bPause || m_bEndAnimate ) return;
	
	m_TotElapsedTime += fElapsedTime;
	if ( m_TotElapsedTime < m_fEndTime)
	{

		if ( m_TotElapsedTime - m_fLastRenderTime > m_fAniTime )
		{
			
			switch ( m_dwAniMode )
			{
			case NORMAL_ANI:
				{	
					m_dwRenderPic++;
					if( m_dwRenderPic >= m_pAniControl.size() )
					{
						if( m_bLoop )
						{
							m_dwRenderPic = 0;
						}else{
							m_dwRenderPic = m_pAniControl.size()-1;
						}
					}
				}
				break;

			case PINGPONG_ANI:
				{
					if ( m_bMoveDir)
					{
						m_dwRenderPic++;
						if( m_dwRenderPic >= m_pAniControl.size() )	
						{
							m_dwRenderPic = (DWORD)m_pAniControl.size() - 2;
							m_bMoveDir = FALSE;
						}
					}
					else
					{
						m_dwRenderPic--;
						if( m_dwRenderPic < 0) 
						{
							m_dwRenderPic = 1;
							m_bMoveDir = TRUE;
						}
					}
				}
				break;

			case BACK_ANI:
				{
					m_dwRenderPic--;
					if( m_dwRenderPic < 0) m_dwRenderPic = (DWORD)m_pAniControl.size() - 1;
				}
				break;
			}
			
			for( int index=0;index<signed(m_pAniControl.size());index++)
			{		
				m_pAniControl[index]->SetVisibleSingle(FALSE);
			}

			m_pAniControl[m_dwRenderPic]->SetVisibleSingle(TRUE);

			m_fLastRenderTime = m_TotElapsedTime;
		}
	}else{
		m_bEndAnimate = TRUE;
	}
		
	
}

