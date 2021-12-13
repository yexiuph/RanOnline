#pragma once
#include <map>
#include <string>
#include "GLContrlMsg.h"

class GLFriendClient
{
public:
	typedef std::map<std::string,SFRIEND>	FRIENDMAP;
	typedef FRIENDMAP::iterator				FRIENDMAP_ITER;

protected:
	bool		m_bReqList;
	FRIENDMAP	m_mapFriend;

public:
	GLFriendClient::FRIENDMAP& GetFriend ()	{ return m_mapFriend; }
	SFRIEND* GetFriend ( const char *szName );

	INT GetFriendSize( EMFRIEND_FLAGS emFLAGS );

public:
	void Clear ();

protected:
	void DelFriendInfo ( const char* szName );

public:
	void ReqFriendList ();
	void ReqFriendAdd ( const char* szName );
	void ReqFriendLureAns ( const char* szReqName, bool bOK );
	void ReqFriendDel ( const char* szDelName );
	void ReqFriendBlock ( const char* szName, bool bBLOCK );

protected:
	void MsgReqFriendAddFb ( NET_MSG_GENERIC* nmg );
	void MsgReqFriendAddLure ( NET_MSG_GENERIC* nmg );
	void MsgReqFriendDelFb ( NET_MSG_GENERIC* nmg );
	void MsgReqFriendBlockFb ( NET_MSG_GENERIC* nmg );
	void MsgReqFriendInfo ( NET_MSG_GENERIC* nmg );
	void MsgReqFriendState ( NET_MSG_GENERIC* nmg );
	void MsgReqFriendRename ( NET_MSG_GENERIC* nmg );
	void MsgReqFriendPhoneNumber ( NET_MSG_GENERIC* nmg );

public:
	void MsgProcess ( NET_MSG_GENERIC* nmg );

public:
	static GLFriendClient& GetInstance ();

protected:
	GLFriendClient(void);

public:
	~GLFriendClient(void);
};
