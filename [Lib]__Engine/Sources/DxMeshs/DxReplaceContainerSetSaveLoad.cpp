#include "pch.h"
#include "DxFrameMesh.h"
#include "DxReplaceContainer.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DWORD	DXPIECEOBJ::GetSaveSetSize ()
{	
	DWORD	NODESIZE = 0;
	NODESIZE += sizeof ( float );	//	fCurTime;
	NODESIZE += sizeof ( BOOL );	//	if ( szFileName ) BOOL
	if ( szFrameName )
	{
        NODESIZE += sizeof ( int );	//	StrLength;
        NODESIZE += sizeof ( char ) * ( strlen ( szFrameName ) + 1 );
	}

	NODESIZE += sizeof ( BOOL );	//	if ( pReplacePiece )
	if ( pReplacePiece )
	{
		NODESIZE += sizeof ( BOOL );	//	if ( pReplacePiece->szFileName )
		if ( pReplacePiece->szFileName )
		{
			NODESIZE += sizeof ( int );
			NODESIZE += sizeof ( char ) * ( strlen ( pReplacePiece->szFileName ) + 1 );
		}
	}

	return NODESIZE;
}

void	DXPIECEOBJ::SaveSet ( CSerialFile &SFile )
{
	SFile << m_dwVERSION;
	SFile << GetSaveSetSize ();

	SFile << fCurTime;

	if ( szFrameName )
	{
		SFile << BOOL ( TRUE );
		int StrLength = strlen ( szFrameName ) + 1;
		SFile << StrLength;
		SFile.WriteBuffer ( szFrameName, sizeof ( char ) * StrLength );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pReplacePiece )
	{
		SFile << BOOL ( TRUE );
		
		if ( pReplacePiece->szFileName )
		{
			SFile << BOOL ( TRUE );
			int	StrLength = strlen ( pReplacePiece->szFileName ) + 1;
			SFile << StrLength;
			SFile.WriteBuffer ( pReplacePiece->szFileName, sizeof ( char ) * StrLength );
		}
		else
		{
			SFile << BOOL ( FALSE );
		}
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pNext )
	{
		SFile << BOOL ( TRUE );
		pNext->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}