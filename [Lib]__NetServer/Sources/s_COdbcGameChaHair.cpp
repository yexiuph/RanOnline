#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 캐릭터의 머리색을 변경한다.
* \return DB_OK, DB_ERROR
*/
int COdbcManager::SetChaHairColor(DWORD dwChaNum, int nHairColor)
{
	if (nHairColor < 0)
		return DB_ERROR;

	//std::strstream strTemp;
	//strTemp << "{call UpdateChaHairColor(";
	//strTemp << dwChaNum   << ",";
	//strTemp << nHairColor;
	//strTemp << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaHairColor(%u,%d,?)}", dwChaNum, nHairColor );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

/**
* 캐릭터의 헤어스타일을 변경한다.
* \return DB_OK, DB_ERROR
*/
int COdbcManager::SetChaHairStyle(DWORD dwChaNum, int nHairStyle)
{	
	if (nHairStyle < 0)
		return DB_ERROR;

	//std::strstream strTemp;
	//strTemp << "{call UpdateChaHairStyle(";
	//strTemp << dwChaNum   << ",";
	//strTemp << nHairStyle;
	//strTemp << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaHairStyle(%u,%d,?)}", dwChaNum, nHairStyle );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}


int COdbcManager::SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle)
{
	if (nFaceStyle < 0)
		return DB_ERROR;

	//std::strstream strTemp;
	//strTemp << "{call UpdateChaHairStyle(";
	//strTemp << dwChaNum   << ",";
	//strTemp << nHairStyle;
	//strTemp << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaFaceStyle(%u,%d,?)}", dwChaNum, nFaceStyle );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}


int COdbcManager::SetChaGenderChange( DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor)
{
	if (dwChaNum <= 0 || nClass <= 0 || nHairColor < 0)
	{
		Print(_T("ERROR:COdbcManager::SetChaGenderChange"));
		return DB_ERROR;
	}

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdateChaGender(%u,%d,%d,%d,%d,%d,?)}",
														dwChaNum, nClass, nSex, nFace, nHair, nHairColor );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}