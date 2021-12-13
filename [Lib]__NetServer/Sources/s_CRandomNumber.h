#ifndef S_CRANDOM_NUMBER_H
#define S_CRANDOM_NUMBER_H 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace SERVER_UTIL
{
	/**
	* \ingroup NetServerLib
	*
	* \par requirements
	* win98 or later\n
	* win2k or later\n
	* MFC\n
	*
	* \version 1.0
	* first version
	*
	* \date 2002.05.30
	*
	* \author jgkim
	*
	* \par license
	* Copyright 2002-2005 (c) Mincoms. All rights reserved.
	* 
	* \todo 
	*
	* \bug 
	*
	*/
	class CRandomNumber
	{
	public:
		CRandomNumber(void);
		virtual ~CRandomNumber(void);
		
		/**
		* Generate randim string
		* \param str 생성된 문자열을 받을 곳
		* \param Length 문자열길이
		* \param Small 
		* \param Capital 
		* \param Numbers 
		* \return true / false
		*/
		bool GenerateRandomString(CString& str, int Length, int Small, int Capital, int Numbers);
	};
}

#endif // S_CRANDOM_NUMBER_H