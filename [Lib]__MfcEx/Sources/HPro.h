// By ���
#pragma once

#include <string>
#include <vector>
#include <map>
#include "BlockProg.h"

struct SIMAGE_INFO
{
	std::string strPathName;
	std::string	strFileName;
	std::string	strWindowText;
	std::string	strProductName;
	std::string	strFileDescription;
	CStringList listModuleFileNameList;

	bool FINDDLL ( const char* szDLL );

	SIMAGE_INFO& operator= ( const SIMAGE_INFO& value )
	{
		strPathName = value.strPathName;
		strFileName = value.strFileName;
		strWindowText = value.strWindowText;
		strProductName = value.strProductName;
		strFileDescription = value.strFileDescription;
		
		POSITION pos = value.listModuleFileNameList.GetHeadPosition ();
		for ( ; pos != NULL; )
		{
			CString strElemt = value.listModuleFileNameList.GetNext ( pos );

			listModuleFileNameList.AddTail ( strElemt );
		}

		return *this;
	}

	std::string getinfo ();
};

class CHPro
{
private:
	typedef std::map<DWORD, SIMAGE_INFO*>		IMAGEMAP;
	typedef IMAGEMAP::iterator					IMAGEMAP_ITER;

	typedef std::multimap<std::string, DWORD>	STRMAP;
	typedef STRMAP::iterator					STRMAP_ITER;

protected:
	IMAGEMAP	m_mapImage;

public:
	DWORD DoDETECT_BLOCK ( VECBLOCK& vecBlock, SIMAGE_INFO* pIMAGE );
	bool CreateProcessUPX(const char * szPathName, CString &strTmpFile );
	DWORD IsMatchStream(const char * szPathName, VECBLOCK& vecBlock);
	CBLOCK_PROG::VECDETECT ProcessDetection (VECBLOCK & vecBlock);		// ���μ����� �˻��ϴ� �Լ�
	VOID Release();											// ����Ʈ�� �����͸� ����

public:
	SIMAGE_INFO * GetImageInfo(DWORD);						// �̹��� ������ ����ü �����͸� ��´�.

public:
	CHPro();
	~CHPro();
};
