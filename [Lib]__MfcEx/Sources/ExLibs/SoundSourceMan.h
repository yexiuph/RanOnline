////////////////////////////////////////////////////////////////////////////////
//	이름	:	CSoundSourceManagerDoc
//	목적	:	실질적인 데이타(wav, 코멘트...) 관리를 함
//	부가설명:
//	설계기능:	Search, Backup, 강제 수정, 예전 이름과 비교후 파일 중복 체크
//				Category 이용.
//	작성자	:	성기엽
//	
//	로그
//	$6.	2002-11-05T10:21	[버퍼 생성개수를 여기서 지정하는것이 좋겠다.
//							따라서, 여기다가 개수를 넣는 메쏘드를 넣는데, 예전
//							데이타파일과 호환성을 위해 버전을 1 올려서 처리한다.]
//	$5.	2002-10-31T09:30	[SoundSourceMan 클래스로 Doc에 박힌 구조 뜯어내기]
//							[구조 뜯어내서 최초 작성]
//	$4.	2002-10-24T17:36	[데이타 Locking 구현, 구간 삭제 구현]
//	$3.	2002-10-24T11:03	[SSoundSource구조체상에,
//							코드이름으로 바뀌기 전의 파일 이름 저장 필드삽입]
//							[Lock 필드 삽입 ( 수정할수 없도록 만드는 필드 )]
//							[음원 테스트 모듈 삽입]
//							[파일 이름 포맷 지정]
//	$2.	2002-10-23T16:43	[버전 관리 추가]
//	$1.	2002-10-22T21:07	[최초 작성, 세이브 로드]
//							[오류 복구 시스템]
//							[Primary Key 생성 모듈 작성]
//
////////////////////////////////////////////////////////////////////////////////

#ifndef	__C_SOUNDSOURCEMAN__
#define	__C_SOUNDSOURCEMAN__

#include "CList.h"

class	CSerialFile;


//	<--	사운드 타입 설명	-->	//
namespace	NTypeDesc
{
	extern	int	DescCount;
    extern	CString	Desc[8];
};

//	<--	사운드 타입 정의	-->	//
enum		ESoundType
{
	eMap,		//	맵
	eMob,		//	몹
	ePc,		//	플레이어
	eNpc,		//	NPC
	eWhether,	//	기후, 기타
    eItem,		//	아이템
	eEffect,	//	효과, 마법
	eReserved	//	미분류
};


//	<--	사운드 레코드 정의	-->	//
struct	SSoundSource
{
	BOOL		State;			//	Lock인가?
	WORD		ID;				//	고유ID
	ESoundType	TYPE;			//	Category 제작용 ( 옵션 )
    CString		FileName;		//	파일 이름
	CString		LastUpdated;	//	최근 수정일
	CString		Comment;		//	간단한 설명
	CString		OriginFileName;	//	실제 파일 이름 ( 코드 값 아님 )
//	CString		FullPath;		//	파일의 존재 경로

	//	<--	$.6	추가된 사항
	WORD		BufferCount;	//	버퍼 개수
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
//	번호	:	$1
//	요약	:	고유 ID 생성 멤버
//	설명	:	고유 ID를 만들고 관리한다. 절대 중복이 있어서는 안된다.
private:
static	WORD	m_UniqueID;

private:
	void		InitUniqueKey ( WORD UniqueKey )		{ m_UniqueID = UniqueKey; }

public:	
	WORD		GetUniqueKey ( void )					{ return m_UniqueID; }
	void		GenerateUniqueKey ( void )				{ m_UniqueID++; }
////////////////////////////////////////////////////////////////////////////////

//	<--	테이블 관리	-->	//
protected:
	CMList<PSOUNDSOURCE>	m_SoundSourceTable;
	CString				m_DataDirectory;

public:
	CSoundSourceMan();
	virtual	~CSoundSourceMan();

public:	
	//	<--	새로 만든 함수들
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