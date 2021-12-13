#include "pch.h"

#include "DxMethods.h"
#include "DxLightMan.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void ColorUp( D3DCOLORVALUE& d3dColor, float fValue )
{
	float fDiffR = d3dColor.r;
	float fDiffG = d3dColor.g;
	float fDiffB = d3dColor.b;

	fDiffR *= fValue;
	fDiffG *= fValue;
	fDiffB *= fValue;

	d3dColor.r = d3dColor.r + fDiffR;
	d3dColor.g = d3dColor.g + fDiffG;
	d3dColor.b = d3dColor.b + fDiffB;

	if( d3dColor.r > 1.f )		d3dColor.r = 1.f;
	if( d3dColor.g > 1.f )		d3dColor.g = 1.f;
	if( d3dColor.b > 1.f )		d3dColor.b = 1.f;
}

void	DXLIGHT::SaveSet ( CSerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.WriteBuffer( m_matWorld, sizeof( D3DXMATRIX ) );
   	
	SFile.WriteBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile << m_bEnable;
	SFile << m_BaseDiffuse;

	m_Light.Diffuse = m_BaseDiffuse;	// �⺻�� ����

	if ( m_Light.Type == D3DLIGHT_POINT )	// �����Ʈ �� ����
	{
		m_Light.Ambient.r = 0.f;
		m_Light.Ambient.g = 0.f;
		m_Light.Ambient.b = 0.f;
	}

	SFile.WriteBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

	if ( pNext )
	{
		SFile << (BOOL)TRUE;
		pNext->SaveSet ( SFile );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}
}
void	DXLIGHTNODE::SaveSet ( CSerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.WriteBuffer ( &vMax, sizeof ( D3DXVECTOR3 ) );
	SFile.WriteBuffer ( &vMin, sizeof ( D3DXVECTOR3 ) );

	if ( pLight )
	{
		SFile << BOOL ( TRUE );
		SFile.WriteBuffer ( pLight->m_szLightName, sizeof ( char ) * 256 );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pLeftChild )
	{
		SFile << BOOL ( TRUE );
		pLeftChild->SaveSet ( SFile );		
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pRightChild )
	{
		SFile << BOOL ( TRUE );
		pRightChild->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}

void	DXLIGHT::LoadSet( CSerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

	SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile >> m_bEnable;
	SFile >> m_BaseDiffuse;

	SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

	if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
		(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
	{
		m_Light.Ambient	= m_BaseDiffuse;
		m_BaseDiffuse	= m_Light.Diffuse;
	}

	m_Light.Attenuation0 = 1.0f;
	m_Light.Attenuation1 = 1.0f/m_Light.Range;		// �ִ�Ÿ��� ���, (�� = �� / 1.5f)
	m_Light.Attenuation2 = 0.0f;

	float	fSpecular;
	fSpecular = m_Light.Diffuse.r;
	if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
	else						m_Light.Specular.r = fSpecular;

	fSpecular = m_Light.Diffuse.g;
	if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
	else						m_Light.Specular.g = fSpecular;

	fSpecular = m_Light.Diffuse.b;
	if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
	else						m_Light.Specular.b = fSpecular;

	SFile >> bResult;
	if ( bResult )
	{		
        pNext = new DXLIGHT();
        pNext->LoadSet ( SFile );	
	}
}

void	DXLIGHT::LoadSet_Ver101_2( CSerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

	SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile >> m_bEnable;
	SFile >> m_BaseDiffuse;

	SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

	if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
		(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
	{
		m_Light.Ambient	= m_BaseDiffuse;
		m_BaseDiffuse	= m_Light.Diffuse;
	}

	if ( m_Light.Type == D3DLIGHT_POINT )
	{
		ColorUp( m_Light.Diffuse, 0.6f );			// MODULATE2X -> MODULATE �� �ٲ�鼭 �����۾�
		m_Light.Ambient.r = 0.f;
		m_Light.Ambient.g = 0.f;
		m_Light.Ambient.b = 0.f;
	}

	m_Light.Attenuation0 = 1.0f;
	m_Light.Attenuation1 = 1.0f/m_Light.Range;		// �ִ�Ÿ��� ���, (�� = �� / 1.5f)
	m_Light.Attenuation2 = 0.0f;

	float	fSpecular;
	fSpecular = m_Light.Diffuse.r;
	if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
	else						m_Light.Specular.r = fSpecular;

	fSpecular = m_Light.Diffuse.g;
	if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
	else						m_Light.Specular.g = fSpecular;

	fSpecular = m_Light.Diffuse.b;
	if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
	else						m_Light.Specular.b = fSpecular;

	SFile >> bResult;
	if ( bResult )
	{		
        pNext = new DXLIGHT();
        pNext->LoadSet_Ver101_2( SFile );	
	}
}

void	DXLIGHT::LoadSet_Ver100( CSerialFile &SFile )
{
	BOOL	bResult = FALSE;

	DXAFFINEPARTS m_sAffineParts;

	SFile >> bResult;
	if ( bResult )
	{
		SFile.ReadBuffer( &m_sAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	D3DXMatrixCompX( m_matWorld, m_sAffineParts );	// m_matWorld �� ����ϰ� �Ǿ ��ȯ

	SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile >> m_bEnable;
	SFile >> m_BaseDiffuse;

	SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

	if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
		(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
	{
		m_Light.Ambient	= m_BaseDiffuse;
		m_BaseDiffuse	= m_Light.Diffuse;
	}

	if ( m_Light.Type == D3DLIGHT_POINT )
	{
		ColorUp( m_Light.Diffuse, 0.6f );			// MODULATE2X -> MODULATE �� �ٲ�鼭 �����۾�
		m_Light.Ambient.r = 0.f;
		m_Light.Ambient.g = 0.f;
		m_Light.Ambient.b = 0.f;
	}

	m_Light.Attenuation0 = 1.0f;
	m_Light.Attenuation1 = 1.0f/m_Light.Range;		// �ִ�Ÿ��� ���, (�� = �� / 1.5f)
	m_Light.Attenuation2 = 0.0f;

	float	fSpecular;
	fSpecular = m_Light.Diffuse.r;
	if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
	else						m_Light.Specular.r = fSpecular;

	fSpecular = m_Light.Diffuse.g;
	if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
	else						m_Light.Specular.g = fSpecular;

	fSpecular = m_Light.Diffuse.b;
	if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
	else						m_Light.Specular.b = fSpecular;

	SFile >> bResult;
	if ( bResult )
	{		
        pNext = new DXLIGHT();
        pNext->LoadSet_Ver100( SFile );	
	}
}

void	DXLIGHTNODE::LoadSet ( CSerialFile &SFile, DXLIGHT	*pLtListHead )
{
	BOOL	bResult = FALSE;

	SFile.ReadBuffer ( &vMax, sizeof ( D3DXVECTOR3 ) );
	SFile.ReadBuffer ( &vMin, sizeof ( D3DXVECTOR3 ) );

	SFile >> bResult;
	if ( bResult )
	{
		char	szLightName[STRING_LENGTH_256] = "";		
		SFile.ReadBuffer ( szLightName, sizeof ( char ) * STRING_LENGTH_256 );

		//	<--	�̸����� ��带 ã�� ������
		DXLIGHT	*pLtListNode = pLtListHead;
		while ( pLtListNode )
		{
			if ( !strcmp ( szLightName, pLtListNode->m_szLightName ) )
			{
				pLight = pLtListNode;
				break;
			}
			pLtListNode = pLtListNode->pNext;
		}
	}

	SFile >> bResult;
	if ( bResult )
	{
		pLeftChild = new DXLIGHTNODE();
		pLeftChild->LoadSet ( SFile, pLtListHead );
	}

	SFile >> bResult;
	if ( bResult )
	{
		pRightChild = new DXLIGHTNODE();
		pRightChild->LoadSet ( SFile, pLtListHead );
	}
}

void DxLightMan::Save ( CSerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile << VERSION;

	SFile << m_bLighting;
	SFile << m_bNightAndDay;
	SFile << m_bWhenDayPointOff;

	m_LtDirectNight.SaveSet ( SFile );
	m_LtDirectNoon.SaveSet ( SFile );

	SFile << BOOL ( m_pLtListHead );
	if ( m_pLtListHead )
	{
		m_pLtListHead->SaveSet ( SFile );		
	}

	SFile << BOOL ( m_pTreeHead );
    if ( m_pTreeHead )
	{
		m_pTreeHead->SaveSet ( SFile );		
	}
}

void DxLightMan::Load ( CSerialFile &SFile, DWORD dwLandVer, BOOL bSETLAND )
{
	//	Note : ��� ���� �����͸� �ʱ�ȭ.
	//
	CleanUp ();

	BOOL bResult = FALSE;

	//	Note : ���� ����.

	DWORD dwOldVer;
	if ( bSETLAND )	dwOldVer = 0x0025;	//	DxSetLandMan::VERSION old version,
	else			dwOldVer = 0x0104;	//	DxLandMan::VERSION old version

	if ( dwLandVer < dwOldVer )
	{
		SFile >> m_bLighting;
		m_LtDirectNight.LoadSet_Ver100( SFile );	

		//	�� Direct Light ����.
		m_LtDirectNoon.m_BaseDiffuse = m_LtDirectNight.m_BaseDiffuse;
		m_LtDirectNoon.m_bEnable = m_LtDirectNight.m_bEnable;
		m_LtDirectNoon.m_Light = m_LtDirectNight.m_Light;
		StringCchCopy( m_LtDirectNoon.m_szLightName, 256, m_LtDirectNight.m_szLightName );
		StringCchCat( m_LtDirectNoon.m_szLightName, 256, "[NOON]" );

		ColorUp( m_LtDirectNoon.m_Light.Diffuse, 0.6f );
		ColorUp( m_LtDirectNight.m_Light.Ambient, 0.2f );
		ColorUp( m_LtDirectNoon.m_Light.Ambient, 0.2f );

		SFile >> bResult;
		if ( bResult )
		{
			m_pLtListHead = new DXLIGHT();
			m_pLtListHead->LoadSet_Ver100( SFile );
		}

		SFile >> bResult;
		if ( bResult )
		{	
			m_pTreeHead = new DXLIGHTNODE();
			m_pTreeHead->LoadSet ( SFile, m_pLtListHead );		
		}
	}
	//	���� ������ ����.
	else
	{
		DWORD dwVersion;
		SFile >> dwVersion;

		if( VERSION==dwVersion )
		{
			SFile >> m_bLighting;
			SFile >> m_bNightAndDay;
			SFile >> m_bWhenDayPointOff;

			m_LtDirectNight.LoadSet ( SFile );
			m_LtDirectNoon.LoadSet ( SFile );

			SFile >> bResult;
			if ( bResult )
			{
				m_pLtListHead = new DXLIGHT();
				m_pLtListHead->LoadSet ( SFile );
			}

			SFile >> bResult;
			if ( bResult )
			{	
				m_pTreeHead = new DXLIGHTNODE();
				m_pTreeHead->LoadSet ( SFile, m_pLtListHead );		
			}
		}
		else if( 0x00102==dwVersion )
		{
			SFile >> m_bLighting;
			SFile >> m_bNightAndDay;
			SFile >> m_bWhenDayPointOff;

			m_LtDirectNight.LoadSet_Ver101_2( SFile );
			m_LtDirectNoon.LoadSet_Ver101_2( SFile );

			SFile >> bResult;
			if ( bResult )
			{
				m_pLtListHead = new DXLIGHT();
				m_pLtListHead->LoadSet_Ver101_2 ( SFile );
			}

			SFile >> bResult;
			if ( bResult )
			{	
				m_pTreeHead = new DXLIGHTNODE();
				m_pTreeHead->LoadSet ( SFile, m_pLtListHead );		
			}
		}
		else if( 0x00101==dwVersion )
		{
			SFile >> m_bLighting;
			SFile >> m_bNightAndDay;
			SFile >> m_bWhenDayPointOff;

			m_LtDirectNight.LoadSet_Ver101_2( SFile );
			m_LtDirectNoon.LoadSet_Ver101_2( SFile );

			ColorUp( m_LtDirectNoon.m_Light.Diffuse, 0.6f );
			ColorUp( m_LtDirectNight.m_Light.Ambient, 0.2f );
			ColorUp( m_LtDirectNoon.m_Light.Ambient, 0.2f );

			SFile >> bResult;
			if ( bResult )
			{
				m_pLtListHead = new DXLIGHT();
				m_pLtListHead->LoadSet_Ver101_2 ( SFile );
			}

			SFile >> bResult;
			if ( bResult )
			{	
				m_pTreeHead = new DXLIGHTNODE();
				m_pTreeHead->LoadSet ( SFile, m_pLtListHead );		
			}
		}
		else if( 0x00100==dwVersion )
		{
			SFile >> m_bLighting;
			SFile >> m_bNightAndDay;
			SFile >> m_bWhenDayPointOff;

			m_LtDirectNight.LoadSet_Ver100( SFile );
			m_LtDirectNoon.LoadSet_Ver100( SFile );

			ColorUp( m_LtDirectNoon.m_Light.Diffuse, 0.6f );
			ColorUp( m_LtDirectNight.m_Light.Ambient, 0.2f );
			ColorUp( m_LtDirectNoon.m_Light.Ambient, 0.2f );

			SFile >> bResult;
			if ( bResult )
			{
				m_pLtListHead = new DXLIGHT();
				m_pLtListHead->LoadSet_Ver100( SFile );
			}

			SFile >> bResult;
			if ( bResult )
			{	
				m_pTreeHead = new DXLIGHTNODE();
				m_pTreeHead->LoadSet ( SFile, m_pLtListHead );		
			}
		}
	}

	// Note : �ε� �� ���� ���Ϳ� �ִ´�.
	ListToVectorLIGHT();
}


