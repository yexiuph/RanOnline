#ifndef PLANE_H
#define PLANE_H

/*	Plane
------------------------------------------------------------------------------------------
	
	A Plane in 3D Space represented in point-normal form (Ax + By + Cz + D = 0).

	The convention for the distance constant D is:

	D = -(A, B, C) dot (X, Y, Z)

------------------------------------------------------------------------------------------
*/

class Plane
{
public:

	// ----- CREATORS ---------------------

    Plane();
    Plane(const Plane& Plane);
    Plane(const D3DXVECTOR3& Point0, const D3DXVECTOR3& Point1, const D3DXVECTOR3& Point2);
    ~Plane();

	// ----- OPERATORS --------------------

	Plane& operator=( const Plane& Src );


	// tests if data is identical
	friend bool operator==( const Plane& PlaneA, const Plane& PlaneB )
	{
		return (PlaneA.m_Normal == PlaneB.m_Normal && PlaneA.m_Point==PlaneB.m_Point);
	}

	// ----- MUTATORS ---------------------

    void Set(const D3DXVECTOR3& Point0, const D3DXVECTOR3& Point1, const D3DXVECTOR3& Point2);

	float SolveForX(float Y, float Z)const;
	float SolveForY(float X, float Z)const;
	float SolveForZ(float X, float Y)const;

	// ----- ACCESSORS --------------------

    const D3DXVECTOR3& Normal() const;
    const D3DXVECTOR3& Point() const;
    const float& Distance() const;
                     
    float SignedDistance(const D3DXVECTOR3& Point) const;

private:

	// ----- DATA -------------------------

    D3DXVECTOR3     m_Normal;
    D3DXVECTOR3     m_Point;
    float       m_Distance;

	// ----- HELPER FUNCTIONS -------------

	// ----- UNIMPLEMENTED FUNCTIONS ------

};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	Plane
------------------------------------------------------------------------------------------
	
	Default Constructor
	
------------------------------------------------------------------------------------------
*/
inline Plane::Plane()
{
}

/*	Plane
------------------------------------------------------------------------------------------
	
	Default Copy Constructor
	
------------------------------------------------------------------------------------------
*/
inline Plane::Plane(const Plane& Rhs)
{
    m_Normal = Rhs.m_Normal;
    m_Point = Rhs.m_Point;
    m_Distance = Rhs.m_Distance;
}


inline Plane::Plane(const D3DXVECTOR3& Point0, const D3DXVECTOR3& Point1, const D3DXVECTOR3& Point2)
{
    Set(Point0, Point1, Point2);
}

inline Plane::~Plane()
{
}

//= OPERATORS ============================================================================


inline Plane& Plane::operator=( const Plane& Src )
{
    m_Normal = Src.m_Normal;
    m_Point = Src.m_Point;
    m_Distance = Src.m_Distance;

	return (*this);
}

//= MUTATORS =============================================================================

//:	SolveForX
//----------------------------------------------------------------------------------------
//
//	Given Z and Y, Solve for X on the plane 
//
//-------------------------------------------------------------------------------------://
inline float Plane::SolveForX(float Y, float Z)const
{
	//Ax + By + Cz + D = 0
	// Ax = -(By + Cz + D)
	// x = -(By + Cz + D)/A

	if (m_Normal[0])
	{
		return ( -(m_Normal[1]*Y + m_Normal[2]*Z + m_Distance) / m_Normal[0] );
	}

	return (0.0f);
}

//:	SolveForY
//----------------------------------------------------------------------------------------
//
//	Given X and Z, Solve for Y on the plane 
//
//-------------------------------------------------------------------------------------://
inline float Plane::SolveForY(float X, float Z)const
{
	//Ax + By + Cz + D = 0
	// By = -(Ax + Cz + D)
	// y = -(Ax + Cz + D)/B

	if (m_Normal[1])
	{
		return ( -(m_Normal[0]*X + m_Normal[2]*Z + m_Distance) / m_Normal[1] );
	}

	return (0.0f);
}

//:	SolveForZ
//----------------------------------------------------------------------------------------
//
//	Given X and Y, Solve for Z on the plane 
//
//-------------------------------------------------------------------------------------://
inline float Plane::SolveForZ(float X, float Y)const
{
	//Ax + By + Cz + D = 0
	// Cz = -(Ax + By + D)
	// z = -(Ax + By + D)/C

	if (m_Normal[2])
	{
		return ( -(m_Normal[0]*X + m_Normal[1]*Y + m_Distance) / m_Normal[2] );
	}

	return (0.0f);
}


//:	Set
//----------------------------------------------------------------------------------------
//
//	Setup Plane object given a clockwise ordering of 3D points 
//
//-------------------------------------------------------------------------------------://
inline void Plane::Set(const D3DXVECTOR3& Point0, const D3DXVECTOR3& Point1, const D3DXVECTOR3& Point2)
{
	D3DXVECTOR3 v1, v2;
	v1 = Point1 - Point0;
	v2 = Point2 - Point0;
    D3DXVec3Cross(&m_Normal, &v1, &v2);     
   	D3DXVec3Normalize ( &m_Normal, &m_Normal );
    m_Point = Point0;

    m_Distance = -D3DXVec3Dot(&m_Point,&m_Normal);
}

//= ACCESSORS ============================================================================


inline const D3DXVECTOR3& Plane::Normal() const
{
    return(m_Normal);
}

inline const D3DXVECTOR3& Plane::Point() const
{
    return(m_Point);
}

inline const float& Plane::Distance() const
{
    return(m_Distance);
}

//- End of Plane -------------------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( Plane.h )

