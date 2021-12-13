#include "pch.h"
#include "gamecode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// unsigned char g_uszTransData[Net_Packet_Max_Size];

// ****************************************************************************
// CGameClinet
// ****************************************************************************
using namespace ApexChina;

CGameClient::CGameClient()
{
	m_pdhkey = new CDHKey(64);
	m_pstPacket = (NetPacket_St *) m_client_buf;
	m_pstData   = (NetData_St *) m_pstPacket->uszData;
	memset( m_key_a0, 0, sizeof(m_key_a0) );
	memset( m_key_c0, 0, sizeof(m_key_c0) );
	for ( int i=0; i<8; i++ )
		m_PublicKey_Key0[i] = i;
	memset( m_PrivateKey_Key1, 0, sizeof(m_PrivateKey_Key1) );	
	memset( m_iv, 0, sizeof(m_iv) );
	srand( (unsigned) time(NULL) );
}
CGameClient::~CGameClient()
{
	if ( m_pdhkey != NULL )
		delete m_pdhkey;
}

void CGameClient::create_a0()
{
	m_pdhkey->CreateKeyPair( m_key_a0 );
}

void CGameClient::encrypt_a0_with_PublicKey_Key0()
{
	m_pstData->uszRandNum = (unsigned char)rand();
	memcpy( m_pstData->uszData, m_key_a0, sizeof(m_key_a0) );

	// encrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.encrypt( (unsigned char *)m_pstData, (unsigned char *)m_pstData
		, sizeof(m_pstData->uszRandNum) + sizeof(m_key_a0)
		, m_PublicKey_Key0, m_iv );

	// fill in packet header
	m_pstPacket->sPacketSize = sizeof(m_pstPacket->sPacketSize) + sizeof(m_pstData->uszRandNum) + sizeof(m_key_a0);
}

short CGameClient::get_a0( unsigned char* pData )
{
	memcpy( pData, m_pstPacket, m_pstPacket->sPacketSize );
	return m_pstPacket->sPacketSize;
}

/*
void CGameClient::send_a0()
{
	memcpy( pData, m_pstPacket, m_pstPacket->sPacketSize );
}
*/

void CGameClient::set_c0( unsigned char* pData )
{
	memcpy( m_client_buf, pData, Net_Packet_Max_Size );
}

/*
void CGameClient::recv_c0()
{
	memcpy( m_client_buf, g_uszTransData, Net_Packet_Max_Size );
}
*/

void CGameClient::decrypt_c0_with_PublicKey_Key0()
{
	// decrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.decrypt( (unsigned char *)m_pstData, (unsigned char *)m_pstData, m_pstPacket->sPacketSize, m_PublicKey_Key0, m_iv );

	memcpy( m_key_c0, m_pstData->uszData, sizeof(m_key_c0) );
}
void CGameClient::create_PrivateKey_Key1_with_c0()
{
	m_pdhkey->Agree( m_PrivateKey_Key1, m_key_c0 );
}
void CGameClient::encrypt_data_with_PrivateKey_Key1( const char *pbuf, int nLen )
{
	m_pstData->uszRandNum = (unsigned char )rand();
	memcpy( m_pstData->uszData, pbuf, nLen );

	// encrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.encrypt( (unsigned char *)m_pstData, (unsigned char *)m_pstData
		, sizeof(m_pstData->uszRandNum) + nLen
		, m_PrivateKey_Key1, m_iv );

	// fill in packet header
	m_pstPacket->sPacketSize = sizeof(m_pstPacket->sPacketSize) + sizeof(m_pstData->uszRandNum) + nLen;
}

void CGameClient::get_data( unsigned char* pData )
{
	memcpy( 
		pData,
		m_pstPacket,
		m_pstPacket->sPacketSize );
}

/*
void CGameClient::send_data()
{
	memcpy( g_uszTransData, m_pstPacket, m_pstPacket->sPacketSize );
}
*/

/*
void CGameClient::recv_data()
{
	memcpy( m_client_buf, g_uszTransData, Net_Packet_Max_Size );
}
*/

int CGameClient::decrypt_data_with_PrivateKey_Key1( char * pbuf, int nLen )
{
	// decrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.decrypt( (unsigned char *)m_pstData, (unsigned char *)m_pstData, m_pstPacket->sPacketSize, m_PrivateKey_Key1, m_iv );

	int nRecvLen = m_pstPacket->sPacketSize - sizeof(m_pstPacket->sPacketSize) - sizeof(m_pstData->uszRandNum);
	if ( nRecvLen > nLen )
		return -1;

	memcpy( pbuf, m_pstData->uszData, nRecvLen );
	return nRecvLen;
}

// ****************************************************************************
// CGameServer
// ****************************************************************************
CGameServer::CGameServer()
{
	m_pdhkey = new CDHKey(64);
	m_pstPacket = (NetPacket_St *)m_server_buf;
	m_pstData   = (NetData_St *)m_pstPacket->uszData;
	memset( m_key_a0, 0, sizeof(m_key_a0) );
	memset( m_key_c0, 0, sizeof(m_key_c0) );
	for ( int i=0; i<8; i++ )
		m_PublicKey_Key0[i] = i;
	memset( m_PrivateKey_Key1, 0, 8 );
	memset( m_iv, 0, 8 );
	srand( (unsigned)time(NULL) );
}
CGameServer::~CGameServer()
{
	if ( m_pdhkey != NULL )
		delete m_pdhkey;
}

void CGameServer::create_c0()
{
	m_pdhkey->CreateKeyPair( m_key_c0 );
}

void CGameServer::encrypt_c0_with_PublicKey_Key0()
{
	m_pstData->uszRandNum = (unsigned char) rand();
	memcpy( m_pstData->uszData, m_key_c0, sizeof(m_key_c0) );

	// encrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.encrypt(
		(unsigned char *) m_pstData,
		(unsigned char *) m_pstData,
		sizeof(m_pstData->uszRandNum) + sizeof(m_key_c0),
		m_PublicKey_Key0,
		m_iv );
	// fill in packet header
	m_pstPacket->sPacketSize = sizeof(m_pstPacket->sPacketSize) + sizeof(m_pstData->uszRandNum) + sizeof(m_key_c0);
}

short CGameServer::get_c0( unsigned char* pData )
{
	memcpy( pData, m_pstPacket, m_pstPacket->sPacketSize );
	return m_pstPacket->sPacketSize;
}

/*
void CGameServer::send_c0()
{
	memcpy( g_uszTransData, m_pstPacket, m_pstPacket->sPacketSize );
}
*/

void CGameServer::set_a0( unsigned char* pData )
{
	memcpy( m_server_buf, pData, Net_Packet_Max_Size );
}

/*
void CGameServer::recv_a0()
{
	memcpy( m_server_buf, g_uszTransData, Net_Packet_Max_Size );
}
*/

void CGameServer::decrypt_a0_with_PublicKey_Key0()
{
	// decrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.decrypt( 
		(unsigned char *) m_pstData,
		(unsigned char *) m_pstData,
		m_pstPacket->sPacketSize,
		m_PublicKey_Key0,
		m_iv );

	memcpy( m_key_a0, m_pstData->uszData, sizeof(m_key_a0) );
}

void CGameServer::create_PrivateKey_Key1_with_a0()
{
	m_pdhkey->Agree( m_PrivateKey_Key1, m_key_a0 );
}

void CGameServer::encrypt_data_with_PrivateKey_Key1( const char * pbuf, int nLen )
{
	m_pstData->uszRandNum = (unsigned char )rand();
	memcpy( m_pstData->uszData, pbuf, nLen );

	// encrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.encrypt( (unsigned char *)m_pstData, (unsigned char *)m_pstData
		, sizeof(m_pstData->uszRandNum) + nLen
		, m_PrivateKey_Key1, m_iv );

	// fill in packet header
	m_pstPacket->sPacketSize = sizeof(m_pstPacket->sPacketSize) + sizeof(m_pstData->uszRandNum) + nLen;
}

//void CGameServer::send_data()
//{
//	memcpy( g_uszTransData, m_pstPacket, m_pstPacket->sPacketSize );
//}

void CGameServer::set_data( unsigned char* pData )
{
	memcpy( m_server_buf, pData, Net_Packet_Max_Size );
}

/*
void CGameServer::recv_data()
{
	memcpy( m_server_buf, g_uszTransData, Net_Packet_Max_Size );
}
*/

int CGameServer::decrypt_data_with_PrivateKey_Key1( char * pbuf, int nLen )
{
	// decrypt data
	memset( m_iv, 0, sizeof(m_iv) );
	m_cfbdes.decrypt( (unsigned char *)m_pstData, (unsigned char *)m_pstData, m_pstPacket->sPacketSize, m_PrivateKey_Key1, m_iv );

	int nRecvLen = m_pstPacket->sPacketSize - sizeof(m_pstPacket->sPacketSize) - sizeof(m_pstData->uszRandNum);
	if ( nRecvLen > nLen )
		return -1;

	memcpy( pbuf, m_pstData->uszData, nRecvLen );
	return nRecvLen;
}
