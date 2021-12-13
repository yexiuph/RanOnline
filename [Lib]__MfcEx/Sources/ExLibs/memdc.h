#ifndef _MEMDC_H_
#define _MEMDC_H_

class CmyMemDC : public CDC
{
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;

	CFont		*m_pOldFont;

public:
	void SetBkFill ( COLORREF BkColor=RGB(255,255,255) );
	void SetFont ( CFont *pFont );

	void ReleaseMemDC ();

public:
	CmyMemDC(CDC* pDC, const CRect& rcBounds);
	~CmyMemDC();

	CmyMemDC* operator->();
};

#endif //_MEMDC_H_