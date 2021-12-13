#include "pch.h"
#include "creportfiles.h"
#include "../Hash/CHash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace REPORTFILES
{
	CString GetAppPath ()
	{
		CString _strAppPath;

		// Note : 실행파일의 경로를 찾아서 저장한다. - 반드시 CBasicWnd(CD3dApp) 이전에 수행해야 한다.
		//
		CString strCommandLine;

		TCHAR szPath[MAX_PATH] = {0};
		GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
		strCommandLine = szPath;

		if ( !strCommandLine.IsEmpty() )
		{
			DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
			if ( dwFind != -1 )
			{
				_strAppPath = strCommandLine.Left ( dwFind );
				if ( _strAppPath.IsEmpty() )	return CString();
				
				if ( _strAppPath.GetAt(0) == '"' )		_strAppPath = _strAppPath.Right ( _strAppPath.GetLength() - 1 );
			}
		}

		return _strAppPath;
	}

	const std::string g_strReport = "_filelist.lst";
	const std::string g_strReportExe = "reportfiles.exe";
	
	std::fstream g_file;

	// Hash 를 하기위한 오브젝트.
	HASH::CHash g_hashObj;

	void MakeFileList( const char *szPath )
	{
		// MD5 해쉬를 위해서 설정한다.		
		// Set the algorithm
		g_hashObj.SetHashAlgorithm( HASH::MD5 ); // MD5 로 설정
		// Set the operation
		g_hashObj.SetHashOperation( HASH::FILE_HASH ); // 파일 해쉬로 설정
		// 스페이스 없는 대문자.
		g_hashObj.SetHashFormat( HASH::UPPERCASE_NOSPACES );

		CString strROOT = GetAppPath ();
		strROOT.MakeLower();
		std::string strRoot = strROOT;		

		std::string strSAVEFILE = strRoot + "\\" + g_strReport.c_str();
		g_file.open ( strSAVEFILE.c_str(), std::ios_base::out | std::ios_base::trunc );

		g_file << "source_path" << "	" << strRoot << std::endl;
		g_file << std::endl;
		g_file << "\\" << std::endl;

		PathRecurse ( strRoot, strRoot );

		g_file.close();
	}

	void PathRecurse(
		const std::string strRootPath,
		const std::string strCurPath )
	{
		CString strRoot = GetAppPath ();

		CFileFind finder;

		//	build a string with wildcards
		std::string strWildcard ( strCurPath );
		strWildcard += _T("\\*.*");

		//	start working for files
		BOOL bWorking = finder.FindFile( strWildcard.c_str() );

		while ( bWorking )
		{
			bWorking = finder.FindNextFile ();

			//	skip . and .. files; otherwise, we'd
			//	recur infinitely!
			if ( finder.IsDots() ) continue;

			//	if it's a directory, recursively search it
			if ( !finder.IsDirectory() )
			{
				CString strNAME = finder.GetFileName();
				strNAME.MakeLower ();
				CString strPATH = finder.GetFilePath();
				
				// 2006-01-17 Jgkim
				// versionmanager.exe
	            // versionmanager.cfg
	            // makecab.exe
				// P_make_cap.bat
				if (strNAME != CString(g_strReport.c_str()) &&
					strNAME != CString(g_strReportExe.c_str()) &&
					strNAME.CompareNoCase(_T("versionmanager.exe")) != 0 &&
					strNAME.CompareNoCase(_T("versionmanager.cfg")) != 0 &&
					strNAME.CompareNoCase(_T("makecab.exe")) != 0 &&					
					strNAME.CompareNoCase(_T("agentserver.exe")) != 0 &&
					strNAME.CompareNoCase(_T("fieldserver.exe")) != 0 &&
					strNAME.CompareNoCase(_T("loginserver.exe")) != 0 &&
					strNAME.CompareNoCase(_T("sessionserver.exe")) != 0 &&
					strNAME.CompareNoCase(_T("gmcharedit.exe")) != 0 &&
					strNAME.CompareNoCase(_T("lp_make_cab.bat")) != 0 && 
					strNAME.CompareNoCase(_T("lp_make_cap.bat")) != 0 && 
					strNAME.CompareNoCase(_T("p_make_cab.bat")) != 0 &&					
					strNAME.CompareNoCase(_T("p_make_cap.bat")) != 0 &&
					strNAME.CompareNoCase(_T("filelist_cap.bat")) != 0 &&
					strNAME.CompareNoCase(_T("NPGEClient.exe")) != 0 &&
					strNAME.CompareNoCase(_T("NPGEClient.LOG")) != 0 &&
					strNAME.CompareNoCase(_T("MakeCabL.bat")) != 0 &&
					strNAME.CompareNoCase(_T("MakeCabLP.bat")) != 0 &&
					strNAME.CompareNoCase(_T("ServerController.exe")) !=0 &&
					strNAME.CompareNoCase(_T("Launcher.exe")) !=0 &&
					strNAME.CompareNoCase(_T("Launcherpatch.exe")) !=0 &&
					strNAME.CompareNoCase(_T("glogicserver.rcc")) !=0)
				{
#ifdef CH_PARAM
					// Set the file
					g_hashObj.SetHashFile( strPATH );
					// Hash the file
					CString strHash = g_hashObj.DoHash();
					g_file << "\t" << strNAME.GetString() << "\t" << strHash.GetString() << std::endl;
#else
					g_file << "\t" << strNAME.GetString() << std::endl;
#endif
				}
			}
		}

		finder.Close();

		//	start working for files
		bWorking = finder.FindFile( strWildcard.c_str() );

		while ( bWorking )
		{
			bWorking = finder.FindNextFile ();

			//	skip . and .. files; otherwise, we'd
			//	recur infinitely!
			if ( finder.IsDots() ) continue;

			//	if it's a directory, recursively search it
			if ( finder.IsDirectory() )
			{
				CString strPATH = finder.GetFilePath();
				strPATH.MakeLower ();
				std::string strPath = strPATH;
				
				std::string strSubPath = std::string ( strPath.begin()+strRootPath.length(), strPath.end() );
				
				g_file << std::endl;
				g_file << strSubPath.c_str() << "\\" << std::endl;

				PathRecurse ( strRootPath, strPath );
			}
		}

		finder.Close();
	}
};

