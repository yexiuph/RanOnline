#include "pch.h"
#include "actor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Actor::Actor() :
	m_Parent(NULL),
	m_CurrentCellID(UINT_MAX),

	m_Position(0.0f,0.0f,0.0f),
	m_Movement(0.0f,0.0f,0.0f),
	m_NextPosition(D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)),
	m_MaxSpeed(5.0f),

	m_CorrectY(0.0f),

	m_PathActive(false),

	m_bFirstCalc(FALSE),
	m_bFirstWayPointPass(FALSE),
	m_fFirstPathDist(0.0f),

	m_fMovedDist(0.0f),
	m_fMovedTime(0.0f)
{
}

Actor::~Actor()
{
}

void Actor::Release ()
{
	m_Parent = NULL;
	m_CurrentCellID = UINT_MAX;

	m_Position = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_Movement = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_NextPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_MaxSpeed = 5.0f;
	
	m_CorrectY = 0.0f;

	m_PathActive = false;
	m_Path.Clear();
	m_NextWaypoint;
	
	m_bFirstCalc = FALSE;
	m_bFirstWayPointPass = FALSE;
	m_fFirstPathDist = 0.0f;

	m_fMovedDist = 0.0f;
	m_fMovedTime = 0.0f;
}

void Actor::Create(NavigationMesh* Parent, const D3DXVECTOR3& Position, DWORD CurrentCellID)
{
	m_Parent = Parent;
	m_Position = Position;
	m_CurrentCellID = CurrentCellID;

	m_Movement.x=m_Movement.y=m_Movement.z=0.0f;

	if (m_Parent)
	{
		BOOL bCollision = FALSE;

		D3DXVECTOR3		vDest;
		DWORD			CollisionID;

		D3DXVECTOR3 vFromPt = m_Position + D3DXVECTOR3(0,5,0);
		D3DXVECTOR3 vTargetPt = m_Position + D3DXVECTOR3(0,-5,0);
		m_Parent->IsCollision ( vFromPt, vTargetPt, vDest, CollisionID, bCollision );

        if ( bCollision )	// m_CurrentCellID이 -1인것인지 체크하는 것과 동일한 수식이다.
		{
			m_CurrentCellID = CollisionID;
			m_Position = vDest;
		}
		else
		{
			m_CurrentCellID = m_Parent->FindClosestCell(m_Position);			
			m_Position =  m_Parent->SnapPointToCell(m_CurrentCellID, m_Position);
		}
	}
	//m_Parent = Parent;
	//m_Position = Position;
	//m_CurrentCellID = CurrentCellID;

	//m_Movement.x=m_Movement.y=m_Movement.z=0.0f;

	//if (m_Parent)
	//{
	//	//	-1은 존재하지 않는 셀... 초기화할때 사용한다.		
	//	if (m_CurrentCellID == -1)
	//	{
	//		m_CurrentCellID = m_Parent->FindClosestCell(m_Position);			 
	//	}
	//	
	//	m_Position =  m_Parent->SnapPointToCell(m_CurrentCellID, m_Position);
	//}
}

void Actor::SetPosition ( const D3DXVECTOR3& Position, DWORD CurrentCellID )
{
	m_Position = Position;
	m_CurrentCellID = CurrentCellID;

	m_Movement.x=m_Movement.y=m_Movement.z=0.0f;

	if (m_Parent)
	{
		BOOL bCollision = FALSE;

		D3DXVECTOR3		vDest;
		DWORD			CollisionID;

		D3DXVECTOR3 vFromPt = m_Position + D3DXVECTOR3(0,5,0);
		D3DXVECTOR3	vTargetPt = m_Position + D3DXVECTOR3(0,-5,0);
		m_Parent->IsCollision ( vFromPt, vTargetPt, vDest, CollisionID, bCollision );

        if ( bCollision )	// m_CurrentCellID이 -1인것인지 체크하는 것과 동일한 수식이다.
		{
			m_CurrentCellID = CollisionID;
			m_Position = vDest;
		}
		else
		{
			m_CurrentCellID = m_Parent->FindClosestCell(m_Position);			
			m_Position =  m_Parent->SnapPointToCell(m_CurrentCellID, m_Position);
		}
	}

	//if (m_Parent)
	//{
	//	//	-1은 존재하지 않는 셀... 초기화할때 사용한다.		
	//	if (m_CurrentCellID == -1)
	//	{
	//		m_CurrentCellID = m_Parent->FindClosestCell(m_Position);			 
	//	}
	//	
	//	m_Position =  m_Parent->SnapPointToCell(m_CurrentCellID, m_Position);
	//}
}

HRESULT Actor::Render(LPDIRECT3DDEVICEQ pd3dDevice)
{
	const	DWORD ACTVERTEXFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	struct ACTVERTEX
	{
		union
		{
			struct { D3DXVECTOR3 vPos; };
			struct { float x, y, z; };
		};

		DWORD		Diffuse;
		
		ACTVERTEX ( float _x, float _y, float _z, DWORD diffuse )
		{
			x = _x; y = _y; z = _z; Diffuse = diffuse;
		}
	};
/*
//	<--	디버그용 스파이크 설치
    const			int	nSpikeVertex = 7;
	ACTVERTEX		SpikeVB[nSpikeVertex] = 
	{
		ACTVERTEX ( 5.0000f, 10.0000f, 0.0000f, D3DCOLOR_XRGB(255,0,0) ),
		ACTVERTEX ( 2.5000f, 10.0000f, 4.3301f, D3DCOLOR_XRGB(255,0,0) ),
		ACTVERTEX ( -2.5000f, 10.0000f, 4.3301f, D3DCOLOR_XRGB(255,0,0) ),
		ACTVERTEX ( -5.0000f, 10.0000f, 0.0000f, D3DCOLOR_XRGB(255,0,0) ),
		ACTVERTEX ( -2.5000f, 10.0000f, -4.3301f, D3DCOLOR_XRGB(255,0,0) ),
		ACTVERTEX ( 2.5000f, 10.0000f, -4.3301f, D3DCOLOR_XRGB(255,0,0) ),
		ACTVERTEX ( 0.0000f, 0.0000f, 0.0000f, D3DCOLOR_XRGB(255,0,0) ),
	};

	const	int	nSpikeIndex = 30;
	WORD SpikeIB[nSpikeIndex] = 
	{
		5, 4, 3,
		0, 5, 3,
		0, 3, 2,
		0, 2, 1,
		6, 0, 1,
		6, 1, 2,
		6, 2, 3,
		6, 3, 4,
		6, 4, 5,
		6, 5, 0
	};

	D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );		
	D3DXMatrixTranslation(&matWorld, m_Position.x, m_Position.y, m_Position.z );	
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		
	pd3dDevice->SetFVF( ACTVERTEXFVF );
	pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, nSpikeVertex, nSpikeIndex / 3,
		SpikeIB, D3DFMT_INDEX16, SpikeVB, D3DXGetFVFVertexSize(ACTVERTEXFVF) );	
//	-->	디버그용 스파이크 설치
*/

	//
	// render the waypoint path if active
	//
	if (m_PathActive)
	{
		DWORD dwLight;
		pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwLight);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE );

		DWORD dwColorOP, dwColorARG2;
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2,	&dwColorARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ); 

		//	<--	네비게이션 선을 화면에 표시하기 위한 기본 정보
		//		*IB, VB, Type을 아래로 연속 두번 쓰기 때문에
		//		두 번 정의하는 것 보다, 공유해서 쓰는 것이
		//		더 좋을 것 같아서 이렇게 쓴다.*
		const			int	nLineVertex = 2;
		DIFFUSEVERTEX	LineVB[nLineVertex] = 
		{
			DIFFUSEVERTEX( 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0,0,0) ),
			DIFFUSEVERTEX( 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0,0,0) )
		};

		const	int	nLineIndex = 2;
		WORD		LineIB[nLineIndex] = 
		{
			0,
			1
		};
		//	-->	네비게이션 선을 화면에 표시하기 위한 기본 정보


		// connect the waypoins to draw lines in blue
		NavigationPath::WAYPOINT_LIST::const_iterator iter = m_Path.WaypointList().begin();
		if (iter != m_Path.WaypointList().end())
		{
			NavigationPath::WAYPOINT LastWaypoint = *iter;
			++iter;
			for (;iter != m_Path.WaypointList().end();++iter)
			{
				const NavigationPath::WAYPOINT& waypoint= *iter;

				LineVB[0].x = LastWaypoint.Position.x;
				LineVB[0].y = LastWaypoint.Position.y + 1.0f;
				LineVB[0].z = LastWaypoint.Position.z;
				LineVB[0].Diffuse = D3DCOLOR_XRGB(0,255,255);


				LineVB[1].x = waypoint.Position.x;
				LineVB[1].y = waypoint.Position.y + 1.0f;
				LineVB[1].z = waypoint.Position.z;
				LineVB[1].Diffuse = D3DCOLOR_XRGB(0,255,255);

				pd3dDevice->SetFVF( DIFFUSEVERTEX::FVF );
				pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, nLineIndex, nLineIndex/2,
					LineIB, D3DFMT_INDEX16, LineVB, D3DXGetFVFVertexSize(DIFFUSEVERTEX::FVF) );
 
				LastWaypoint = waypoint;
			}
		}

		const NavigationPath::WAYPOINT& waypoint= *m_NextWaypoint;

		//	<--	선 정보
		//		*인덱스는 변하지 않기 때문에 그대로 쓰고,
		//		버텍스 정보만 치환한다.*
		LineVB[0].x = m_Position.x;
		LineVB[0].y = m_Position.y + 0.1f;
		LineVB[0].z = m_Position.z;
		LineVB[0].Diffuse = D3DCOLOR_XRGB(255,0,0);

		LineVB[1].x = waypoint.Position.x;
		LineVB[1].y = waypoint.Position.y + 0.1f;
		LineVB[1].z = waypoint.Position.z;
		LineVB[1].Diffuse = D3DCOLOR_XRGB(255,0,0);
		//	-->	선 정보

		pd3dDevice->SetFVF( DIFFUSEVERTEX::FVF );
		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, nLineIndex, nLineIndex/2,
			LineIB, D3DFMT_INDEX16, LineVB, D3DXGetFVFVertexSize(DIFFUSEVERTEX::FVF) );

		pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	dwColorARG2 );
	}

	return S_OK;
}

HRESULT Actor::Update(float elapsedTime)
{
	if (!m_Parent)		return E_FAIL;
	if (!m_PathActive)	return S_OK;

	//	이동 명령이후, 경과 시간
	m_fMovedTime += elapsedTime;

	//	Note	:	경로를 따라 이동
	//
	if (m_NextWaypoint != m_Path.WaypointList().end())
	{		
		//	크기가 있는 방향벡터를 얻어냄		
		m_Movement = (*m_NextWaypoint).Position;
		m_Movement -= m_Position;
	}
	else
	{
		//	끝에 도착함
		m_PathActive = false;
		m_Movement.x=m_Movement.y=m_Movement.z=0.0f;
	}

	//	Note	:	방향벡터의 길이가 시간당 움직일 수 있는 최대
	//				길이보다 길 경우 그것을 제한함
	float max_distance = m_MaxSpeed * elapsedTime;
	float distance = D3DXVec3Length ( &m_Movement );
	if ( distance > max_distance )
	{
		D3DXVec3Normalize ( &m_Movement, &m_Movement ); 		
		m_Movement *= max_distance;		
	}

	//	이동 명령이후, 경과 거리
	//	평면 이동만 고려 ( y를 0로 세팅 )
	D3DXVECTOR3 vMoveDist ( m_Movement.x, 0.0f, m_Movement.z );
	float fMoveDist = D3DXVec3Length ( &m_Movement );	
	m_fMovedDist += fMoveDist;

	//	Note	:	달달달~ 떠는 것을 방지함
	if (fabs(m_Movement.x) < 0.001f) m_Movement.x = 0.0f;
	if (fabs(m_Movement.y) < 0.001f) m_Movement.y = 0.0f;
	if (fabs(m_Movement.z) < 0.001f) m_Movement.z = 0.0f;

	//	Note	:	하나의 구간내에서 아직 움직여야 하는 곳이 있다면 처리한다.
	if ( distance > 0.01f )
	{
		D3DXVECTOR3 NextPosition = m_Position + m_Movement;	//	다음지점 계산		

		NavigationCell* NextCell;			
		DWORD	NextCellID;

		m_Parent->ResolveMotionOnMesh(m_Position, m_CurrentCellID, NextPosition, &NextCellID);

		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
		//
		NextCell = m_Parent->GetCell ( NextCellID );
		if ( !NextCell )	return E_FAIL;

		if ( NextCell->Normal().y <= 0.0001f )
		{
			NextPosition.y = m_CorrectY;
		}
		else
		{
			m_CorrectY = NextPosition.y;
		}

		m_Position = NextPosition;
		m_CurrentCellID = NextCellID;//NextCell->CellID();	
	}
	else
	{		
		m_Position = (*m_NextWaypoint).Position;    
		DWORD	CellID = (*m_NextWaypoint).CellID;
		NavigationCell *pCell = m_Parent->GetCell ( CellID );
		if ( !pCell )	return E_FAIL;

		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
		//
		if ( pCell->Normal().y <= 0.0001f )
		{
            m_Position.y = m_CorrectY;
		}
		else
		{
			m_CorrectY = m_Position.y;
		}

		distance = 0.0f;
		m_Movement.x = m_Movement.y = m_Movement.z = 0.0f;

		m_NextWaypoint= m_Path.GetFurthestVisibleWayPoint(m_NextWaypoint);	

		if (m_NextWaypoint == m_Path.WaypointList().end())
		{
			m_PathActive = false;
			m_Movement.x=m_Movement.y=m_Movement.z=0.0f;
			m_NextPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		}
		else
		{
			m_NextPosition = (*m_NextWaypoint).Position;
			Update ( elapsedTime );
		}
		
		if ( m_bFirstCalc )
		{
			m_bFirstWayPointPass = TRUE;
		}
		m_bFirstCalc = TRUE;
	}

	return S_OK;
}

void Actor::GotoLocation(const D3DXVECTOR3& Position, DWORD CellID)
{
	if (m_Parent)
	{
		ResetMovedData ();

		m_Movement.x = m_Movement.y=m_Movement.z = 0.0f;
		m_fFirstPathDist = 0.0f;

		// Appended Code [Author:서광수 2006.12.06] Begin ====>
		DWORD cellTest = m_CurrentCellID;
		NavigationCell* StartCell =	m_Parent->GetCell(m_CurrentCellID);
		if ( !StartCell ) return;
		if ( !StartCell->IsPointInCellCollumn(m_Position) )
		{
			// CDebugSet::ToListView ( "Modified big bug!" );
			m_CurrentCellID = m_Parent->FindClosestCell ( m_Position );
		}

		if ( m_Parent->LineOfSightTest ( m_CurrentCellID, m_Position, CellID, Position ) )
		{
			m_Path.Setup(m_Parent, m_Position, m_CurrentCellID, Position, CellID);

			m_PathActive = true;
			m_Path.AddWayPoint(m_Position, m_CurrentCellID);
			m_Path.AddWayPoint(Position, CellID);

			m_NextWaypoint=m_Path.WaypointList().begin();

			m_bFirstCalc = FALSE;
			m_bFirstWayPointPass = FALSE;

			D3DXVECTOR3	vMovePosition ( Position.x - m_Position.x, 0.0f, Position.z - m_Position.z );
			m_fFirstPathDist = D3DXVec3Length ( &vMovePosition );
			return;
		}
		// Appended Code [Author:서광수 2006.12.06] <==== End

		m_PathActive=m_Parent->BuildNavigationPath(m_Path,
			m_CurrentCellID, m_Position,  CellID, Position );

		if(m_PathActive)
		{
			m_NextWaypoint=m_Path.WaypointList().begin();
			m_NextPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

			m_bFirstCalc = FALSE;
			m_bFirstWayPointPass = FALSE;
			//	첫번째 구간까지의 거리
			//
			NavigationPath::WayPointID& NextWaypoint = m_Path.GetFurthestVisibleWayPoint(m_NextWaypoint);
			D3DXVECTOR3 vNextPosition = (*NextWaypoint).Position;
			D3DXVECTOR3	vMovePosition ( vNextPosition.x - m_Position.x, 0.0f, vNextPosition.z - m_Position.z );
			m_fFirstPathDist = D3DXVec3Length ( &vMovePosition );			
		}
	}
}

BOOL Actor::GotoLocation ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2 )
{
	if ( !m_Parent )	return FALSE;

	BOOL bCollision = FALSE;

	D3DXVECTOR3		vDest;
	DWORD			CollisionID;

	D3DXVECTOR3 vFromPt = vP1;
	D3DXVECTOR3	vTargetPt = vP2;	
	m_Parent->IsCollision ( vFromPt, vTargetPt, vDest, CollisionID, bCollision );
	
	if ( bCollision )
	{		
		GotoLocation ( vDest, CollisionID );
	}

	return bCollision;
}

void	Actor::MovedMidWayCheck ()
{
	m_fFirstPathDist = m_fFirstPathDist - m_fMovedDist;
	if ( m_fFirstPathDist < 0.0f )		m_bFirstWayPointPass = TRUE;

	ResetMovedData ();
}

D3DXVECTOR3 Actor::GetTargetPosition ()
{
	const NavigationPath::WAYPOINT_LIST &listWAY = m_Path.WaypointList();
	if ( listWAY.empty() )		return D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	return (*listWAY.rbegin()).Position;
}

void Actor::SetNextWayPoint ( NavigationPath::WAYPOINT WayPoint )
{
	NavigationPath::WAYPOINT_LIST::const_iterator iter = m_Path.WaypointList().begin();
	for (;iter != m_Path.WaypointList().end();++iter)
	{
		const NavigationPath::WAYPOINT& waypoint= *iter;
		if ( waypoint.CellID == WayPoint.CellID ) 
		{
			m_NextWaypoint = iter;
			m_NextPosition = (*m_NextWaypoint).Position;
			break;
		}
	}
}