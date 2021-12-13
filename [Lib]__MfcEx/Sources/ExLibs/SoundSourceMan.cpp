#include "pch.h"
#include "SoundSourceMan.h"
#include "SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////////////////////////////////////////
//	��ȣ	:	$6
//	���	:	��������
//	����	:	
//				
//				
//				
//	�α�
//	$2.	2002-11-05T10:32	[���۰��� �߰�, 101]
//	$1.	2002-10-XXT??:??	[���ʹ���, 100]
//				
WORD	SSoundSource::VER = 101;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//	��ȣ	:	$1
//	���	:	���� ID ���� ���
//	����	:	���� �����ÿ��� -1�̾�� �Ѵ�. ���⼭ ���ϴ� '����'��, ���α׷���
//				����� �̷ʷ� ó������ ����Ÿ�� ��ϵɶ��� �ǹ��Ѵ�.
//				-1�� �����ν� ���� ����Ÿ�� ID�� 0������ �����ȴ�.
//				����, �̰��� ���̺������� �����Ǹ�, �� ���Ϸκ��� ����ID�� �о�
//				�����δ�. �� �����ʹ� -1�� �����ϴ°��� ���� �ʱ�ȭ�� �ǹ̿�
//				�Ұ��ϴ�.
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

	//	<--	���� ��ü�� �ٲ�� ���
	if ( bChangeFile )
	{
		CString SrcFile = FilePath;
		CString DstFile = m_DataDirectory + pCurRecord->FileName;

		CopyFile ( SrcFile.GetString(), DstFile.GetString (), FALSE );
	}
	//	<--	Ÿ���� �ٲ� ����, ���� �̸��� ����Ǵ� ���	
	else
	{
		CString SrcFile = m_DataDirectory + pCurRecord->FileName;

		//	���� ���븸 ������ ���
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
//	��ȣ	:	$1
//	���	:	���� ID �ε�
//	����	:	���� ID�� �ε��ؼ� �ʱ�ȭ ��Ų��.
		InitUniqueKey ( UniqueKey );
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	��ȣ	:	$2
//	���	:	��������
//	����	:	Ȥ�ó� ���Ŀ� �߰��Ǵ� �ʵ尡 �����츦 ����Ѵ�.
		WORD	VER = 0;
		SFile.ReadBuffer ( &VER, sizeof ( WORD ) );

		//	<--	����Ÿ������ ������ ���α׷��������� ���� ��� ����	-->	//
		if ( SSoundSource::VER < VER )
		{
			return FALSE;
		}
//		SSoundSource::VER = VER;	<--	�ε��� �� ������ 
//										�Ѱ��༭ ó���ϵ��� ������	-->	//
//		<**	��� ������ ����Ÿ������ �а�, �������� �����Ҷ��� �ֽŹ�����
//			����Ÿ���Ϸ� ��ȯ�ؼ� �����Ѵ�.
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

	//	<--	Lock ����	-->	//
	SFile.WriteBuffer ( &State, sizeof ( BOOL ) );

	//	<--	����ID	-->	//
	SFile.WriteBuffer ( &ID, sizeof ( WORD ) );

	//	<--	Category ���ۿ� ( �ɼ� )	-->	//
	SFile.WriteBuffer ( &TYPE, sizeof ( ESoundType ) );

	//	<--	���� �̸�	-->	//
	Size = FileName.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, FileName.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;


	//	<--	�ֱ� ������	-->	//
	Size = LastUpdated.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, LastUpdated.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;

	//	<--	������ ����	-->	//
	Size = Comment.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, Comment.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;

	//	<--	���� ���� �̸�	-->	//
	Size = OriginFileName.GetLength() + 1;
	pString = new char [ Size ];
	memset ( pString, 0, Size );
	memcpy ( pString, OriginFileName.GetString(), Size );
	SFile.WriteBuffer ( &Size, sizeof ( int ) );
	SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	delete [] pString;
	pString = NULL;
	Size = 0;


	//	<--	��ü���	-->	//
	//Size = FullPath.GetLength() + 1;
	//pString = new char [ Size ];
	//memset ( pString, 0, Size );
	//memcpy ( pString, FullPath.GetString(), Size );
	//SFile.WriteBuffer ( &Size, sizeof ( int ) );
	//SFile.WriteBuffer ( pString, sizeof ( char ) * Size );
	//delete [] pString;
	//pString = NULL;
	//Size = 0;

	//	<--	���� ����	-->	//
	SFile.WriteBuffer ( &BufferCount, sizeof ( WORD ) );

	return TRUE;
}

BOOL	SSoundSource::LoadFile ( CSerialFile &SFile, WORD VER )
{
	int		Size = 0;
	char	*pString = NULL;

	//	<--	Lock ����	-->	//
	SFile.ReadBuffer ( &State, sizeof ( BOOL ) );

	//	<--	����ID	-->	//
	SFile.ReadBuffer ( &ID, sizeof ( WORD ) );

	//	<--	Category ���ۿ� ( �ɼ� )	-->	//
	SFile.ReadBuffer ( &TYPE, sizeof ( ESoundType ) );

	//	<--	���� �̸�	-->	//
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

	//	<--	�ֱ� ������	-->	//
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


	//	<--	������ ����	-->	//	
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

	//	<--	���� ���� �̸� ( �ڵ� �� �ƴ� )	-->	//	
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

	//	<--	��ü���	-->	//	
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
		"��",
		"��",
		"�÷��̾�",
		"NPC",
		"����, ��Ÿ",
		"������",
		"ȿ��, ����",
		"�̺з�"
	};
};