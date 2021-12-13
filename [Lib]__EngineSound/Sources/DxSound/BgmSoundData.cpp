#include "pch.h"
#include "BgmSoundData.h"
#include "SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


WORD	SBGMDATA::VER = 1;

BOOL	SBGMDATA::SaveSet ( CSerialFile& SFile )
{
	SFile << (BOOL)((strFileName.GetLength ()) ? TRUE : FALSE);
	if ( strFileName.GetLength () )
	{
		int StrLength = strFileName.GetLength () + 1;
		SFile.WriteBuffer ( &StrLength, sizeof ( int ) );
		SFile.WriteBuffer ( strFileName.GetString(), sizeof ( char ) * StrLength );
	}

	return TRUE;
}

BOOL	SBGMDATA::LoadSet ( CSerialFile& SFile )
{
	BOOL bExist = FALSE;
	SFile >> bExist;
	if ( bExist )
	{
		int StrLength = 0;
		SFile.ReadBuffer ( &StrLength, sizeof ( int ) );        
		
		char* szName = new char [ StrLength ];

		SFile.ReadBuffer ( szName, StrLength );
		strFileName = szName;

		SAFE_DELETE_ARRAY ( szName );
	}

	return TRUE;
}