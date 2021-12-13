/////////////////////////////////////////////////////////////////////////////
// Separator.cpp : source file for CSeparator.
//
// Written by Michael Dunn (mdunn at inreach dot com).  Copyright and all
// that stuff.  Use however you like but give me credit where it's due.  I'll
// know if you don't. bwa ha ha ha ha!
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Separator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeparator

CSeparator::CSeparator()
{
}

CSeparator::~CSeparator()
{
}


BEGIN_MESSAGE_MAP(CSeparator, CStatic)
	//{{AFX_MSG_MAP(CSeparator)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeparator message handlers

// This function draws the horizontal line, followed by the text. The general
// idea for the line drawing comes from Hans Buehler's article at
// http://www.codeguru.com/staticctrl/draw_bevel.shtml
//
// Note that this class is intended to be just a simple separator, so the
// only static styles I implement here are SS_LEFT, SS_CENTER, SS_RIGHT, and
// SS_NOPREFIX.  Also note that the line is always drawn vertically centered
// in the control, so if you make the control tall enough you can get the 
// text totally above the line.

void CSeparator::OnPaint() 
{
CPaintDC dc(this);                      // device context for painting
CRect    rectWnd;                       // RECT of the static control
CRect    rectText;                      // RECT in which text will be drawn
CString  cstrText;                      // text to draw
CFont*   pfontOld;
UINT     uFormat;                       // format for CDC::DrawText()
DWORD    dwStyle = GetStyle();          // this window's style


    // Get the screen & client coords.

    GetWindowRect ( rectWnd );
    GetClientRect ( rectText );

    // If the control is taller than it is wide, draw a vertical line.
    // No text is drawn in this case.

    if ( rectWnd.Height() > rectWnd.Width() )
        {
        dc.Draw3dRect( 
            rectWnd.Width()/2, 0,       // upper-left point
            2, rectWnd.Height(),        // width & height  
            ::GetSysColor(COLOR_3DSHADOW),
            ::GetSysColor(COLOR_3DHIGHLIGHT) );

        /************************************************************
        If you feel adventurous, here is the code to do vertical text
        drawing.  I have it commented out because getting the text
        aligned correctly over the line looks like it'll be a pain.

        // Start by getting this control's font, and then tweak it a bit.

        LOGFONT lf;
        GetFont()->GetObject ( sizeof(LOGFONT), &lf );

        // Text will be rotated counter-clockwise 90 degrees.
        lf.lfOrientation = lf.lfEscapement = 900;

        // We need a TrueType font to get rotated text.  MS Sans Serif
        // won't cut it!
        lstrcpy ( lf.lfFaceName, _T("Tahoma") );

        // Create a font with the tweaked attributes.
        CFont font;
        font.CreateFontIndirect ( &lf );

        pfontOld = dc.SelectObject ( &font );

        dc.SetBkColor ( ::GetSysColor(COLOR_BTNFACE) );

        GetWindowText ( cstrText );

        dc.DrawText ( cstrText, rectText,
                      DT_VCENTER | DT_CENTER | DT_SINGLELINE );

        dc.SelectObject ( pfontOld );
        font.DeleteObject();
        ************************************************************/
        }
    else
        {
        // We're drawing a horizontal separator.
        // The text will always be at the top of the control.  Also check
        // if the SS_NOPREFIX style is set.                                        

        uFormat = DT_TOP;

        if ( dwStyle & SS_NOPREFIX )
            {
            uFormat |= DT_NOPREFIX;
            }

        // Voila!  One 3D line coming up.

        dc.Draw3dRect (
            0, rectWnd.Height()/2,      // upper-left point
            rectWnd.Width(), 2,         // width & height
            ::GetSysColor(COLOR_3DSHADOW),
            ::GetSysColor(COLOR_3DHIGHLIGHT) );


        // Now get the text to be drawn.  I add two spaces to the string
        // to make a bit of space between the end of the line and the text.
        // Space is added to one or both ends of the text as appropriate.
        // Along the way, I add another formatting flag to uFormat.

        GetWindowText ( cstrText );

        if ( dwStyle & SS_CENTER )
            {
            cstrText = _T("  ") + cstrText;
            cstrText += _T("  ");

            uFormat |= DT_CENTER;
            }
        else if ( dwStyle & SS_RIGHT )
            {
            cstrText = _T("  ") + cstrText;

            uFormat |= DT_RIGHT;
            }
        else
            {
            cstrText += _T("  ");
        
            uFormat |= DT_LEFT;
            }

        // Get the font for the text.
    
        pfontOld = dc.SelectObject ( GetFont() );

        // Set the background color to the same as the dialog, so the text
        // will be opaque.  This erases all traces of the 3D line as the
        // text is drawn over it.  (No need to call CDC::SetBkMode() because
        // the default mode for a DC is opaque.)

        dc.SetBkColor ( ::GetSysColor(COLOR_BTNFACE) );
        dc.DrawText ( cstrText, rectText, uFormat );

        // Clean up GDI objects like a good lil' programmer.

        dc.SelectObject ( pfontOld );
        }
}
