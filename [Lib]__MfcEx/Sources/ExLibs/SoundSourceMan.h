////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	CSoundSourceManagerDoc
//	����	:	�������� ����Ÿ(wav, �ڸ�Ʈ...) ������ ��
//	�ΰ�����:
//	������:	Search, Backup, ���� ����, ���� �̸��� ���� ���� �ߺ� üũ
//				Category �̿�.
//	�ۼ���	:	���⿱
//	
//	�α�
//	$6.	2002-11-05T10:21	[���� ���������� ���⼭ �����ϴ°��� ���ڴ�.
//							����, ����ٰ� ������ �ִ� �޽�带 �ִµ�, ����
//							����Ÿ���ϰ� ȣȯ���� ���� ������ 1 �÷��� ó���Ѵ�.]
//	$5.	2002-10-31T09:30	[SoundSourceMan Ŭ������ Doc�� ���� ���� ����]
//							[���� ���� ���� �ۼ�]
//	$4.	2002-10-24T17:36	[����Ÿ Locking ����, ���� ���� ����]
//	$3.	2002-10-24T11:03	[SSoundSource����ü��,
//							�ڵ��̸����� �ٲ�� ���� ���� �̸� ���� �ʵ����]
//							[Lock �ʵ� ���� ( �����Ҽ� ������ ����� �ʵ� )]
//							[���� �׽�Ʈ ��� ����]
//							[���� �̸� ���� ����]
//	$2.	2002-10-23T16:43	[���� ���� �߰�]
//	$1.	2002-10-22T21:07	[���� �ۼ�, ���̺� �ε�]
//							[���� ���� �ý���]
//							[Primary Key ���� ��� �ۼ�]
//
////////////////////////////////////////////////////////////////////////////////

#ifndef	__C_SOUNDSOURCEMAN__
#define	__C_SOUNDSOURCEMAN__

#include "CList.h"

class	CSerialFile;


//	<--	���� Ÿ�� ����	-->	//
namespace	NTypeDesc
{
	extern	int	DescCount;
    extern	CString	Desc[8];
};

//	<--	���� Ÿ�� ����	-->	//
enum		ESoundType
{
	eMap,		//	��
	eMob,		//	��
	ePc,		//	�÷��̾�
	eNpc,		//	NPC
	eWhether,	//	����, ��Ÿ
    eItem,		//	������
	eEffect,	//	ȿ��, ����
	eReserved	//	�̺з�
};


//	<--	���� ���ڵ� ����	-->	//
struct	SSoundSource
{
	BOOL		State;			//	Lock�ΰ�?
	WORD		ID;				//	����ID
	ESoundType	TYPE;			//	Category ���ۿ� ( �ɼ� )
    CString		FileName;		//	���� �̸�
	CString		LastUpdated;	//	�ֱ� ������
	CString		Comment;		//	������ ����
	CString		OriginFileName;	//	���� ���� �̸� ( �ڵ� �� �ƴ� )
//	CString		FullPath;		//	������ ���� ���

	//	<--	$.6	�߰��� ����
	WORD		BufferCount;	//	���� ����
	//	-->

public:
static	WORD	VER;

public:
    SSoundSource()
	{
		State = FALSE;
		ID = 0xFFFF;
		BufferCount = 0;
	}
	~SSoundSource()
	{
		State = FALSE;
		ID = 0xFFFF;
		BufferCount = 0;
	}

public:
	BOOL	SaveFile ( CSerialFile &SFile );
	BOOL	LoadFile ( CSerialFile &SFile, WORD VER );
};
typedef	SSoundSource	*PSOUNDSOURCE;


class	CSoundSourceMan
{
////////////////////////////////////////////////////////////////////////////////
//	��ȣ	:	$1
//	���	:	���� ID ���� ���
//	����	:	���� ID�� ����� �����Ѵ�. ���� �ߺ��� �־�� �ȵȴ�.
private:
static	WORD	m_UniqueID;

private:
	void		InitUniqueKey ( WORD UniqueKey )		{ m_UniqueID = UniqueKey; }

public:	
	WORD		GetUniqueKey ( void )					{ return m_UniqueID; }
	void		GenerateUniqueKey ( void )				{ m_UniqueID++; }
////////////////////////////////////////////////////////////////////////////////

//	<--	���̺� ����	-->	//
protected:
	CMList<PSOUNDSOURCE>	m_SoundSourceTable;
	CString				m_DataDirectory;

public:
	CSoundSourceMan();
	virtual	~CSoundSourceMan();

public:	
	//	<--	���� ���� �Լ���
	void	AddRecord ( SSoundSource *pRecord, CString &FullPath );
	void	SetRecord ( WORD ID, BOOL bChangeFile, CString FilePath );
	void	DelRecord ( WORD ID );

	SSoundSource*	GetRecord ();
	SSoundSource*	FindRecord ( WORD ID );

	SSoundSource*	GetHead ();
	SSoundSource*	GetTail ();
	//	-->

	void	GoHead	();
	void	GoTail	();
	void	GoNext	();
	void	GoPrev	();
	BOOL	IsEnd	();

	int		GetCount ();
	void	ClearTable();

public:
	BOOL	SaveFile ( const char *szFileName );
	BOOL	LoadFile ( const char *szFileName );

public:
	void	SetDataDirectory( CString &DataDirectory ) { m_DataDirectory = DataDirectory; }
	CString	GetDataDirectory()		{	return m_DataDirectory;		}
};

#endif	//	__C_SOUNDSOURCEMAN__