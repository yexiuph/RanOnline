#include "pch.h"
#include "UIControlDefine.h"

BOOL CHECK_ALIGN_FLAG ( WORD wFlag )
{
	int nCHECK = 0;
	if ( wFlag & UI_FLAG_LEFT )		++nCHECK;
	if ( wFlag & UI_FLAG_RIGHT )	++nCHECK;
	if ( wFlag & UI_FLAG_CENTER_X ) ++nCHECK;		
	if ( 1 < nCHECK )
	{
		GASSERT ( 0 && "가로 정렬 플래그가 중복됩니다." );
		return FALSE;
	}

	nCHECK = 0;
	if ( wFlag & UI_FLAG_TOP )		++nCHECK;
	if ( wFlag & UI_FLAG_BOTTOM )	++nCHECK;
	if ( wFlag & UI_FLAG_CENTER_Y ) ++nCHECK;
    if ( 1 < nCHECK )
	{
		GASSERT ( 0 && "세로 정렬 플래그가 중복됩니다." );
		return FALSE;
	}

	nCHECK = 0;
	if ( wFlag & UI_FLAG_XSIZE )			++nCHECK;
	if ( wFlag & UI_FLAG_XSIZE_PROPORTION )	++nCHECK;
	if ( 1 < nCHECK )
	{
		GASSERT ( 0 && "X 사이즈 플래그가 중복됩니다." );
		return FALSE;
	}

	nCHECK = 0;
	if ( wFlag & UI_FLAG_YSIZE )			++nCHECK;
	if ( wFlag & UI_FLAG_YSIZE_PROPORTION )	++nCHECK;
	if ( 1 < nCHECK )
	{
		GASSERT ( 0 && "Y 사이즈 플래그가 중복됩니다." );
		return FALSE;
	}

	return TRUE;
}

WORD GET_ALIGN_FLAG( const TCHAR * szAlignX, const TCHAR * szAlignY, const TCHAR * szSizeX, const TCHAR * szSizeY )
{
	WORD wFlag(0);

	if( szAlignX[0] == 0 )								wFlag |= UI_FLAG_LEFT;
	else if( !_tcscmp( szAlignX, _T("LEFT") ) )			wFlag |= UI_FLAG_LEFT;
	else if( !_tcscmp( szAlignX, _T("RIGHT") )	)		wFlag |= UI_FLAG_RIGHT;
	else if( !_tcscmp( szAlignX, _T("CENTER") )	)		wFlag |= UI_FLAG_CENTER_X;

	if( szAlignY[0] == 0 )								wFlag |= UI_FLAG_TOP;
	else if( !_tcscmp( szAlignY, _T("TOP") ) )			wFlag |= UI_FLAG_TOP;
	else if( !_tcscmp( szAlignY, _T("BOTTOM") )	)		wFlag |= UI_FLAG_BOTTOM;
	else if( !_tcscmp( szAlignY, _T("CENTER") )	)		wFlag |= UI_FLAG_CENTER_Y;

	if( !_tcscmp( szSizeX, _T("SIZE") ) )				wFlag |= UI_FLAG_XSIZE;
	else if( !_tcscmp( szSizeX, _T("PROPORTION") )	)	wFlag |= UI_FLAG_XSIZE_PROPORTION;

	if( !_tcscmp( szSizeY, _T("SIZE") ) )				wFlag |= UI_FLAG_YSIZE;
	else if( !_tcscmp( szSizeY, _T("PROPORTION") )	)	wFlag |= UI_FLAG_YSIZE_PROPORTION;

	return wFlag;
}