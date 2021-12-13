#include "pch.h"
#include "./GLMoney.h"
#include "./GLItemMan.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/DxTools/Collision.h"
#include "../[Lib]__Engine/Sources/DxTools/DxCustomTypes.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSimpleMeshMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CMoneyDrop::IsTakeItem ( DWORD dwPartyID, DWORD dwGaeaID )
{
	bool bhold(false);

	switch ( emGroup )
	{
	case EMGROUP_ONE:
		if ( dwHoldGID==GAEAID_NULL )		bhold = true;
		else if ( dwHoldGID==dwGaeaID )		bhold = true;
		break;

	case EMGROUP_PARTY:
		if ( dwHoldGID==PARTY_NULL )		bhold = true;
		else if ( dwHoldGID==dwPartyID )	bhold = true;
		break;

	case EMGROUP_GUILD:
		break;
	};

	return bhold;
}

bool CMoneyDrop::IsDropOut()
{
	return ( GLCONST_CHAR::wMAXITEM_AGE < fAge );
}

void CMoneyDrop::Update ( float fElapsedTime )
{
	fAge += fElapsedTime;

	//	Note : 임시 소유자 유효성 검사.
	//
	if ( dwHoldGID!=GAEAID_NULL )
	{
		if ( GLCONST_CHAR::wMAXITEM_HOLD < fAge )
		{
			switch ( emGroup )
			{
			case EMGROUP_ONE:
				dwHoldGID = GAEAID_NULL;
				break;

			case EMGROUP_PARTY:
				dwHoldGID = PARTY_NULL;
				break;

			case EMGROUP_GUILD:
				break;
			};
		}
	}	
}

bool CMoneyClientDrop::IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const
{
	DxSimMesh* pMoneyMesh = NULL;
	if ( lnAmount>200 )			pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 2 );
	else if ( lnAmount>60 )		pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 1 );
	else						pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 0 );
	if ( !pMoneyMesh )	return false;

	D3DXVECTOR3 vMax, vMin;
	pMoneyMesh->GetBoundBox ( vMax, vMin );
	COLLISION::MINDETECTAABB ( vMax, vMin, 1.1f, 4.0f );

	COLLISION::TransformAABB ( vMax, vMin, matWld );

	BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, vMax, vMin );
	return FALSE != bCol;
}

bool CMoneyClientDrop::IsCollision ( CLIPVOLUME &cv ) const
{
	DxSimMesh* pMoneyMesh = NULL;
	if ( lnAmount>200 )			pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 2 );
	else if ( lnAmount>60 )		pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 1 );
	else						pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 0 );
	if ( !pMoneyMesh )	return false;

	D3DXVECTOR3 vMax, vMin;
	pMoneyMesh->GetBoundBox ( vMax, vMin );
	COLLISION::MINDETECTAABB ( vMax, vMin, 1.1f, 4.0f );

	COLLISION::TransformAABB ( vMax, vMin, matWld );

	BOOL bCol = COLLISION::IsCollisionVolume ( cv, vMax, vMin );
	return FALSE != bCol;
}

HRESULT CMoneyClientDrop::RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials )
{
	if ( pMoneyMesh )
	{
		pMoneyMesh->RenderItem ( pd3dDevice, matWld, pMaterials );
	}

	return S_OK;
}
