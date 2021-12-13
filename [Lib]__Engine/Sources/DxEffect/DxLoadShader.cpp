#include "pch.h"

#include "./DxLoadShader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DXShaderMan& DXShaderMan::GetInstance()
{
	static DXShaderMan Instance;
	return Instance;
}

DXSHADER* DXShaderMan::FindShader ( const char *szFile, const DWORD* pDeclaration )
{
	DXSHADER *pNode = m_pHead;
	while ( pNode )
	{
		BOOL bEqual = FALSE;

		if ( !strcmp ( szFile, pNode->szFileName ) )
		{
			bEqual = TRUE;

			int nSize = (int)pNode->Declaration.size();
			for ( int i=0; i<nSize; i++ )
			{
				if ( pNode->Declaration[i] != pDeclaration[i] )
				{
					bEqual = FALSE;
					break;
				}
			}
		}

		if ( bEqual )	return pNode;

		pNode = pNode->pNext;
	}

	return NULL;
}

HRESULT DXShaderMan::LoadAndCreateShader (	LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile,
						const DWORD* pDeclaration, DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle )
{
	char szShaderFullName[MAX_PATH] = "";
	char szErrorMessage[MAX_PATH] = "";
	
	GASSERT(szFile);
	GASSERT(pd3dDevice);

	//	Note : Shader 가 이미 등록되어 있는지 검사후 처리.
	//
	DXSHADER* pShader = FindShader ( szFile, pDeclaration );
	if ( pShader )
	{
		pShader->dwReference++;
		*pHandle = pShader->dwShaderHandle;
		return S_OK;
	}

	//	Note : Shader 등록 시도.
	//
	DXSHADER *pDxShader = new DXSHADER;
	GASSERT(pDxShader);

	StringCchCopy( pDxShader->szFileName, MAX_PATH, szFile );

	//GASSERT(pDeclaration);
	//DWORD *pDecl = (DWORD*) pDeclaration;
	//while(0)
	//{
	//	pDxShader->Declaration.push_back ( *pDecl );
	//	if ( *pDecl == D3DVSD_END() )	break;
	//	pDecl++;
	//}
	//pDxShader->sType = ShaderType;

	try
	{
		HANDLE hFile;
		//HRESULT hr;

		StringCchCat( szShaderFullName, MAX_PATH, m_szShaderDir );
		StringCchCat( szShaderFullName, MAX_PATH, szFile );

		hFile = CreateFile ( szShaderFullName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			StringCchPrintf( szErrorMessage, MAX_PATH, "Could not find file %s", szFile );
			goto E_ERROR;
		}
		
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		
		const DWORD* pShader = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
		if ( !pShader )
		{
			StringCchPrintf( szErrorMessage, MAX_PATH, "Failed to allocate memory to load shader %s", szFile );
			goto E_ERROR;
		}
		
		ReadFile(hFile, (void*)pShader, dwFileSize, &dwFileSize, NULL);
		
		CloseHandle(hFile);
		
		//if (ShaderType == SHADERTYPE_VERTEX)
		//{
		//	hr = pd3dDevice->CreateVertexShader(pDeclaration, pShader, pHandle, Usage);
		//}
		//else if (ShaderType == SHADERTYPE_PIXEL)
		//{
		//	hr = pd3dDevice->CreatePixelShader(pShader, pHandle);
		//}
		
		HeapFree(GetProcessHeap(), 0, (void*)pShader);
		
		//if (FAILED(hr))
		//{
		//	StringCchPrintf( szErrorMessage, MAX_PATH, "Failed to create shader %s", szFile );
		//	goto E_ERROR;
		//}
	}
	catch(...)
	{
		StringCchPrintf( szErrorMessage, MAX_PATH, "Error opening file %s", szFile );
		goto E_ERROR;
	}
	
	//	Note : 읽어진 핸들을 등록...
	//
	pDxShader->dwShaderHandle = *pHandle;
	pDxShader->dwReference++;
	
	pDxShader->pNext = m_pHead;
	m_pHead = pDxShader;
	return S_OK;

E_ERROR:
	MessageBox ( NULL, szErrorMessage, "ERROR", MB_OK );
	delete pDxShader;

	return E_FAIL;
}

HRESULT DXShaderMan::ReleaseShader ( const char *szFile )
{
	GASSERT(m_pd3dDevice);

	DXSHADER *pNode = m_pHead, *pNodePrev=NULL;

	//if ( !strcmp ( szFile, pNode->szFileName ) )
	//{
	//	m_pHead = m_pHead->pNext;

	//	if ( --pNode->dwReference == 0 )
	//	{
	//		m_pd3dDevice->DeleteVertexShader ( pNode->dwShaderHandle );
	//		pNode->pNext = NULL;
	//		delete pNode;
	//	}
	//	else return S_OK;
	//}

	pNode = m_pHead;
	while ( pNode )
	{
		//if ( !strcmp ( szFile, pNode->szFileName ) )
		//{
		//	GASSERT(pNodePrev);
		//	pNodePrev->pNext = pNode->pNext;

		//	if ( --pNode->dwReference == 0 )
		//	{
		//		m_pd3dDevice->DeleteVertexShader ( pNode->dwShaderHandle );
		//		pNode->pNext = NULL;
		//		delete pNode;
		//	}
		//	else return S_OK;
		//}

		pNodePrev = pNode;
		pNode = pNode->pNext;
	}

	return S_OK;
}

HRESULT DXShaderMan::ReleaseShader ( const DWORD dwShaderHandle )
{
	GASSERT(m_pd3dDevice);

	DXSHADER *pNode = m_pHead, *pNodePrev=NULL;

	if ( dwShaderHandle == pNode->dwShaderHandle )
	{
		m_pHead = m_pHead->pNext;

		//if ( --pNode->dwReference == 0 )
		//{
		//	m_pd3dDevice->DeleteVertexShader ( pNode->dwShaderHandle );
		//	pNode->pNext = NULL;
		//	delete pNode;
		//}
		//else return S_OK;
	
		return S_OK;
	}

	pNode = m_pHead;
	while ( pNode )
	{
		if ( dwShaderHandle == pNode->dwShaderHandle )
		{
			GASSERT(pNodePrev);
			pNodePrev->pNext = pNode->pNext;

			//if ( --pNode->dwReference == 0 )
			//{
			//	m_pd3dDevice->DeleteVertexShader ( pNode->dwShaderHandle );
			//	pNode->pNext = NULL;
			//	delete pNode;
			//}
			//else return S_OK;
		}

		pNodePrev = pNode;
		pNode = pNode->pNext;
	}

	return S_OK;
}

void DXShaderMan::DeleteAllShader ()
{
	GASSERT(m_pd3dDevice);

	//DXSHADER *pNode = m_pHead;
	//while ( pNode )
	//{
	//	m_pd3dDevice->DeleteVertexShader ( pNode->dwShaderHandle );
	//	pNode = pNode->pNext;
	//}

	SAFE_DELETE(m_pHead);
}

HRESULT DXShaderMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DXShaderMan::DeleteDeviceObjects()
{
	DeleteAllShader ();

	return S_OK;
}
