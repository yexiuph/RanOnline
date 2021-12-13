#pragma once

#include <list>

class CD3DFontX;
class CTextObject;

/**
 * \class	CTextGroup
 * \date	2005/10/06
 * \author	Jun-Hyuk Choi
 */
class CTextGroup
{
protected:
	typedef std::list< CTextObject* >	OBJECT_LIST;

protected:
	CD3DFontX*	m_pFont;

	OBJECT_LIST	m_listObject;

public:
	CTextGroup();
	virtual ~CTextGroup();

	VOID FrameMovePrev();
	VOID FrameMoveNext();

	BOOL DrawText( const TCHAR* strText, D3DCOLOR dwColor, FLOAT fX, FLOAT fY );

	CTextObject* CreateTextObject( const TCHAR* strText );
	CTextObject* GetTextObject( const TCHAR* strText );
	VOID DeleteTextObject( CTextObject* pObject );

public:
	VOID SetFont( CD3DFontX* pFont )	{ m_pFont = pFont; }

public:
	CD3DFontX* GetFont()				{ return m_pFont; }
};