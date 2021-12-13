#ifndef	__C_COLLISION_MAPCELL__
#define	__C_COLLISION_MAPCELL__

class	CSerialFile;
class	CCollisionMapCell
{
public:
	enum
	{
		VERT_A = 0,
		VERT_B = 1,
		VERT_C = 2
	};

public:
				CCollisionMapCell ();
	virtual		~CCollisionMapCell();

public:
	BOOL	SaveFile ( CSerialFile& SFile );
	BOOL	LoadFile ( CSerialFile& SFile );

public:	
	void	SetIndex  ( int Point, DWORD IndexID );
	void	SetIndices( DWORD IndexA, DWORD IndexB, DWORD IndexC );
	void	SetIndices( DWORD* pIndexID );
	
	DWORD	GetIndex ( int Point );
	const	DWORD*	GetIndices ();

	void	SetCellID ( DWORD CellID );
	DWORD	GetCellID ();

private:
	union
	{
		struct
		{
			DWORD	m_IndexA;
			DWORD	m_IndexB;
			DWORD	m_IndexC;
		};
        struct
		{
			DWORD	m_Indices[3];
		};
	};
	DWORD	m_CellID;
};

#endif	//	__C_COLLISION_MAPCELL__