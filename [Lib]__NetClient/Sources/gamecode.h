#ifndef __GAMECODE_H__
#define __GAMECODE_H__

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "des.h"
#include "dhkey.h"

// using namespace ApexSec;

namespace ApexChina
{

#define		Net_Packet_Max_Size		256

struct NetPacket_St
{
	short			sPacketSize;			// net data packet size
	unsigned char	uszData[1];				// net data 
};

struct NetData_St
{
	unsigned char	uszRandNum;
	unsigned char	uszData[1];
};

// ****************************************************************************
// CGameClinet
// ****************************************************************************
class CGameClient
{
public:
	CGameClient();
	~CGameClient();

public:
	void create_a0();
	void encrypt_a0_with_PublicKey_Key0();
	
	// void send_a0();
	short get_a0( unsigned char* pData );

	void set_c0( unsigned char* pData );
	// void recv_c0();
	void decrypt_c0_with_PublicKey_Key0();
	void create_PrivateKey_Key1_with_c0();
	void encrypt_data_with_PrivateKey_Key1( const char *pbuf, int nLen );
	void get_data( unsigned char* pData );
	// void send_data();
	// void recv_data();
	int  decrypt_data_with_PrivateKey_Key1( char * pbuf, int nLen );

private:
	NetPacket_St *m_pstPacket;
	NetData_St   *m_pstData;
	char m_client_buf[Net_Packet_Max_Size];
	unsigned char m_key_a0[8];
	unsigned char m_key_c0[8];
	unsigned char m_PublicKey_Key0[8];
	unsigned char m_iv[8];
	unsigned char m_PrivateKey_Key1[8];
	CDHKey	*m_pdhkey;
	CCFBDes	m_cfbdes;
};

// ****************************************************************************
// CGameServer
// ****************************************************************************
class CGameServer
{
public:
	CGameServer();
	~CGameServer();

public:
	void create_c0();
	void encrypt_c0_with_PublicKey_Key0();
	short get_c0( unsigned char* pData );
	// void send_c0();
	
	void set_a0( unsigned char* pData );
	// void recv_a0();

	void decrypt_a0_with_PublicKey_Key0();
	void create_PrivateKey_Key1_with_a0();
	void encrypt_data_with_PrivateKey_Key1( const char * pbuf, int nLen );
	// void send_data();
	void set_data( unsigned char* pData );
	// void recv_data();
	int  decrypt_data_with_PrivateKey_Key1( char * pbuf, int nLen );

private:
	NetPacket_St *m_pstPacket;
	NetData_St   *m_pstData;
	char m_server_buf[Net_Packet_Max_Size];
	unsigned char m_key_a0[8];
	unsigned char m_key_c0[8];
	unsigned char m_PublicKey_Key0[8];
	unsigned char m_iv[8];
	unsigned char m_PrivateKey_Key1[8];
	CDHKey	*m_pdhkey;
	CCFBDes	m_cfbdes;
};

} // End of namespace ApexChina
#endif // eof #ifndef __GAMECODE_H__