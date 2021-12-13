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
	DWORD	m_sClubMark[WIDTH*HEIGHT];	// ���ο� Ŭ�� ��ũ �迭

	DWORD		COL;
	DWORD		ROW;

	D3DXVECTOR2	UV_DIS;

	const static std::string	m_strText;		// ����
	std::string					m_strBaseName;	// �̹��� �⺻
	std::string					m_strExtName;	// �̹��� Ȯ����

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
	DXDATA m_sTextureData;	// GetClubData() �޼ҵ忡�� ���ϰ��� �ֱ� ���ؼ� ������.

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
	typedef std::map<DWORD,ID_DATA*>	ID_DATA_MAP;		// Ŭ�����̵�, Ŭ�� ����
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
	typedef std::map<DWORD,ID_SERVER*>	ID_SERVER_MAP;		// ������ȣ, Ŭ�� ������
	typedef ID_SERVER_MAP::iterator		ID_SERVER_MAP_ITER;

	ID_SERVER_MAP		m_mapServer;

protected:
	BOOL	LoadBMPFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor );
	BOOL	LoadTGAFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor );

protected:
	void	NewServerData ( int nServer );
	void	NewClubData ( LPDIRECT3DDEVICEQ pd3dDevice, ID_SERVER_MAP_ITER& _iter, int nID, DWORD dwVer, DWORD* pColor );
	void	ModifyClubData ( LPDIRECT3DDEVICEQ pd3dDevice, ID_SERVER_MAP_ITER& _iter, int nID, DWORD dwVer, DWORD* pColor );

	void	DrawClubTex ( LPDIRECT3DDEVICEQ pd3dDevice, TEXDATA_MAP& mapTex, const char* cName, int nCount, DWORD* pColor );	// Tex�� ������ ����� �׸���.

	D3DXVECTOR2	SetTexUV ( int nCount );							// UV ��ǥ�� �Ѱ� �ش�.

	void	SaveData();
	void	LoadData( LPDIRECT3DDEVICEQ pd3dDevice );
	void	NewData();
	BOOL	LoadTex( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//	Note : �űԷ� ����� Ŭ����ũ�� �̹����� ���Ͽ��� �о����.
	BOOL	LoadClubMark ( const char* szFileName, LPDWORD& pMarkColor );

public:
	//	Note : Ŭ�� ��ũ�� �ؽ��� ������ ��������.
	DXDATA	GetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer );
	//	Note : Ŭ�� ��ũ�� �ؽ�ó ������ ����.
	bool	SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer, DWORD *pClubMark );
	//	Note : Ŭ�� ��ũ�� �ؽ��� ������ ��ȿ���� �˻�.
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
