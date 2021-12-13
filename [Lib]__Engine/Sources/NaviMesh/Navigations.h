//-------------------------------------------------------------------------------------------------------
//								N	a	v	i	g	a	t	i	o	n	s
//
//		기존 Navigation은 1개밖에 사용을 못하고 여러개가 있으면 그곳을 Picking 시 에러를 발생했다.
//		이것은 다수의 Navigation이 공존 가능 하게 해줄 것이다.
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
