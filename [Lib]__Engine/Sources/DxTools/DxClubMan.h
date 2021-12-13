#if !defined(_DXCLUBMAN_H__INCLUDED_)
#define _DXCLUBMAN_H__INCLUDED_

#include <map>
#include <list>
#include <algorithm>

class CSerialFile;

struct DxClubMan
{
protected:
	const	static	DWORD	VERSION;

	const	static	DWORD	CHAR_NUM;
	const	static	DWORD	FULL_WIDTH;
	const	static	DWORD	FULL_HEIGHT;

	enum	
	{
		WIDTH	= 16,
		HEIGHT	= 11,
	};
	DWORD	m_sClubMark[WIDTH*HEIGHT];	// 새로운 클럽 마크 배열

	DWORD		COL;
	DWORD		ROW;

	D3DXVECTOR2	UV_DIS;

	const static std::string	m_strText;		// 문서
	std::string					m_strBaseName;	// 이미지 기본
	std::string					m_strExtName;	// 이미지 확장자

protected:
	static char			m_szPath[MAX_PATH];
	std::string			m_strREGISTPATH;

public:
	static void			SetPath ( char* szPath )				{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	static char*		GetPath ()								{ return m_szPath; }

	void				SetRegistPath ( std::string strPath )	{ m_strREGISTPATH = strPath; }

public:
	struct DXDATA
	{
		LPDIRECT3DTEXTUREQ	pddsTexture;
		D3DXVECTOR2			vTex_1_LU;
		D3DXVECTOR2			vTex_2_RU;
		D3DXVECTOR2			vTex_3_LD;
		D3DXVECTOR2			vTex_4_RD;

		DXDATA () :
			pddsTexture(NULL),
			vTex_1_LU(0,0),
			vTex_2_RU(0,0),
			vTex_3_LD(0,0),
			vTex_4_RD(0,0)
		{
		}
	};
	DXDATA m_sTextureData;	// GetClubData() 메소드에서 리턴값을 주기 위해서 쓰였다.

protected:
	struct DXTEXDATA
	{
		std::string			szName;
		LPDIRECT3DTEXTUREQ	pTexture;

		DXTEXDATA () :
			pTexture(NULL)
		{
		};

		~DXTEXDATA ();
	};
	typedef std::map<std::string,DXTEXDATA*>	TEXDATA_MAP;
	typedef TEXDATA_MAP::iterator				TEXDATA_MAP_ITER;

protected:
	struct ID_DATA
	{
		DWORD		dwVer;
		std::string	strTex;
		DWORD		dwCount;
	};
	typedef std::map<DWORD,ID_DATA*>	ID_DATA_MAP;		// 클럽아이디, 클럽 정보
	typedef ID_DATA_MAP::iterator		ID_DATA_MAP_ITER;

protected:
	struct ID_SERVER
	{
		ID_DATA_MAP	mapData;
		TEXDATA_MAP	mapTex;

		DWORD		COUNT_NUM;
		std::string	CUR_TEXTURE;

		void Save ( CSerialFile &SFile );
		void Load ( CSerialFile &SFile );

		ID_SERVER();
		~ID_SERVER ();
	};
	typedef std::map<DWORD,ID_SERVER*>	ID_SERVER_MAP;		// 서버번호, 클럽 데이터
	typedef ID_SERVER_MAP::iterator		ID_SERVER_MAP_ITER;

	ID_SERVER_MAP		m_mapServer;

protected:
	BOOL	LoadBMPFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor );
	BOOL	LoadTGAFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor );

protected:
	void	NewServerData ( int nServer );
	void	NewClubData ( LPDIRECT3DDEVICEQ pd3dDevice, ID_SERVER_MAP_ITER& _iter, int nID, DWORD dwVer, DWORD* pColor );
	void	ModifyClubData ( LPDIRECT3DDEVICEQ pd3dDevice, ID_SERVER_MAP_ITER& _iter, int nID, DWORD dwVer, DWORD* pColor );

	void	DrawClubTex ( LPDIRECT3DDEVICEQ pd3dDevice, TEXDATA_MAP& mapTex, const char* cName, int nCount, DWORD* pColor );	// Tex에 정해진 모양을 그린다.

	D3DXVECTOR2	SetTexUV ( int nCount );							// UV 좌표를 넘겨 준다.

	void	SaveData();
	void	LoadData( LPDIRECT3DDEVICEQ pd3dDevice );
	void	NewData();
	BOOL	LoadTex( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//	Note : 신규로 등록할 클럽마크의 이미지를 파일에서 읽어오기.
	BOOL	LoadClubMark ( const char* szFileName, LPDWORD& pMarkColor );

public:
	//	Note : 클럽 마크의 텍스쳐 정보를 가져오기.
	DXDATA	GetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer );
	//	Note : 클럽 마크의 텍스처 정보를 설정.
	bool	SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer, DWORD *pClubMark );
	//	Note : 클럽 마크의 텍스쳐 정보의 유효성을 검사.
	bool	IsValidData ( int nServer, int nID, DWORD dwVer );


public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxClubMan();

public:
	virtual ~DxClubMan();

public:
	static DxClubMan& GetInstance();
};

#endif // !defined(_DXCLUBMAN_H__INCLUDED_)
