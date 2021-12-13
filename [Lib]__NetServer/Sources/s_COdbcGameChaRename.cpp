#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 캐릭터 이름을 변경한다.
* \param dwChaNum 캐릭터번호
* \param szCharName 변경할 캐릭터이름
* \return DB_OK, DB_ERROR
*/
int COdbcManager::RenameCharacter (DWORD dwChaNum, char* szCharName)
{
	SQLRETURN	sReturn = 0;	

	CString strName (szCharName);
	strName.Trim(_T(" ")); // 앞뒤 공백제거
	strName.Replace(_T("'"), _T("''")); // ' 문자열 '' 로 치환

	// 캐릭터 이름이 허용된 길이를 넘는지 조사한다.
    if (strName.GetLength() >= (CHR_ID_LENGTH-1))
	{
		return DB_ERROR;
	}

    // 캐릭터 이름을 바꾼다.	
	//std::strstream strTemp;
	//strTemp << "{call RenameCharacter(";
	//strTemp << dwChaNum   << ",'";
	//strTemp << strName.GetString();
	//strTemp << "',?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call RenameCharacter(%d,'%s',?)}", dwChaNum, strName.GetString() ); 

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}