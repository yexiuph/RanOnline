#ifndef NAVIGATIONCELL_H
#define NAVIGATIONCELL_H

#include "plane.h"
#include "line2d.h"

// a forward declaration for NavigationHeap is required
class	NavigationHeap;
class	CSerialFile;


/*	NavigationCell
------------------------------------------------------------------------------------------
	
	A NavigationCell represents a single triangle within a NavigationMesh. It contains 
	functions for testing a path against the cell, and various ways to resolve collisions
	with the cell walls. Portions of the A* path finding algorythm are provided within this
	class as well, but the path finding process is managed by the parent Navigation Mesh.
	
------------------------------------------------------------------------------------------
*/

class NavigationCell
{
public:
	// ----- ENUMERATIONS & CONSTANTS -----

	enum CELL_VERT
	{
		VERT_A = 0,
		VERT_B,
		VERT_C
	};

	enum CELL_SIDE
	{
		SIDE_AB = 0,
		SIDE_BC,
		SIDE_CA
	};

	enum PATH_RESULT
	{
		NO_RELATIONSHIP,		// the path does not cross this cell
		ENDING_CELL,			// the path ends in this cell	
		EXITING_CELL,			// the path exits this cell through side X
	};

	// ----- CREATORS ---------------------

	NavigationCell();
	inline NavigationCell( const NavigationCell& Src){*this = Src;};
	~NavigationCell();

	// ----- OPERATORS --------------------
	NavigationCell& operator=( const NavigationCell& Src);

	// ----- MUTATORS ---------------------
	HRESULT	Initialize ( D3DXVECTOR3 *pNaviVertex, const DWORD& PointA, const DWORD& PointB, const DWORD& PointC, const DWORD& CellID );
	HRESULT ComputeCellData ( D3DXVECTOR3 *pNaviVertex );
	bool RequestLink(const DWORD& PointA, const DWORD& PointB, NavigationCell* Caller, D3DXVECTOR3 *pNaviVertex);
	void SetLink(CELL_SIDE Side, NavigationCell* Caller);

	PATH_RESULT ClassifyPathToCell(const Line2D& MotionPath, NavigationCell** pNextCell, CELL_SIDE& Side, D3DXVECTOR2* pPointOfIntersection)const;

	void ProjectPathOnCellWall(CELL_SIDE SideNumber, Line2D& MotionPath)const;

	void MapVectorHeightToCell(D3DXVECTOR3& MotionPoint)const;

	bool ForcePointToCellCollumn(D3DXVECTOR3& TestPoint)const;
	bool ForcePointToCellCollumn(D3DXVECTOR2& TestPoint)const;
	bool ForcePointToWallInterior(CELL_SIDE SideNumber, D3DXVECTOR2& TestPoint)const;
	bool ForcePointToWallInterior(CELL_SIDE SideNumber, D3DXVECTOR3& TestPoint)const;

	bool ProcessCell(NavigationHeap* pHeap);
	bool QueryForPath(NavigationHeap* pHeap, NavigationCell* Caller, float arrivalcost);

	// ----- ACCESSORS --------------------

	bool IsPointInCellCollumn(const D3DXVECTOR3& TestPoint)const;
	bool IsPointInCellCollumn(const D3DXVECTOR2& TestPoint)const;
	const DWORD& Vertex(int Vert)const;
	const D3DXVECTOR3& CenterPoint()const;
	NavigationCell* Link(int Side)const;

	const D3DXVECTOR3& Normal() const;
	const D3DXVECTOR3& LinkPoint()const;

	float ArrivalCost()const;
	float Heuristic()const;
	float PathfindingCost()const;

	int ArrivalWall()const;
	const D3DXVECTOR3 WallMidpoint(int Side)const;

	const DWORD&	CellID () const;

public:
	void	SaveFile ( CSerialFile	&SFile );
	void	LoadFile ( CSerialFile	&SFile );

private:

	// ----- DATA -------------------------
	Plane	m_CellPlane;		// A plane containing the cell triangle
	D3DXVECTOR3 m_CenterPoint;		// The center of the triangle
	Line2D	m_Side[3];			// a 2D line representing each cell Side
	DWORD	m_Vertex[3];		// pointers to the verticies of this triangle held in the NavigationMesh's vertex pool
//	D3DXVECTOR3	m_Normal;
	//	<--	셀의 고유ID
	DWORD	m_CellID;
	//	-->	셀의 고유ID
	NavigationCell* m_Link[3];// pointers to cells that attach to this cell. A NULL link denotes a solid edge.	


	// Pathfinding Data...
	int		m_SessionID;		// an identifier for the current pathfinding session.
	float	m_ArrivalCost;		// total cost to use this cell as part of a path
	float	m_Heuristic;		// our estimated cost to the goal from here
	bool	m_Open;				// are we currently listed as an Open cell to revisit and test?
	int		m_ArrivalWall;		// the side we arrived through.
	D3DXVECTOR3 m_WallMidpoint[3];	// the pre-computed midpoint of each wall.
	float	m_WallDistance[3];	// the distances between each wall midpoint of sides (0-1, 1-2, 2-0)
  
	// ----- HELPER FUNCTIONS -------------
	void ComputeHeuristic(const D3DXVECTOR3& Goal); // estimates the distance to the goal for A*

	// ----- UNIMPLEMENTED FUNCTIONS ------
};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

//= OPERATORS ============================================================================
inline NavigationCell& NavigationCell::operator=( const NavigationCell& Src)
{
	if (this != &Src)
	{
		m_CellPlane = Src.m_CellPlane;		
		m_CenterPoint = Src.m_CenterPoint;	
		m_SessionID= Src.m_SessionID;
		m_ArrivalCost= Src.m_ArrivalCost;
		m_Heuristic= Src.m_Heuristic;
		m_Open= Src.m_Open;
		m_ArrivalWall= Src.m_ArrivalWall;

		for (int i=0;i<3;i++)
		{
			m_Vertex[i] = Src.m_Vertex[i];
			m_Side[i] = Src.m_Side[i];
			m_Link[i] = Src.m_Link[i];
			m_WallMidpoint[i] = Src.m_WallMidpoint[i];
			m_WallDistance[i] = Src.m_WallDistance[i];
		}
	}
	return (*this);
}

//= MUTATORS =============================================================================

//:	Initialize
//----------------------------------------------------------------------------------------
//
//	Initialize our Cell object, given it's vertices in clockwise order 
//
//-------------------------------------------------------------------------------------://
inline HRESULT NavigationCell::Initialize( D3DXVECTOR3 *pNaviVertex, const DWORD& PointA, const DWORD& PointB, const DWORD& PointC, const DWORD& CellID)
{
	//	<--	고유 NavigationCell ID 설정
	m_CellID = CellID;
	//	-->	고유 NavigationCell ID 설정

	m_Vertex[VERT_A] = PointA;
	m_Vertex[VERT_B] = PointB;
	m_Vertex[VERT_C] = PointC;

	// object must be re-linked
	m_Link[SIDE_AB] = 0;
	m_Link[SIDE_BC] = 0;
	m_Link[SIDE_CA] = 0;

	// now that the vertex pointers are set, compute additional data about the Cell
	return ComputeCellData ( pNaviVertex );
}

//:	ComputeCellData
//----------------------------------------------------------------------------------------
//
//	Compute additional data about the cell used for navigation tests and pathfinding 
//
//-------------------------------------------------------------------------------------://
inline HRESULT NavigationCell::ComputeCellData( D3DXVECTOR3 *pNaviVertex )
{
	//	<**	아래로 인덱싱해서 따라가야하는 부분이 많아서,
	//		위에서 하나의 변수에 넣고 그놈을 쓰는 방식을 채택했다.
	D3DXVECTOR3	RealVertex[3];

	RealVertex[VERT_A] = pNaviVertex[m_Vertex[VERT_A]];
	RealVertex[VERT_B] = pNaviVertex[m_Vertex[VERT_B]];
	RealVertex[VERT_C] = pNaviVertex[m_Vertex[VERT_C]];
	//	**>

	// create 2D versions of our verticies
	D3DXVECTOR2 Point1(RealVertex[VERT_A].x,RealVertex[VERT_A].z);
	D3DXVECTOR2 Point2(RealVertex[VERT_B].x,RealVertex[VERT_B].z);
	D3DXVECTOR2 Point3(RealVertex[VERT_C].x,RealVertex[VERT_C].z);

	// innitialize our sides
	m_Side[SIDE_AB].SetPoints(Point1,Point2);	// line AB
	m_Side[SIDE_BC].SetPoints(Point2,Point3);	// line BC
	m_Side[SIDE_CA].SetPoints(Point3,Point1);	// line CA

	m_CellPlane.Set(RealVertex[VERT_A],RealVertex[VERT_B],RealVertex[VERT_C]);

	// compute midpoint as centroid of polygon
	m_CenterPoint.x=((RealVertex[VERT_A].x + RealVertex[VERT_B].x + RealVertex[VERT_C].x)/3);
	m_CenterPoint.y=((RealVertex[VERT_A].y + RealVertex[VERT_B].y + RealVertex[VERT_C].y)/3);
	m_CenterPoint.z=((RealVertex[VERT_A].z + RealVertex[VERT_B].z + RealVertex[VERT_C].z)/3);

	// compute the midpoint of each cell wall
	m_WallMidpoint[0].x = (RealVertex[VERT_A].x + RealVertex[VERT_B].x)/2.0f;
	m_WallMidpoint[0].y = (RealVertex[VERT_A].y + RealVertex[VERT_B].y)/2.0f;
	m_WallMidpoint[0].z = (RealVertex[VERT_A].z + RealVertex[VERT_B].z)/2.0f;

	m_WallMidpoint[1].x = (RealVertex[VERT_C].x + RealVertex[VERT_B].x)/2.0f;
	m_WallMidpoint[1].y = (RealVertex[VERT_C].y + RealVertex[VERT_B].y)/2.0f;
	m_WallMidpoint[1].z = (RealVertex[VERT_C].z + RealVertex[VERT_B].z)/2.0f;

	m_WallMidpoint[2].x = (RealVertex[VERT_C].x + RealVertex[VERT_A].x)/2.0f;
	m_WallMidpoint[2].y = (RealVertex[VERT_C].y + RealVertex[VERT_A].y)/2.0f;
	m_WallMidpoint[2].z = (RealVertex[VERT_C].z + RealVertex[VERT_A].z)/2.0f;

	// compute the distances between the wall midpoints
	D3DXVECTOR3 WallVector;
	WallVector = m_WallMidpoint[0] - m_WallMidpoint[1];
	m_WallDistance[0] =  (float)sqrt ( WallVector.x * WallVector.x +
						WallVector.y * WallVector.y +
						WallVector.z * WallVector.z );
 

	WallVector = m_WallMidpoint[1] - m_WallMidpoint[2];
	m_WallDistance[1] =  (float)sqrt ( WallVector.x * WallVector.x +
						WallVector.y * WallVector.y +
						WallVector.z * WallVector.z );
 

	WallVector = m_WallMidpoint[2] - m_WallMidpoint[0];
	m_WallDistance[2] =  (float)sqrt ( WallVector.x * WallVector.x +
						WallVector.y * WallVector.y +
						WallVector.z * WallVector.z );

	//	<--	셀의 노말을 구해둠	
/*	D3DXVECTOR3 v1, v2;

	v1 = RealVertex[VERT_B] - RealVertex[VERT_A];
	v2 = RealVertex[VERT_C] - RealVertex[VERT_A];

	D3DXVec3Cross(&m_Normal, &v1, &v2);     
	D3DXVec3Normalize ( &m_Normal, &m_Normal );*/
	//	-->	셀의 노말을 구해둠

	return S_OK;
	
}

//:	RequestLink
//----------------------------------------------------------------------------------------
//
//	Navigation Mesh is created as a pool of raw cells. The cells are then compared against 
//	each other to find common edges and create links. This routine is called from a 
//	potentially adjacent cell to test if a link should exist between the two. 
//
//-------------------------------------------------------------------------------------://
inline bool NavigationCell::RequestLink(const DWORD& PointA, const DWORD& PointB, NavigationCell* Caller, D3DXVECTOR3 *pNaviVertex)
{
	// return true if we share the two provided verticies with the calling cell.
	if (pNaviVertex[m_Vertex[VERT_A]] == pNaviVertex[PointA])
	{
		if (pNaviVertex[m_Vertex[VERT_B]] == pNaviVertex[PointB])
		{
			m_Link[SIDE_AB] = Caller;
			return (true);
		}
		else if (pNaviVertex[m_Vertex[VERT_C]] == pNaviVertex[PointB])
		{
			m_Link[SIDE_CA] = Caller;
			return (true);
		}
	}
	else if (pNaviVertex[m_Vertex[VERT_B]] == pNaviVertex[PointA])
	{
		if (pNaviVertex[m_Vertex[VERT_A]] == pNaviVertex[PointB])
		{
			m_Link[SIDE_AB] = Caller;
			return (true);
		}
		else if (pNaviVertex[m_Vertex[VERT_C]] == pNaviVertex[PointB])
		{
			m_Link[SIDE_BC] = Caller;
			return (true);
		}
	}
	else if (pNaviVertex[m_Vertex[VERT_C]] == pNaviVertex[PointA])
	{
		if (pNaviVertex[m_Vertex[VERT_A]] == pNaviVertex[PointB])
		{
			m_Link[SIDE_CA] = Caller;
			return (true);
		}
		else if (pNaviVertex[m_Vertex[VERT_B]] == pNaviVertex[PointB])
		{
			m_Link[SIDE_BC] = Caller;
			return (true);
		}
	}

	// we are not adjacent to the calling cell
	return (false);
}


//:	SetLink
//----------------------------------------------------------------------------------------
//
//	Sets a link to the calling cell on the enumerated edge. 
//
//-------------------------------------------------------------------------------------://
inline void NavigationCell::SetLink(CELL_SIDE Side, NavigationCell* Caller)
{
	m_Link[Side] = Caller;
}

//:	MapVectorHeightToCell
//----------------------------------------------------------------------------------------
//
//	Uses the X and Z information of the vector to calculate Y on the cell plane 
//
//-------------------------------------------------------------------------------------://
inline void NavigationCell::MapVectorHeightToCell(D3DXVECTOR3& MotionPoint)const
{
	MotionPoint.y= m_CellPlane.SolveForY(MotionPoint.x, MotionPoint.z);
}

//= ACCESSORS ============================================================================

//:	IsPointInCellCollumn
//----------------------------------------------------------------------------------------
//
//	Test to see if a 2D point is within the cell. There are probably better ways to do 
//	this, but this seems plenty fast for the time being. 
//
//-------------------------------------------------------------------------------------://
inline bool NavigationCell::IsPointInCellCollumn(const D3DXVECTOR2& TestPoint) const
{
	// we are "in" the cell if we are on the right hand side of all edge lines of the cell
	int InteriorCount = 0;

	for (int i=0;i<3;i++)
	{
		Line2D::POINT_CLASSIFICATION SideResult = m_Side[i].ClassifyPoint(TestPoint);

		if (SideResult != Line2D::LEFT_SIDE)
		{
			InteriorCount++;
		}
	}

	return (InteriorCount == 3);
}

//:	IsPointInCellCollumn
//----------------------------------------------------------------------------------------
//
//	Test to see if a 3D point is within the cell by projecting it down to 2D and calling 
//	the above method. 
//
//-------------------------------------------------------------------------------------://
inline bool NavigationCell::IsPointInCellCollumn(const D3DXVECTOR3& TestPoint) const
{
	D3DXVECTOR2 TestPoint2D(TestPoint.x,TestPoint.z);

	return (IsPointInCellCollumn(TestPoint2D));
}

inline const DWORD& NavigationCell::Vertex(int Vert)const
{
	return(m_Vertex[Vert]);
}

inline const D3DXVECTOR3& NavigationCell::CenterPoint()const
{
	return(m_CenterPoint);
}

inline NavigationCell* NavigationCell::Link(int Side)const
{
	return(m_Link[Side]);
}

inline float NavigationCell::ArrivalCost()const
{
	return(m_ArrivalCost);
}

inline float NavigationCell::Heuristic()const
{
	return(m_Heuristic);
}

inline float NavigationCell::PathfindingCost()const
{
	return(m_ArrivalCost + m_Heuristic);
}

inline int NavigationCell::ArrivalWall()const
{
	return(m_ArrivalWall);
}

inline const D3DXVECTOR3 NavigationCell::WallMidpoint(int Side)const
{
	return(m_WallMidpoint[Side]);
}

inline const D3DXVECTOR3& NavigationCell::Normal() const
{
//	return (m_Normal);
	return (m_CellPlane.Normal());
}

inline const DWORD&	NavigationCell::CellID () const
{
	return (m_CellID);
}

//- End of NavigationCell ----------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( NavigationCell.h )

