#include "pch.h"
#include "s_Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SERVER_UTIL
{
	// MS Windows version
	// Win 98, Win Me, Win NT, Win 2000, Win XP
	//__int64 GetFileSize64( const char * szFileName ) 
	//{ 
	//	struct __stat64 fileStat; 
	//	int err = _stat64( szFileName, &fileStat ); 
	//	if (0 != err) return 0; 
	//	return fileStat.st_size; 
	//}

	// C++ library version
	// Many C++ compilers provide the C run-time function stat():
	// First check. user compiler and library
	//int GetFileSizeCPP( const char * szFileName ) 
	//{ 
	//	struct stat fileStat; 
	//	int err = stat( szFileName, &fileStat ); 
	//	if (0 != err) return 0; 
	//	return fileStat.st_size; 
	//}
	
	// MFC version
	// slow...
	ULONGLONG  GetFileSizeMFC( const char * szFileName )
	{
		CFile* pFile = NULL;
		ULONGLONG llLength=0;

		TRY
		{
			pFile = new CFile(szFileName, CFile::modeRead | CFile::shareDenyNone);
			llLength = pFile->GetLength();
		}
		CATCH(CFileException, pEx)
		{
			// Simply show an error message to the user.
			pEx->ReportError();			
		}
		AND_CATCH(CMemoryException, pEx)
		{
			// We can't recover from this memory exception, so we'll
			// just terminate the app without any cleanup. Normally, an
			// an application should do everything it possibly can to
			// clean up properly and _not_ call AfxAbort().
			AfxAbort();			
		}
		END_CATCH
		// If an exception occurs in the CFile constructor,
		// the language will free the memory allocated by new
		// and will not complete the assignment to pFile.
		// Thus, our clean-up code needs to test for NULL.
		if (pFile != NULL)   
		{
			pFile->Close();
			delete pFile;
			return llLength;
		}
		else
		{
			return 0;
		}
	}

    // Standard C++ library version
	/*
	int FileSize( const char* szFileName )
	{
		std::ifstream f;
		f.open(sFileName, std::ios_base::binary | std::ios_base::in);
		if (!f.good() || f.eof() || !f.is_open()) 
		{ 
			return 0; 
		}
		
		f.seekg(0, std::ios_base::beg);
		std::ifstream::pos_type begin_pos = f.tellg();
		f.seekg(0, std::ios_base::end);
		
		return static_cast<int>(f.tellg() - begin_pos);
	}
	*/
};