#ifndef __DXLOADSHADER__H__
#define __DXLOADSHADER__H__

#include <vector>

typedef enum tagEBSHADERTYPE
{
	// D3D shader types
	SHADERTYPE_VERTEXSHADER  = 0,
	SHADERTYPE_PIXELSHADER   = 1,

	// D3D shader types (old names)
	SHADERTYPE_VERTEX        = 0,
	SHADERTYPE_PIXEL         = 1,

	SHADERTYPE_FORCEDWORD    = 0xFFFFFFFF
} EBSHADERTYPE;

struct DXSHADER
{
	char				szFileName[MAX_PATH];
	EBSHADERTYPE		sType;
	std::vector<DWORD>	Declaration;
	DWORD				dwShaderHandle;
	DWORD				dwReference;
	
	DXSHADER			*pNext;

	DXSHADER () :
		sType(SHADERTYPE_VERTEXSHADER),
		dwShaderHandle(NULL),
		dwReference(0),
		pNext(NULL)
	{
		memset( szFileName, 0, sizeof(char)*MAX_PATH );
	}

	~DXSHADER ()
	{
		SAFE_DELETE ( pNext );
	}
};

class DXShaderMan
{
protected:
	char m_szShaderDir[MAX_PATH];
	LPDIRECT3DDEVICEQ m_pd3dDevice;

protected:
	DXSHADER	*m_pHead;

protected:
	DXShaderMan () :
		m_pHead(NULL),
		m_pd3dDevice(NULL)
	{
		memset( m_szShaderDir, 0, sizeof(char)*MAX_PATH );
	}

public:
	~DXShaderMan ()
	{
	}

public:
	void SetPath ( const char *szPath )
	{
		StringCchCopy( m_szShaderDir, MAX_PATH, szPath );
	}

	const char* GetPath ()
	{
		return m_szShaderDir;
	}

	DXSHADER* FindShader ( const char *szFile, const DWORD* pDeclaration );

public:
	HRESULT LoadAndCreateShader ( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile,
							const DWORD* pDeclaration, DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle );

	HRESULT ReleaseShader ( const char *szFile );
	HRESULT ReleaseShader ( const DWORD dwShaderHandle );
	void DeleteAllShader ();

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

public:
	static DXShaderMan& GetInstance();
};

#endif // __DXLOADSHADER__H__