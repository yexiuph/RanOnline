#include "pch.h"

#include "DxFrameMesh.h"
#include "./DxLandMan.h"
#include "./SerialFile.h"
#include "./TextureManager.h"
#include "./DxEffectFrame.h"
#include "./DxEffectMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL DxFrame::SaveFile ( CSerialFile &SFile )
{
	SFile.WriteBuffer ( &vTreeMax, sizeof ( D3DXVECTOR3 ) );
	SFile.WriteBuffer ( &vTreeMin, sizeof ( D3DXVECTOR3 ) );

	//	Note : Ʈ������ ����.
	//
	SFile.WriteBuffer ( &matRot, sizeof ( D3DXMATRIXA16 ) );			//	�ش�ð�(���ϰ� ������)�� Ʈ������.
	SFile.WriteBuffer ( &matRotOrig, sizeof ( D3DXMATRIXA16 ) );		//	���� �������� �ƴ� ���� Ʈ������.
	SFile.WriteBuffer ( &matCombined, sizeof ( D3DXMATRIXA16 ) );		//	�θ� ���� ������ ���� Ʈ������.	
	
	if ( szName )
	{
		SFile << BOOL ( TRUE );

		int StrLength;
		StrLength = strlen ( szName );
		SFile << StrLength;
		if ( StrLength )
		{
			SFile.WriteBuffer ( szName, sizeof ( char ) * strlen ( szName ) );
		}
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pmsMeshs )
	{
		SFile << BOOL ( TRUE );
		pmsMeshs->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	//	Note : (0) ����Ʈ ó����.
	//
	DWORD dwCount = 0;
	DxEffectBase *pEffCur;

	pEffCur = pEffect;
	while ( pEffCur )
	{
		dwCount++;
		pEffCur = pEffCur->pLocalFrameEffNext;
	}
	SFile << dwCount;

	pEffCur = pEffect;
	while ( pEffCur )
	{
		pEffCur->SaveFile ( SFile );
		pEffCur = pEffCur->pLocalFrameEffNext;
	}


	//	Note : (1) ����Ʈ ó����.
	//
	dwCount = 0;
	pEffCur = pEffectNext;
	while ( pEffCur )
	{
		dwCount++;
		pEffCur = pEffCur->pLocalFrameEffNext;
	}
	SFile << dwCount;

	pEffCur = pEffectNext;
	while ( pEffCur )
	{
		pEffCur->SaveFile ( SFile );
		pEffCur = pEffCur->pLocalFrameEffNext;
	}


	//	Note : �ڽ� ��� �� ���� ��� ����.
	//
	if ( pframeSibling )
	{
		SFile << BOOL ( TRUE );
		pframeSibling->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pframeFirstChild )
	{
		SFile << BOOL ( TRUE );
		pframeFirstChild->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	return TRUE;
}

DxEffectBase* DxFrame::LoadEffect ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bResult = FALSE;
	HRESULT	hr = E_FAIL;
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
	BYTE	*pProperty = NULL;
	DWORD	dwVer = 0;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwSize )
	{
		pProperty = new BYTE [ dwSize ];
		SFile.ReadBuffer ( pProperty, sizeof ( BYTE ) * dwSize );
	}

	LPDXAFFINEPARTS	pAffineParts = NULL;
	SFile >> bResult;
	if ( bResult )
	{
		pAffineParts = new DXAFFINEPARTS;
		SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect ) return NULL;

	//	Note : Property, Affine  ���� ����.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), Ư�� Effect ������ ���Ǵ� �޸� ���۸� �д´�.
	//
	pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );

	//	Note : ���� ����Ʈ�� ��� �� Frame ���� ����.
	//
	pEffect->AdaptToDxFrame ( this, pd3dDevice );

	//	Note : Create Device.
	//
	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return NULL;
	}

    SAFE_DELETE_ARRAY ( pProperty );
	SAFE_DELETE ( pAffineParts );
	SAFE_DELETE_ARRAY ( szName );

	return pEffect;
}

BOOL DxFrame::LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, PDXFRAME &pAlphaMapHead )
{
	BOOL	bExist = FALSE;

	SFile.ReadBuffer ( &vTreeMax, sizeof ( D3DXVECTOR3 ) );
	SFile.ReadBuffer ( &vTreeMin, sizeof ( D3DXVECTOR3 ) );

	//	Note : Ʈ������ ����.
	//
	SFile.ReadBuffer ( &matRot, sizeof ( D3DXMATRIXA16 ) );				//	�ش�ð�(���ϰ� ������)�� Ʈ������.
	SFile.ReadBuffer ( &matRotOrig, sizeof ( D3DXMATRIXA16 ) );			//	���� �������� �ƴ� ���� Ʈ������.
	SFile.ReadBuffer ( &matCombined, sizeof ( D3DXMATRIXA16 ) );		//	�θ� ���� ������ ���� Ʈ������.	

	SFile >> bExist;
	
	if ( bExist )
	{
		int StrLength;
		SFile >> StrLength;		
		if ( StrLength )
		{
			szName = new char [ StrLength + 1 ];			
			SFile.ReadBuffer ( szName, sizeof ( char ) * StrLength );
			szName [ StrLength ] = '\0';
		}		
	}
	
	SFile >> bExist;
	if ( bExist )
	{
		pmsMeshs = new DxMeshes;
		pmsMeshs->LoadFile ( SFile, pd3dDevice );
	}

	//	Note : ����Ʈ ó����.
	//
	DWORD i;
	DWORD dwCount;

	SFile >> dwCount;
	for ( i=0; i<dwCount; i++ )
	{
		DxEffectBase* pEffCur = LoadEffect ( SFile, pd3dDevice );

		//	Note : ���� ����Ʈ�� ���.
		//
		pEffCur->pEffectNext = pEffGlobalHead;
		pEffGlobalHead = pEffCur;
	}

	SFile >> dwCount;
	for ( i=0; i<dwCount; i++ )
	{
		DxEffectBase* pEffCur = LoadEffect ( SFile, pd3dDevice );
		if ( !pEffCur )	continue;

		//	Note : ���� ����Ʈ�� ���.
		//
		pEffCur->pEffectNext = pEffGlobalHead;
		pEffGlobalHead = pEffCur;
	}

	//	Note : ���ĸ��� �����ϴ� �������� ��� ���ĸ� ����Ʈ�� ����.
	//
	if ( IsAlphaMapMesh () )
	{
		this->pNextAlphaMapFrm = pAlphaMapHead;
		pAlphaMapHead = this;
	}

	///////////////////////////////
	//	Note : �ڽ� ��� �� ���� ��� ����.
	//
	SFile >> bExist;
	if ( bExist )
	{	
		pframeSibling = new DxFrame;
		pframeSibling->LoadFile ( SFile, pd3dDevice, pEffGlobalHead, pAlphaMapHead );
	}

	SFile >> bExist;
	if ( bExist )
	{
		pframeFirstChild = new DxFrame;
		pframeFirstChild->LoadFile ( SFile, pd3dDevice, pEffGlobalHead, pAlphaMapHead );
	}

	OctreeDebugInfo::AmountDxFrame++;	

	return TRUE;
}