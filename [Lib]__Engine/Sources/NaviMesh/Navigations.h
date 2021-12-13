//-------------------------------------------------------------------------------------------------------
//								N	a	v	i	g	a	t	i	o	n	s
//
//		���� Navigation�� 1���ۿ� ����� ���ϰ� �������� ������ �װ��� Picking �� ������ �߻��ߴ�.
//		�̰��� �ټ��� Navigation�� ���� ���� �ϰ� ���� ���̴�.
//
//															Create	: Hwanny
//															Date	: 2006. 01. 05
//															Update	: 
//-------------------------------------------------------------------------------------------------------

#ifndef NAVIGATIONS_H
#define NAVIGATIONS_H

#include "navigationmesh.h"

#include <map>

typedef std::map<DWORD,NavigationMesh*>		MAP_NAVIGATIONMESH;
typedef MAP_NAVIGATIONMESH::iterator		MAP_NAVIGATIONMESH_ITER;

class Navigations
{
private:
	MAP_NAVIGATIONMESH	m_mapNavigations;

private:
	void CleanUp();

public:
	Navigations();
	~Navigations();
};

#endif
