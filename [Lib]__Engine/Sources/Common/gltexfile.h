#pragma once
#include "stringutils.h"

#include <string>
#include <vector>
#include <map>

class gltexfile
{
public:
	typedef std::string							STRING;
	typedef STRING::size_type					STRING_SIZE;
	typedef std::vector<STRING>					VECSTRING;
	typedef VECSTRING::iterator					VECSTRING_ITER;
	typedef std::multimap<STRING,VECSTRING*>	MAPFLAG;
	typedef MAPFLAG::iterator					MAPFLAG_ITER;

	typedef std::pair<MAPFLAG_ITER,MAPFLAG_ITER>	MAPFLAG_PAIR;

protected:
	STRING			m_strFileName;
	CSEPARATOR		m_cSeperator;

	MAPFLAG			m_mapFlags;
	VECSTRING*		m_pFOUND_VEC;	//	찾은 스트링 백터. 포인터가 무효화되지 않는다고 가정, (open후)

	STRING			m_strZipFileName;
	BOOL			m_bZipFile;

public:
	VECSTRING* findflag ( const std::string strFlag );
	VECSTRING* validflag ( const std::string strFLAG, const int nIDX, const int nSIZE );
	VECSTRING* validflag ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE );

	DWORD getflagnum ( const std::string strFLAG );
	DWORD getparamnum ( const std::string strFLAG );
	DWORD getparamnum ( const DWORD dwITER, const std::string strFLAG );

	void SetZipFile ( const std::string & strName )
	{
		m_strZipFileName = strName;
		m_bZipFile = TRUE;
	}

public:
	void reg_sep ( char cVAL );
	bool open ( const std::string strFile, bool bDEC, BOOL bPack = FALSE );
	void clean ();

public:
	template<typename TYPE>
	bool getflag ( const std::string strFLAG, const int nIDX, const int nSIZE, TYPE &VALUE )
	{
		VECSTRING* pFOUND = validflag ( strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		VALUE = static_cast<TYPE> ( atoi(strVAL.c_str()) );

		return true;
	}

	template<>
	bool getflag<bool> ( const std::string strFLAG, const int nIDX, const int nSIZE, bool &bVALUE )
	{
		VECSTRING* pFOUND = validflag ( strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		int nTMP = atoi ( strVAL.c_str() );
		bVALUE = ( 0!=nTMP );

		return true;
	}

	template<>
	bool getflag<float> ( const std::string strFLAG, const int nIDX, const int nSIZE, float &fVALUE )
	{
		VECSTRING* pFOUND = validflag ( strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		fVALUE = (float) atof ( strVAL.c_str() );

		return true;
	}

	template<>
	bool getflag<std::string> ( const std::string strFLAG, const int nIDX, const int nSIZE, std::string &strVALUE )
	{
		VECSTRING* pFOUND = validflag ( strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL;

		return true;
	}

	template<>
	bool getflag<CString> ( const std::string strFLAG, const int nIDX, const int nSIZE, CString &strVALUE )
	{
		VECSTRING* pFOUND = validflag ( strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL.c_str();

		return true;
	}

	bool getflag_sz ( const std::string strFLAG, const int nIDX, const int nSIZE, char* szVALUE, int nStrLen )
	{
		GASSERT(szVALUE&&"gltexfile::getflag_sz");
		if ( NULL==szVALUE )	return false;

		VECSTRING* pFOUND = validflag ( strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		StringCchCopy( szVALUE, nStrLen, strVAL.c_str() );

		return true;
	}

	template<typename TYPE>
	bool getflag ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE, TYPE &VALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, strFLAG, nIDX, nSIZE );
		if ( !pFOUND )			return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		VALUE = static_cast<TYPE> ( atoi(strVAL.c_str()) );

		return true;
	}

	template<>
	bool getflag<bool> ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE, bool &bVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		int nTMP = atoi ( strVAL.c_str() );
		bVALUE = ( 0!=nTMP );

		return true;
	}

	template<>
	bool getflag<float> ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE, float &fVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		fVALUE = (float) atof ( strVAL.c_str() );

		return true;
	}

	template<>
	bool getflag<std::string> ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE, std::string &strVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL;

		return true;
	}

	template<>
	bool getflag<CString> ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE, CString &strVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL.c_str();

		return true;
	}

	bool getflag_sz ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE, char* szVALUE, int nStrLen )
	{
		GASSERT(szVALUE&&"gltexfile::getflag_sz");
		if ( NULL==szVALUE )	return false;

		VECSTRING* pFOUND = validflag ( dwITER, strFLAG, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		StringCchCopy( szVALUE, nStrLen, strVAL.c_str() );

		return true;
	}

public:
	gltexfile(void);
	~gltexfile(void);
};
