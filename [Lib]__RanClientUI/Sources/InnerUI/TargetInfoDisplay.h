#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CD3DFontPar;
class	CBasicTextBox;
class	CBasicProgressBar;

class	CTargetInfoDisplay : public CUIGroup
{
//public:
//	enum
//	{
//		nSCHOOLMARK = 3,
//		nPARTYMARK = 5,
//		NODATA = UINT_MAX
//	};

public:
	CTargetInfoDisplay ();
	virtual	~CTargetInfoDisplay ();

public:
	void	CreateSubControl ();
	CUIControl*	CreateControl ( const char* szControl, WORD wAlignFlag );

public:
	void	SetTargetInfo ( float fPercent, CString strName, D3DCOLOR dwColor ); /*, int nSchoolMark = NODATA, int nPartyMark = NODATA );*/
	const CString& GetTargetName () const			{ return m_strName; }

private:
	CBasicTextBox*		m_pNameBox;
	CBasicProgressBar*	m_pHP;

	CUIControl*			m_pNameBoxDummy;

private:
	CString		m_strName;

private:
	CD3DFontPar*	m_pFont8;

//private:
//	CUIControl*		m_pSchoolMark[nSCHOOLMARK];
//	CUIControl*		m_pPartyMark[nPARTYMARK];
};