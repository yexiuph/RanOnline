//	Note :
//
//		# - ���ĸ� �׸���ÿ� ����ȭ ������ ����. (������)
//
//
//
//
//
#include "pch.h"

#include "DxAnimationManager.h"
#include "DxFrameMesh.h"
#include "./SerialFile.h"
#include "./DxOctree.h"
#include "./DxRenderStates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD DxAnimationMan::m_dwAniTypeNum = 3;
char DxAnimationMan::m_szAniType[3][24] = { "LOOP", "RND", "HIT" };
DWORD DxAnimationMan::m_dwAniManCount = 0;
DWORD DxAnimationMan::m_dwVERSION = 0x00000100;

float DxAnimation::GetAnimTimeLine ()
{
	DWORD dwTime = 0;

	if ( m_pMatrixKeys )
	{
		dwTime = m_pMatrixKeys[m_cMatrixKeys-1].dwTime;
	}
	else
	{
		if ( m_pScaleKeys )
		{
			dwTime = m_pScaleKeys[m_cScaleKeys-1].dwTime;
		}

		if ( m_pRotateKeys )
		{
			if ( m_pRotateKeys[m_cRotateKeys-1].dwTime > dwTime )
				dwTime = m_pRotateKeys[m_cRotateKeys-1].dwTime;
		}

		if ( m_pPositionKeys )
		{
			if ( m_pPositionKeys[m_cPositionKeys-1].dwTime > dwTime )
				dwTime = m_pPositionKeys[m_cPositionKeys-1].dwTime;
		}
	}

	return (float) dwTime;
}

void DxAnimation::operator= ( DxAnimation& rValue )
{
	pframeToAnimate = NULL;

	m_cPositionKeys = rValue.m_cPositionKeys;
	if ( m_cPositionKeys )
	{
		m_pPositionKeys = new SPositionKey [ m_cPositionKeys ];
		memcpy ( m_pPositionKeys, rValue.m_pPositionKeys, sizeof ( SPositionKey ) * m_cPositionKeys );
	}

	m_cRotateKeys = rValue.m_cRotateKeys;
	if ( m_cRotateKeys )
	{
		m_pRotateKeys = new SRotateKey [ m_cRotateKeys ];
		memcpy ( m_pRotateKeys, rValue.m_pRotateKeys, sizeof ( SRotateKey ) * m_cRotateKeys );
	}

	m_cScaleKeys = rValue.m_cScaleKeys;
	if ( m_cScaleKeys )
	{
		m_pScaleKeys = new SScaleKey [ m_cScaleKeys ];
		memcpy ( m_pScaleKeys, rValue.m_pScaleKeys, sizeof ( SScaleKey ) * m_cScaleKeys );
	}

	m_cMatrixKeys = rValue.m_cMatrixKeys;
	if ( m_cMatrixKeys )
	{
		m_pMatrixKeys = new SMatrixKey [ m_cMatrixKeys ];
		memcpy ( m_pMatrixKeys, rValue.m_pMatrixKeys, sizeof ( SMatrixKey ) * m_cMatrixKeys );
	}

	int StringLength = 0;
	if ( rValue.szName )
	{
		StringLength = strlen( rValue.szName ) + 1;

		if ( StringLength )
		{		
			szName = new char [ StringLength ];	
			StringCchCopy( szName, StringLength, rValue.szName );
		}
	}

	pAnimNext = NULL;
}

LPD3DXMATRIX DxAnimation::SetTime ( float fGlobalTime, DWORD dwFlag )
{
	UINT iKey;
	
	UINT dwp2 = 0;
	UINT dwp3 = 0;
	
	static D3DXMATRIX matResult;
	D3DXMATRIX matTemp;
	
	float fTime1;
	float fTime2;
	
	float fLerpValue;
	D3DXVECTOR3 vScale;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION quat;
	
	BOOL bAnimate = false;
	
	float fTime = fGlobalTime;

	if ( m_pMatrixKeys )
	{
		//	Note : f = fmod ( x, y )  : x = i*y + f ( ������ ���ϴ� �Լ� )
		//		���ϸ��̼� ����.
		//
		if ( dwFlag&DXANI_MOD )
		{
			UINT LastKey = m_cMatrixKeys - 1;
			if ( m_cMatrixKeys > 3)
			{
				DWORD dwUnitTime = m_pMatrixKeys[1].dwTime - m_pMatrixKeys[0].dwTime;
				DWORD dwLastDxTime = m_pMatrixKeys[m_cMatrixKeys-1].dwTime - m_pMatrixKeys[m_cMatrixKeys-2].dwTime;

				if ( dwUnitTime != dwLastDxTime )	LastKey -= 1;
			}

			fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pMatrixKeys[LastKey].dwTime) );
		}
		else if ( (DWORD)fGlobalTime >= m_pMatrixKeys[m_cMatrixKeys-1].dwTime )
		{
			fTime = m_pMatrixKeys[m_cMatrixKeys-1].dwTime - 1.0f;
		}

		//	Note : ���� fTime�� ���� �ð����� Ű�� ����.
		//
		for ( iKey = 0 ; iKey < m_cMatrixKeys ; iKey++ )
		{
			if ( m_pMatrixKeys[iKey].dwTime >= (DWORD)fTime )
			{
				dwp3 = iKey;

				if ( iKey > 0 )
				{
					dwp2= iKey - 1;
				}
				else  // when iKey == 0, then dwp2 == 0
				{
					dwp2 = iKey;
				}

				break;
			}
		}

		fTime1 = (float)m_pMatrixKeys[dwp2].dwTime;
		fTime2 = (float)m_pMatrixKeys[dwp3].dwTime;

        
		//	Note : �� Ű �߿� ���� ����� Ű�� ����.
		//
		if ((fTime2 - fTime1) ==0)
			fLerpValue = 0;
		else
			fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

		if (fLerpValue > 0.5)
		{
			iKey = dwp3;
		}
		else
		{
			iKey = dwp2;
		}

		//	Note : ���ϸ��̼��� ����� �����ӿ� Ű ����.
		//
		if ( pframeToAnimate )		pframeToAnimate->matRot = m_pMatrixKeys[iKey].mat;

		return &(m_pMatrixKeys[iKey].mat);
	}
	else
	{
		D3DXMatrixIdentity(&matResult);

		if (m_pScaleKeys)
		{
			dwp2 = dwp3 = 0;

			if ( dwFlag&DXANI_MOD )
				fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pScaleKeys[m_cScaleKeys-1].dwTime) );
			else if ( fGlobalTime > m_pScaleKeys[m_cScaleKeys-1].dwTime )
				fTime = m_pScaleKeys[m_cScaleKeys-1].dwTime - 1.0f;

			for (iKey = 0 ;iKey < m_cScaleKeys ; iKey++)
			{
				if ((float)m_pScaleKeys[iKey].dwTime > fTime)
				{
					dwp3 = iKey;

					if (iKey > 0)
					{
						dwp2= iKey - 1;
					}
					else  // when iKey == 0, then dwp2 == 0
					{
						dwp2 = iKey;
					}

					break;
				}
			}
			fTime1 = (float)m_pScaleKeys[dwp2].dwTime;
			fTime2 = (float)m_pScaleKeys[dwp3].dwTime;

			if ((fTime2 - fTime1) ==0)
				fLerpValue = 0;
			else
				fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

			//	Note : ���� ����.
			//
			D3DXVec3Lerp ( &vScale,
					&m_pScaleKeys[dwp2].vScale,
					&m_pScaleKeys[dwp3].vScale,
					fLerpValue );


			D3DXMatrixScaling ( &matTemp, vScale.x, vScale.y, vScale.z );

			D3DXMatrixMultiply ( &matResult, &matResult, &matTemp );

			bAnimate = true;
		}

		//check rot keys
		if ( m_pRotateKeys )
		{
			int i1 = 0;
			int i2 = 0;

			if ( dwFlag&DXANI_MOD )
				fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pRotateKeys[m_cRotateKeys-1].dwTime) );
			else if ( fGlobalTime > m_pRotateKeys[m_cRotateKeys-1].dwTime )
				fTime = m_pRotateKeys[m_cRotateKeys-1].dwTime - 1.0f;

			for (iKey = 0 ;iKey < m_cRotateKeys ; iKey++)
			{
				if ( (float)m_pRotateKeys[iKey].dwTime > fTime )
				{
					i1 = (iKey > 0) ? iKey - 1 : 0;
					i2 = iKey;
					break;
				}
			}

			fTime1 = (float)m_pRotateKeys[i1].dwTime;
			fTime2 = (float)m_pRotateKeys[i2].dwTime;

			if ((fTime2 - fTime1) ==0)
				fLerpValue = 0;
			else
				fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

#define USE_SQUAD
#ifdef USE_SQUAD
			int i0 = i1 - 1;
			int i3 = i2 + 1;

			if(i0 < 0)
				i0 += m_cRotateKeys;

			if(i3 >= (INT) m_cRotateKeys)
				i3 -= m_cRotateKeys;

			D3DXQUATERNION qA, qB, qC;
			D3DXQuaternionSquadSetup(&qA, &qB, &qC, 
				&m_pRotateKeys[i0].quatRotate, &m_pRotateKeys[i1].quatRotate, 
				&m_pRotateKeys[i2].quatRotate, &m_pRotateKeys[i3].quatRotate);

			D3DXQuaternionSquad(&quat, &m_pRotateKeys[i1].quatRotate, &qA, &qB, &qC, fLerpValue);
#else
			D3DXQuaternionSlerp(&quat, &m_pRotateKeys[i1].quatRotate, &m_pRotateKeys[i2].quatRotate, fLerpValue);
#endif
			quat.w = -quat.w;
			D3DXMatrixRotationQuaternion(&matTemp, &quat);
			D3DXMatrixMultiply(&matResult, &matResult, &matTemp);

			bAnimate = true;
		}

		if (m_pPositionKeys)
		{
			dwp2=dwp3=0;

			if ( dwFlag&DXANI_MOD )
				fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pPositionKeys[m_cPositionKeys-1].dwTime) );
			else if ( fGlobalTime > m_pPositionKeys[m_cPositionKeys-1].dwTime )
				fTime = m_pPositionKeys[m_cPositionKeys-1].dwTime - 1.0f;

			for (iKey = 0 ;iKey < m_cPositionKeys ; iKey++)
			{
				if ((float)m_pPositionKeys[iKey].dwTime > fTime)
				{
					dwp3 = iKey;

					if (iKey > 0)
					{
						dwp2= iKey - 1;
					}
					else  // when iKey == 0, then dwp2 == 0
					{
						dwp2 = iKey;
					}

					break;
				}
			}
			
			fTime1 = (float)m_pPositionKeys[dwp2].dwTime;
			fTime2 = (float)m_pPositionKeys[dwp3].dwTime;

			if ( (fTime2 - fTime1) ==0 )
				fLerpValue = 0;
			else
				fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);


			D3DXVec3Lerp ( (D3DXVECTOR3*)&vPos,
					&m_pPositionKeys[dwp2].vPos,
					&m_pPositionKeys[dwp3].vPos,
					fLerpValue );

			D3DXMatrixTranslation ( &matTemp, vPos.x, vPos.y, vPos.z );

			D3DXMatrixMultiply ( &matResult, &matResult, &matTemp );
			bAnimate = true;
		}
		else if ( pframeToAnimate )
		{
			D3DXMatrixTranslation ( &matTemp, pframeToAnimate->matRotOrig._41, pframeToAnimate->matRotOrig._42, pframeToAnimate->matRotOrig._43 );
			D3DXMatrixMultiply ( &matResult, &matResult, &matTemp );
		}

		//	Note : ���ϸ��̼��� ����� �����ӿ� Ű ����.
		//
		if ( bAnimate && pframeToAnimate )
		{
			pframeToAnimate->matRot = matResult;
		}
	}

	return &matResult;
}

DxAnimationMan::DxAnimationMan () :
	m_AniType(ANT_LOOP),
	fCurTime(0.0f),
	m_pDxFrameRoot(NULL),
	m_pDxAlphaFrameHead(NULL),
	m_fUNITTIME(FLT_MAX),
	m_pDxAnimationHead(NULL),
	m_pNext(NULL),
	m_pEffectHead(NULL)
{
	D3DXMatrixIdentity( &m_matWorld );
}

DxAnimationMan::~DxAnimationMan()
{
	SAFE_DELETE ( m_pDxFrameRoot );
	SAFE_DELETE ( m_pDxAnimationHead );
	SAFE_DELETE ( m_pEffectHead );

	SAFE_DELETE ( m_pNext );
}

void DxAnimationMan::InsertDxAnimation ( DxAnimation *pDxAnimation )
{
	pDxAnimation->pAnimNext = m_pDxAnimationHead;
	m_pDxAnimationHead = pDxAnimation;
}

void DxAnimationMan::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	//	Note : [ANIROOT] Frame�� ���ϸ��̼��� �ݿ��� ��� �ڽ� ���� ������ �����Ƿ� �״�� ��.
	//
	vMax = m_pDxFrameRoot->vTreeMax;
	vMin = m_pDxFrameRoot->vTreeMin;
}

BOOL DxAnimationMan::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3 vCenter = m_pDxFrameRoot->GetTreeCenter ();
	return COLLISION::IsWithInPoint(vDivMax,vDivMin,vCenter);
}

void DxAnimationMan::FrameMove ( float fTime, float fElapsedTime )
{
	fCurTime += fElapsedTime * UNITANIKEY_PERSEC;
	if ( fCurTime > 1.0e15f )	fCurTime = 0;
}

void DxAnimationMan::SetTime ()
{
	DxAnimation* pAnimCur = m_pDxAnimationHead;
	while ( pAnimCur != NULL )
	{
		pAnimCur->SetTime ( fCurTime );
		pAnimCur = pAnimCur->pAnimNext;
	}
}

void DxAnimationMan::UpdateFrames ( DxFrame *pframeCur, const D3DXMATRIX &matCur )
{    
	////	Note : pframeCur�� ��Ʈ������ ���. matCombined = matRot * matParent
	//				���� �𸣰����� ������ if �˻� �����ΰ� ??
	D3DXMatrixMultiply( &pframeCur->matCombined, &pframeCur->matRot, &matCur );

	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
    while( pframeChild )
    {
		UpdateFrames ( pframeChild, pframeCur->matCombined );

        pframeChild = pframeChild->pframeSibling;
    }
}

void DxAnimationMan::UpdateFrames_EDIT( DxFrame *pframeCur, const D3DXMATRIX &matCur )
{    
	//	Note : pframeCur�� ��Ʈ������ ���.
	D3DXMatrixMultiply( &pframeCur->matCombined, &pframeCur->matRotOrig, &matCur );

	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
    while( pframeChild )
    {
		UpdateFrames_EDIT( pframeChild, pframeCur->matCombined );

        pframeChild = pframeChild->pframeSibling;
    }
}

HRESULT DxAnimationMan::DrawFrames( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const D3DXMATRIX& matParent, const float fAlpha )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : pframeCur�� ��Ʈ������ ���. matCombined = matRot * matParent
	D3DXMatrixMultiply( &pframeCur->matCombined, &pframeCur->matRot, &matParent );

	//	Note : ���� Render
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pDxOctreeMesh )
			{
				pmsMeshs->RenderOctree( pd3dDevice, fAlpha );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFrames ( pd3dDevice, pframeChild, pframeCur->matCombined, fAlpha );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxAnimationMan::DrawFramesAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const float fAlpha )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ���� Render
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pDxOctreeMesh )
			{
				pmsMeshs->RenderOctreeAlphaMap( pd3dDevice, FALSE, fAlpha );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFramesAlpha ( pd3dDevice, pframeChild, fAlpha );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxAnimationMan::DrawFramesSTOP( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const D3DXMATRIX& matParent, const float fAlpha )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : pframeCur�� ��Ʈ������ ���. matCombined = matRot * matParent
	D3DXMatrixMultiply( &pframeCur->matCombined, &pframeCur->matRotOrig, &matParent );

	//	Note : ���� Render
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pDxOctreeMesh )
			{
				pmsMeshs->RenderOctree( pd3dDevice, fAlpha );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFramesSTOP( pd3dDevice, pframeChild, pframeCur->matCombined, fAlpha );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxAnimationMan::DrawFramesAlphaSTOP( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const float fAlpha )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ���� Render
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pDxOctreeMesh )
			{
				pmsMeshs->RenderOctreeAlphaMap( pd3dDevice, FALSE, fAlpha );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFramesAlphaSTOP( pd3dDevice, pframeChild, fAlpha );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxAnimationMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT hr;

	//	Note : ���ϸ��̼� ��Ʈ���� ����.
	SetTime ();

	//	Note : �޽� �׸��� ����.
	hr = DrawFrames( pd3dDevice, m_pDxFrameRoot, m_matWorld, 1.f );
	if (FAILED(hr))		return hr;

	//	Note : �޽� �׸��� ����.
	//
	hr = DrawFramesAlpha( pd3dDevice, m_pDxFrameRoot, 1.f );

	if (FAILED(hr))		return hr;

	return S_OK;
}

void DxAnimationMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha )
{
	//	Note : ���ϸ��̼� ��Ʈ���� ����.
	SetTime ();

	//	Note : �޽� �׸��� ����.
	DrawFrames( pd3dDevice, m_pDxFrameRoot, m_matWorld, fAlpha );

	//	Note : �޽� �׸��� ����.
	DrawFramesAlpha( pd3dDevice, m_pDxFrameRoot, fAlpha );
}

void DxAnimationMan::Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha )
{
	//	Note : �޽� �׸��� ����.
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	DrawFramesSTOP( pd3dDevice, m_pDxFrameRoot, matIdentity, fAlpha );

	//	Note : �޽� �׸��� ����.
	DrawFramesAlphaSTOP( pd3dDevice, m_pDxFrameRoot, fAlpha );
}

DxFrame* DxAnimationMan::FindFrame( const char* szName )
{
	DxFrame* pRoot = NULL;
	DxFrame* pFrame = NULL;
	DxAnimationMan* pCur = this;
	while( pCur )
	{
		pRoot = pCur->GetDxFrameRoot();
		if( pRoot )
		{
			pFrame = pRoot->FindFrame( szName );
			if( pFrame )	return pFrame;
		}
		pCur = pCur->m_pNext;
	}

	return NULL;
}

void DxAnimationMan::UpdateFrames( const D3DXMATRIX &matCur )
{
	SetTime();
	UpdateFrames( m_pDxFrameRoot, matCur );
}

void DxAnimationMan::UpdateFrames_EDIT( const D3DXMATRIX &matCur )
{
	SetTime();
	UpdateFrames_EDIT( m_pDxFrameRoot, matCur );
}