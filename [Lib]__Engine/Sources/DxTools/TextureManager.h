//	[class TextureManager], (2002.12.07), JDH.
//
//	UPDATE : (2002.12.07), JDH, �ؽ��� �⺻ ���� �����ÿ� ���� �������� ��� ���ϵ��� Ž���Ͽ�
//			�̸� ���ĵ� Ʈ���� ������ �̸� �˻��Ͽ� �����.
//			�� ���� �������� ��� �ִ� ������ unique �̸��� �������ϸ� ���� �ƴ� ��쿡�� ��Ư��
//			�ؽ����� �ε� ������ �ִ�.
//			 �� ����� Ȯ������ ���� �ؽ��ĸ� �뵵���� �з��Ͽ� ������ �־ ������ �����ϰԵ�.
//			
//			��, �������� ������ ��������, �ٸ� �ڵ�, �ڿ��� ������ ���� ����.
//			��, ó�� ���� ������ Ǯ��ġ�� �ʿ��ϰ� ��.
//
//
#if !defined(_TEXTUREMANAGER_H__INCLUDED_)
#define _TEXTUREMANAGER_H__INCLUDED_

#include "CFileFind.h"

#include <map>
#include <string>
#include "StlFunctions.h"

#define TC_COMMONFILE

class LOADING_THREAD
{
protected:
	DWORD	dwThreadID;			//	������ ID
	HANDLE	hThread;			//	������ �ڵ�
	HANDLE	hEventTerminate;	//	������ ����

protected:
	struct DATA
	{
		LPDIRECT3DDEVICEQ	pd3dDevice;
		BOOL				bEnable;
		BOOL				bDelete;
		DATA() :
			bEnable(FALSE),
			bDelete(FALSE),
			pd3dDevice(NULL)
		{
		};
	};
	DATA	m_sData;

public:
	BOOL	StartThread ( LPDIRECT3DDEVICEQ pd3dDevice );
	void	EndThread ();

protected:
	static unsigned int WINAPI LoadThread(  LPVOID pData );

public:
	LOADING_THREAD() :
		dwThreadID ( UINT_MAX ),
		hThread ( NULL ),
		hEventTerminate ( NULL )
	{
	};
	~LOADING_THREAD();
};

namespace TextureManager
{
	enum	EM_TEXTYPE
	{
		EMTT_NORMAL			= 0,
		EMTT_ALPHA_HARD		= 1,
		EMTT_ALPHA_SOFT		= 2,
		EMTT_ALPHA_SOFT01	= 3,
		EMTT_ALPHA_SOFT02	= 4,

		EMTT_SIZE			= 5
	};

	class TextureContainer
	{
	public:
		std::string		m_strName;			// Name of texture (doubles as image filename)
	    
		DWORD		m_dwNameExtNum;			// �߰� �ؽ��� �̸��� ����.
		char**		m_ppNameExt;			// Ext. Name of texture (doubles as image filename)

		DWORD		m_dwWidth;
		DWORD		m_dwHeight;
		DWORD		m_dwBPP;
		BOOL		m_bHasAlpha;
		BOOL		m_bJPG;

		DWORD		m_dwStage;				// Mip Map Count
		DWORD		m_dwFlags;
		D3DCOLOR	m_d3dColorKey;

		DWORD		m_refCount;				// ���� Ƚ��.

		LPDIRECT3DTEXTUREQ		m_pddsTexture;	// Surface of the texture
		LPDIRECT3DCUBETEXTUREQ	m_pddsCubeTexture;
		DWORD**					m_pRGBAData;

		EM_TEXTYPE	m_emTexType;
		
	public:
		void AddRef ()	{ m_refCount++; }
		LPDIRECT3DTEXTUREQ GetDDS ()	{ return m_pddsTexture; }

	protected:
		HRESULT LoadBMPFile ( const char* strPathname );
		HRESULT LoadSBGFile ( const char* strPathname );
		HRESULT LoadTargaFile ( const char* strPathname );
		void DeleteRGBAData ();

		HRESULT LoadImageData ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCube, BOOL bEncrypt );
		HRESULT CopyRGBADataToSurface ();
		HRESULT Restore ( LPDIRECT3DDEVICEQ pd3dDevice );

	public:
		HRESULT LoadTexture ( const char* strName, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlags, DWORD dwStage=0, BOOL bEncrypt = FALSE );
		HRESULT LoadTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

		HRESULT LoadCubeTexture ( const char* szName, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlags, DWORD dwStage, BOOL bEncrypt = FALSE );

		bool	IsEncrypt( const char* szFileName );

	public:
		TextureContainer ();
		TextureContainer ( const char* strName, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlags, DWORD dwStage=0 );
		~TextureContainer ();
	};

	typedef std::map<std::string,TextureContainer*>		TEXTUREMAP;
	typedef TEXTUREMAP::iterator						TEXTUREMAP_ITER;

	extern TEXTUREMAP	g_mapLoading;
	extern TEXTUREMAP	g_mapComplete;

	typedef std::map<std::string,DWORD>			TEXTUREIDMAP;
	typedef TEXTUREIDMAP::iterator				TEXTUREIDMAP_ITER;

	extern TEXTUREIDMAP	g_mapTextureID;

	char* GetTexturePath ();
	void SetTexturePath ( const char* strTexturePath );
	void ReScanTexture ();
	BOOL IsTexture( const char* pName );

	EM_TEXTYPE	GetTexType ( const char* strName );
	void		EnableTexTypeDEBUG();

	//	Note : �ؽ��ĸ� �а� RefCount �� ������Ų��.
	//		���� �̹� ���� �Ѵٸ� RefCount�� ����.
	//
	HRESULT LoadTexture ( const char* strTextureName,
		LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ &pddsTexture,
		DWORD dwFlags, DWORD dwStage, BOOL bThread=FALSE );

	//	Note : Cube �ؽ��ĸ� �а� RefCount �� ������Ų��.
	//		���� �̹� ���� �Ѵٸ� RefCount�� ����.
	//
	HRESULT LoadCubeTexture ( const char* strTextureName,
		LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DCUBETEXTUREQ &pddsCubeTexture,
		DWORD dwFlags, DWORD dwStage );

	// Note : Loading Data Get		 * No RefCount *
	void GetTexture( const char* strName, LPDIRECT3DTEXTUREQ &pddsTexture );
	DWORD GetTextureID( const char* strName );

	//	Note : �ؽ����� RefCount �� ���� ���Ѽ� ����
	//		�� ���� '0' �� �ȴٸ� �� �ؽ��ĸ� �ı��Ѵ�.
	//
	HRESULT ReleaseTexture( const char* strName, LPDIRECT3DTEXTUREQ &pddsTexture );		// Note : �����带 ���ٸ� �� �̸����� �ؾ� �Ѵ�.
	HRESULT ReleaseTexture( const char* strName, LPDIRECT3DCUBETEXTUREQ &pddsTexture );	// Note : �����带 ���ٸ� �� �̸����� �ؾ� �Ѵ�.
	HRESULT DeleteTexture ( const char* strName );

	HRESULT ReportUsingTexture ();

	void OneTimeSceneInit();
	void StartThread( LPDIRECT3DDEVICEQ pd3dDevice );
	void DeleteAllTexture();
	void CleanUp();
};

#endif // !defined(_TEXTUREMANAGER_H__INCLUDED_)
