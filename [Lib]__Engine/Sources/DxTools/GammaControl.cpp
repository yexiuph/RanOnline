#include "pch.h"

#include "GammaControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------------------------
GammaControl& GammaControl::GetInstance()
{
	static GammaControl Instance;
	return Instance;
}

GammaControl::GammaControl() :
	m_bBackUp(FALSE)
{
}

GammaControl::~GammaControl()
{
}

void GammaControl::SetGammaControl( HWND hWnd, float gamma, int overbright, float fContrast )
{
	if( gamma == 0.f )
	{
		CDebugSet::ToLogFile( _T("GammaControl::SetGammaControl - gamma=0.f - ERROR") );
		return;
	}

	HDC hDC;
	hDC = GetDC( hWnd );

	unsigned short ramp[3*256];
    float f(0.f);
	   
    GetDeviceGammaRamp( hDC, m_wRampBackUp );
	   
    for ( int i = 0; i < 256; i++ )
	{
        f = (float)(255 * pow((float)i/256, 1/gamma));
        f = f * fContrast + overbright;
        if (f < 0)
            f = 0;
        if (f > 255)
            f = 255;
        ramp[i+0] = ramp[i+256] = ramp[i+512] = ((unsigned short)f<<8);
    }
	   
    SetDeviceGammaRamp( hDC, ramp );

	ReleaseDC( hWnd, hDC );
}

void GammaControl::BackUp( HWND hWnd )
{
	HDC hDC;
	hDC = GetDC( hWnd );
    GetDeviceGammaRamp( hDC, m_wRampBackUp );
	ReleaseDC( hWnd, hDC );

	m_bBackUp = TRUE;
}

void GammaControl::RollBack( HWND hWnd )
{
	HDC hDC;
	hDC = GetDC( hWnd );

	if( m_bBackUp )
	{
		SetDeviceGammaRamp( hDC, m_wRampBackUp );
	}
	else
	{
		int   nOverBright = 1;
		float fGamma = 1.0f;
		float fContrast = 1.0f;

		unsigned short ramp[3*256];
		float f(0.f);
		for ( int i = 0; i < 256; i++ )
		{
			f = (float)(255 * pow((float)i/256, 1/fGamma));
			f = f * fContrast + nOverBright;
			if (f < 0)
				f = 0;
			if (f > 255)
				f = 255;
			ramp[i+0] = ramp[i+256] = ramp[i+512] = ((unsigned short)f<<8);
		}

		SetDeviceGammaRamp( hDC, ramp );
	}

	ReleaseDC( hWnd, hDC );
}

