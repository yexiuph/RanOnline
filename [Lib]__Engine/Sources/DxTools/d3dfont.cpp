//-----------------------------------------------------------------------------
// File: D3DFont.cpp
//
// Desc: Texture-based font class
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "pch.h"

#include "DxFontMan.h"
#include "D3DFont.h"

#include "./charset.h"
#include "./stlfunctions.h"
#include "./SerialFile.h"

#include "../TextTexture/TextColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	const WORD FONT_VERSION = 0x0001;
}

//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont( TCHAR* strFontName, DWORD dwHeight, DWORD dwBufferMode, DWORD dwFlags, LANGFLAG emLang ) :
	m_pSavedStateBlock(NULL),
	m_pDrawTextStateBlock(NULL),
	m_pd3dDevice(NULL),

	m_dwBufferMode(dwBufferMode),
	m_ntexNum(0),

	m_fFontHeight(0),
	m_fFontBlinkWidth(0)
{
	m_emLangFlag = emLang;
	m_dwFontHeight = (dwHeight),
	m_dwFontFlags = (dwFlags),
	_tcscpy_s( m_strFontName, strFontName );
}


//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
	CleanUp ();
}

// 수정 및 추가 : 2004. 09. By 경대
//
HRESULT CD3DFont::CreateFont ()
{
	HRESULT hr = S_OK;

	//	Note : ascii small image.
	if ( m_dwFontFlags&D3DFONT_ASCII )	m_dwBufferMode = 2;

	DWORD dwBackBufferMode = m_dwBufferMode;

	//	Note : 미리 만들어진 폰트 케시가 있을 때.
	//
	bool bloadcache = LoadFontCache();
	if ( bloadcache )	return S_OK;

	for ( DWORD dwMODE=0; dwMODE<3; ++dwMODE )
	{
		ReleaseFont();

		typedef std::set<short>		SETCHAR;
		typedef SETCHAR::iterator	SETCHAR_ITER;

		typedef std::vector<short>	VECCHAR;
		typedef VECCHAR::iterator	VECCHAR_ITER;

		SETCHAR setCHAR;
		VECCHAR vecCHAR;

		//	Note : charset hash_map 초기화.
		//
		do
		{
			//if( m_dwFontFlags & D3DFONT_THAI )
			//{
			//	for( int c=0x00; c<=0xff; ++c )
			//	{
			//		SETCHAR_ITER iter = setCHAR.find(c);
			//		if ( iter!=setCHAR.end() )	continue;

			//		vecCHAR.push_back(c);
			//		setCHAR.insert(c);
			//	}

			//	break;
			//}

			int i;

			char str[2] = "X";

			for( short c=32; c<127; c++ )
			{
				vecCHAR.push_back(c);
				setCHAR.insert(c);
			}

			// 아스키코드만 출력할때
			if ( m_dwFontFlags & D3DFONT_ASCII )		break;

			if ( !(m_dwFontFlags&(D3DFONT_TCBIGFIVE | D3DFONT_SHIFTJIS | D3DFONT_SCGB | D3DFONT_THAI | D3DFONT_VIETNAM ) ) )
			{
				for( i=0; i<2350; i++ )
				{
					str[0] = 0xb0+i/(255-0xa1);
					str[1] = 0xa1+i%(255-0xa1);

					short nCHAR = ((0xb0+i/(255-0xa1))<<8) | (0xa1+i%(255-0xa1));
					vecCHAR.push_back(nCHAR);
					setCHAR.insert(nCHAR);
				}

				for( i=0; i<94; i++ )
				{
					str[0] = 0xa4+i/(255-0xa1);
					str[1] = 0xa1+i%(255-0xa1);

					short nCHAR = ((0xa4+i/(255-0xa1))<<8) | (0xa1+i%(255-0xa1));
					vecCHAR.push_back(nCHAR);
					setCHAR.insert(nCHAR);
				}

				// 한글완성형만 출력할때
				if( m_dwFontFlags & D3DFONT_KSC5601 )		break;

				if ( m_dwBufferMode==0 )
				{
					for( i=0x81; i<0xff; i++ )
					{
						for( int j=0xa1; j<0xff; j++ )
						{
							str[0] = i;
							str[1] = j;
							short nCHAR = (i<<8) | j;
							
							SETCHAR_ITER iter = setCHAR.find(nCHAR);
							if ( iter!=setCHAR.end() )	continue;

							vecCHAR.push_back(nCHAR);
							setCHAR.insert(nCHAR);
						}
					}

					for( i=0x81; i<=0xc6; i++ )
					{
						for( int j=0x41; j<0xa1; j++ )
						{
							str[0] = i;
							str[1] = j;
							short nCHAR = (i<<8) | j;

							SETCHAR_ITER iter = setCHAR.find(nCHAR);
							if ( iter!=setCHAR.end() )	continue;

							vecCHAR.push_back(nCHAR);
							setCHAR.insert(nCHAR);
						}
					}
				}
			}
			else if ( m_dwFontFlags & D3DFONT_TCBIGFIVE )
			{
				for( i=0xa1; i<=0xf9; i++ )
				{
					for( int j=0x40; j<=0x7e; j++ )
					{
						str[0] = i;
						str[1] = j;
						short nCHAR = (i<<8) | j;
						
						SETCHAR_ITER iter = setCHAR.find(nCHAR);
						if ( iter!=setCHAR.end() )	continue;

						vecCHAR.push_back(nCHAR);
						setCHAR.insert(nCHAR);
					}
				}

				for( i=0xa1; i<=0xf9; i++ )
				{
					for( int j=0xa1; j<=0xfe; j++ )
					{
						str[0] = i;
						str[1] = j;
						short nCHAR = (i<<8) | j;

						SETCHAR_ITER iter = setCHAR.find(nCHAR);
						if ( iter!=setCHAR.end() )	continue;

						vecCHAR.push_back(nCHAR);
						setCHAR.insert(nCHAR);
					}
				}
			}
			else if ( m_dwFontFlags & D3DFONT_SHIFTJIS )
			{
				for( short c=0xA1; c<=0xDF; c++ )
				{
					SETCHAR_ITER iter = setCHAR.find(c);
					if ( iter!=setCHAR.end() )	continue;

					vecCHAR.push_back(c);
					setCHAR.insert(c);
				}

				for( i=0x80; i<=0xA0; i++ )
				{
					for( int j=0x40; j<=0xFC; j++ )
					{
						str[0] = i;
						str[1] = j;
						short nCHAR = (i<<8) | j;
						
						SETCHAR_ITER iter = setCHAR.find(nCHAR);
						if ( iter!=setCHAR.end() )	continue;

						vecCHAR.push_back(nCHAR);
						setCHAR.insert(nCHAR);
					}
				}

				for( i=0xE0; i<=0xEA; i++ )
				{
					for( int j=0x40; j<=0xFC; j++ )
					{
						str[0] = i;
						str[1] = j;
						short nCHAR = (i<<8) | j;
						
						SETCHAR_ITER iter = setCHAR.find(nCHAR);
						if ( iter!=setCHAR.end() )	continue;

						vecCHAR.push_back(nCHAR);
						setCHAR.insert(nCHAR);
					}
				}

				for( i=0xED; i<=0xEE; i++ )
				{
					for( int j=0x40; j<=0xFC; j++ )
					{
						str[0] = i;
						str[1] = j;
						short nCHAR = (i<<8) | j;
						
						SETCHAR_ITER iter = setCHAR.find(nCHAR);
						if ( iter!=setCHAR.end() )	continue;

						vecCHAR.push_back(nCHAR);
						setCHAR.insert(nCHAR);
					}
				}

				for( i=0xFA; i<=0xFC; i++ )
				{
					for( int j=0x40; j<=0xFC; j++ )
					{
						str[0] = i;
						str[1] = j;
						short nCHAR = (i<<8) | j;
						
						SETCHAR_ITER iter = setCHAR.find(nCHAR);
						if ( iter!=setCHAR.end() )	continue;

						vecCHAR.push_back(nCHAR);
						setCHAR.insert(nCHAR);
					}
				}
			}
			else if ( m_dwFontFlags & D3DFONT_SCGB )
			{
				for( i=0x81; i<=0xff; i++ )
				{
					for( int j=0x40; j<=0xff; j++ )
					{
						str[0] = i;
						str[1] = j;
						short nCHAR = (i<<8) | j;
						
						SETCHAR_ITER iter = setCHAR.find(nCHAR);
						if ( iter!=setCHAR.end() )	continue;

						vecCHAR.push_back(nCHAR);
						setCHAR.insert(nCHAR);
					}
				}
			}
			else if( m_dwFontFlags & D3DFONT_VIETNAM )
			{
				for( short c=0x80; c<=0xFF; c++ )
				{
					SETCHAR_ITER iter = setCHAR.find(c);
					if ( iter!=setCHAR.end() )	continue;

					vecCHAR.push_back(c);
					setCHAR.insert(c);
				}
			}
		}
		while(0);

		switch ( dwMODE )
		{
		case 0:
			m_dwTexWidth = 512;
			m_dwTexHeight = 1024;
			break;

		case 1:
			m_dwTexWidth = 512;
			m_dwTexHeight = 512;
			break;

		case 2:
			m_dwTexWidth = 256;
			m_dwTexHeight = 256;
			break;
		};
		m_dwBufferMode = dwMODE;

		//	device가 지원 가능한 텍스쳐 크기 인지 검사.
		//
		D3DCAPSQ d3dCaps;
		m_pd3dDevice->GetDeviceCaps ( &d3dCaps );

		if( m_dwTexWidth > d3dCaps.MaxTextureWidth )
		{
			m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
		}

		m_ntexNum = 0;
		VECCHAR_ITER iter = vecCHAR.begin();
		VECCHAR_ITER iter_end = vecCHAR.end();
		while(iter!=iter_end)
		{
			LPDIRECT3DTEXTUREQ pTexture = NULL;

			// Create a new texture for the font
			hr = m_pd3dDevice->CreateTexture( m_dwTexWidth, m_dwTexHeight, 1, 0,
				D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pTexture, NULL );
			if( FAILED(hr) )	return hr;

			// Prepare to create a bitmap
			DWORD* pBitmapBits;
			BITMAPINFO bmi;
			SecureZeroMemory ( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
			bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth       =  (int)m_dwTexWidth;
			bmi.bmiHeader.biHeight      = -(int)m_dwTexHeight;
			bmi.bmiHeader.biPlanes      = 1;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biBitCount    = 32;

			// Create a DC and a bitmap for the font
			HDC     hDC       = CreateCompatibleDC( NULL );
			HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
												(VOID**)&pBitmapBits, NULL, 0 );
			SetMapMode( hDC, MM_TEXT );

			// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
			// antialiased font, but this is not guaranteed.
			INT nHeight    = -MulDiv ( m_dwFontHeight, int(GetDeviceCaps(hDC,LOGPIXELSY)), 72 );
			DWORD dwBold   = (m_dwFontFlags&D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
			DWORD dwItalic = (m_dwFontFlags&D3DFONT_ITALIC) ? TRUE    : FALSE;
			HFONT hFont    = ::CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
								FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
								VARIABLE_PITCH, m_strFontName );
			if( NULL==hFont )	return E_FAIL;

			SelectObject( hDC, hbmBitmap );
			SelectObject( hDC, hFont );

			// Loop through all printable character and output them to the bitmap..
			// Meanwhile, keep track of the corresponding tex coords for each character.
			INT nX_StartPos = 0;
			INT nY_StartPos = 0;
			INT nX_ShadowThickness = 0;
			INT nY_ShadowThickness = 0;
			INT nY_AlphaThickness = 0;

			if( m_dwFontFlags & D3DFONT_SHADOW )
			{
				nX_StartPos = 1;
				nY_StartPos = 1;
				nX_ShadowThickness = 2;
				nY_ShadowThickness = 2;
			}
			else if( m_dwFontFlags & D3DFONT_SHADOW_EX )
			{
				nX_ShadowThickness = 1;
				nY_ShadowThickness = 1;
			}

			DWORD x = 0 + nX_StartPos; // 여기서 1은 그림자 영역
			DWORD y = 0 + nY_StartPos; // 여기서 1은 그림자 영역
			char str[3] = {0};
			short nLMASK = 0x00ff;
			SIZE size, sizeSMP;
			sizeSMP.cx = 0;
			sizeSMP.cy = 0;

			//GetTextExtentPoint32( hDC, str, 1, &sizeSMP );

			for ( ; iter!=iter_end; ++iter )
			{
				const short &nCHAR = (*iter);
				STEXUV sTexUV;

				int nLen = 1;
				if ( (nCHAR>>8)!=NULL )
				{
					str[0] = (char) ((nCHAR>>8)&nLMASK);
					str[1] = (char) (nCHAR&nLMASK);
					str[2] = NULL;

					nLen = 2;
				}
				else
				{
					str[0] = (char) (nCHAR);
					str[1] = NULL;
				}

				BOOL bSUC = GetTextExtentPoint32( hDC, str, nLen, &size );
				if ( !bSUC )	continue;

				if( sizeSMP.cy < size.cy ) sizeSMP.cy = size.cy;

				// 한줄이 다 차면 다음줄로 이동
				if ( (DWORD)(x+size.cx+nX_ShadowThickness) > m_dwTexWidth )
				{
					x = 0 + nX_StartPos;
					y += sizeSMP.cy + nY_ShadowThickness + nY_AlphaThickness;
				}

				SetBkColor( hDC, 0x00000000 ); // 배경색 지정, 의미 없음

				if( m_dwFontFlags & D3DFONT_SHADOW )
				{
					// Note : 글자 테두리
					//
					SetTextColor( hDC, RGB( 10, 10, 10 ) );
					SetBkMode( hDC, TRANSPARENT );
					for( int i=-1; i < 2; ++i )
					{
						for( int j=-1; j < 2; ++j )
						{
							if( i==0 && j==0 ) continue;
							ExtTextOut ( hDC, x+i, y+j, ETO_OPAQUE, NULL, str, nLen, NULL );
						}
					}
				}
				else if( m_dwFontFlags & D3DFONT_SHADOW_EX )
				{
					// Note : 글자 테두리
					//
					SetTextColor( hDC, RGB( 10, 10, 10 ) );
					SetBkMode( hDC, TRANSPARENT );
					ExtTextOut ( hDC, x+1, y, ETO_OPAQUE, NULL, str, nLen, NULL );
					ExtTextOut ( hDC, x+1, y+1, ETO_OPAQUE, NULL, str, nLen, NULL );
				}

				// Note : 글자
				//
				SetTextColor( hDC, RGB( 255, 255, 255 ) );
				ExtTextOut ( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, nLen, NULL );

				sTexUV.ntexINDEX = m_ntexNum;
				sTexUV.nX1 = short(x+0-nX_StartPos);
				sTexUV.nY1 = short(y+0-nY_StartPos);
				sTexUV.nX2 = short(x+0-nX_StartPos+size.cx+nX_ShadowThickness);
				sTexUV.nY2 = short(y+0-nY_StartPos+size.cy+nY_ShadowThickness+nY_AlphaThickness);

				//	regist char texture uv
				//
				m_mapTEXUV.insert ( std::make_pair(nCHAR,sTexUV) );

				x += size.cx+nX_ShadowThickness; // 여기서 2는 그림자 영역

				if ( (y+(sizeSMP.cy+nY_ShadowThickness+nY_AlphaThickness)*2)>m_dwTexHeight && 
					 (x+(size.cx+nX_ShadowThickness)*2)>m_dwTexWidth )
				{
					++iter;
					break;
				}
			}

			// Lock the surface and write the alpha values for the set pixels
			D3DLOCKED_RECT d3dlr;
			hr = pTexture->LockRect( 0, &d3dlr, 0, 0 );
			if ( FAILED(hr) )
			{
				CDebugSet::ToLogFile( "CreateFont() -- pTexture->LockRect()" );
				return hr;
			}

			DWORD dwCur;
			DWORD* pSrc;
			WORD* pDst16;
			WORD* pDstRow = (WORD*)d3dlr.pBits;

			for( y=0; y < m_dwTexHeight; ++y )
			{
				dwCur = y * m_dwTexWidth;

				pSrc = &pBitmapBits[ dwCur ];
				pDst16 = &pDstRow[ dwCur ];

				for( x=0; x < m_dwTexWidth; ++x, ++pSrc, ++pDst16 )
				{
					if( *pSrc == 0x00000000 )
						*pDst16 = 0x0000;
					else
						*pDst16 = Conv32to16( *pSrc | 0xFF000000 );
				}
			}

			// Done updating texture, so clean up used objects
			pTexture->UnlockRect(0);
			DeleteObject( hFont );
			DeleteObject( hbmBitmap );
			DeleteDC( hDC );

			//	dxt2 으로 압축된 텍스쳐를 제작.
			LPDIRECT3DTEXTUREQ pTextureDXT(NULL);
			hr = m_pd3dDevice->CreateTexture ( m_dwTexWidth, m_dwTexHeight, 1, 0,
				D3DFMT_DXT2, D3DPOOL_MANAGED, &pTextureDXT, NULL );
			if( FAILED(hr) )
			{
				//	텍스쳐 등록.
				m_vecTEX.push_back ( pTexture );
			}
			else
			{
				//	표면을 복사한다.
				LPDIRECT3DSURFACEQ psurfSrc = NULL;
				LPDIRECT3DSURFACEQ psurfDest = NULL;
				hr = pTexture->GetSurfaceLevel ( 0, &psurfSrc );
				hr = pTextureDXT->GetSurfaceLevel ( 0, &psurfDest );
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, 
					psurfSrc, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
				SAFE_RELEASE(psurfSrc);
				SAFE_RELEASE(psurfDest);

				//	임시표면 삭제.
				SAFE_RELEASE(pTexture);

				//	텍스쳐 등록.
				m_vecTEX.push_back ( pTextureDXT );
			}

			//	텍스쳐 갯수 증가.
			++m_ntexNum;
		}

		//	Note : 폰트 display에 필요한 자원 초기화.
		//
		DoFontReady();

		//	만들어진 폰트 정보를 파일에 저장.
		//
		SaveFontCache ();
	}

	ReleaseFont();

	m_dwBufferMode = dwBackBufferMode;

	bloadcache = LoadFontCache();
	if ( bloadcache )	return S_OK;

	return E_FAIL;
}

HRESULT CD3DFont::ReleaseFont ()
{
	std::for_each ( m_vecTEX.begin(), m_vecTEX.end(), std_afunc::ReleaseObject() );

	m_ntexNum = 0;
	m_vecTEX.clear();
	m_mapTEXUV.clear();
	m_vecVERTNUM.clear();

	std::for_each ( m_vecVERTEX.begin(), m_vecVERTEX.end(), std_afunc::DeleteArray() );
	m_vecVERTEX.clear();

	return S_OK;
}

void CD3DFont::DoFontReady()
{
	m_vecVERTNUM.resize(m_ntexNum);
	std::fill ( m_vecVERTNUM.begin(), m_vecVERTNUM.end(), 0 );

	for ( int i=0; i<m_ntexNum; ++i )
	{
		FONT2DVERTEX *pVERTEX = new FONT2DVERTEX[_MAXVERTEX];
		m_vecVERTEX.push_back(pVERTEX);
	}

	MAPTEXUV_ITER iter;

	iter = m_mapTEXUV.find ( (short)'X');
	if ( iter!=m_mapTEXUV.end() )
	{
		const STEXUV &sTEXUV = (*iter).second;
		m_fFontHeight = float(sTEXUV.nY2-sTEXUV.nY1);
	}

	iter = m_mapTEXUV.find ( (short)' ');
	if ( iter!=m_mapTEXUV.end() )
	{
		const STEXUV &sTEXUV = (*iter).second;
		m_fFontBlinkWidth= float(sTEXUV.nX2-sTEXUV.nX1);
	}
}

bool CD3DFont::LoadFontCache ()
{
	HRESULT hr = S_OK;

	CString strFont;
	strFont.Format (_T("%s%s.%d.%d.%d"),
		            DxFontMan::GetInstance().GetPath(),
					m_strFontName,
					m_dwFontHeight,
					m_dwFontFlags,
					m_dwBufferMode);

	#ifdef KRT_PARAM
	{
		CString strMark = strFont;
		strMark += _T( ".mark" );

		CSerialFile SFile;
		BOOL bOk = SFile.OpenFile( FOT_READ, strMark.GetString() );
		if( !bOk )
			return false;

		WORD wVersion;
		SFile >> wVersion;

		if( wVersion != FONT_VERSION )
			return false;

		SFile.CloseFile();
	}
	#endif

	CString strFontTable = strFont;
	strFontTable += ".table";

	CSerialFile SFile;
	BOOL bOk = SFile.OpenFile ( FOT_READ, strFontTable.GetString() );
	if ( !bOk )	return false;
	{
		SFile >> m_dwTexWidth;
		SFile >> m_dwTexHeight;
		SFile >> m_ntexNum;

		DWORD dwSIZE(0);
		SFile >> dwSIZE;

		short nCHAR(0);
		STEXUV sTEXUV;
		for ( DWORD i=0; i<dwSIZE; ++i )
		{
			SFile >> nCHAR;
			SFile.ReadBuffer ( &sTEXUV, sizeof(STEXUV) );

			m_mapTEXUV.insert ( std::make_pair(nCHAR,sTEXUV) );
		}
	}

	SFile.CloseFile ();

	CString strFontTexture = strFont;
	strFontTexture += ".dds";

	for ( short i=0; i<m_ntexNum; ++i )
	{
		CString strFontTexture;
		strFontTexture.Format (_T("%s.%d"),
			                   strFont.GetString(),
							   i);
		strFontTexture += _T(".dds");

		LPDIRECT3DTEXTUREQ pTexture = NULL;


		D3DXIMAGE_INFO ImageInfo;
		hr = D3DXCreateTextureFromFileEx ( m_pd3dDevice, strFontTexture.GetString(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
									(D3DFORMAT)D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
									D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &pTexture );
		if ( FAILED(hr) )	return false;

		m_vecTEX.push_back ( pTexture );
	}

	//	Note : 폰트 display에 필요한 자원 초기화.
	//
	DoFontReady();

	return true;
}

bool CD3DFont::SaveFontCache ()
{
	CString strFont;
	strFont.Format (_T("%s%s.%d.%d.%d"),
		            DxFontMan::GetInstance().GetPath(),
					m_strFontName,
					m_dwFontHeight,
					m_dwFontFlags,
					m_dwBufferMode);

	#ifdef KRT_PARAM
	{
		CString strMark = strFont;
		strMark += _T( ".mark" );

		CSerialFile SFile;
		BOOL bOk = SFile.OpenFile( FOT_WRITE, strMark.GetString() );
		if( bOk )
		{
			SFile << FONT_VERSION;
			SFile.CloseFile();
		}
	}
	#endif

	CString strFontTable = strFont;
	strFontTable += _T(".table");

	CSerialFile SFile;
	BOOL bOk = SFile.OpenFile ( FOT_WRITE, strFontTable.GetString() );
	if ( !bOk )	return false;
	{
		SFile << m_dwTexWidth;
		SFile << m_dwTexHeight;
		SFile << m_ntexNum;

		SFile << (DWORD)m_mapTEXUV.size();

		MAPTEXUV_ITER iter = m_mapTEXUV.begin();
		MAPTEXUV_ITER iter_end = m_mapTEXUV.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SFile << (*iter).first;
			SFile.WriteBuffer ( &(*iter).second, sizeof(STEXUV) );
		}
	}
	SFile.CloseFile ();

	{
		VECTEX_ITER iter = m_vecTEX.begin();
		VECTEX_ITER iter_end = m_vecTEX.end();
		for ( int ntex=0; iter!=iter_end; ++iter, ++ntex )
		{
			CString strFontTexture;
			strFontTexture.Format ( "%s.%d", strFont.GetString(), ntex );
			strFontTexture += ".dds";

			D3DXSaveTextureToFile ( strFontTexture.GetString(), D3DXIFF_DDS, (*iter), NULL );
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
    HRESULT hr = S_OK;

    // Keep a local copy of the device
    m_pd3dDevice = pd3dDevice;

	hr = CreateFont ();
    if( FAILED(hr) )	return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFont::RestoreDeviceObjects()
{
    HRESULT hr = S_OK;

	InvalidateDeviceObjects();

    // Create the state blocks for rendering text
    for( UINT which=0; which<2; which++ )
    {
        m_pd3dDevice->BeginStateBlock();

		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

        if( which==0 )	m_pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
        else			m_pd3dDevice->EndStateBlock( &m_pDrawTextStateBlock );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pSavedStateBlock );
    SAFE_RELEASE( m_pDrawTextStateBlock );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects()
{
    m_pd3dDevice = NULL;

	VECTEX_ITER iter = m_vecTEX.begin();
	VECTEX_ITER iter_end = m_vecTEX.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SAFE_RELEASE ( (*iter) );
	}

	m_ntexNum = 0;
	m_vecTEX.clear();

	m_mapTEXUV.clear();
	
	m_vecVERTNUM.clear();


	std::for_each ( m_vecVERTEX.begin(), m_vecVERTEX.end(), std_afunc::DeleteArray() );
	m_vecVERTEX.clear();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent( const TCHAR* strText, SIZE &Size )
{
	if( NULL==strText )
		return E_FAIL;

	BOOL bUsageCS = m_bUsageCS;
	if( bUsageCS )
		EnterCriticalSection( &m_csLock );

	MAPTEXUV_ITER iter = m_mapTEXUV.find ( (short)'X');
	if ( iter == m_mapTEXUV.end() )
	{
		if( bUsageCS )
			LeaveCriticalSection( &m_csLock );
		return E_FAIL;
	}

	const STEXUV &sTEXUV = (*iter).second;

	float fRowWidth  = 0.0f;
	float fRowHeight = float(sTEXUV.nY2-sTEXUV.nY1);
	float fWidth     = 0.0f;
	float fHeight    = fRowHeight;

	char str[2] = "X";
	short nLMASK = 0x00ff;

	short nCHAR;

	while ( *strText )
	{
		short cA = short(char(*strText)) & nLMASK;
		strText++;

		if ( CHARSET::IsLeadChar((char) cA) )
		{
			short cB = short(char(*strText)) & nLMASK;
			strText++;

			nCHAR = (cA<<8) | (cB);

			str[0] = (char) ((nCHAR>>8)&nLMASK);
			str[1] = (char) (nCHAR&nLMASK);
		}
		else if ( cA== _T('\n') )
		{
			//fRowWidth = 0.0f;
			//fHeight += fRowHeight;
			continue;
		}
		else
		{
			nCHAR = (short)cA;
		}

		MAPTEXUV_ITER iter = m_mapTEXUV.find ( (short)nCHAR );
		if ( iter==m_mapTEXUV.end() )	continue;
		
		const STEXUV &sTEXUV = (*iter).second;

		fRowWidth += (sTEXUV.nX2-sTEXUV.nX1);

		if( fRowWidth > fWidth )	fWidth = fRowWidth;
	}

	Size.cx = (int)fWidth;
	Size.cy = (int)fHeight;

	if( bUsageCS )
		LeaveCriticalSection( &m_csLock );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawText ( FLOAT sx, FLOAT sy, DWORD dwColor, const TCHAR* strText, DWORD dwFlags, BOOL bImmediately )
{
	if( m_pd3dDevice == NULL )
		return E_FAIL;

	BOOL bUsageCS = m_bUsageCS;
	if( bUsageCS )
		EnterCriticalSection( &m_csLock );

	sx = (float)((int)sx);
	sy = (float)((int)sy);

	// Setup renderstate
	m_pSavedStateBlock->Capture();
	m_pDrawTextStateBlock->Apply();

	m_pd3dDevice->SetFVF( FONT2DVERTEX::FVF );

	// Set filter states
	if( dwFlags & D3DFONT_FILTERED )
	{
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	}

	FLOAT fStartX = sx;

	std::fill ( m_vecVERTNUM.begin(), m_vecVERTNUM.end(), 0 );

	char str[2] = "X";
	short nLMASK = 0x00ff;

	float fDivTexWidth = 1.f/float(m_dwTexWidth);
	float fDivTexHeight = 1.f/float(m_dwTexHeight);

	short nCHAR;
	FONT2DVERTEX sFONT2DVERT;
	while( *strText )
	{
		short cA = short(char(*strText)) & nLMASK;
		strText++;

		FLOAT tx1, tx2, ty1, ty2;
		if ( CHARSET::IsLeadChar((char) cA) )
		{
			short cB = short(char(*strText)) & nLMASK;
			nCHAR = (cA<<8) | (cB);
			strText++;

			str[0] = (char) ((nCHAR>>8)&nLMASK);
			str[1] = (char) (nCHAR&nLMASK);
		}
		else if ( cA == _T('\n') )
		{
			//sx = fStartX;
			//sy += m_fFontHeight;
			continue;
		}
		else if ( cA == _T(' ') )
		{
			sx += m_fFontBlinkWidth;
			continue;
		}
		else if ( cA == _T('\t') )
		{
			sx += m_fFontBlinkWidth*4;
			continue;
		}
		else
		{
			nCHAR = (short)cA;
		}

		MAPTEXUV_ITER iter = m_mapTEXUV.find ( (short)nCHAR );
		if ( iter==m_mapTEXUV.end() )	continue;

		const STEXUV &sTEXUV = (*iter).second;
		tx1 = sTEXUV.nX1*fDivTexWidth;
		ty1 = sTEXUV.nY1*fDivTexHeight;
		tx2 = sTEXUV.nX2*fDivTexWidth;
		ty2 = sTEXUV.nY2*fDivTexHeight;
		const short nCURTEXINDEX = sTEXUV.ntexINDEX;

		FLOAT w = float(sTEXUV.nX2-sTEXUV.nX1);
		FLOAT h = float(sTEXUV.nY2-sTEXUV.nY1);
		
		const DWORD dwTEXINDEX =  m_vecVERTNUM[nCURTEXINDEX];
		FONT2DVERTEX * pFONT2DVER = m_vecVERTEX[nCURTEXINDEX];
		pFONT2DVER[dwTEXINDEX+0] = FONT2DVERTEX ( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
		pFONT2DVER[dwTEXINDEX+1] = FONT2DVERTEX ( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
		pFONT2DVER[dwTEXINDEX+2] = FONT2DVERTEX ( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );

		pFONT2DVER[dwTEXINDEX+3] = FONT2DVERTEX ( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
		pFONT2DVER[dwTEXINDEX+4] = FONT2DVERTEX ( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
		pFONT2DVER[dwTEXINDEX+5] = FONT2DVERTEX ( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );

		m_vecVERTNUM[nCURTEXINDEX] += 6;

		if ( m_vecVERTNUM[nCURTEXINDEX] == _MAXVERTEX )
		{
			m_pd3dDevice->SetTexture ( 0, m_vecTEX[nCURTEXINDEX] );

			FONT2DVERTEX * pFONT2DVER = m_vecVERTEX[nCURTEXINDEX];
			DWORD dwNumTriangles = _MAXVERTEX / 3;

			m_pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, dwNumTriangles, pFONT2DVER, sizeof(FONT2DVERTEX) );

			m_pd3dDevice->SetTexture ( 0, NULL );

			m_vecVERTNUM[nCURTEXINDEX] = 0;
		}

		sx += w;
	}

	for ( short i=0; i<m_ntexNum; ++i )
	{
		const DWORD dwTEXNUM =  m_vecVERTNUM[i];
		if ( dwTEXNUM==0 )		continue;

		m_pd3dDevice->SetTexture ( 0, m_vecTEX[i] );

		FONT2DVERTEX * pFONT2DVER = m_vecVERTEX[i];
		DWORD dwNumTriangles = dwTEXNUM / 3;

		m_pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, dwNumTriangles, pFONT2DVER, sizeof(FONT2DVERTEX) );

		m_pd3dDevice->SetTexture ( 0, NULL );
	}

	// Restore the modified renderstates
	m_pSavedStateBlock->Apply();

	if( bUsageCS )
		LeaveCriticalSection( &m_csLock );

    return S_OK;
}
