#ifndef NAVIGATIONPATH_H
#define NAVIGATIONPATH_H

#include <list>
#include "navigationcell.h"
#include "navigationmesh.h"

// forward declaration of our parents
class NavigationPath
{
public:

	struct WAYPOINT
	{
		D3DXVECTOR3 Position;		// 3D position of waypoint
		DWORD		CellID;
	};

	typedef std::list <WAYPOINT> WAYPOINT_LIST;
	typedef WAYPOINT_LIST::const_iterator WayPointID;

	NavigationPath();
	~NavigationPath();

	void	Clear();

	void Setup(NavigationMesh* Parent, const D3DXVECTOR3& StartPoint, DWORD StartID, const D3DXVECTOR3& EndPoint, DWORD EndID);
	void AddWayPoint(const D3DXVECTOR3& Point, DWORD CellID);
	void EndPath();

	NavigationMesh*			Parent()const;
	const WAYPOINT&			StartPoint()const;
	const WAYPOINT&			EndPoint()const;
	WAYPOINT_LIST&			WaypointList();
	WayPointID				GetFurthestVisibleWayPoint(const WayPointID& VantagePoint)const;

private:
	NavigationMesh*		m_Parent;
	WAYPOINT			m_StartPoint;
	WAYPOINT			m_EndPoint;
	WayPointID			m_EndID;
	WAYPOINT_LIST		m_WaypointList;

	NavigationPath( const NavigationPath& Src);
	NavigationPath& operator=( const NavigationPath& Src);


};

inline NavigationPath::NavigationPath()
{
}

inline NavigationPath::~NavigationPath()
{
}

inline void NavigationPath::Clear()
{
	m_WaypointList.clear();
}

inline void NavigationPath::Setup(NavigationMesh* Parent, const D3DXVECTOR3& StartPoint, DWORD StartID, const D3DXVECTOR3& EndPoint, DWORD EndID)
{
	m_WaypointList.clear();

	m_Parent = Parent;
	m_StartPoint.Position = StartPoint;
	m_StartPoint.CellID = StartID;
	m_EndPoint.Position = EndPoint;
	m_EndPoint.CellID = EndID;

	m_WaypointList.push_back(m_StartPoint);
}

inline void NavigationPath::AddWayPoint(const D3DXVECTOR3& Point, DWORD CellID)
{
	WAYPOINT NewPoint;
	
	NewPoint.Position = Point;
	NewPoint.CellID = CellID;

	m_WaypointList.push_back(NewPoint);
}

inline void NavigationPath::EndPath()
{	
	m_WaypointList.push_back(m_EndPoint);
}

inline NavigationMesh* NavigationPath::Parent()const
{
	return(m_Parent);
}

inline const NavigationPath::WAYPOINT& NavigationPath::StartPoint()const
{
	return(m_StartPoint);
}

inline const NavigationPath::WAYPOINT&	NavigationPath::EndPoint()const
{
	return(m_EndPoint);
}

inline NavigationPath::WAYPOINT_LIST& NavigationPath::WaypointList()
{
	return(m_WaypointList);
}

inline NavigationPath::WayPointID NavigationPath::GetFurthestVisibleWayPoint(const WayPointID& VantagePoint)const
{
	// see if we are already talking about the last waypoint
	//	<--	여기가 현재 있는 곳이 마지막 셀인가?, 이번셀이 마지막셀?
	//	<**	이것은, 현재 포지션이 삼각형 한 변의 중점이고,
	//		목표점이 그 삼각형의 어딘가라고 할 때.... 만족되는 구문이다.
	//	**>
	if (VantagePoint == m_WaypointList.end())
	{
		return(VantagePoint);
	}
	//	-->	여기가 현재 있는 곳이 마지막 셀인가?, 이번셀이 마지막셀?


	const WAYPOINT& Vantage = *VantagePoint;
	WayPointID TestWaypoint = VantagePoint;
	++TestWaypoint;	

	//	<--	다음 셀이 마지막 셀인가?
	if (TestWaypoint == m_WaypointList.end())
	{
		return(TestWaypoint);	
	}
	//	-->	다음 셀이 마지막 셀인가?

	WayPointID VisibleWaypoint = TestWaypoint;
	++TestWaypoint;
	
	while (TestWaypoint != m_WaypointList.end())
	{
		const WAYPOINT& Test = *TestWaypoint;
		if (!m_Parent->LineOfSightTest(Vantage.CellID, Vantage.Position, Test.CellID, Test.Position))
		{
			return(VisibleWaypoint);
		}
		VisibleWaypoint = TestWaypoint;
		++TestWaypoint;	
	}
	return(VisibleWaypoint);
}

#endif  // end of file      ( NavigationPath.h )

