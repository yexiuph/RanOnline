#pragma once

class	CSerialFile;

//	����, �������� ����Ÿ�� ������,
//	������ �����ε� �ؾ��մϴ�.

struct	SBGMDATA
{
static	WORD	VER;

	CString	strFileName;

public:
	BOOL	SaveSet ( CSerialFile& SFile );
	BOOL	LoadSet ( CSerialFile& SFile );

public:
	void	SetFile ( CString &strFile )
	{
		strFileName = strFile;
	}
	void	ClearFile ()
	{
		strFileName.Empty ();
	}
};