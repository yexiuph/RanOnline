#include "pch.h"
#include "DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./DxTextureEffMan.h"
#include "./DxEffectFrame.h"
#include "DxReplaceContainer.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD DxFrameMesh::VERSION = 0x100;

void DxFrameMesh::SaveSet ( CSerialFile &SFile )
{
	if ( m_pszFileName )
	{
		SFile << BOOL ( TRUE );

		int StrLength = strlen ( m_pszFileName ) + 1;
		SFile << StrLength;
		//	<**	+1�� �ι��ڱ��� ����ִ� ����
		SFile.WriteBuffer ( m_pszFileName, sizeof ( char ) * StrLength );
		//	**>	
	}
	else
	{
		SFile << BOOL ( FALSE );
	}


	if ( m_pDxFrame )
	{
		SFile << BOOL ( TRUE );
		DWORD	nNodeCount = 0;
		m_pDxFrame->DxFrameNodeCount( nNodeCount );
		SFile << nNodeCount;
		m_pDxFrame->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( m_pPieceObjHead )
	{
		SFile << BOOL ( TRUE );
		m_pPieceObjHead->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pAnimManHead )
	{
		SFile << BOOL ( TRUE );
		pAnimManHead->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( m_pEffectHead )
	{
		SFile << BOOL ( TRUE );
		m_pEffectHead->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	// Note : VER100 ���� ���� ���� ����. Version
	SFile << VERSION;

	// Note : 
	SFile << BOOL ( TRUE );
	if( m_pTextureEff )	m_pTextureEff->SavePSF( SFile );

	// Note : ���� ������ ������ ���� ����
	SFile << BOOL ( FALSE );
}

BOOL DxFrameMesh::LoadSet_VER100( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bResult = FALSE;

	// Note : ���� �۾�
	LoadSet( SFile, pd3dDevice );

	// Note : 
	DWORD dwVer;
	SFile >> dwVer;

	CDebugSet::ToFile ( "LoadSet.log", "-. Texture Effect Loading..." );
	SFile >> bResult;
	if( !bResult )		return TRUE;
	if( m_pTextureEff )	m_pTextureEff->LoadPSF( pd3dDevice, SFile );

	// Note : �����Ⱚ.
	SFile >> bResult;

	return TRUE;
}

BOOL DxFrameMesh::LoadSet ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bResult = FALSE;
	HRESULT	hr;

	SFile >> bResult;
	if ( bResult )
	{
		int		StrLength;
		char	*szFileName = NULL;
		SFile >> StrLength;
		szFileName = new char [ StrLength ];
		SFile.ReadBuffer ( szFileName, StrLength );

		CleanUp ();
		//if ( FAILED(hr) )	goto e_ERROR;
		
		SetFile ( szFileName );
		SetFVF ( OCTREEVERTEX::FVF );
	
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY ( szFileName );
			MessageBox ( NULL, "xFile Load Failed!", NULL, MB_OK );
			return FALSE;
		}

		//	Note : DxFrame �� matComp �� �ʱ�ȭ�Ѵ�.
		//
		UpdateFrames ();

		SAFE_DELETE_ARRAY(szFileName);
	}
    
	BOOL	bBothFalse = FALSE;
	SFile >> bResult;

	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. exMaterials Loading..." );
		DWORD	nNodeCount = 0, iNodeCount = 0;
		SFile >> nNodeCount;
		while ( iNodeCount < nNodeCount )
		{
			SFile >> bResult;
			if ( bResult )
			{
				char	*pszName = NULL;
				int		StrLength;
				SFile >> StrLength;
				pszName = new char [ StrLength ];
				SFile.ReadBuffer ( pszName, sizeof ( char ) * StrLength );
				
				D3DEXMATERIAL	exMaterials;
				SFile >> bResult;
				if ( bResult )
				{
					DWORD	dwVer, NODESIZE;
					SFile >> dwVer;
					SFile >> NODESIZE;

					if ( dwVer == D3DEXMATERIAL::VERSION )
					{
						SFile.ReadBuffer ( &exMaterials, sizeof ( D3DEXMATERIAL ) );
					}
					else
					{
						//	<**	CSerialFile���� fseek ( offset, SEEK_CUR )�� ã������
						//		�ٸ� ��� ��ȸ�ؼ� �� ����� �����ߴ�.
						//	**>
						long	CurOffset;
						CurOffset = SFile.GetfTell();

						//	<--	������ �ٸ���� ��ŵ -->
						SFile.SetOffSet ( CurOffset + NODESIZE );

						CDebugSet::ToFile ( "LoadSet.log", "   [%x]...������ �ٸ��ϴ�.", dwVer );
					}
				}

				DxFrame	*pDxFrame = FindFrame ( pszName );
				if ( pDxFrame )
				{
					DxMeshes	*pMesh = pDxFrame->pmsMeshs;
					if ( pMesh->exMaterials )
					{
						*(pMesh->exMaterials) = exMaterials;
					}				
				}
				else
				{					
					CDebugSet::ToFile ( "LoadSet.log", "   [%s]...Not Found", pszName );
				}

				
				SAFE_DELETE_ARRAY(pszName);
			}
			iNodeCount++;
		}		
	}

	CDebugSet::ToFile ( "LoadSet.log", "-. DXPIECEOBJ Loading..." );
	SFile >> bResult;
	while ( bResult )
	{
		DWORD	NODESIZE = 0;
		DWORD	dwVer = 0;

		SFile >> dwVer;
		SFile >> NODESIZE;		

		if ( dwVer == DXPIECEOBJ::m_dwVERSION )
		{
			float			fCurTime;
			char*			pXFileName = NULL;		
			char*			szFrameName = NULL;

			SFile >> fCurTime;
			SFile >> bResult;
			int	StrLength;
			if ( bResult )
			{			
				SFile >> StrLength;
				szFrameName = new char [ StrLength ];
				SFile.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
	        
			SFile >> bResult;
			if ( bResult )
			{
				SFile >> bResult;

				if ( bResult )
				{
					SFile >> StrLength;
					pXFileName = new char [ StrLength ];
					SFile.ReadBuffer ( pXFileName, sizeof ( char ) * StrLength );
				}
			}

			AddPieceObj ( szFrameName, pXFileName );
			SFile >> bResult;

			SAFE_DELETE_ARRAY ( szFrameName );
			SAFE_DELETE_ARRAY ( pXFileName );
		}
		else
		{
			long	CurOffset;
			CurOffset = SFile.GetfTell();
			
			//	<--	������ �ٸ���� ��ŵ -->
			SFile.SetOffSet ( CurOffset + NODESIZE );
			CDebugSet::ToFile ( "LoadSet.log", "   [%x]...������ �ٸ��ϴ�.", dwVer );
		}
	}

	//	Note : ġȯ���� ��������
	//
	MakePieceAABBTree ();

	CDebugSet::ToFile ( "LoadSet.log", "-. AnimeMan Loading..." );
	SFile >> bResult;
	while ( bResult )
	{
		DWORD	NODESIZE = 0;
		DWORD	dwVer = 0;

		SFile >> dwVer;
		SFile >> NODESIZE;		

		if ( dwVer == DxAnimationMan::m_dwVERSION )
		{
			ANIMATETYPE	AniType;
			SFile.ReadBuffer ( &AniType, sizeof ( ANIMATETYPE ) );

			char	*szName = NULL;
			BOOL	bNameExist = FALSE;
			SFile >> bResult;		//	DxFrame ���� ����
			SFile >> bNameExist;	//	DxFrame->szName ��������
			if ( bResult && bNameExist )
			{
				int	StrLength;
				SFile >> StrLength;
				szName = new char [ StrLength ];
				SFile.ReadBuffer ( szName, sizeof ( char ) * StrLength );		

				//	Note	:	Loading Data �����۾�
				//
				BOOL	bAniFrameExist = FALSE;
				DxAnimationMan* pAniManCur = pAnimManHead;
				while (pAniManCur)
				{
					if ( !strcmp(pAniManCur->GetDxFrameRoot()->szName, szName) )
					{
						pAniManCur->m_AniType = AniType;
						bAniFrameExist = TRUE;
						break;
					}					

					pAniManCur = pAniManCur->m_pNext;
				}
				if ( !bAniFrameExist )
				{
					CDebugSet::ToFile ( "LoadSet.log", "   [%s]...Not Found", szName );	
				}
			}			
			
			SFile >> bResult;

			SAFE_DELETE_ARRAY(szName);
		}
		else
		{
			long	CurOffset;
			CurOffset = SFile.GetfTell();
			
			//	<--	������ �ٸ���� ��ŵ -->
			SFile.SetOffSet ( CurOffset + NODESIZE );
			CDebugSet::ToFile ( "LoadSet.log", "   [%x]...������ �ٸ��ϴ�.", dwVer );
		}
	}

	CDebugSet::ToFile ( "LoadSet.log", "-. Effect Loading..." );
	SFile >> bResult;
	while ( bResult )
	{
		char	*szName = NULL;
		int		StrLength;

		SFile >> bResult;
		if ( bResult )
		{
			SFile >> StrLength;
			szName = new char [ StrLength ];
			SFile.ReadBuffer ( szName, StrLength );
		}

        DWORD	TypeID;
		SFile >> TypeID;
		
		DWORD	dwSize = 0;
		BYTE	*pProp = NULL;

		DWORD	dwVer = 0;

		SFile >> dwVer;
		SFile >> dwSize;
		if ( dwSize )
		{
			pProp = new BYTE [ dwSize ];
			SFile.ReadBuffer ( pProp, sizeof ( BYTE ) * dwSize );
		}

		LPDXAFFINEPARTS	pAffineParts = NULL;
		SFile >> bResult;		
		if ( bResult )
		{
			pAffineParts = new DXAFFINEPARTS;
			SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
		}
		
		DxFrame	*pDxFrame=NULL;
		if ( szName ) pDxFrame = FindFrame ( szName );

		if ( szName && pDxFrame )
		{
			DxEffectBase *pEffect = DxEffectMan::GetInstance().AdaptToDxFrame ( TypeID, this, pDxFrame,
				pd3dDevice, pProp, dwSize, dwVer, pAffineParts );

			if ( !pEffect )
			{
				DWORD NODESIZE;
				SFile >> NODESIZE;
				SFile.SetOffSet ( SFile.GetfTell() + NODESIZE );

				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] �ν��Ͻ� ������ ����.", TypeID );
				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] NunNamed Effect Buffer Skip..", TypeID );
			}
			else
			{
				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %x] �ν��Ͻ� ����.", TypeID );
			}

			//	Note : LoadBuffer(), Ư�� Effect ������ ���Ǵ� �޸� ���۸� �д´�.
			//
			pEffect->LoadBufferSet ( SFile, dwVer, pd3dDevice );

			pEffect->CleanUp ();
			pEffect->Create ( pd3dDevice );
		}
		else
		{
			DWORD NODESIZE;
			SFile >> NODESIZE;
			SFile.SetOffSet ( SFile.GetfTell() + NODESIZE );

			if ( szName )	CDebugSet::ToFile ( "LoadSet.log", "[EFF TYPEID %d] DxFrame for Adapt [%s]...Not Found", TypeID, szName );
			else			CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] NunNamed Effect Buffer Skip..", TypeID );

			// Note : �ƹ����� ���Ѵ�.
			DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance( TypeID );
			pEffect->LoadBufferSet( SFile, 0, pd3dDevice );
			SAFE_DELETE( pEffect );
		}

		SAFE_DELETE_ARRAY ( pProp );
        SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );

		SFile >> bResult;
	}

	return TRUE;
}


void DxFrame::SaveSet ( CSerialFile &SFile )
{
	if( pmsMeshs && szName )
	{
		SFile << BOOL ( TRUE );

		//	<--	DxFrame���� �ݵ�� �̸��� �����Ѵ�.
		int	StrLength = strlen ( szName ) + 1;
		SFile << StrLength;
		SFile.WriteBuffer ( szName, sizeof ( char ) * StrLength );
		//	-->	DxFrame���� �ݵ�� �̸��� �����Ѵ�.

		pmsMeshs->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pframeSibling )
	{
		pframeSibling->SaveSet ( SFile );
	}
	
	if ( pframeFirstChild )
	{
		pframeFirstChild->SaveSet ( SFile );
	}	
}


void DxMeshes::SaveSet ( CSerialFile &SFile )
{
	if ( exMaterials )
	{
		SFile << BOOL ( TRUE );
		DWORD	NodeSize = sizeof ( D3DEXMATERIAL );

		//	<--	������ ������, ������ �ٸ� ��� �������ŭ ��ŵ�Ѵ�.
		SFile << exMaterials->VERSION;
        SFile << NodeSize;
		//	-->	������ ������, ������ �ٸ� ��� �������ŭ ��ŵ�Ѵ�.

		SFile.WriteBuffer ( exMaterials, sizeof ( D3DEXMATERIAL ) );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}

void DxFrameMesh::SaveFrameEff( CSerialFile &SFile )
{
	if ( m_pEffectHead )
	{
		SFile << BOOL ( TRUE );
		m_pEffectHead->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}
