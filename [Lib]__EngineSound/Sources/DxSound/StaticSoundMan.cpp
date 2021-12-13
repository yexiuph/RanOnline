#include "pch.h"

#include "StaticSoundMan.h"
#include "SerialFile.h"
#include "DxSoundMan.h"
#include "EditMeshs.h"
#include "DxViewPort.h" //../[Lib]__Engine/Sources/DxCommon/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

WORD	SVecSound::m_VecSoundVER = 102;
WORD	CStaticSoundMan::m_StaticSoundManVER = 100;

////////////////////////////////////////////////////////////////////////////////
//	<--	SVecSound 구조체 관련 메쏘드들
SVecSound::SVecSound()
	: m_pNext(NULL)
	, m_MinRange(0.0f)
	, m_MaxRange(0.0f)
	, m_CurrentTime(0.0f)
	, m_LoopTime(0.0f)
{	
	SecureZeroMemory( m_szSoundName, sizeof( m_szSoundName ) );
}

SVecSound::~SVecSound()
{	
	m_pNext = NULL;
}

SVecSound::SVecSound(const SVecSound& rValue) : SSound ( rValue )
{
	*this = rValue;
}

SVecSound&	SVecSound::operator= ( const SVecSound& rValue )
{
	if ( this != &rValue )
	{
		m_FramePos = 0;
		StringCchCopy( m_szSoundName, 256, rValue.m_szSoundName );
		//m_Position = rValue.m_Position;
		m_matWorld = rValue.m_matWorld;
		m_MinRange = rValue.m_MinRange;
		m_MaxRange = rValue.m_MaxRange;
		m_LoopTime = rValue.m_LoopTime;		
		//	일부러 이렇게 한 것임, 최초에 한 번 플레이를 위해
		m_CurrentTime = rValue.m_LoopTime;
		m_FadeOutRange = rValue.m_FadeOutRange;

		m_pNext = rValue.m_pNext;

		SSound::operator = ( rValue );
	}

	return *this;
}

bool SVecSound::Play ()
{
	if ( SSound::IsPlaying () ) return true;

	bool bResult = true;
	bResult = SSound::Play ();
	if ( !bResult ) return false;

	D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );

	if ( DxSoundMan::GetInstance().IsStereo () )
	{
		const long& lPan = DxSoundMan::GetInstance().CALC_PAN ( vPos );
		bResult = SSound::SetPan ( lPan );
		if ( !bResult ) return false;
	}
	else
	{
		bResult = SSound::SetMinDistance ( m_MinRange, DS3D_DEFERRED );
		if ( !bResult ) return false;

		bResult = SSound::SetMaxDistance ( m_MaxRange, DS3D_DEFERRED );
		if ( !bResult ) return false;

		bResult = SSound::SetPosition ( vPos, DS3D_DEFERRED );	
		if ( !bResult ) return false;
	}

	return true;
}

BOOL	SVecSound::SaveSet ( CSerialFile &SFile )
{
	D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	SFile.WriteBuffer ( m_szSoundName, sizeof ( char ) * 256 );
	SFile.WriteBuffer ( &vPos, sizeof ( D3DXVECTOR3 ) );
	SFile << m_MinRange;
	SFile << m_MaxRange;
	SFile << m_LoopTime;

	SSound::SaveSet ( SFile );

	return TRUE;
}

BOOL	SVecSound::LoadSet ( CSerialFile &SFile, WORD VecSoundVer, WORD SoundVer )
{
	D3DXVECTOR3 vPos;
	SFile.ReadBuffer ( m_szSoundName, sizeof ( char ) * 256 );
	SFile.ReadBuffer ( &vPos, sizeof ( D3DXVECTOR3 ) );
	SFile >> m_MinRange;
	SFile >> m_MaxRange;
	SFile >> m_LoopTime;

	m_CurrentTime = m_LoopTime;

	if ( VecSoundVer == 101 )
	{
		long lVolume;
		SFile.ReadBuffer ( &lVolume, sizeof ( long ) );
	}

	SSound::LoadSet ( SFile, SoundVer );

	m_matWorld._41 = vPos.x;
	m_matWorld._42 = vPos.y; 
	m_matWorld._43 = vPos.z;

	return TRUE;
}

BOOL	SVecSound::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	return COLLISION::IsWithInPoint ( vDivMax, vDivMin, vPos );
}

OBJAABB*SVecSound::GetNext ()
{
	return m_pNext;
}

//	-->	SVecSound 구조체 관련 메쏘드들
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//	<--	CStaticSoundMan 클래스 관련 메쏘드들
CStaticSoundMan::CStaticSoundMan ()
	: m_VecSoundCount(0)
	, m_pVecSoundHead(NULL)	
	, m_bRenderRange(FALSE)	
	, m_bEnableSound(TRUE)
	, m_pVecSoundTree(NULL)
	, m_bDSPName(FALSE)
	, m_VecSoundNewCount(0)
	, m_VecSoundOldCount(0)
{
}

CStaticSoundMan::~CStaticSoundMan()
{		
	FinalCleanup();
}

BOOL	CStaticSoundMan::AddVecSound ( SVecSound *pVecSound )
{
	pVecSound->m_pNext = m_pVecSoundHead;
	m_pVecSoundHead = pVecSound;

	m_VecSoundCount++;

	return TRUE;
}

BOOL	CStaticSoundMan::DelVecSound ( char *szSoundName )
{
	if ( !m_pVecSoundHead )
	{
		return FALSE;
	}

	PSVECSOUND pCur = m_pVecSoundHead, pBack = NULL;
	if ( !strcmp(pCur->m_szSoundName,szSoundName) )
	{
		m_pVecSoundHead = m_pVecSoundHead->m_pNext;
		pCur->m_pNext = NULL;
		SAFE_DELETE(pCur);
		m_VecSoundCount--;

		m_VecSoundNewCount = 0;
		m_VecSoundOldCount = 0;
		COLLISION::MakeAABBTree ( GetOBJAABBNode(), GetVecSoundHead() );

		return TRUE;
	}

	pBack = m_pVecSoundHead;
	pCur = m_pVecSoundHead->m_pNext;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szSoundName,szSoundName) )
		{
			pBack->m_pNext = pCur->m_pNext;
			pCur->m_pNext = NULL;
			SAFE_DELETE(pCur);
			m_VecSoundCount--;

			m_VecSoundNewCount = 0;
			m_VecSoundOldCount = 0;
			COLLISION::MakeAABBTree ( GetOBJAABBNode(), GetVecSoundHead() );

			return TRUE;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}

	return FALSE;
}

void		CStaticSoundMan::EnableSound ( BOOL bEnableSound )
{
	m_bEnableSound = bEnableSound;
	if ( m_bEnableSound )
	{
		COLLISION::MakeAABBTree ( GetOBJAABBNode(), GetVecSoundHead() );
	}
}

SVecSound* CStaticSoundMan::GetVecSound ( const char* szSoundName )
{
	SVecSound* pVecSound = m_pVecSoundHead;
	while ( pVecSound )
	{
		if ( !strcmp(pVecSound->m_szSoundName, szSoundName) )
			return pVecSound;

		pVecSound = pVecSound->m_pNext;
	}

	return NULL;
}

void CStaticSoundMan::Clone_MouseShift( LPD3DXMATRIX& pMatrix )
{
	SVecSound* pVecSound = m_pVecSoundHead;
	while( pVecSound )
	{
		DWORD dwBuffer = (DWORD)&pVecSound->m_matWorld;
		if( dwBuffer == (DWORD)pMatrix )
		{
			//// Note : 새로 만들어 준다.
			//DXLIGHT* pNewLight = new DXLIGHT;
			//*pNewLight = *pLight;	// 값셋팅

			//char szNum[256];
			//DWORD dwNum = 0;
			//while (1)
			//{
			//	if ( dwNum >= 1000 )	sprintf ( szNum, "[%d]", dwNum++ );
			//	else					sprintf ( szNum, "[%03d]", dwNum++ );

			//	StringCchCopy( pNewLight->m_szLightName, 256, "UNNAMED" );
			//	StringCchCat( pNewLight->m_szLightName, 256, szNum );

			//	if ( !GetLight( pNewLight->m_szLightName ) )	// 이름 체크후 없는것으로 생성.
			//		break;
			//}

			//pNewLight->pNext = m_pLtListHead;
			//m_pLtListHead = pNewLight;

			//// Note : DxObjectMRS 커서를 바꿔준다. < 중요 >
			//pMatrix = &pNewLight->m_matWorld;

			return;
		}
		pVecSound = pVecSound->m_pNext;
	}
}

void CStaticSoundMan::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	SVecSound* pCur = m_pVecSoundHead;
	while( pCur )
	{
		D3DXMatrixMultiply( &pCur->m_matWorld, &pCur->m_matWorld, &matRotate );
		pCur = pCur->m_pNext;
	}
}

void CStaticSoundMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_bRenderRange )
	{
		SVecSound* pVecSound = m_pVecSoundHead;
		while ( pVecSound )
		{
			if( m_bDSPName )
			{
				D3DXVECTOR3 vOut;
				D3DXVECTOR3 vSrc( pVecSound->m_matWorld._41, pVecSound->m_matWorld._42, pVecSound->m_matWorld._43 );
				D3DXMATRIX matIdentity;

				D3DXMatrixIdentity( &matIdentity );
				D3DXVec3Project(&vOut,
								&vSrc,
								&DxViewPort::GetInstance().GetViewPort(),
								&DxViewPort::GetInstance().GetMatProj(),
								&DxViewPort::GetInstance().GetMatView(), 
								&matIdentity);

				CDebugSet::ToPos( vOut.x, vOut.y, "%s", pVecSound->m_szSoundName );
			}

			D3DXVECTOR3 vMax, vMin;
			pVecSound->GetAABBSize ( vMax, vMin );
			//EDITMESHS::RENDERAABB ( pd3dDevice, vMax, vMin );
			D3DXVECTOR3 vPos( pVecSound->m_matWorld._41, pVecSound->m_matWorld._42, pVecSound->m_matWorld._43 );
			EDITMESHS::RENDERSPHERE ( pd3dDevice, vPos, pVecSound->m_MaxRange, NULL, 0xffffff11 );
			EDITMESHS::RENDERSPHERE ( pd3dDevice, vPos, pVecSound->m_MinRange, NULL, 0xffff1111 );
			pVecSound = pVecSound->m_pNext;
		}
	}
}

HRESULT CStaticSoundMan::OneTimeSceneInit()
{
	m_bEnableSound = TRUE;
	return S_OK;
}

HRESULT CStaticSoundMan::FinalCleanup()
{
	PSVECSOUND	pCur = m_pVecSoundHead;

	while ( pCur != NULL )
	{		
		m_pVecSoundHead = m_pVecSoundHead->m_pNext;
		delete pCur;
		pCur = m_pVecSoundHead;
	}
	
	m_pVecSoundHead = NULL;
	SAFE_DELETE(m_pVecSoundTree);
	
	m_VecSoundNewCount = 0;
	m_VecSoundOldCount = 0;

	m_VecSoundCount = 0;

	return S_OK;
}

BOOL	CStaticSoundMan::Import ( CStaticSoundMan::LPCSTATICSOUNDMAN& rpStaticSoundMan )
{
	PSVECSOUND	pCur = m_pVecSoundHead;
	PSVECSOUND	pNew = NULL;

	while ( pCur != NULL )
	{
		pNew = new SVecSound;

		*pNew = *pCur;
		pNew->m_pNext = NULL;

		rpStaticSoundMan->AddVecSound ( pNew );

		pCur = pCur->m_pNext;
	}

	//	<--	공간정렬	-->	//
	COLLISION::MakeAABBTree ( rpStaticSoundMan->GetOBJAABBNode(),
		rpStaticSoundMan->GetVecSoundHead() );

	return TRUE;
}

BOOL	CStaticSoundMan::SaveSet ( CSerialFile &SFile )
{
	SFile << m_StaticSoundManVER;	
	SFile << SVecSound::m_VecSoundVER;
	SFile << SSound::m_SoundVER;

	SFile << m_VecSoundCount;

	SVecSound *pCur = m_pVecSoundHead;
	for ( int i = 0; i < (int)m_VecSoundCount; i++, pCur = pCur->m_pNext )
	{
		pCur->SaveSet ( SFile );		
	}

	return TRUE;
}
//	-->	CStaticSoundMan 클래스 관련 메쏘드들
////////////////////////////////////////////////////////////////////////////////