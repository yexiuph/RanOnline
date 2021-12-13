#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* ĳ���� �̸��� �����Ѵ�.
* \param dwChaNum ĳ���͹�ȣ
* \param szCharName ������ ĳ�����̸�
* \return DB_OK, DB_ERROR
*/
int COdbcManager::RenameCharacter (DWORD dwChaNum, char* szCharName)
{
	SQLRETURN	sReturn = 0;	

	CString strName (szCharName);
	strName.Trim(_T(" ")); // �յ� ��������
	strName.Replace(_T("'"), _T("''")); // ' ���ڿ� '' �� ġȯ

	// ĳ���� �̸��� ���� ���̸� �Ѵ��� �����Ѵ�.
    if (strName.GetLength() >= (CHR_ID_LENGTH-1))
	{
		return DB_ERROR;
	}

    // ĳ���� �̸��� �ٲ۴�.	
	//std::strstream strTemp;
	//strTemp << "{call RenameCharacter(";
	//strTemp << dwChaNum   << ",'";
	//strTemp << strName.GetString();
	//strTemp << "',?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call RenameCharacter(%d,'%s',?)}", dwChaNum, strName.GetString() ); 

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}