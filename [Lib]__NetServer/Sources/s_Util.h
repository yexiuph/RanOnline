#ifndef S_UTIL_H_
#define S_UTIL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace SERVER_UTIL
{
	/**
	* 주민번호를 체크해서 성인인지 아닌지를 검사
	* \param strCheck 주민등록번호
	* \param nCheckAge 기준이 되는 나이 
	* \return true / false
	*/
	bool CheckAdult(CString strCheck, int nCheckAge);

	/**
	* Fast getting file size function
	* MS Windows version
	* \param szFileName File Name (FULL PATH)
	* \return File size
	*/
	//__int64    GetFileSize64 ( const char * szFileName ); 

	/**
	* Fast getting file size function
	* C++ version
	* \param szFileName File Name (FULL PATH)
	* \return File size
	*/
	//int        GetFileSizeCPP( const char * szFileName ); 

	/**
	* Fast getting file size function
	* MFC version
	* \param szFileName File Name (FULL PATH)
	* \return File Size
	*/
	ULONGLONG  GetFileSizeMFC( const char * szFileName );
	// int     GetFileSize  ( const char * szFileName ); // Standard C++ library version

	/**
	* int 형의 랜덤한 숫자를 발생시킨다.
	* 숫자는 1 부터 nMax 까지 발생시킨다.
	* 주의:0 은 발생시키지 않는다.
	*/
	int makeRandomNumber(int nMax);
}

#endif // S_UTIL_H_
