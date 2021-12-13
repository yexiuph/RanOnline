#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "DxFrameMesh.h"
#include "./DxLandMan.h"
#include "./SerialFile.h"
#include "./TextureManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL	DxMeshes::SaveFile ( CSerialFile &SFile )
{
	SFile << cMaterials;

	if ( cMaterials )
	{		
		SFile.WriteBuffer ( rgMaterials, sizeof ( D3DMATERIALQ ) * cMaterials );
		SFile.WriteBuffer ( exMaterials, sizeof ( D3DEXMATERIAL ) * cMaterials );
	}

	if ( strTextureFiles )
	{
		SFile << BOOL ( TRUE );
		for ( DWORD i = 0; i < cMaterials; i++ )
		{
			int StrLength;
			StrLength = strTextureFiles[i].GetLength();
			SFile << StrLength;

			if ( StrLength )
			{
				SFile.WriteBuffer ( (void *)strTextureFiles[i].GetString(), sizeof ( char ) * StrLength );
			}
		}
	}
	else
	{
		SFile << BOOL ( FALSE );
	}


	if ( szName )
	{
		SFile << BOOL ( TRUE );
		int	StrLength;
		StrLength = strlen ( szName );
		SFile << StrLength;
		if ( StrLength )
		{
			SFile.WriteBuffer ( szName, sizeof ( char ) * StrLength );
		}
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	for ( DWORD i=0; i<m_pDxOctreeMesh->m_dwAttribTableSize; i++ )
	{
		if ( cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId )
		{
			MessageBox ( NULL, "cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId", "ERROR", MB_OK );
		}
	}

	m_pDxOctreeMesh->SaveFile ( SFile );
	
	if ( pMeshNext )
	{
		SFile << BOOL ( TRUE );
		pMeshNext->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	return TRUE;
}

BOOL	DxMeshes::LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bExist = FALSE;

	SFile >> cMaterials;	

	if ( cMaterials == 0 )
    {
        rgMaterials = new D3DMATERIALQ[1];
		exMaterials = new D3DEXMATERIAL[1];		
        pTextures = new LPDIRECT3DTEXTUREQ[1];
		strTextureFiles = new CString[1];

        if ( rgMaterials == NULL || pTextures == NULL || strTextureFiles == NULL || exMaterials == NULL )
        {
            return FALSE;
        }
       
        memset(rgMaterials, 0, sizeof(D3DMATERIALQ));
        rgMaterials[0].Diffuse.r = 0.5f;
        rgMaterials[0].Diffuse.g = 0.5f;
        rgMaterials[0].Diffuse.b = 0.5f;
        rgMaterials[0].Specular = rgMaterials[0].Diffuse;

		strTextureFiles[0] = "";
    }
	//	Note : 메터리얼 값이 있을 경우.
	//
    else
    {
        rgMaterials = new D3DMATERIALQ[cMaterials];
		exMaterials = new D3DEXMATERIAL[cMaterials];	
        pTextures = new LPDIRECT3DTEXTUREQ[cMaterials];
		memset ( pTextures, 0, sizeof ( LPDIRECT3DTEXTUREQ ) * cMaterials );
		strTextureFiles = new CString[cMaterials];		
        if (rgMaterials == NULL || pTextures == NULL || strTextureFiles == NULL || exMaterials == NULL )
        {
            return	FALSE;
        }
        
		SFile.ReadBuffer ( rgMaterials, sizeof ( D3DMATERIALQ ) * cMaterials );
		SFile.ReadBuffer ( exMaterials, sizeof ( D3DEXMATERIAL ) * cMaterials );

		SFile >> bExist;
		if ( bExist )
		{
			//	Note	:	strTextureFiles로드 및 텍스쳐 로드
			//
			for ( DWORD i = 0; i < cMaterials; i++ )
			{	
				int StrLength;			
				SFile >> StrLength;

				if ( StrLength )
				{
					char	*TmpStr = new char [StrLength + 1];
					SFile.ReadBuffer ( TmpStr, sizeof ( char ) * StrLength );
					TmpStr[StrLength] = '\0';

					strTextureFiles[i] = TmpStr;

					delete	[] TmpStr;


					//	Note : 텍스쳐 로드.
					//
					pTextures[i] = NULL;
					if ( strTextureFiles[i].GetString() != NULL )
					{
						HRESULT hr;
						hr = TextureManager::LoadTexture ( (char *)strTextureFiles[i].GetString(),
								pd3dDevice, pTextures[i], 0, 0 );
						if (FAILED(hr))
						{
							hr = S_OK;
							pTextures[i] = NULL;
						}

						// DXMTRLTYPE 재설정
						if( (exMaterials[i].MtrlType==MATERIAL_ALPHA_NOZWRI) || 
							(exMaterials[i].MtrlType==MATERIAL_ALPHA_NOTEST) )	
						{
						}
						else
						{
							TextureManager::EM_TEXTYPE emTexType = TextureManager::GetTexType( (char *)strTextureFiles[i].GetString() );

							if( emTexType==TextureManager::EMTT_NORMAL )			exMaterials[i].MtrlType = MATERIAL_NORMAL;
							else if( emTexType==TextureManager::EMTT_ALPHA_HARD )	exMaterials[i].MtrlType = MATERIAL_ALPHA_HARD;
							else if( emTexType==TextureManager::EMTT_ALPHA_SOFT )	exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==TextureManager::EMTT_ALPHA_SOFT01 )	exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==TextureManager::EMTT_ALPHA_SOFT02 )	exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
						}
					}				
				}
			}
		}
    }

	SFile >> bExist;
	if ( bExist )
	{
		int	StrLength;		
		SFile >> StrLength;
		
		if ( StrLength )
		{
			szName = new char[StrLength];
			SFile.ReadBuffer ( szName, sizeof ( char ) * StrLength );
		}
	}

	m_pDxOctreeMesh = new DxOctreeMesh;
	m_pDxOctreeMesh->LoadFile ( SFile, pd3dDevice );
	

	for ( DWORD i=0; i<m_pDxOctreeMesh->m_dwAttribTableSize; i++ )
	{
		if ( cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId )
		{
			//MessageBox ( NULL, "cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId", "ERROR", MB_OK );
		}
	}

	SFile >> bExist;
	if ( bExist )
	{
		pMeshNext = new DxMeshes;
		pMeshNext->LoadFile ( SFile, pd3dDevice );
	}

	return TRUE;
}

