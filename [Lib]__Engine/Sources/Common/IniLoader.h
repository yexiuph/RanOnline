#pragma once

#include <string>
#include <vector>
#include <map>

class CIniLoader
{
public:
	typedef std::string							STRING;
	typedef STRING::size_type					STRING_SIZE;

	typedef std::vector<STRING>					VECSTRING;
	typedef VECSTRING::iterator					VECSTRING_ITER;

	typedef std::multimap<STRING,VECSTRING*>	MAPKEY;
	typedef MAPKEY::iterator					MAPKEY_ITER;

	typedef std::pair<MAPKEY_ITER,MAPKEY_ITER>	MAPKEY_PAIR;

	typedef std::map<STRING,MAPKEY*>			MAPSECTION;
	typedef MAPSECTION::iterator				MAPSECTION_ITER;

public:
	CIniLoader( STRING sep = _T(",") );
	~CIniLoader();

	bool open( const STRING strFile, bool bDEC, BOOL bPack = FALSE );
	void reg_sep( const STRING & strSep ) { m_fieldsep = strSep; }

	DWORD GetKeySize( const STRING section, const STRING key );
	UINT GetKeyDataSize( const STRING section, const STRING key );

	void SetZipFile ( const STRING & strName )
	{
		m_strZipFileName = strName;
		m_bZipFile = TRUE;
	}

	void SetUseErrorMsgBox( BOOL bUse )
	{
		m_bUseErrorMsgBox = bUse;
	}

private:
	STRING m_fieldsep;			// 구분 문자들
	MAPSECTION m_mapSection;
	STRING m_strFileName;

	STRING m_strZipFileName;
	BOOL m_bZipFile;

	BOOL m_bUseErrorMsgBox;

	int split( const STRING & str, VECSTRING * pVec );
	int advplain( const STRING & line, STRING & fld, UINT i );
	int advquoted( const STRING & line, STRING & fld, UINT i );
	int findfirstof( const STRING & line, const STRING & sep, UINT i );

	VECSTRING* FindKey( const STRING section, const std::string key );
	MAPKEY* FindSection( const STRING section );

	VECSTRING* validflag( const STRING section, const STRING key, const int nIDX, const int nSIZE );
	VECSTRING* validflag( const DWORD dwITER, const STRING section, const STRING key, const int nIDX, const int nSIZE );

	bool IsSection( const CString & line, STRING & sec );
	bool IsNotes( const CString & line );

	int GetKeyData( const CString & line, STRING & key, VECSTRING * pVec );

	void ErrorMegBox( const TCHAR * section, const TCHAR * szText );
	void ErrorMegBox( const TCHAR * section, const TCHAR * key, const TCHAR * szText );
	void ErrorMegBox( const TCHAR * section, const TCHAR * key, const DWORD dwLine, const TCHAR * szText );

public:
	template<typename TYPE>
	bool getflag ( const STRING section, const STRING key, const int nIDX, const int nSIZE, TYPE &VALUE )
	{
		VECSTRING* pFOUND = validflag( section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		VALUE = static_cast<TYPE> ( _tstoi(strVAL.c_str()) );

		return true;
	}

	template<>
	bool getflag<bool> ( const STRING section, const STRING key, const int nIDX, const int nSIZE, bool &bVALUE )
	{
		VECSTRING* pFOUND = validflag( section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		int nTMP = _tstoi ( strVAL.c_str() );
		bVALUE = ( 0!=nTMP );

		return true;
	}

	template<>
	bool getflag<float> ( const STRING section, const STRING key, const int nIDX, const int nSIZE, float &fVALUE )
	{
		VECSTRING* pFOUND = validflag( section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		fVALUE = (float) _tstof ( strVAL.c_str() );

		return true;
	}

	template<>
	bool getflag<STRING> ( const STRING section, const STRING key, const int nIDX, const int nSIZE, STRING &strVALUE )
	{
		VECSTRING* pFOUND = validflag( section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL;

		return true;
	}

	template<>
	bool getflag<CString> ( const STRING section, const STRING key, const int nIDX, const int nSIZE, CString &strVALUE )
	{
		VECSTRING* pFOUND = validflag( section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL.c_str();

		return true;
	}

	bool getflag_sz ( const STRING section, const STRING key, const int nIDX, const int nSIZE, TCHAR* szVALUE, int nStrLen )
	{
		GASSERT(szVALUE&&"gltexfile::getflag_sz");
		if ( NULL==szVALUE )	return false;

		VECSTRING* pFOUND = validflag( section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		StringCchCopy( szVALUE, nStrLen, strVAL.c_str() );

		return true;
	}

	template<typename TYPE>
		bool getflag ( const DWORD dwITER, const STRING section, const STRING key, const int nIDX, const int nSIZE, TYPE &VALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, section, key, nIDX, nSIZE );
		if ( !pFOUND )			return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		VALUE = static_cast<TYPE> ( atoi(strVAL.c_str()) );

		return true;
	}

	template<>
		bool getflag<bool> ( const DWORD dwITER, const STRING section, const STRING key, const int nIDX, const int nSIZE, bool &bVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		int nTMP = atoi ( strVAL.c_str() );
		bVALUE = ( 0!=nTMP );

		return true;
	}

	template<>
		bool getflag<float> ( const DWORD dwITER, const STRING section, const STRING key, const int nIDX, const int nSIZE, float &fVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		fVALUE = (float) atof ( strVAL.c_str() );

		return true;
	}

	template<>
		bool getflag<STRING> ( const DWORD dwITER, const STRING section, const STRING key, const int nIDX, const int nSIZE, STRING &strVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL;

		return true;
	}

	template<>
		bool getflag<CString> ( const DWORD dwITER, const STRING section, const STRING key, const int nIDX, const int nSIZE, CString &strVALUE )
	{
		VECSTRING* pFOUND = validflag ( dwITER, section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		strVALUE = strVAL.c_str();

		return true;
	}

	bool getflag_sz ( const DWORD dwITER, const STRING section, const STRING key, const int nIDX, const int nSIZE, char* szVALUE, int nStrLen )
	{
		GASSERT(szVALUE&&"gltexfile::getflag_sz");
		if ( NULL==szVALUE )	return false;

		VECSTRING* pFOUND = validflag ( dwITER, section, key, nIDX, nSIZE );
		if ( !pFOUND )	return false;

		STRING &strVAL = (*pFOUND)[nIDX];
		StringCchCopy( szVALUE, nStrLen, strVAL.c_str() );

		return true;
	}
};
