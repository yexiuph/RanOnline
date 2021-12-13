#pragma once

class	CSerialFile;

//	만약, 포인터형 데이타가 생길경우,
//	연산자 오버로딩 해야합니다.

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