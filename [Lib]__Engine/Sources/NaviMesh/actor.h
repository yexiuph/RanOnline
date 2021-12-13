#ifndef Actor_H
#define Actor_H

#include "./DxCustomTypes.h"
#include "NavigationCell.h"
#include "NavigationPath.h"
#include "NavigationMesh.h"

class Actor
{
private:
	typedef NavigationPath::WayPointID WAYPT_ID;

protected:
	NavigationMesh*		m_Parent;				//	the mesh we are sitting on
	DWORD				m_CurrentCellID;
	D3DXVECTOR3			m_Position;				//	our 3D control point position
	D3DXVECTOR3			m_Movement;				//	the current movement vector
	D3DXVECTOR3			m_NextPosition;			//	케릭터의 방향 설정을 위해서만 쓰인다. 무효한 값 = (FLT_MAX,FLT_MAX,FLT_MAX)
	float				m_MaxSpeed;				//	our maximum traveling distance per frame
	
	//	노멀 Y가 0인 경우, 가장 최근에 만져본 셀의 것을 기억해둬야 튀는것을 방지할수 있다.
	//	이것을 함수에 static으로 박을수도 있지만, 그렇게 할 경우
	//	여러개의 인스턴스에서 호출이 되어 값이 꼬여버릴 가능성이 있기때문에....
	//	이렇게 멤버로 쓰기로 했다.
	float				m_CorrectY;

	bool				m_PathActive;			// true when we are using a path to navigate
	NavigationPath		m_Path;					// our path object	
	WAYPT_ID			m_NextWaypoint;			// ID of the next waypoint we will move to

private:
	BOOL	m_bFirstCalc;
	BOOL	m_bFirstWayPointPass;
	float	m_fFirstPathDist;

	float	m_fMovedDist;
	float	m_fMovedTime;

public:
	void Create ( NavigationMesh* Parent, const D3DXVECTOR3& Position, DWORD CurrentCellID );
	void SetPosition ( const D3DXVECTOR3& Position, DWORD CurrentCellID );

	void Release ();

public:
	HRESULT Update ( float elapsedTime = 1.0f );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void GotoLocation ( const D3DXVECTOR3& Position, DWORD CellID );

public:
	void Stop ();
	BOOL GotoLocation ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2 );

public:
	float	GetFirstPathDist ()				{ return m_fFirstPathDist; }

	float	GetMovedDist()					{ return m_fMovedDist; }
	float	GetMovedTime()					{ return m_fMovedTime; }

	void	ResetMovedData ()				{ m_fMovedDist = m_fMovedTime = 0.0f; }
	BOOL	IsFirstWayPointPass ()			{ return m_bFirstWayPointPass; }

	void	MovedMidWayCheck ();

public:
	void AddMovement(const D3DXVECTOR3& Movement);
	void SetMovement(const D3DXVECTOR3& Movement);
	void SetMovementX(float X);
	void SetMovementY(float Y);
	void SetMovementZ(float Z);
	
public:
	void SetMaxSpeed(float speed);
	float GetMaxSpeed () const { return m_MaxSpeed; }

public:
	bool PathIsActive()const;
	const D3DXVECTOR3& Position()const;
	const D3DXVECTOR3& Movement()const;
	const D3DXVECTOR3& NextPosition()const { return m_NextPosition; }
	DWORD CurrentCellID()const;	
	NavigationMesh*	GetParentMesh() { return m_Parent; }

	D3DXVECTOR3 GetTargetPosition ();

	NavigationPath::WAYPOINT GetNextWayPoint () const { return (*m_NextWaypoint); } // 추가된거
	void		             SetNextWayPoint ( NavigationPath::WAYPOINT WayPoint ); // 추가된거

public:
	Actor();
	~Actor();

	//	Note : *주의* 복사의 의미로 사용되지 않는다.
	//		생성자() 로 초기화시에 정상적인 초기화를 가능하게 하기 위해서 사용.
	Actor& operator= ( const Actor& Src)
	{
		m_PathActive = false;
		m_Parent = NULL;
		m_CurrentCellID = -1;
		m_Movement = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_Position = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_NextPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		m_MaxSpeed = 5.0f;
		m_CorrectY = 0.0f;

		m_Path.Clear ();

		return *this;
	}
};

inline void Actor::AddMovement(const D3DXVECTOR3& Movement)
{
	m_Movement += Movement;
}

inline void Actor::SetMovement(const D3DXVECTOR3& Movement)
{
	m_Movement += Movement;
}

inline void Actor::SetMovementX(float X)
{
	m_Movement.x += X;
}

inline void Actor::SetMovementY(float Y)
{
	m_Movement.y += Y;
}

inline void Actor::SetMovementZ(float Z)
{
	m_Movement.z += Z;
}

inline void Actor::SetMaxSpeed(float speed)
{
	m_MaxSpeed = speed;
}

inline bool Actor::PathIsActive()const
{
	return(m_PathActive);
}

inline const D3DXVECTOR3& Actor::Position()const
{
	return(m_Position);
}

inline const D3DXVECTOR3& Actor::Movement()const
{
	return(m_Movement);
}

inline DWORD Actor::CurrentCellID() const
{
	return(m_CurrentCellID);
}

inline	void	Actor::Stop ()
{
	m_PathActive = false;
	m_Path.Clear();		
}

#endif  // end of file      ( Actor.h )

