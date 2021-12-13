#include "pch.h"
#include "./GLSummonField.h"
#include "./GLARoundSlot.h"
#include "./GLGaeaServer.h"
#include "./GLChar.h"
#include "./GLCrow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLARoundSlot::GLARoundSlot(void) :
	m_pThisChar(NULL),
	m_pThisSummon(NULL),
	m_dwAmount(0),
	m_bUseSummon(FALSE)
{
	for ( int i=0; i<SLOT_SIZE; i++ )	m_ARoundSlot[i] = EMTARGET_NULL;
}

GLARoundSlot::~GLARoundSlot(void)
{
}

void GLARoundSlot::Reset()
{
	m_dwAmount = 0;
	for ( int i=0; i<SLOT_SIZE; i++ )	m_ARoundSlot[i] = EMTARGET_NULL;
}

void GLARoundSlot::SetChar ( PGLCHAR pThisChar )
{
	SetUseSummon( FALSE );
	m_pThisChar = pThisChar;	
}

void GLARoundSlot::SetSummon ( PGLSUMMONFIELD pThisSummon )
{
	SetUseSummon( TRUE );
	m_pThisSummon = pThisSummon;
}

D3DXVECTOR3 GLARoundSlot::CalculateTracePos ( const int nDegree, const D3DXVECTOR3 vThisPos,
											 const float fRange )
{
	D3DXVECTOR3 vTracePos;
	float fThetaY = (D3DX_PI*2.0f) / 8.0f * nDegree;
	
	D3DXMATRIX matYRot;
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	D3DXVECTOR3 vDir = vDIRORIG;
	D3DXVec3TransformCoord ( &vDir, &vDir, &matYRot );

	vTracePos = vThisPos - vDir*fRange;
	return vTracePos;
}

void GLARoundSlot::SetTargetOptSlot (  const DWORD dwAttID, const DWORD dwSlot  )
{
	if ( dwSlot>=SLOT_SIZE )	return;
	m_ARoundSlot[dwSlot] = dwAttID;
}

void GLARoundSlot::ResetTargetOptSlot ( const DWORD dwAttID, const DWORD dwSlot )
{
	if ( dwSlot>=SLOT_SIZE )	return;

	if ( m_ARoundSlot[dwSlot]==dwAttID )
	{
		m_ARoundSlot[dwSlot] = EMTARGET_NULL;
		return;
	}

	for ( DWORD i=0; i<SLOT_SIZE; i++ )
	{
		if ( m_ARoundSlot[i]==dwAttID )	m_ARoundSlot[i] = EMTARGET_NULL;
	}
}

BOOL GLARoundSlot::ValidPosition ( NavigationMesh* pNaviMesh,
								  const D3DXVECTOR3 &vTarPos, const DWORD dwTarCell,
								  const D3DXVECTOR3 &vTracePos )
{
	//	유효한 위치인지 점검.
	BOOL bCollision=FALSE;
	DWORD dwCollisionID;
	D3DXVECTOR3 vCollision;
	D3DXVECTOR3 vPos1 = vTracePos; vPos1.y += 5.0f;
	D3DXVECTOR3 vPos2 = vTracePos; vPos2.y -= 5.0f;

	pNaviMesh->IsCollision ( vPos1, vPos2, vCollision, dwCollisionID, bCollision );
	if ( !bCollision )	return FALSE;

	bCollision = pNaviMesh->LineOfSightTest ( dwCollisionID, vCollision, dwTarCell, vTarPos );
	return bCollision;
}

DWORD GLARoundSlot::TestOptimumSlot ( PGLCROW pCrowAttackor, float fRange, NavigationMesh* pNaviMesh, D3DXVECTOR3 &vTracePos )
{
	DWORD dwTarCell = 0;
	D3DXVECTOR3 vTarPos;
	if( m_bUseSummon == FALSE )
	{
		dwTarCell = m_pThisChar->m_actorMove.CurrentCellID();
		vTarPos   = m_pThisChar->GetPosition();
	}else{
		dwTarCell = m_pThisSummon->m_actorMove.CurrentCellID();
		vTarPos   = m_pThisSummon->GetPosition();
	}



	const D3DXVECTOR3 &vAttPos = pCrowAttackor->GetPosition();

	D3DXVECTOR3 vDir = vAttPos - vTarPos;
	float fThetaY = DXGetThetaYFromDirection ( vDir, vDIRORIG ) + D3DX_PI;	// 0 ~ 2PI 각도로.
	int nDegree = int ( fThetaY * SLOT_SIZE / (D3DX_PI*2.0f) );
	if ( nDegree<0 )			nDegree = 0;
	if ( nDegree >= SLOT_SIZE )	nDegree = SLOT_SIZE-1;

	if ( (m_ARoundSlot[nDegree]==EMTARGET_NULL) || (m_ARoundSlot[nDegree]==pCrowAttackor->m_dwGlobID) )
	{
		vTracePos = CalculateTracePos ( nDegree, vTarPos, fRange );
		return nDegree;
	}

	int nDeg = 0;
	for ( int i=0; i<SLOT_SIZE; ++i )
	{
		nDeg = nDegree+i;
		if ( nDeg<SLOT_SIZE )
		{
			if ( (m_ARoundSlot[nDeg]==EMTARGET_NULL) || (m_ARoundSlot[nDeg]==pCrowAttackor->m_dwGlobID) )
			{
				vTracePos = CalculateTracePos ( nDeg, vTarPos, fRange );
				return nDeg;
			}
		}

		nDeg = nDegree-i;
		if ( nDeg<SLOT_SIZE )
		{
			if ( (m_ARoundSlot[nDeg]==EMTARGET_NULL) || (m_ARoundSlot[nDeg]==pCrowAttackor->m_dwGlobID) )
			{
				vTracePos = CalculateTracePos ( nDeg, vTarPos, fRange );
				return nDeg;
			}
		}
	}

	//	Note : 슬롯이 없을 경우.
	return SLOT_SIZE;
}

BOOL GLARoundSlot::ValidOptimumSlot ( NavigationMesh* pNaviMesh, const D3DXVECTOR3 &vTracePos )
{
	DWORD dwTarCell = 0;
	D3DXVECTOR3 vTarPos;
	if( m_bUseSummon == FALSE )
	{
		dwTarCell = m_pThisChar->m_actorMove.CurrentCellID();
		vTarPos   = m_pThisChar->GetPosition();
	}else{
		dwTarCell = m_pThisSummon->m_actorMove.CurrentCellID();
		vTarPos   = m_pThisSummon->GetPosition();
	}


	return ValidPosition ( pNaviMesh, vTarPos, dwTarCell, vTracePos );
}

DWORD GLARoundSlot::ReqOptimumSlot ( PGLCROW pCrowAttackor, float fRange, NavigationMesh* pNaviMesh,
									D3DXVECTOR3 &vTracePos )
{
	DWORD dwTarCell = 0;
	D3DXVECTOR3 vTarPos;
	if( m_bUseSummon == FALSE )
	{
		dwTarCell = m_pThisChar->m_actorMove.CurrentCellID();
		vTarPos   = m_pThisChar->GetPosition();
	}else{
		dwTarCell = m_pThisSummon->m_actorMove.CurrentCellID();
		vTarPos   = m_pThisSummon->GetPosition();
	}

	const D3DXVECTOR3 &vAttPos = pCrowAttackor->GetPosition();

	D3DXVECTOR3 vDir = vAttPos - vTarPos;
	float fThetaY = DXGetThetaYFromDirection ( vDir, vDIRORIG ) + D3DX_PI;	// 0 ~ 2PI 각도로.
	int nDegree = int ( fThetaY * SLOT_SIZE / (D3DX_PI*2.0f) );
	if ( nDegree<0 )			nDegree = 0;
	if ( nDegree >= SLOT_SIZE )	nDegree = SLOT_SIZE-1;

	if ( m_ARoundSlot[nDegree] == EMTARGET_NULL )
	{
		vTracePos = CalculateTracePos ( nDegree, vTarPos, fRange );
		if ( ValidPosition ( pNaviMesh, vTarPos, dwTarCell, vTracePos ) )
		{
			m_dwAmount++;
			m_ARoundSlot[nDegree] = pCrowAttackor->m_dwGlobID;
			return nDegree;
		}
	}

	int nDeg = 0;
	for ( int i=0; i<SLOT_SIZE; i++ )
	{
		nDeg = nDegree+i;
		if ( (nDeg<SLOT_SIZE) && (m_ARoundSlot[nDeg]==EMTARGET_NULL) )
		{
			vTracePos = CalculateTracePos ( nDeg, vTarPos, fRange );
			if ( ValidPosition ( pNaviMesh, vTarPos, dwTarCell, vTracePos ) )
			{
				m_dwAmount++;
				m_ARoundSlot[nDeg] = pCrowAttackor->m_dwGlobID;
				return nDeg;
			}
		}
		
		nDeg = nDegree-i;
		if ( (nDeg>=0)  && (m_ARoundSlot[nDeg]==EMTARGET_NULL) )
		{
			vTracePos = CalculateTracePos ( nDeg, vTarPos, fRange );
			if ( ValidPosition ( pNaviMesh, vTarPos, dwTarCell, vTracePos ) )
			{
				m_dwAmount++;
				m_ARoundSlot[nDeg] = pCrowAttackor->m_dwGlobID;
				return nDeg;
			}
		}
	}

	//	Note : 슬롯이 없을 경우.
	return SLOT_SIZE;
}

void GLARoundSlot::Update ( GLLandMan* pLandMan )
{
	//	Note : 자신을 타겟으로 한 근접 유닛이 유효한지 검사. 그 유닛이 자신을 타겟으로한지 검사.
	//
	m_dwAmount = 0;
	DWORD dwTargetID = 0;
	if( m_bUseSummon == FALSE )
	{
		dwTargetID = m_pThisChar->m_dwGaeaID;
	}else{
		dwTargetID = m_pThisSummon->m_dwGUID;
	}

	for ( int i=0; i<SLOT_SIZE; i++ )
	{
		DWORD &dwSlot = m_ARoundSlot[i];

		if ( dwSlot != EMTARGET_NULL )
		{
			PGLCROW pCrow = pLandMan->GetCrow ( dwSlot );
			if ( !pCrow )
			{
				dwSlot = EMTARGET_NULL;
			}
			else
			{
				const STARGETID &sTarID = pCrow->GetTargetID();
			
				if ( dwTargetID!=sTarID.dwID || !pCrow->IsFLAG(GLCrow::EMTARGET) )
				{
					dwSlot = EMTARGET_NULL;
				}
				else
				{
					m_dwAmount++;
				}
			}
		}
	}
}





