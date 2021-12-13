
#include "DxViewPort.h"
#include "DxLightMan.h"

class GammaControl
{
private:
	BOOL	m_bBackUp;
	unsigned short m_wRampBackUp[3*256];

public:
	void SetGammaControl( HWND hWnd, float gamma, int overbright, float fContrast );
	void BackUp( HWND hWnd );
	void RollBack( HWND hWnd );

protected:
	GammaControl();

public:
	virtual ~GammaControl();

public:
	static GammaControl& GetInstance();
};
