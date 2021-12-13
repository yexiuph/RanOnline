//	[DxGrapUtils], (2002.12.06), JDH
//
//	ScreenGrab (), 화면을 파일에 저장.
//	CaptureScreen(), 파일 이름을 자동으로 매겨주면서 화면을 파일에 저장.
//
#pragma once

namespace DxGrapUtils
{
	void SetPath( LPCTSTR szPath );

	HRESULT ScreenGrab ( LPDIRECT3DDEVICEQ pDev, LPCTSTR fileName );
	HRESULT CaptureScreen ( LPDIRECT3DDEVICEQ pDev );
	BOOL BitmapToJPG ( LPCTSTR lpszPathName, BOOL bBitmapDelete );
};
