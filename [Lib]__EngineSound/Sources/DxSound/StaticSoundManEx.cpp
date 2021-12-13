#include "pch.h"
#include "StaticSoundMan.h"
#include "SerialFile.h"
#include "DxSoundMan.h"
#include "dsutil.h"
#include "EditMeshs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	���̵� �ƿ��Ǵ� ����
//	WorldSoundPage���� INTERVALSIZE��� �̸����� ������,
//	�����ؾ��մϴ�. 
static	const	float	INTERVALSIZE = (40.0f);
static	const	float	FADEINTERPOLIAION = (0.73f);


void	CStaticSoundMan::FrameMove( D3DXVECTOR3 ListenerPos, float fTime )
{
	SVecSound *pCurSound = m_pVecSoundHead;

	if ( !m_bEnableSound || DxSoundMan::GetInstance().IsMapMute () )
	{
		while ( pCurSound )
		{			
			pCurSound->Stop ();
			pCurSound = pCurSound->m_pNext;
		}
		return;
	}

	if ( m_pVecSoundTree )
	{
		//	<--	���� ���� ������
		m_FramePos++;
		//	-->	//

		FindSoundToPlay ( m_pVecSoundTree, ListenerPos, fTime );
		FindSoundToStop ();
	}	
}

void	CStaticSoundMan::FindSoundToPlay ( POBJAABBNODE pTreeNode, D3DXVECTOR3& ListenerPos, float& fElapsedTime )
{
	if ( !COLLISION::IsWithInPoint ( pTreeNode->vMax, pTreeNode->vMin, ListenerPos ) )	return;

	//	<--	�������� ���� Node	-->	//

	if ( pTreeNode->pObject )
	{
		SVecSound *pVecSound = (SVecSound*)pTreeNode->pObject;
		
		m_pVecSoundNew[m_VecSoundNewCount] = pVecSound;
		m_VecSoundNewCount++;

		PlaySound ( pVecSound, ListenerPos, fElapsedTime );
		return;
	}

	if ( pTreeNode->pLeftChild )	FindSoundToPlay ( pTreeNode->pLeftChild, ListenerPos, fElapsedTime );
	if ( pTreeNode->pRightChild )	FindSoundToPlay ( pTreeNode->pRightChild, ListenerPos, fElapsedTime );
}

void	CStaticSoundMan::PlaySound ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos, float& fElapsedTime )
{
	pVecSound->m_FramePos = m_FramePos;		

	//	<--	�����ɸ� ���϶�	-->	//
	if ( pVecSound->IsLoop() )
	{
		PlayLoopSound ( pVecSound, ListenerPos );
	}
	//	<--	�����ð������� �����Ǵ� ���϶�	-->	//
	else
	{
		PlayTimeSound ( pVecSound, ListenerPos, fElapsedTime );
	}
}

void	CStaticSoundMan::PlayLoopSound ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos )
{
	//	<--	���۰� ����ֳ�?	-->	//
	if ( !pVecSound->IsPlaying () )
	{
		pVecSound->Play ();
	}

	CtrlDSFade ( pVecSound, ListenerPos );
	CtrlDSBuffer ( pVecSound, ListenerPos );
}

void	CStaticSoundMan::PlayTimeSound ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos, float& fElapsedTime )
{
	//	���۰� ����ֳ�?
	if ( !pVecSound->IsPlaying () )
	{
        //	�ֱ� �ð���ŭ ����Ǿ��°�?
		pVecSound->m_CurrentTime += fElapsedTime;
		if ( pVecSound->m_LoopTime <= pVecSound->m_CurrentTime )
		{			
			pVecSound->Stop ();
			pVecSound->Play ();

			pVecSound->m_CurrentTime = 0.0f;
		}
		else
		{
		}
	}	

	//	������϶��� ������Ʈ
	if ( pVecSound->IsPlaying () )
	{
		CtrlDSFade ( pVecSound, ListenerPos );
		CtrlDSBuffer ( pVecSound, ListenerPos );
	}
}

void CStaticSoundMan::CtrlDSFade ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos )
{
	D3DXVECTOR3 vPos( pVecSound->m_matWorld._41, pVecSound->m_matWorld._42, pVecSound->m_matWorld._43 );
	D3DXVECTOR3 vDistance = vPos - ListenerPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	if ( DxSoundMan::GetInstance().IsStereo () )
	{
		const long& lPan = DxSoundMan::GetInstance().CALC_PAN ( vPos );
		pVecSound->SetPan ( lPan );

		m_fFadeRatio = DxSoundMan::GetInstance().CALC_FADE ( vPos, pVecSound->m_MinRange );
	}
	else
	{
		if ( fDistance <= pVecSound->m_MaxRange )
		{
			m_fFadeRatio = 1.0f;
		}
		else if ( fDistance <= pVecSound->m_FadeOutRange )
		{		
			float fCurSize = pVecSound->m_FadeOutRange - fDistance;
			float fFadeOut = fCurSize / INTERVALSIZE;

			//	Note : ���̵� �ƿ��� �ɶ�, �Ҹ������������� �پ�鵵��
			//		   ������ ������. FADEINTERPOLIAION�� ���� ũ�� �����ϸ�,
			//		   �� ��ŭ ������ �Ҹ��� �������.
			fFadeOut += (1.00f-fFadeOut) * FADEINTERPOLIAION;

			m_fFadeRatio = fFadeOut;
		}
		else
		{
			m_fFadeRatio = 0.6f;
		}
	}
}

void	CStaticSoundMan::CtrlDSBuffer ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos )
{
	const long lMaxVolume = -DSBVOLUME_MIN;

	CDebugSet::ToView ( 1, 5, "%2.1f", m_fFadeRatio );

	//	���� ��������
	const float fPostiveVolume = float(DxSoundMan::GetInstance().GetMapVolume() + lMaxVolume);
	//	���̵� ����
    const float fFadeVolume = m_fFadeRatio * fPostiveVolume;

	//	lMaxVolume �ø���ŭ ������� ����
	const long lVolume = long( fFadeVolume ) - lMaxVolume;

	pVecSound->SetVolume ( lVolume );
}


void	CStaticSoundMan::FindSoundToStop ()
{
	for ( int i = 0; i < m_VecSoundOldCount; i++ )
	{
		if ( m_pVecSoundOld[i]->m_FramePos != m_FramePos )
		{
			m_pVecSoundOld[i]->Stop ();
		}
	}

	memcpy ( m_pVecSoundOld, m_pVecSoundNew, sizeof ( SVecSound * ) * 256 );
	m_VecSoundOldCount = m_VecSoundNewCount;
	m_VecSoundNewCount = 0;
}

void	SVecSound::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{	
	D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );

	const	float	INTERVALBOXSIZE = INTERVALSIZE + 1.0f;
	vMax.x = vPos.x + m_MaxRange + INTERVALSIZE;
	vMax.y = vPos.y + m_MaxRange + INTERVALSIZE;
	vMax.z = vPos.z + m_MaxRange + INTERVALSIZE;

	vMin.x = vPos.x - m_MaxRange - INTERVALSIZE;
	vMin.y = vPos.y - m_MaxRange - INTERVALSIZE;
	vMin.z = vPos.z - m_MaxRange - INTERVALSIZE;
}

BOOL	CStaticSoundMan::LoadSet ( CSerialFile &SFile )
{
	WORD StaticSoundManVER;	
	WORD m_VecSoundVER;
	WORD m_SoundVER;

	SFile >> StaticSoundManVER;	
	SFile >> m_VecSoundVER;
	SFile >> m_SoundVER;

	SFile >> m_VecSoundCount;
	
	SVecSound *pCur;
	for ( int i = 0; i < (int)m_VecSoundCount; i++ )
	{
		pCur = new SVecSound;
		pCur->LoadSet ( SFile, m_VecSoundVER, m_SoundVER );		

		if ( pCur->m_MinRange <= pCur->m_MaxRange )
		{			
			pCur->m_FadeOutRange = ( pCur->m_MaxRange + INTERVALSIZE );
		}
		else
		{			
			char szError[128] = "";
			StringCchPrintf( szError, 128, "[%s]{m:%03d M:%03d} Sound Range setting Error.",
				pCur->m_MinRange, pCur->m_MaxRange );			
			MessageBox ( NULL, szError, "Caution", MB_OK );
			SAFE_DELETE ( pCur );
			break;
		}		

		pCur->m_pNext = m_pVecSoundHead;
		m_pVecSoundHead = pCur;
	}

	//	<--	��������	-->	//
	COLLISION::MakeAABBTree ( GetOBJAABBNode(), GetVecSoundHead() );

	return TRUE;
}