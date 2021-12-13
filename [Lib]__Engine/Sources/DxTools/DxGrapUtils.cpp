#include "pch.h"

#include <string>
#include "dxgraputils.h"
#include "./ijl.h"
#include "../[Lib]__MfcEx/Sources/RANPARAM.h"

#pragma comment(lib, "../Dependency/commonlib/ijl15.lib") // lib 파일을 등록해야 합니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace DxGrapUtils
{
	std::string strCapturePath;

	BOOL ValidDirectory ( LPCTSTR szPath )
	{
		//	Note : 종전 활성 디렉토리 경로 백업.
		DWORD dwOldLenth = 0;
		char szOldPath[MAX_PATH] = "";
		dwOldLenth = ::GetCurrentDirectory ( MAX_PATH, szOldPath );

		//	Note : 경로 유효성 검사.
		BOOL bValid = ::SetCurrentDirectory ( szPath );
		
		//	Note : 활성 디렉토리 경로 되돌림.
		if ( dwOldLenth )	::SetCurrentDirectory ( szOldPath );

		return bValid;
	}

	void SetPath( LPCTSTR szPath )
	{
		//if( RANPARAM::emSERVICE_TYPE == EMSERVICE_CHINA )
		//{
		//	if ( ValidDirectory(szPath3) )		strCapturePath = szPath3;
		//	else								strCapturePath = "c:\\";
		//}
		//else
		//{
		//	if ( ValidDirectory(szPath1) )			strCapturePath = szPath1;
		//	else if ( ValidDirectory(szPath2) )		strCapturePath = szPath2;
		//	else if ( ValidDirectory(szPath3) )		strCapturePath = szPath3;
		//	else									strCapturePath = "c:\\";
		//}

		if( ValidDirectory( szPath ) )
			strCapturePath = szPath;
		else
			strCapturePath = _T("c:\\");
	}

	HRESULT CaptureScreen ( LPDIRECT3DDEVICEQ pDev )
	{
		HRESULT hr;

		CString StrPathName;

		// Use time structure to build a customized time string.
		time_t ltime;
		struct tm today;

		time(&ltime);
		localtime_s(&today, &ltime);
		
		char szTime[MAX_PATH] = "";
		// Use strftime to build a customized time string.
		strftime ( szTime, MAX_PATH, "%Y%m%d[%H%M]", &today );

		CFileFind FFind;
		for ( int i=0; i<999; i++ )
		{
			if( RANPARAM::emSERVICE_TYPE == EMSERVICE_CHINA )
			{
				StrPathName.Format ( "%s%s%s%03d.jpg", strCapturePath.c_str(), "직槿빻蹈", szTime, i );
			}
			else if ( RANPARAM::emSERVICE_TYPE == EMSERVICE_FEYA )
			{
				StrPathName.Format ( "%s%s%s%03d.jpg", strCapturePath.c_str(), "쳃 online", szTime, i );
			}
			else
			{
				StrPathName.Format ( "%s%s%s%03d.jpg", strCapturePath.c_str(), "ran", szTime, i );
			}

			if ( FFind.FindFile ( StrPathName ) )	continue;
			else									break;
		}

		StrPathName += "_";
		const char* szBuffer = StrPathName.GetString();
		hr = ScreenGrab( pDev, szBuffer );
		BitmapToJPG( szBuffer, TRUE );

		return hr;
	}

	BOOL BitmapToJPG ( LPCTSTR lpszPathName, BOOL bBitmapDelete )
	{
		CBitmap csBitmap;
		BITMAP  stBitmap;
		bool bRes = true; // 리턴값

		// 비트맵 로드
		csBitmap.m_hObject = LoadImage(NULL, lpszPathName, IMAGE_BITMAP, NULL, NULL,
			LR_LOADFROMFILE|LR_CREATEDIBSECTION);

		csBitmap.GetBitmap(&stBitmap); // 비트맵 정보를 얻는다.

		IJLERR jerr;
		DWORD dib_pad_bytes;

		// Allocate the IJL JPEG_CORE_PROPERTIES structure.
		JPEG_CORE_PROPERTIES jcprops;
		BYTE * imageData; // 이미지 픽셀 정보

		while(true)
		{
			// Initialize the IntelR JPEG Library.
			jerr = ijlInit(&jcprops);

			if(IJL_OK != jerr)
			{
				TRACE0("JCP 초기화에 실패했습니다.\n");
				bRes = false;
				break;
			}

			int nDIBChannels = 3;
			if ( stBitmap.bmBitsPixel != 24 && stBitmap.bmBitsPixel != 32 )
			{
				TRACE0("24비트, 32비트 비트맵이 아닙니다.");
				bRes = false;
				break;
			}

			if ( stBitmap.bmBitsPixel == 32 )

				dib_pad_bytes = IJL_DIB_PAD_BYTES(stBitmap.bmWidth, 3);

			// Set up information to write from the pixel buffer.
			jcprops.DIBWidth    = stBitmap.bmWidth;
			jcprops.DIBHeight   = stBitmap.bmHeight;
			jcprops.DIBChannels = 3;
			jcprops.DIBPadBytes = dib_pad_bytes;
			jcprops.DIBColor    = IJL_BGR;

			int imageSize = (stBitmap.bmWidth*stBitmap.bmBitsPixel/8+dib_pad_bytes) * stBitmap.bmHeight;

			imageData = new BYTE[imageSize]; // 이미지 사이즈 만큼 데어터를 초기화
			if ( imageData==NULL )
			{
				TRACE( "이미지를 위한 메모리 할당에 실패했습니다.\n" );
				AfxThrowUserException();
				bRes = false;
				break;
			}

			DWORD dwRead = csBitmap.GetBitmapBits ( imageSize , imageData); // 비트맵 이지미 정보를 얻는다.
			if ( dwRead==0 )
			{
				TRACE( "GetBitmapBits() dib 읽기에 실패하였습니다.\n" );
				AfxThrowUserException();
				bRes = false;
				break;
			}

			if ( stBitmap.bmBitsPixel==32 )
			{
				for ( int i=0; i<imageSize/4; ++i )
				{
					DWORD dwDATA = * (DWORD*) ( imageData + i*4 );

					*(imageData+i*3+0) = BYTE(dwDATA>>0&0xFF);
					*(imageData+i*3+1) = BYTE(dwDATA>>8&0xFF);
					*(imageData+i*3+2) = BYTE(dwDATA>>16&0xFF);
				}
			}

			jcprops.DIBBytes        = imageData;

			std::string JPGPathName(lpszPathName);
			std::string::size_type idx = JPGPathName.find_last_of(".");
			JPGPathName.replace(idx+1, std::string::npos, "jpg");

			jcprops.JPGFile         = const_cast<LPTSTR>(JPGPathName.c_str());

			// Specify JPEG file creation parameters.
			jcprops.JPGWidth        = stBitmap.bmWidth;
			jcprops.JPGHeight       = stBitmap.bmHeight;

			// Note: the following are default values and thus
			// do not need to be set.
			jcprops.JPGChannels     = 3;
			jcprops.JPGColor        = IJL_YCBCR;
			jcprops.JPGSubsampling  = IJL_411; // 4:1:1 subsampling.
			jcprops.jquality        = 90; // 이미지의 화질, 보통은 75, 최고 100

			// Write the actual JPEG image from the pixel buffer.
			jerr = ijlWrite(&jcprops,IJL_JFILE_WRITEWHOLEIMAGE);

			if(IJL_OK != jerr)
			{
				TRACE0("이미지를 파일에 저장하는데 실패했습니다.\n");
				bRes = false;
				break;
			}

			break; // while 루프를 빠져나옵니다.
		}

		// Clean up the IntelR JPEG Library.
		ijlFree(&jcprops);

		if( bBitmapDelete == TRUE )		// 만약 플래그가 true이면
			DeleteFile(lpszPathName);	// 비트맵 파일을 삭제합니다.

		if ( imageData )
			delete [] imageData; // 동적으로 할당된 메모리 해제

		return bRes;
	}

	//int jpeg_convertor ()
	//{
	//	// open input and output files
	//	FILE* input_file;
	//	FILE* output_file;
	//	
	//	char* input_filename = "input.bmp";
	//	char* output_filename = "output.jpg";

	//	input_file = fopen ( input_filename, "rb" );
	//	output_file = fopen ( output_filename, "wb" );

	//	// create the JPEG compression structure. jpeg_compress_struct cinfo;
	//	// Initialize the JPEG compression object with default error
	//	// handling.
	//	jpeg_error_mgr jerr;
	//	cinfo.err = jpeg_std_error(&jerr);
	//	jpeg_create_compress(&cinfo);

	//	// set the default compression parameters. IJG's defaults produce
	//	// a "standard" JFIF image with 4:1:1 subsampling in a YUV color
	//	// space at average quality.
	//	cinfo.in_color_space = JCS_RGB;
	//	jpeg_set_defaults(&cinfo);

	//	// Set the input format for our source manager as a Windows
	//	// Bitmap file
	//	cjpeg_source_ptr src_mgr = jinit_read_bmp(&cinfo);
	//	src_mgr->input_file = input_file;

	//	// Read the input file header to obtain file size & colorspace.
	//	(*src_mgr->start_input) (&cinfo, src_mgr);

	//	// Now that we know input colorspace, fix colorspace-dependent
	//	// defaults
	//	jpeg_default_colorspace(&cinfo);

	//	// Specify data destination for compression
	//	jpeg_stdio_dest(&cinfo, output_file);

	//	// Start compressor
	//	jpeg_start_compress(&cinfo, TRUE);

	//	// Process data
	//	while (cinfo.next_scanline < cinfo.image_height)
	//	{
	//		// read data from the source into the source buffer
	//		// num_scanlines will be read.
	//		int num_scanlines = (*src_mgr->get_pixel_rows) ( &cinfo, src_mgr );

	//		// write num_scanlines scanlines of data from the source
	//		// manager input buffer (src_mgr->buffer) to the JPEG file.
	//		jpeg_write_scanlines ( &cinfo, src_mgr->buffer, num_scanlines );
	//	}

	//	// Finish reading data from the Bitmap and writing it to
	//	// the JPEG image.
	//	(*src_mgr->finish_input) (&cinfo, src_mgr);
	//	jpeg_finish_compress(&cinfo);
	//	
	//	// Clean up the JPEG Compressor structure.
	//	jpeg_destroy_compress(&cinfo);

	//	// close the input and output files fclose(input_file);
	//	fclose(output_file);
	//	
	//	return 0;
	//}

	HRESULT ScreenGrab ( LPDIRECT3DDEVICEQ pDev, LPCTSTR fileName )
	{ 
		HRESULT hr;

		// get display dimensions 
		// this will be the dimensions of the front buffer 
		D3DDISPLAYMODE mode;
		hr = pDev->GetDisplayMode( 0, &mode );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile ( "ERROR : DxGrapUtils::ScreenGrab()-GetDisplayMode() %s", DXGetErrorString9(hr) );
			return hr;
		}

		// create the image surface to store the front buffer image 
		// note that call to GetFrontBuffer will always convert format to A8R8G8B8 
		LPDIRECT3DSURFACEQ surf; 
		hr = pDev->CreateOffscreenPlainSurface( mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surf, NULL );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile ( "ERROR : DxGrapUtils::ScreenGrab()-CreateImageSurface() %s", DXGetErrorString9(hr) );
			return hr;
		}

		// read the front buffer into the image surface 
		hr = pDev->GetFrontBufferData( 0, surf );
		//pDev->GetBackBuffer ( 0, D3DBACKBUFFER_TYPE_MONO, &surf );
		if ( FAILED(hr) )
		{ 
			CDebugSet::ToLogFile ( "ERROR : DxGrapUtils::ScreenGrab()-GetFrontBuffer() %s", DXGetErrorString9(hr) );
			surf->Release();
			return hr; 
		} 

		// write the entire surface to the requested file 
		hr = D3DXSaveSurfaceToFile(fileName,D3DXIFF_BMP,surf,NULL,NULL); 
		if ( FAILED(hr) )
		{ 
			CDebugSet::ToLogFile ( "ERROR : DxGrapUtils::ScreenGrab()-SaveFile() %s, %s", fileName, DXGetErrorString9(hr) );
			surf->Release();
			return hr; 
		} 

		// release the image surface 
		surf->Release(); 

		// return status of save operation to caller 
		return hr; 
	} 
};