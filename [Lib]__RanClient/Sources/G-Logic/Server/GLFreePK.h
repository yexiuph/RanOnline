#pragma once

class GLFreePK
{
protected:
	float	m_fTIMER;
	float	m_fPKTIME;
	float	m_fUPDATETIMER;

public:
	GLFreePK() : m_fTIMER(0.0f), m_fPKTIME(0.0f), m_fUPDATETIMER(300.0f) {}
	~GLFreePK();


	bool IsFREEPKMODE() { return m_fPKTIME > 0.0f; }

	void RESET() { m_fTIMER = 0.0f; m_fPKTIME = 0.0f; }
	void StartFREEPKMODE( DWORD dwTIME );
	void EndFREEPKMODE();

	bool FrameMove( float fElapsedAppTime );

	static GLFreePK& GetInstance ();
}; // end of decl