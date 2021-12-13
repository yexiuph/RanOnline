#ifndef S_UTIL_H_
#define S_UTIL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace SERVER_UTIL
{
	/**
	* �ֹι�ȣ�� üũ�ؼ� �������� �ƴ����� �˻�
	* \param strCheck �ֹε�Ϲ�ȣ
	* \param nCheckAge ������ �Ǵ� ���� 
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
	* int ���� ������ ���ڸ� �߻���Ų��.
	* ���ڴ� 1 ���� nMax ���� �߻���Ų��.
	* ����:0 �� �߻���Ű�� �ʴ´�.
	*/
	int makeRandomNumber(int nMax);
}

#endif // S_UTIL_H_
