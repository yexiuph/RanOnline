//	[class TextureManager], (2002.12.07), JDH.
//
//	UPDATE : (2002.12.07), JDH, 텍스쳐 기본 폴더 지정시에 하위 폴더내의 모든 파일들을 탐색하여
//			미리 정렬된 트리로 만든후 이를 검색하여 사용함.
//			단 하위 폴더내에 들어 있는 파일은 unique 이름을 가져야하며 만약 아닐 경우에는 불특정
//			텍스쳐의 로드 위험이 있다.
//			 이 기능의 확장으로 이제 텍스쳐를 용도별로 분류하여 폴더에 넣어서 관리가 가능하게됨.
//			
//			장, 폴더별로 관리가 가능해짐, 다른 코드, 자원에 영향을 주지 않음.
//			단, 처음 폴더 지정시 풀서치가 필요하게 됨.
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
	DWORD	dwThreadID;			//	스레드 ID
	HANDLE	hThread;			//	스레드 핸들
	HANDLE	hEventTerminate;	//	스레드 종료

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
	    
		DWORD		m_dwNameExtNum;			// 추가 텍스쳐 이름의 갯수.
		char**		m_ppNameExt;			// Ext. Name of texture (doubles as image filename)

		DWORD		m_dwWidth;
		DWORD		m_dwHeight;
		DWORD		m_dwBPP;
		BOOL		m_bHasAlpha;
		BOOL		m_bJPG;

		DWORD		m_dwStage;				// Mip Map Count
		DWORD		m_dwFlags;
		D3DCOLOR	m_d3dColorKey;

		DWORD		m_refCount;				// 참조 횟수.

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

	//	Note : 텍스쳐를 읽고 RefCount 를 증가시킨다.
	//		만약 이미 존재 한다면 RefCount만 증가.
	//
	HRESULT LoadTexture ( const char* strTextureName,
		LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ &pddsTexture,
		DWORD dwFlags, DWORD dwStage, BOOL bThread=FALSE );

	//	Note : Cube 텍스쳐를 읽고 RefCount 를 증가시킨다.
	//		만약 이미 존재 한다면 RefCount만 증가.
	//
	HRESULT LoadCubeTexture ( const char* strTextureName,
		LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DCUBETEXTUREQ &pddsCubeTexture,
		DWORD dwFlags, DWORD dwStage );

	// Note : Loading Data Get		 * No RefCount *
	void GetTexture( const char* strName, LPDIRECT3DTEXTUREQ &pddsTexture );
	DWORD GetTextureID( const char* strName );

	//	Note : 텍스쳐의 RefCount 를 감소 시켜서 만약
	//		이 값이 '0' 이 된다면 이 텍스쳐를 파괴한다.
	//
	HRESULT ReleaseTexture( const char* strName, LPDIRECT3DTEXTUREQ &pddsTexture );		// Note : 쓰레드를 쓴다면 꼭 이름으로 해야 한다.
	HRESULT ReleaseTexture( const char* strName, LPDIRECT3DCUBETEXTUREQ &pddsTexture );	// Note : 쓰레드를 쓴다면 꼭 이름으로 해야 한다.
	HRESULT DeleteTexture ( const char* strName );

	HRESULT ReportUsingTexture ();

	void OneTimeSceneInit();
	void StartThread( LPDIRECT3DDEVICEQ pd3dDevice );
	void DeleteAllTexture();
	void CleanUp();
};

#endif // !defined(_TEXTUREMANAGER_H__INCLUDED_)
