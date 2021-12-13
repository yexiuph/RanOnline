#include "pch.h"

#include "navigationcell.h"
#include "navigationheap.h"
#include <stdlib.h>
#include <GASSERT.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*	ClassifyPathToCell
------------------------------------------------------------------------------------------
	
	Classifies a Path in relationship to this cell. A path is represented by a 2D line
	where Point A is the start of the path and Point B is the desired position.

	If the path exits this cell on a side which is linked to another cell, that cell index
	is returned in the NextCell parameter and SideHit contains the side number of the wall 
	exited through.
	
	If the path collides with a side of the cell which has no link (a solid edge), 
	SideHit contains the side number (0-2) of the colliding wall.

	In either case PointOfIntersection will contain the point where the path intersected
	with the wall of the cell if it is provided by the caller.
	
------------------------------------------------------------------------------------------
*/
NavigationCell::NavigationCell()
{
	for ( int i = 0; i < 3; i++ )
	{
		m_Link[i] = NULL;
	}
}
NavigationCell::~NavigationCell()
{
}

NavigationCell::PATH_RESULT NavigationCell::ClassifyPathToCell(const Line2D& MotionPath, NavigationCell** pNextCell, CELL_SIDE& Side, D3DXVECTOR2* pPointOfIntersection)const
{
	int InteriorCount = 0;

	// Check our MotionPath against each of the three cell walls
	for (int i=0; i<3; ++i)
	{
		// Classify the MotionPath endpoints as being either ON_LINE,
		// or to its LEFT_SIDE or RIGHT_SIDE. 
		// Since our triangle vertices are in clockwise order, 
		// we know that points  to the right of each line are inside the cell.
		// Points to the left are outside. 
		// We do this test using the ClassifyPoint function of Line2D

		// If the destination endpoint of the MotionPath 
		// is Not on the right side of this wall...
		if (m_Side[i].ClassifyPoint(MotionPath.EndPointB()) != Line2D::RIGHT_SIDE)
		{
			// ..and the starting endpoint of the MotionPath 
			// is Not on the left side of this wall...
			if (m_Side[i].ClassifyPoint(MotionPath.EndPointA()) != Line2D::LEFT_SIDE)
			{
				// Check to see if we intersect the wall 
				// using the Intersection function of Line2D
				Line2D::LINE_CLASSIFICATION IntersectResult = MotionPath.Intersection(m_Side[i], pPointOfIntersection);				
				
				if (IntersectResult == Line2D::SEGMENTS_INTERSECT || IntersectResult == Line2D::A_BISECTS_B)
				{
					// record the link to the next adjacent cell
					// (or NULL if no attachement exists)
					// and the enumerated ID of the side we hit.					
					*pNextCell = m_Link[i];					
					Side = (CELL_SIDE)i;
					return (EXITING_CELL);
				}
			}
		}
		else
		{
			// The destination endpoint of the MotionPath is on the right side.
			// Increment our InteriorCount so we'll know how many walls we were
			// to the right of.
			InteriorCount++;
		}
	}

	// An InteriorCount of 3 means the destination endpoint of the MotionPath 
	// was on the right side of all walls in the cell. 
	// That means it is located within this triangle, and this is our ending cell.
	if (InteriorCount == 3)
	{
		return (ENDING_CELL);
	}

	// We only reach here is if the MotionPath does not intersect the cell at all.
	return (NO_RELATIONSHIP);
}

/*	ProjectPathOnCellWall
------------------------------------------------------------------------------------------
	
	ProjectPathOnCellWall projects a path intersecting the wall with the wall itself. This
	can be used to convert a path colliding with a cell wall to a resulting path moving
	along the wall. The input parameter MotionPath MUST contain a starting point (EndPointA)
	which is the point of intersection with the path and cell wall number [SideNumber]
	and an ending point (EndPointB) which resides outside of the cell.
	
------------------------------------------------------------------------------------------
*/
void NavigationCell::ProjectPathOnCellWall(CELL_SIDE SideNumber, Line2D& MotionPath)const
{
	// compute the normalized vector of the cell wall in question
	D3DXVECTOR2 WallNormal = m_Side[SideNumber].EndPointB() - m_Side[SideNumber].EndPointA();
	D3DXVec2Normalize (&WallNormal,&WallNormal);

	// determine the vector of our current movement
	D3DXVECTOR2 MotionVector = MotionPath.EndPointB() - MotionPath.EndPointA();

	// compute dot product of our MotionVector and the normalized cell wall
	// this gives us the magnatude of our motion along the wall
	float DotResult = D3DXVec2Dot(&MotionVector,&WallNormal);
    
	// our projected vector is then the normalized wall vector times our new found magnatude
	MotionVector = (DotResult * WallNormal);

	// redirect our motion path along the new reflected direction
	MotionPath.SetEndPointB(MotionPath.EndPointA() + MotionVector);

	//
	// Make sure starting point of motion path is within the cell
	//
	D3DXVECTOR2 NewPoint = MotionPath.EndPointA();
	ForcePointToCellCollumn(NewPoint);
	MotionPath.SetEndPointA(NewPoint);

	//
	// Make sure destination point does not intersect this wall again
	//
	NewPoint = MotionPath.EndPointB();
	ForcePointToWallInterior(SideNumber, NewPoint);
	MotionPath.SetEndPointB(NewPoint);

}

//:	ForcePointToWallInterior
//----------------------------------------------------------------------------------------
//
//	Force a 2D point to the interior side of the specified wall. 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ForcePointToWallInterior(CELL_SIDE SideNumber, D3DXVECTOR2& TestPoint)const
{
	float Distance = m_Side[SideNumber].SignedDistance(TestPoint);
	float Epsilon = 0.001f;

	if (Distance <= Epsilon)
	{
		if (Distance <= 0.0f)
		{
			Distance -= Epsilon;
		}

		Distance = (float)fabs(Distance);
		Distance = (Epsilon>Distance ? Epsilon : Distance);

		// this point needs adjustment
		D3DXVECTOR2 Normal = m_Side[SideNumber].Normal();
		TestPoint += (Normal * Distance);
		return (true);
	}
	return (false);
}

//:	ForcePointToWallInterior
//----------------------------------------------------------------------------------------
//
//	Force a 3D point to the interior side of the specified wall. 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ForcePointToWallInterior(CELL_SIDE SideNumber, D3DXVECTOR3& TestPoint)const
{
	D3DXVECTOR2 TestPoint2D(TestPoint.x,TestPoint.z);
	bool PointAltered = ForcePointToWallInterior(SideNumber, TestPoint2D);

	if (PointAltered)
	{
		TestPoint.x = TestPoint2D.x;
		TestPoint.z = TestPoint2D.y;
	}

	return (PointAltered);
}

//:	ForcePointToCellCollumn
//----------------------------------------------------------------------------------------
//
//	Force a 2D point to the interior cell by forcing it to the interior of each wall 
//
//-------------------------------------------------------------------------------------://

bool NavigationCell::ForcePointToCellCollumn(D3DXVECTOR2& TestPoint)const
{
	bool PointAltered = false;

	// create a motion path from the center of the cell to our point
	Line2D TestPath(D3DXVECTOR2(m_CenterPoint.x, m_CenterPoint.z), TestPoint);
	D3DXVECTOR2 PointOfIntersection;
	CELL_SIDE Side;
	NavigationCell* NextCell;

	PATH_RESULT result = ClassifyPathToCell(TestPath, &NextCell, Side, &PointOfIntersection);
	// compare this path to the cell.

	if (result == EXITING_CELL)
	{
		D3DXVECTOR2 PathDirection(PointOfIntersection.x - m_CenterPoint.x, PointOfIntersection.y - m_CenterPoint.z);

		PathDirection *= 0.98f;

		TestPoint.x = m_CenterPoint.x + PathDirection.x;
		TestPoint.y = m_CenterPoint.z + PathDirection.y;

		if ( !IsPointInCellCollumn ( TestPoint ) )
		{
			//CDebugSet::ToFile ( "navigation.log", "ForcePointToCellCollumn x:%6.6f y:%6.6f"
			//	, TestPoint.x, TestPoint.y );
			TestPoint.x = m_CenterPoint.x;
			TestPoint.y = m_CenterPoint.z;
		}

		return (true);
	}
	else if (result == NO_RELATIONSHIP)
	{
		TestPoint.x = m_CenterPoint.x;
		TestPoint.y = m_CenterPoint.z;
		return (true);
	}

	return (false);
}

//:	ForcePointToCellCollumn
//----------------------------------------------------------------------------------------
//
//	Force a 3D point to the interior cell by forcing it to the interior of each wall 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ForcePointToCellCollumn(D3DXVECTOR3& TestPoint)const
{
	D3DXVECTOR2 TestPoint2D(TestPoint.x,TestPoint.z);
	bool PointAltered = ForcePointToCellCollumn(TestPoint2D);

	if (PointAltered)
	{
		TestPoint.x=TestPoint2D.x;
		TestPoint.z=TestPoint2D.y;
	}
	return (PointAltered);
}

//:	ProcessCell
//----------------------------------------------------------------------------------------
//
//	Process this cells neighbors using A* 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ProcessCell(NavigationHeap* pHeap)
{
	if (m_SessionID==pHeap->SessionID())
	{
		// once we have been processed, we are closed
		m_Open  = false;

		/*
		// querry all our neigbors to see if they need to be added to the Open heap
		for (int i=0;i<3;++i)
		{
			if (m_Link[i])
			{
				// abs(i-m_ArrivalWall) is a formula to determine which distance measurement to use.
				// The Distance measurements between the wall midpoints of this cell
				// are held in the order ABtoBC, BCtoCA and CAtoAB. 
				// We add this distance to our known m_ArrivalCost to compute
				// the total cost to reach the next adjacent cell.
				m_Link[i]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[abs(i-m_ArrivalWall)]);
			}
		}		
		*/
		if ( m_ArrivalWall == 0 )
		{
			if ( m_Link[0] ) m_Link[0]->QueryForPath(pHeap, this, 0);//m_ArrivalCost+m_WallDistance[1]);
			if ( m_Link[1] ) m_Link[1]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[0]);
			if ( m_Link[2] ) m_Link[2]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[2]);
		}
		else if ( m_ArrivalWall == 1 )
		{
			if ( m_Link[1] ) m_Link[1]->QueryForPath(pHeap, this, 0);//m_ArrivalCost+m_WallDistance[2]);
			if ( m_Link[2] ) m_Link[2]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[1]);
			if ( m_Link[0] ) m_Link[0]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[0]);
		}
		else if ( m_ArrivalWall == 2 )
		{
			if ( m_Link[2] ) m_Link[2]->QueryForPath(pHeap, this, 0);//m_ArrivalCost+m_WallDistance[0]);
			if ( m_Link[0] ) m_Link[0]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[2]);
			if ( m_Link[1] ) m_Link[1]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[1]);
		}
		

		return(true);
	}
	return(false);
}


//:	QueryForPath
//----------------------------------------------------------------------------------------
//
//	Process this cell using the A* heuristic 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::QueryForPath(NavigationHeap* pHeap, NavigationCell* Caller, float arrivalcost)
{
	//	<--	새로운 새션. 모든 멤버 데이타(길찾기에 관련된...) 초기화	
	if (m_SessionID!=pHeap->SessionID())
	{		
		m_SessionID = pHeap->SessionID();

		//	<**	콜러가 있다는 것은,
		//		이전값과 현재값을 누적해야한다는 것을 의미한다.
		//		물론, 이렇게 된 경우... 이것은 open노드이다.		
		if (Caller)
		{
			m_Open  = true;
			ComputeHeuristic(pHeap->Goal());
			m_ArrivalCost = arrivalcost;			
			
			// remember the side this caller is entering from
			if ( m_Link[0] && m_Link[0]->CellID() == Caller->CellID() )
			{
				m_ArrivalWall = 0;
			}
			else if ( m_Link[1] && m_Link[1]->CellID() == Caller->CellID() )
			{
				m_ArrivalWall = 1;
			}
			else if ( m_Link[2] && m_Link[2]->CellID() == Caller->CellID() )
			{
				m_ArrivalWall = 2;
			}
			else
			{
				m_ArrivalWall = -1;
			}
		}
		//	**>
		//	<**	콜러가 없다는 것은 이제 탐색이 시작됨을 의미한다.
		//		걍.. 초기화 하는거다.
		//		we are the cell that contains the starting location
		//		of the A* search.
		else
		{			
			m_Open  = false;
			m_ArrivalCost = 0;
			m_Heuristic = 0;
			m_ArrivalWall = 0;
		}
		//	**>
		
		// add this cell to the Open heap
		pHeap->AddCell(this);
		return(true);
	}
	//	-->	새로운 새션. 모든 멤버 데이타(길찾기에 관련된...) 초기화

	//	
	else if (m_Open)
	{
		// m_Open means we are already in the Open Heap.
		// If this new caller provides a better path, adjust our data
		// Then tell the Heap to resort our position in the list.
		if ((arrivalcost + m_Heuristic) < (m_ArrivalCost + m_Heuristic))
		{
				m_ArrivalCost = arrivalcost;

				// remember the side this caller is entering from
				if (Caller == m_Link[0])
				{
					m_ArrivalWall = 0;
				}
				else if (Caller == m_Link[1])
				{
					m_ArrivalWall = 1;
				}
				else if (Caller == m_Link[2])
				{
					m_ArrivalWall = 2;
				}
				// ask the heap to resort our position in the priority heap
				pHeap->AdjustCell(this);
				return(true);
		}
	}
	// this cell is closed
	return(false);
}

//:	ComputeHeuristic
//----------------------------------------------------------------------------------------
//
//	Compute the A* Heuristic for this cell given a Goal point 
//
//-------------------------------------------------------------------------------------://
void NavigationCell::ComputeHeuristic(const D3DXVECTOR3& Goal)
{
	// our heuristic is the estimated distance (using the longest axis delta) between our
	// cell center and the goal location

	float XDelta = (float)fabs(Goal.x - m_CenterPoint.x);
	float YDelta = (float)fabs(Goal.y - m_CenterPoint.y);
	float ZDelta = (float)fabs(Goal.z - m_CenterPoint.z);

#ifdef _WIN32
	m_Heuristic = __max(__max(XDelta,YDelta), ZDelta);
#else
	m_Heuristic = max(max(XDelta,YDelta), ZDelta);
#endif
}

//****************************************************************************************
// end of file      ( NavigationCell.cpp )

