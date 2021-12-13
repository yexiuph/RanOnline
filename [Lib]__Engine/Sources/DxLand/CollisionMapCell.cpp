#include "pch.h"
#include "CollisionMapCell.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCollisionMapCell::CCollisionMapCell ()
{
}

CCollisionMapCell::~CCollisionMapCell()
{
}

void CCollisionMapCell::SetIndex ( int Point, DWORD IndexID )
{	
	if ( Point < VERT_A || VERT_C < Point )
	{
		_ASSERT ( 0 && "인덱스가 지정할수 있는 범위를 넘었습니다." );
		return ;
	}
	
	m_Indices[int(Point)] = IndexID;
}

void	CCollisionMapCell::SetIndices( DWORD* pIndexID )
{
	memcpy ( m_Indices, pIndexID, sizeof ( DWORD ) * 3 );
}

void CCollisionMapCell::SetIndices  ( DWORD IndexA, DWORD IndexB, DWORD IndexC )
{
	m_IndexA = IndexA;
	m_IndexB = IndexB;
	m_IndexC = IndexC;
}

DWORD CCollisionMapCell::GetIndex ( int Point )
{
	return m_Indices[int(Point)];
}

const	DWORD* CCollisionMapCell::GetIndices ()
{
	return m_Indices;
}

void CCollisionMapCell::SetCellID ( DWORD CellID )
{
	m_CellID = CellID;
}

DWORD CCollisionMapCell::GetCellID ()
{	
	return m_CellID;
}

BOOL CCollisionMapCell::SaveFile ( CSerialFile& SFile )
{
	SFile.WriteBuffer ( m_Indices, sizeof ( DWORD ) * 3 );
	SFile.WriteBuffer ( &m_CellID, sizeof ( DWORD ) );

	return TRUE;
}

BOOL CCollisionMapCell::LoadFile ( CSerialFile& SFile )
{
	SFile.ReadBuffer ( m_Indices, sizeof ( DWORD ) * 3 );
	SFile.ReadBuffer ( &m_CellID, sizeof ( DWORD ) );

	return TRUE;
}
