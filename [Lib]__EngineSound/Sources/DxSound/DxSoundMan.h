////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	DxSoundMan
//	����	:	��� ȿ�� Wav������ �����Ѵ�.
//	����	:	���̺� ������ ����� �����Ѵ�.
//	�ۼ���	:	���⿱
//
//	�α�
//	$0012.	2003-02-17T19:45	[2D ���� ���� ����� �ֵ��� ����, �������̽� ���]
//								[����, �α� 11���� �ǹ� ����.]
//	$0011.	2003-02-17T17:00	[�ܺ�/���� �������̽� �Ҹ�, ���⼺������ �� ����..
//								Update�κ� �պ�]
//	$0010.	2002-12-05T15:35	[ĳ���� + ����Ʈ ����, �� ����, BG ���� ���� ������
//								�������̽� ����]
//								�ٲ��� �������� ���̴�. ��.]
//	$0009.	2002-12-03T16:58	[Orientation �� ����� ����, ����ؼ� �׿���. ��,.��;;
//								�ٲ��� �������� ���̴�. ��.]
//	$0008.	2002-11-14T15:09	[�����ʴ� x, y, z�� ��ο� ���ؼ� �����ϵ��� �ٲ�,
//								��������, y�࿡ ���ؼ� �������� �ʾ�����, '������'����
//								���ο� ������ ���� ����, ���̰��� ������!! �ʿ����� ������,
//								���� ������]
//	$0007.	2002-11-14T10:47	[���� Root ��θ� ������ ��, �� ���� ���� ��δ�
//								������ �����ϸ�, �����ϴ°͵� �����ϰ� �� �� ����,
//								���Ӵ��� ���� �ݿ�. ��... �� �� �� ����� �� ���ٰ� �����߳�?]
//								[�����̸� �� ���丮 ��� ��� '������ -> �迭'�� �ٲ�]
//	$0006.	2002-11-13T09:31	[SaveSet/LoadSet �۾�]
//	$0005.	2002-11-07T09:31	[����Ʈ ��, Final �����۾� -1,
//								Ŭ������ ����Ȯ�� �۾��� �� ������]				
//	$0004.	2002-11-01T11:34	[���� ��ȯ��...
//								enginelib - ReplaceContainer.h ����]
//	$0003.	2002-10-30T14:47	[SWavFile ����ü�� RefCount ����]
//								[DxSoundMan ������
//								'���� �ѱ�, ����',
//								'���ڵ� ���, ������',
//								'�뷡 �ѱ�, ����'
//								�� �����ؼ�	�޽�� ����]
//	$0002.	2002-10-28T09:51	[�󼼱��� ����]
//	$0001.	2002-10-18T11:23	[���� Ŭ���� ����]
////////////////////////////////////////////////////////////////////////////////
#ifndef	__C_WAVFILEMAN__
#define	__C_WAVFILEMAN__

#include <string>
#include <map>

//	<--	Ŭ���� ����	-->	//
class	CSound;
class	CSoundManager;
class	CSerialFile;

struct	SWavFileKey
{
	std::string strFileName;
	bool		bStereo;

	SWavFileKey () :
		bStereo ( false )
	{
	}

	inline bool operator < (const SWavFileKey& b )
	{		
		return (strFileName < (b.strFileName));
	}

	inline bool operator > (const SWavFileKey& b )
	{
		return (strFileName > (b.strFileName));
	}

	inline bool operator == (const SWavFileKey& b )
	{
		return ((strFileName == b.strFileName) && (bStereo == b.bStereo));
	}
};

inline bool operator < ( const SWavFileKey& a, const SWavFileKey& b )
{
	return (a.strFileName < b.strFileName);
}

inline bool operator > ( const SWavFileKey& a, const SWavFileKey& b )
{
	return (a.strFileName > b.strFileName);
}

inline bool operator == ( const SWavFileKey& a, const SWavFileKey& b )
{
	return ((a.strFileName == b.strFileName) && (a.bStereo == b.bStereo));
}

struct	SWavFileData
{
	CSound*	pSound;
	DWORD	dwRefCount;

	SWavFileData () :
		pSound ( NULL ),
		dwRefCount ( 0 )
	{
	}

	~SWavFileData()
	{
		if( pSound )
		{
			pSound->Stop ();
			SAFE_DELETE ( pSound );
		}
		dwRefCount = 0;
	}

	CSound*	ADD_REF ()
	{
		if ( !pSound ) return NULL;

		++dwRefCount;
		return pSound;
	}

	DWORD	DEL_REF ()
	{		
		--dwRefCount;
		return dwRefCount;
	}
};

//	<--	Ŭ���� ����	-->	//
class	DxSoundMan
{
private:
static	const	FLOAT	fDONT_CARE_DIST_SQ;
static	const	FLOAT	fMAX_DIST_SQ;
static	const	DWORD	dwMAX_BUFFER_SIZE;

public:
static	const	FLOAT	GET_DONT_CARE_DIST_SQ();
static	const	FLOAT	GET_MAX_DIST_SQ();

public:
	typedef	std::map<SWavFileKey,SWavFileData*>	WAVFILE_MAP;
	typedef	WAVFILE_MAP::const_iterator		WAVFILE_MAP_CITER;
	typedef	WAVFILE_MAP::iterator			WAVFILE_MAP_ITER;
	typedef	WAVFILE_MAP::size_type			WAVFILE_MAP_SIZE;

	WAVFILE_MAP	m_mapWavFile;

public:
	//	3D Algorithm
	enum
	{
		DEFAULT,
		HRTF_LIGHT,
		HRTF_FULL,

		ALGORITHM_SIZE = 3
	};	

	GUID	guid3DAlgorithm[ALGORITHM_SIZE];

private:
	static	WORD	m_SoundManVER;

private:
	CSoundManager*			m_pSoundManager;	//	Directsound ��ü	
	LPDIRECTSOUND3DLISTENER m_pDSListener;		//	������ �ϳ� �� ����

	CString m_strPath;		//	������� ��Ʈ ���丮

private:
	int		m_nBufferSize;

private:
	float			m_fRolloffFactor;
	float			m_fDistanceFactor;
	float			m_fDopplerFactor;
	D3DXVECTOR3		m_VelocityFactor;

private:
	BOOL	m_bMapMute;
	BOOL	m_bSfxMute;	

	long	m_lMapVolume;	
	long	m_lSfxVolume;

private:
	int		m_3DAlgorithm;

private:
	BOOL	m_bStereo;

private:
	DxSoundMan ();
	virtual	~DxSoundMan();

	//	<--	����� �ѱ� ����[��ġ, ����̽�]	-->	//
public:
	BOOL	OneTimeSceneInit	( HWND hWnd, char *szDirectory );
	BOOL	FinalCleanup		();

public:
	void	SetPath( char *szDirectory );
	const char*	GetPath();

public:
	void	SetStereo ( BOOL bStereo );
	BOOL	IsStereo ();

public:
	void	SetBufferSize ( const int nBufferSize );
	int		GetBufferSize ();

public:
	const long	CALC_PAN ( const D3DXVECTOR3& vSoundPos );
	const float	CALC_FADE( const D3DXVECTOR3& vSoundPos, const float& fMAX_RANGE );

	//	<--	Listener ����(?) ���� �޽��	-->	//
public:
	void		SetRolloffFactor	( float fRolloffFactor );
	void		SetDistanceFactor	( float	fDistanceFactor );
	void		SetDopplerFactor	( float fDopplerFactor );
	void		SetVelocityFactor	( D3DXVECTOR3 VelocityFactor );

	float		GetRolloffFactor	();
	float		GetDistanceFactor	();
	float		GetDopplerFactor	();
	D3DXVECTOR3	GetVelocityFactor	();

public:
	HRESULT	GetListernerPos ( D3DXVECTOR3* pPos );

	//	<--	���� ����	-->	//
	//	<**	������ ũ��� �ٸ� ����� ������ ���Ͽ�
	//		�����ؾ� �Ѵ�. �̰��� ���α׷� ����Ÿ�ʹ�
	//		������ �����Ǿ� �ִ� ���̱� ������,
	//		�׷��� �ϴ� ���� ���ڴ�.
	//	**>
public:
	void	SetMapVolume ( long	lVolume );
	long	GetMapVolume ();

    void	SetSfxVolume ( long lVolume );
	long	GetSfxVolume ();

public:
	void	SetMapMute ( BOOL bMute );
	void	SetSfxMute ( BOOL bMute );

	BOOL	IsMapMute ();
	BOOL	IsSfxMute ();

public:
	void		Set3DAlgorithm ( int nAlgorithm );
	INT			Get3DAlgorithm ();
	GUID		Getguid3DAlgorithm ();

public:	
	void		FrameMove ( D3DXVECTOR3 OrientFront, D3DXVECTOR3 Position );

	//	<--	���̺����� ���ҽ� ���� �޽��	-->	//
public:
	CSound*		LoadWav ( std::string strFileName, bool bStereo );
	HRESULT		ReleaseWav ( std::string strFileName, bool bStereo );
	HRESULT		DeleateWav ( std::string strFileName, bool bStereo );
	bool		IsLoadedWav ( std::string strFileName, bool bStereo );

	void		DeleteAllWav ();

public:
	BOOL		SaveSet ( CSerialFile &SFile );
	BOOL		LoadSet ( CSerialFile &SFile );

public:
	CSoundManager* const GetSoundManager ()
	{
		return m_pSoundManager;
	}

	//	<--	Singleton	-->	//
public:
	static DxSoundMan& GetInstance();
};

inline void DxSoundMan::Set3DAlgorithm ( int nAlgorithm )
{
	GASSERT ( 0 <= nAlgorithm && nAlgorithm < ALGORITHM_SIZE );
	m_3DAlgorithm = nAlgorithm;
}

inline int DxSoundMan::Get3DAlgorithm ()
{
	return m_3DAlgorithm;
}

inline GUID DxSoundMan::Getguid3DAlgorithm ()
{
	return guid3DAlgorithm[m_3DAlgorithm];
}

#endif	//	__C_WAVFILEMAN__