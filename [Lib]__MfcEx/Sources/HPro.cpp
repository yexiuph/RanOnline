#include "pch.h"
#include "HPro.h"

#include "SimpleProcessAPI.h"
#include "MiniVersion.h"
#include <TlHelp32.h>
#include <algorithm>
#include "CompByte.h"
#include "DebugSet.h"
#include "SUBPATH.h"
#include "GetWinVer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define STREAM_OFFSET	4125
#define STREAM_SIZE		33

TCHAR szWindowText[255] = ""; // 윈도우 텍스트를 저장하기 위한 배열

bool SIMAGE_INFO::FINDDLL ( const char* szDLL )
{
	POSITION pos = listModuleFileNameList.GetHeadPosition();
	
	while( pos != NULL )
	{
		Sleep( 1 );
		CString strModule = listModuleFileNameList.GetNext(pos);

		int nIndex = strModule.ReverseFind ( '\\' ) ;
		if ( nIndex==-1 )			continue;

		CString strFILE = strModule.Right ( strModule.GetLength()-nIndex-1 );

		if ( strFILE.CompareNoCase(szDLL)==0 )
		{
			return true;
		}
	}

	return false;
}

std::string SIMAGE_INFO::getinfo ()
{
	std::string strInfo;
	strInfo = strFileName + "/" + strWindowText + "/" + strProductName + "/" + strFileDescription;
	return strInfo;
}

CHPro::CHPro()
{
}

CHPro::~CHPro()
{
	Release();
}

VOID CHPro::Release()
{
	IMAGEMAP_ITER pos = m_mapImage.begin();
	IMAGEMAP_ITER end = m_mapImage.end();
	for( ; pos != end; ++pos )
	{
		delete pos->second; // 이미지 구조체 포인터 삭제
	}
	
	// 맵 클리어. 모든 원소를 제거한다.
	m_mapImage.clear();
}

BOOL CALLBACK EnumWindowsFunc(HWND hWnd, LPARAM lParam) // 윈도우 핸들을 넘겨준다.
{
	if ( !hWnd )	return FALSE;

	DWORD dwID ;
	GetWindowThreadProcessId(hWnd, &dwID); // 핸들을 가지고 프로세스 ID를 얻는다. 
	if( dwID == lParam )
	{
		::GetWindowText(hWnd, szWindowText, sizeof(szWindowText)); // 텍스트를 얻는다.
		return FALSE;
	}
	return TRUE;
}

bool CHPro::CreateProcessUPX(const char * szPathName, CString &strTmpFile )
{
	if ( szPathName == NULL ) return false;
	const char *szPATH = CDebugSet::GetPath();

	//	임시 파일 이름 설정.
	strTmpFile.Format( "%s_image.pak", szPATH );

	//	종전 파일 삭제.
	DeleteFile ( strTmpFile.GetString() );

	//	cmd 명령.
	CString strCMDLine;
	strCMDLine.Format ("\"%s\\extz.exe\" -d \"%s\" -o \"%s\"", SUBPATH::APP_ROOT, szPathName, strTmpFile.GetString() );

	if ( !iswin_nt() )
	{
		//	nt 기반이 아닌 경우. 95, 98, ME
		UINT nRETURN = WinExec( strCMDLine.GetString(), SW_HIDE );
		if ( nRETURN < 31 )		return false;

		Sleep( 600000 );	//	10 분.
	}
	else
	{
		//	nt 기반의 경우. "CREATE_NO_WINDOW" 가 동작함.
		STARTUPINFO si;
		SecureZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = NULL;
		si.wShowWindow = SW_HIDE;

		PROCESS_INFORMATION pi;
		SecureZeroMemory(&pi, sizeof(pi));

		if ( !CreateProcess(
			NULL,					// name of executable module
			strCMDLine.GetBuffer(),	// command line string
			NULL,					// process attributes
			NULL,					// thread attributes
			FALSE,					// handle inheritance option
			CREATE_NO_WINDOW,		// creation flags
			NULL,					// new environment block
			NULL,					// current directory name
			&si,					// startup information
			&pi))					// process information
		{
			strCMDLine.ReleaseBuffer();
			return false;
		} // formation
		strCMDLine.ReleaseBuffer();

		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}

	return true;
}

DWORD CHPro::IsMatchStream(const char * szPathName, VECBLOCK& vecBlock)
{
	if ( szPathName == NULL ) return UINT_MAX;

	HANDLE hFile = NULL;
	bool bImage(true); // 압축 풀린 파일의 존재여부

	CString strTmpFile;
	//CreateProcessUPX(szPathName, strTmpFile);
	//{
	//	return UINT_MAX; // UPX 실행 실패
	//}

	hFile = CreateFile(strTmpFile.GetString(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) 
	{
		hFile = CreateFile(szPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		bImage = false;
		if(hFile == INVALID_HANDLE_VALUE) 
		{
			return UINT_MAX; // 파일이 없다면 리턴
		}
	}

	//DWORD dwSize = GetFileSize (hFile, NULL); // 파일 사이즈를 얻는다.
	//if( ( dwSize == INVALID_FILE_SIZE ) || ( dwSize < ( STREAM_OFFSET + STREAM_SIZE ) ) )
	//{
	//	CloseHandle(hFile); // 파일 핸들을 닫는다.
	//	if( bImage )	DeleteFile(strTmpFile.GetString()); // 압축풀린 파일 삭제

	//	return UINT_MAX;
	//}

	// 파일에서 디스크립터를 읽어들인다.
	DWORD dwBytesRead = 0;
	std::vector<BYTE> vecBUFFER;
	vecBUFFER.resize( STREAM_SIZE );

	DWORD dwPtrLow = SetFilePointer(hFile, STREAM_OFFSET, NULL, FILE_BEGIN) ; 

	if (dwPtrLow == INVALID_SET_FILE_POINTER && ( GetLastError() != NO_ERROR ) )
	{ 
		CloseHandle(hFile); // 파일 핸들을 닫는다.
		if( bImage )	DeleteFile(strTmpFile.GetString()); // 압축풀린 파일 삭제

		return UINT_MAX;
	}

	BOOL bResult = ReadFile(hFile, &vecBUFFER[0], STREAM_SIZE, &dwBytesRead, NULL); 
	if ( bResult && !dwBytesRead )
	{
		CloseHandle(hFile); // 파일 핸들을 닫는다.
		if( bImage )	DeleteFile(strTmpFile.GetString()); // 압축풀린 파일 삭제

		return UINT_MAX;
	}

	CloseHandle(hFile); // 파일 핸들을 닫는다.
	if( bImage )	DeleteFile(strTmpFile.GetString()); // 압축풀린 파일 삭제

	DWORD dwSIZE = (DWORD) vecBlock.size();
	for ( DWORD i=0; i<dwSIZE; ++i )
	{
		SBLOCK_INFO & INFO = vecBlock[i];
		if ( INFO.strFileStream=="null" )	continue;

		if( vecBUFFER == INFO.vecFileStream )
		{
			return i;
		}

		//size_t nSIZE = INFO.vecFileStream.size();
		//std::vector<BYTE>::iterator found = std::search ( vecBUFFER.begin(), vecBUFFER.end(), INFO.vecFileStream.begin(), INFO.vecFileStream.end() );
		//if ( found != vecBUFFER.end() )
		//{
		//	return i;
		//}
	}

	return UINT_MAX;
}

DWORD CHPro::DoDETECT_BLOCK ( VECBLOCK& vecBlock, SIMAGE_INFO* pIMAGE )
{
	if ( pIMAGE == NULL ) return UINT_MAX;

	DWORD dwVal = UINT_MAX;
	dwVal = IsMatchStream ( pIMAGE->strPathName.c_str(), vecBlock );
	if ( dwVal != UINT_MAX )
	{
		return dwVal;
	}

	dwVal = UINT_MAX;
	DWORD dwSIZE = (DWORD) vecBlock.size();
	for ( DWORD i=0; i<dwSIZE; ++i )
	{
		Sleep( 1 );

		SBLOCK_INFO & INFO = vecBlock[i];
		if ( !INFO.bVALID )						continue;

		if ( INFO.strName!="null"				&& INFO.strName==pIMAGE->strFileName )						dwVal = i;
		if ( INFO.strProductName!="null"		&& INFO.strProductName==pIMAGE->strProductName )			dwVal = i;
		if ( INFO.strWindowText!="null"			&& INFO.strWindowText==pIMAGE->strWindowText )				dwVal = i;
		if ( INFO.strFileDescription!="null"	&& INFO.strFileDescription==pIMAGE->strFileDescription )	dwVal = i;
		if ( INFO.strLoadDll!="null"			&& pIMAGE->FINDDLL ( INFO.strLoadDll.c_str() ) )			dwVal = i;

		if ( dwVal != UINT_MAX )
		{
			return dwVal;
		}
	}

	return UINT_MAX;
}

CBLOCK_PROG::VECDETECT CHPro::ProcessDetection(VECBLOCK & vecBlock)
{
	CBLOCK_PROG::VECDETECT vecDETECT;

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
	if (hSnapShot == INVALID_HANDLE_VALUE)		return vecDETECT;

	CSimpleProcessAPI cPROCESS;

	PROCESSENTRY32 pEntry = {0};
	pEntry.dwSize = sizeof(PROCESSENTRY32);

	if(Process32First(hSnapShot, &pEntry))
	{
		do
		{
			Sleep( 1 );

			bool bFlag(true);
			IMAGEMAP_ITER pos = m_mapImage.find( pEntry.th32ProcessID ); // PID를 검색한다.

			if ( pos != m_mapImage.end() ) // PID가 존재 한다면...
			{
				bFlag = false;

				SIMAGE_INFO * imageInfo = pos->second;
				if ( imageInfo->strFileName != pEntry.szExeFile ) // 파일 이름이 다르다면...
				{
					// 기존에 있던 PID를 삭제
					delete pos->second;
					m_mapImage.erase(pos);
					bFlag = true;
				}
			}
			
			if ( bFlag ) // PID가 존재하지 않고 존재하더라도 파일 이름이 다르다면...
			{
				SIMAGE_INFO image_info;

				image_info.strFileName = pEntry.szExeFile; // 구조체에 파일이름 대입

				szWindowText[0] = '\0';
				EnumWindows( (WNDENUMPROC)EnumWindowsFunc, (LPARAM)pEntry.th32ProcessID );

				if( strlen(szWindowText) )
				{
					image_info.strWindowText = szWindowText; // 구조체에 윈도우 텍스트 대입
				}
				
				HANDLE hModuleSnap = CreateToolhelp32Snapshot ( TH32CS_SNAPMODULE, pEntry.th32ProcessID );
				if (hModuleSnap != INVALID_HANDLE_VALUE) 
				{
					bool bFound(false);

					MODULEENTRY32 me32 = {0};
					me32.dwSize = sizeof(MODULEENTRY32);

					if (Module32First(hModuleSnap, &me32)) 
					{ 
						do 
						{
							Sleep( 1 );

							if ( me32.th32ProcessID == pEntry.th32ProcessID ) 
							{ 
								bFound = true;
								break;
							} 
						} 
						while ( !bFound && Module32Next(hModuleSnap, &me32) ); 
					}

					if ( bFound )
					{
						CMiniVersion ver(me32.szExePath);
                        image_info.strPathName = me32.szExePath; // 전체 경로 대입

						_TCHAR szTemp[200] = "";
						ver.GetProductName(szTemp, sizeof(_TCHAR) * 200); // 제품이름을 얻는다.

						if( _tcslen(szTemp) )
							image_info.strProductName = szTemp; // 구조체에 ProductName 대입

						szTemp[0] = '\0';
						ver.GetFileDescription(szTemp, sizeof(_TCHAR) * 200); // 파일 디스트립션을 얻는다.

						if( _tcslen(szTemp) )
							image_info.strFileDescription = szTemp;

						//	이 프로세스가 사용하는 dll 모듈 검출.
						cPROCESS.BuildModuleList ( pEntry.th32ProcessID, image_info.listModuleFileNameList );

						// 각 맵에 삽입
						SIMAGE_INFO *pIMAGE = new SIMAGE_INFO;
						if ( pIMAGE )
						{
							*pIMAGE = image_info;
							m_mapImage.insert( std::make_pair ( pEntry.th32ProcessID, pIMAGE ) );
						}

						DWORD dwDETECT = DoDETECT_BLOCK ( vecBlock, pIMAGE );
						if ( dwDETECT!=UINT_MAX )
						{
							vecDETECT.push_back(CBLOCK_PROG::SDETECT(dwDETECT,false,pIMAGE->getinfo().c_str()));
						}

						ver.Release(); // CMiniVersion 릴리즈
					}
					
					CloseHandle (hModuleSnap);
				}
			}
		}
		while (Process32Next (hSnapShot,&pEntry));
	}
	CloseHandle (hSnapShot);

	return vecDETECT;
}

SIMAGE_INFO * CHPro::GetImageInfo(DWORD dwPID)
{
	IMAGEMAP_ITER pos = m_mapImage.find( dwPID );
	if ( pos != m_mapImage.end() )
		return pos->second;
	else
		return NULL;
}