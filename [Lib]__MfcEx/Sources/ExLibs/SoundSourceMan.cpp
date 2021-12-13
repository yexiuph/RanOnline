#include "pch.h"
#include "SoundSourceMan.h"
#include "SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////////////////////////////////////////
//	번호	:	$6
//	요약	:	버전관리
//	설명	:	
//				
//				
//				
//	로그
//	$2.	2002-11-05T10:32	[버퍼관리 추가, 101]
//	$1.	2002-10-XXT??:??	[최초버전, 100]
//				
WORD	SSoundSource::VER = 101;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//	번호	:	$1
//	요약	:	고유 ID 생성 멤버
//	설명	:	최초 생성시에는 -1이어야 한다. 여기서 말하는 '최초'란, 프로그램을
//				사용한 이례로 처음으로 데이타가 등록될때를 의미한다.
//				-1로 함으로써 실제 데이타의 ID는 0번부터 생성된다.
//				물론, 이것은 세이브파일이 생성되면, 그 파일로부터 고유ID를 읽어
//				들임인다. 그 때부터는 -1로 세팅하는것은 그저 초기화의 의미에
//				불과하다.
WORD		CSoundSourceMan::m_UniqueID = -1;
////////////////////////////////////////////////////////////////////////////////

CSoundSourceMan::CSoundSourceMan()
{
}

CSoundSourceMan::~CSoundSourceMan()
{
	ClearTable();
}

void	CSoundSourceMan::AddRecord ( SSoundSource *pRecord, CString &FullPath )
{
	CString	Target;
	Target = m_DataDirectory + pRecord->FileName;
	CopyFile( FullPath.GetString(), Target.GetString(), FALSE );

	m_SoundSourceTable.AddTail ( pRecord );		
}

void	CSoundSourceMan::SetRecord ( WORD ID, BOOL bChangeFile, CString FilePath )
{
	PSOUNDSOURCE	pCurRecord = NULL;

	pCurRecord = FindRecord ( ID );
	if ( !pCurRecord )
	{
		return ;
	}

	//	<--	파일 자체가 바뀌는 경우
	if ( bChangeFile )
	{
		CString SrcFile = FilePath;
		CString DstFile = m_DataDirectory + pCurRecord->FileName;

		CopyFile ( SrcFile.GetString(), DstFile.GetString (), FALSE );
	}
	//	<--	타입이 바뀜에 따라, 파일 이름만 변경되는 경우	
	else
	{
		CString SrcFile = m_DataDirectory + pCurRecord->FileName;

		//	세부 내용만 변경한 경우
		if ( SrcFile != FilePath )
		{
			CopyFile ( FilePath.GetString(), SrcFile.GetString (), FALSE );
			DeleteFile ( FilePath.GetString () );
		}
	}
}



void	CSoundSourceMan::DelRecord ( WORD ID )
{
	PSOUNDSOURCE	pDelRecord = NULL;

	pDelRecord = FindRecord ( ID );
	if ( pDelRecord )
	{
		CString	Target;
		Target = m_DataDirectory + pDelRecord->FileName;
		DeleteFile ( Target.GetString() );

		delete pDelRecord;
		pDelRecord = NULL;

		m_SoundSourceTable.DelCurrent ();
	}
}


SSoundSource* CSoundSourceMan::GetRecord ()
{
	PSOUNDSOURCE	pCurRecord = NULL;
	m_SoundSourceTable.GetCurrent ( pCurRecord );
	return pCurRecord;
}


SSoundSource*	CSoundSourceMan::GetHead ()
{
	PSOUNDSOURCE	pHeadRecord = NULL;
	m_SoundSourceTable.GetHead ( pHeadRecord );
	return pHeadRecord;
}

SSoundSource*	CSoundSourceMan::GetTail ()
{
	PSOUNDSOURCE	pTailRecord = NULL;
	m_SoundSourceTable.GetTail ( pTailRecord );
	return pTailRecord;
}


SSoundSource*	CSoundSourceMan::FindRecord ( WORD ID )
{
	m_SoundSourceTable.SetHead ();
	while ( !m_SoundSourceTable.IsEnd () )
	{
		PSOUNDSOURCE pCurRecord = NULL;
		m_SoundSourceTable.GetCurrent ( pCurRecord );

		if ( pCurRecord->ID == ID )
		{
			return pCurRecord;
		}

		m_SoundSourceTable.GoNext ();
	}

	return NULL;
}

void	CSoundSourceMan::GoHead	()
{
	m_SoundSourceTable.SetHead();
}

void	CSoundSourceMan::GoTail	()
{
	m_SoundSourceTable.SetTail();
}

void	CSoundSourceMan::GoNext	()
{
	m_SoundSourceTable.GoNext();
}

void	CSoundSourceMan::GoPrev	()
{
	m_SoundSourceTable.GoPrev();
}

BOOL	CSoundSourceMan::IsEnd	()
{
	return m_SoundSourceTable.IsEnd();
}

int		CSoundSourceMan::GetCount ()
{
	return (int)m_SoundSourceTable.GetAmount();
}

void	CSoundSourceMan::ClearTable()
{
	PSOUNDSOURCE	pDelRecord = NULL;
	
	m_SoundSourceTable.SetHead();
	while ( !m_SoundSourceTable.IsEnd() )
	{
		m_SoundSourceTable.GetCurrent ( pDelRecord );
		
		if ( pDelRecord )
		{
			delete pDelRecord;
			pDelRecord = NULL;
		}

		m_SoundSourceTable.GoNext ();
	}

	m_SoundSourceTable.RemoveAll();
}


BOOL	CSoundSourceMan::SaveFile ( const char *szFileName )
{
	if ( !szFileName )
	{
		return FALSE;
	}

	CSerialFile	SFile;	
	if ( SFile.OpenFile ( FOT_WRITE, szFileName ) )
	{
		WORD	UniqueKey = GetUniqueKey();
		SFile.WriteBuffer ( &UniqueKey, sizeof ( WORD ) );

		WORD	VER = SSoundSource::VER;
		SFile.WriteBuffer ( &VER, sizeof ( WORD ) );

		int		Count = GetCount();
		SFile.WriteBuffer ( &Count, sizeof ( int ) );

		PSOUNDSOURCE pSoundSource = NULL;
		m_SoundSourceTable.SetHead();
		while ( !m_SoundSourceTable.IsEnd() )
		{
			pSoundSource = NULL;
			m_SoundSourceTable.GetCurrent ( pSoundSource );
			pSoundSource->SaveFile ( SFile );
			m_SoundSourceTable.GoNext();
		}

		SFile.CloseFile();

		return TRUE;
	}

	return FALSE;
}

BOOL	CSoundSourceMan::LoadFile ( const char *szFileName )
{
	CSerialFile	SFile;
	
	if ( SFile.OpenFile ( FOT_READ, szFileName ) )
	{
		WORD	UniqueKey;
		SFile.ReadBuffer ( &UniqueKey, sizeof ( WORD ) );

////////////////////////////////////////////////////////////////////////////////
//	번호	:	$1
//	요약	:	고유 ID 로드
//	설명	:	고유 ID를 로드해서 초기화 시킨다.
		InitUniqueKey ( UniqueKey );
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	번호	:	$2
//	요약	:	버전관리
//	설명	:	혹시나 추후에 추가되는 필드가 생길경우를 대비한다.
		WORD	VER = 0;
		SFile.ReadBuffer ( &VER, sizeof ( WORD ) );

		//	<--	데이타파일의 버전이 프로그램버전보다 높은 경우 오류	-->	//
		if ( SSoundSource::VER < VER )
		{
			return FALSE;
		}
//		SSoundSource::VER = VER;	<--	로드할 때 버전을 
//										넘겨줘서 처리하도록 만들자	-->	//
//		<**	모든 버전의 데이타파일을 읽고, 마지막에 저장할때는 최신버전의
//			데이타파일로 변환해서 저장한다.
//		**>
////////////////////////////////////////////////////////////////////////////////

		int		Count;
		SFile.ReadBuffer ( &Count, sizeof ( int ) );		

		ClearTable();

		PSOUNDSOURCE pSoundSource = NULL;
		for ( int i = 0; i < Count; ++i )
		{
            pSoundSource = new SSoundSource;
			pSoundSource->LoadFile ( SFile, VER );
			m_SoundSourceTable.AddTail ( pSoundSource );
		}

		SFile.CloseFile();

		return TRUE;
	}	
	return FALSE;
}



BOOL	SSoundSource::SaveFile ( CSerialFile &SFile )
{
	int		Size = 0;
	char	*pString = NULL;

	//	<--	Lock 상태	-->	//
	SFile.WriteBuffer ( &State, sizeof ( BOOL ) );

	//	<--	고유ID	-->	//
	SFile.WriteBuffer ( &ID, sizeof ( WORD ) );

	//	<--	Category 제작용 ( 옵션 )	-->	//
	SFile.WriteBuffer ( &TYPE, sizeof ( ESoundType ) );

	//	<--	파일 이름	-->	//
	Size = FileName.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, FileName.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;


	//	<--	최근 수정일	-->	//
	Size = LastUpdated.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, LastUpdated.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;

	//	<--	간략한 설명	-->	//
	Size = Comment.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, Comment.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;

	//	<--	실제 파일 이름	-->	//
	Size = OriginFileName.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, OriginFileName.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;


	//	<--	전체경로	-->	//
	//Size = FullPath.GetLength() + 1;
	//pString = new char [ Size ];
	//memset ( pString, 0, Size );
	//memcpy ( pString, FullPath.GetString(), Size );
	//SFile.WriteBuffer ( &Size, sizeof ( int ) );
	//SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	//delete [] pString;
	//pString = NULL;
	//Size = 0;

	//	<--	버퍼 개수	-->	//
	SFile.WriteBuffer ( &BufferCount, sizeof ( WORD ) );

	return TRUE;
}

BOOL	SSoundSource::LoadFile ( CSerialFile &SFile, WORD VER )
{
	int		Size = 0;
	char	*pString = NULL;

	//	<--	Lock 상태	-->	//
	SFile.ReadBuffer ( &State, sizeof ( BOOL ) );

	//	<--	고유ID	-->	//
	SFile.ReadBuffer ( &ID, sizeof ( WORD ) );

	//	<--	Category 제작용 ( 옵션 )	-->	//
	SFile.ReadBuffer ( &TYPE, sizeof ( ESoundType ) );

	//	<--	파일 이름	-->	//
	SFile.ReadBuffer ( &Size, sizeof ( int ) );
	if ( Size )
	{
		pString = new char [ Size ];
		memset ( pString, 0, Size );
		SFile.ReadBuffer ( pString, sizeof ( char ) * Size );
		FileName = pString;
		delete [] pString;
		pString = NULL;
		Size = 0;
	}	

	//	<--	최근 수정일	-->	//
	SFile.ReadBuffer ( &Size, sizeof ( int ) );
	if ( Size )
	{
		pString = new char [ Size ];
		memset ( pString, 0, Size );
		SFile.ReadBuffer ( pString, sizeof ( char ) * Size );
		LastUpdated = pString;
		delete [] pString;
		pString = NULL;
		Size = 0;
	}	


	//	<--	간략한 설명	-->	//	
	SFile.ReadBuffer ( &Size, sizeof ( int ) );
	if ( Size )
	{
		pString = new char [ Size ];
		memset ( pString, 0, Size );
		SFile.ReadBuffer ( pString, sizeof ( char ) * Size );
		Comment = pString;
		delete [] pString;
		pString = NULL;
		Size = 0;
	}	

	//	<--	실제 파일 이름 ( 코드 값 아님 )	-->	//	
	SFile.ReadBuffer ( &Size, sizeof ( int ) );
	if ( Size )
	{
		pString = new char [ Size ];
		memset ( pString, 0, Size );
		SFile.ReadBuffer ( pString, sizeof ( char ) * Size );
		OriginFileName = pString;
		delete [] pString;
		pString = NULL;
		Size = 0;
	}

	//	<--	전체경로	-->	//	
	//SFile.ReadBuffer ( &Size, sizeof ( int ) );
	//if ( Size )
	//{
	//	pString = new char [ Size ];
	//	memset ( pString, 0, Size );
	//	SFile.ReadBuffer ( pString, sizeof ( char ) * Size );
	//	FullPath = pString;
	//	delete [] pString;
	//	pString = NULL;
	//	Size = 0;
	//}	

	if ( 100 < VER )
	{
		SFile.ReadBuffer ( &BufferCount, sizeof ( WORD ) );
	}

	return TRUE;
}

namespace	NTypeDesc
{
	int		DescCount = 8;
    CString	Desc[8] = 
	{
		"맵",
		"몹",
		"플레이어",
		"NPC",
		"기후, 기타",
		"아이템",
		"효과, 마법",
		"미분류"
	};
};