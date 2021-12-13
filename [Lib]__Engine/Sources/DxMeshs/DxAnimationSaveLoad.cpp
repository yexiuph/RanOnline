#include "pch.h"

#include "DxFrameMesh.h"
#include "./DxLandMan.h"
#include "./SerialFile.h"
#include "./TextureManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL DxAnimation::SaveFile ( CSerialFile &SFile )
{
	SFile.WriteBuffer ( &m_cPositionKeys, sizeof ( UINT ) );
	if ( m_cPositionKeys )
	{
		SFile.WriteBuffer ( m_pPositionKeys, sizeof ( SPositionKey ) * m_cPositionKeys );
	}

	SFile.WriteBuffer ( &m_cRotateKeys, sizeof ( UINT ) );
	if ( m_cRotateKeys )
	{
		SFile.WriteBuffer ( m_pRotateKeys, sizeof ( SRotateKey ) * m_cRotateKeys );
	}

	SFile.WriteBuffer ( &m_cScaleKeys, sizeof ( UINT ) );
	if ( m_cScaleKeys )
	{
		SFile.WriteBuffer ( m_pScaleKeys, sizeof ( SScaleKey ) * m_cScaleKeys );
	}
	
	SFile.WriteBuffer ( &m_cMatrixKeys, sizeof ( UINT ) );
	if ( m_cMatrixKeys )
	{
		SFile.WriteBuffer ( m_pMatrixKeys, sizeof ( SMatrixKey ) * m_cMatrixKeys );
	}

	if ( pAnimNext )
	{
		SFile << BOOL ( TRUE );
		pAnimNext->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( szName )
	{
		SFile << BOOL ( TRUE );

		int	StrLength = 0;
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

	return TRUE;
}

BOOL DxAnimation::LoadFile ( CSerialFile &SFile, DxAnimationMan *pDxAnimation )
{
	BOOL	bExist = FALSE;

	SFile.ReadBuffer ( &m_cPositionKeys, sizeof ( UINT ) );
	if ( m_cPositionKeys )
	{
		m_pPositionKeys = new SPositionKey [ m_cPositionKeys ];
		SFile.ReadBuffer ( m_pPositionKeys, sizeof ( SPositionKey ) * m_cPositionKeys );
	}

	SFile.ReadBuffer ( &m_cRotateKeys, sizeof ( UINT ) );
	if ( m_cRotateKeys )
	{
		m_pRotateKeys = new SRotateKey [ m_cRotateKeys ];
		SFile.ReadBuffer ( m_pRotateKeys, sizeof ( SRotateKey ) * m_cRotateKeys );
	}

	SFile.ReadBuffer ( &m_cScaleKeys, sizeof ( UINT ) );
	if ( m_cScaleKeys )
	{
		m_pScaleKeys = new SScaleKey [ m_cScaleKeys ];
		SFile.ReadBuffer ( m_pScaleKeys, sizeof ( SScaleKey ) * m_cScaleKeys );
	}
	
	SFile.ReadBuffer ( &m_cMatrixKeys, sizeof ( UINT ) );
	if ( m_cMatrixKeys )
	{
		m_pMatrixKeys = new SMatrixKey [ m_cMatrixKeys ];
		SFile.ReadBuffer ( m_pMatrixKeys, sizeof ( SMatrixKey ) * m_cMatrixKeys );
	}

	SFile >> bExist;
	if ( bExist )
	{
		pAnimNext = new DxAnimation;
		pAnimNext->LoadFile ( SFile, pDxAnimation );
	}

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

	//	Note : 연결된 프레임을 지정.
	//
	if ( pDxAnimation )
	{
		DxFrame* pFrameRoot = pDxAnimation->GetDxFrameRoot();
		pframeToAnimate = pFrameRoot->FindFrame ( szName );	 
		if (pframeToAnimate == NULL)	return FALSE;
		
		pframeToAnimate->pframeFromAnimate = this;
	}

	return TRUE;
}
