#include "pch.h"
#include "./DxLandDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void SLAND_FILEMARK::Assign ( SLAND_FILEMARK_000 &sOldMark )
{
	dwNAVI_MARK = sOldMark.dwNAVI_MARK;
	dwGATE_MARK = sOldMark.dwGATE_MARK;
	dwCOLL_MARK = sOldMark.dwCOLL_MARK;
}

BOOL SLAND_FILEMARK::LoadSet ( CSerialFile &SFile )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	GASSERT ( dwVer==VERSION && dwSize==sizeof(SLAND_FILEMARK) );
	if ( dwVer!=VERSION || dwSize!=sizeof(SLAND_FILEMARK) )
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwSize );
		return FALSE;
	}

	SFile.ReadBuffer ( this, sizeof(SLAND_FILEMARK) );

	return TRUE;
}

BOOL SLAND_FILEMARK::SaveSet ( CSerialFile &SFile )
{
	SFile << static_cast<DWORD> ( VERSION );
	SFile << static_cast<DWORD> ( sizeof(SLAND_FILEMARK) );

	SFile.WriteBuffer ( this, sizeof(SLAND_FILEMARK) );

	return TRUE;
}
