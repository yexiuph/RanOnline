#include "pch.h"
#include "checklistboxlog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCheckListBoxLog, CCheckListBox)
END_MESSAGE_MAP()

CCheckListBoxLog::CCheckListBoxLog(void)
{
}

CCheckListBoxLog::~CCheckListBoxLog(void)
{
}

void	CCheckListBoxLog::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	

	// You must override DrawItem and MeasureItem for LBS_OWNERDRAWVARIABLE
	ASSERT((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) ==
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		int cyItem = GetItemHeight(lpDrawItemStruct->itemID);
		BOOL fDisabled = !IsWindowEnabled() || !IsEnabled(lpDrawItemStruct->itemID);
		
		if (m_cyText == 0)
			VERIFY(cyItem >= CalcMinimumItemHeight());

		CString strText;
		GetText(lpDrawItemStruct->itemID, strText);

		BOOL	bCheck = GetCheck ( lpDrawItemStruct->itemID );
		if ( bCheck )
		{
			COLORREF newTextColor = fDisabled ?
			RGB(0x80, 0x80, 0x80) : RGB(0xC0, 0xC0, 0xC0);  // light gray
			COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

			COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
			COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

			if (newTextColor == newBkColor)
				newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

			if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
			{
				pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				pDC->SetBkColor(RGB(0x80, 0x80, 0x80));
			}

			pDC->ExtTextOut(lpDrawItemStruct->rcItem.left,
				lpDrawItemStruct->rcItem.top + max(0, (cyItem - m_cyText) / 2),
				ETO_OPAQUE, &(lpDrawItemStruct->rcItem), strText, (int)strText.GetLength(), NULL);
            
			pDC->MoveTo ( lpDrawItemStruct->rcItem.left, ( lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom ) / 2 );
			pDC->LineTo ( lpDrawItemStruct->rcItem.right, ( lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom ) / 2 );

			pDC->SetTextColor(oldTextColor);
			pDC->SetBkColor(oldBkColor);
		}
		else
		{
			COLORREF newTextColor = fDisabled ?
				RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);  // light gray
				COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

			COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
			COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

			if (newTextColor == newBkColor)
				newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

			if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
			{
				pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				pDC->SetBkColor(RGB(0x80, 0x80, 0x80));
			}

			pDC->ExtTextOut(lpDrawItemStruct->rcItem.left,
				lpDrawItemStruct->rcItem.top + max(0, (cyItem - m_cyText) / 2),
				ETO_OPAQUE, &(lpDrawItemStruct->rcItem), strText, (int)strText.GetLength(), NULL);


			pDC->SetTextColor(oldTextColor);
			pDC->SetBkColor(oldBkColor);
		}
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
}