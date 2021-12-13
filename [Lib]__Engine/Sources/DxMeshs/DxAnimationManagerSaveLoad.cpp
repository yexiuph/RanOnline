#include "pch.h"

#include "DxAnimationManager.h"
#include "DxFrameMesh.h"
#include "./SerialFile.h"
#include "DxOctree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL	DxAnimationMan::SaveFile ( CSerialFile &SFile )
{
	SFile.WriteBuffer ( &m_AniType, sizeof ( ANIMATETYPE ) );
	SFile << fCurTime;

	if ( m_pDxFrameRoot )
	{
		SFile << BOOL ( TRUE );
		m_pDxFrameRoot->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
	
	if ( m_pDxAnimationHead )
	{
		SFile << BOOL ( TRUE );
		m_pDxAnimationHead->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( m_pNext )
	{
		SFile << BOOL ( TRUE );
		m_pNext->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
	
	return TRUE;
}

BOOL	DxAnimationMan::LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bExist = FALSE;

	SFile.ReadBuffer ( &m_AniType, sizeof ( ANIMATETYPE ) );
	SFile >> fCurTime;

	SFile >> bExist;
	if ( bExist )
	{
		m_pDxFrameRoot = new DxFrame;
		m_pDxFrameRoot->LoadFile ( SFile, pd3dDevice, m_pEffectHead, m_pDxAlphaFrameHead );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pDxAnimationHead = new DxAnimation;
		m_pDxAnimationHead->LoadFile ( SFile, this );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pNext = new DxAnimationMan;
		m_pNext->LoadFile ( SFile, pd3dDevice );
	}
	
	return TRUE;
}