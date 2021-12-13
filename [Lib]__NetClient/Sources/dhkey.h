/////////////////////////////////////////////////////////////////
//目前只支持 64(bit),128(bit),512(bit)
//最后的key以16进制存储在缓冲区;
//注意,缓冲区,必须足够容纳key;
//
//64(bit)   需 8(字节)
//128(bit)  需 16(字节)
//512(bit)  需 64(字节)
//
//缺省  128(bit)  需16(字节)
/////////////////////////////////////////////////////////////////
#ifndef dhkey_h
#define dhkey_h
#include <string.h>

namespace ApexChina
{

class flex_unit // Provides storage allocation and index checking
{
public:
  unsigned * a; // array of units
  unsigned z; // units allocated
  unsigned n; // used units (read-only)
  flex_unit();
  ~flex_unit();
  void clear(); // set n to zero
  unsigned get( unsigned i ) const;     // get ith unsigned
  void set( unsigned i, unsigned x );   // set ith unsigned
  void reserve( unsigned x );           // storage hint

  // Time critical routine
  void fast_mul( flex_unit &x, flex_unit &y, unsigned n );
};

class vlong_value : public flex_unit
{
  public:
  unsigned share; // share count, used by vlong to delay physical copying
  int is_zero() const;
  int test( unsigned i ) const;
  unsigned bits() const;
  int cf( vlong_value& x ) const;
  void shl();
  void shr();
  void shr( unsigned n );
  void add( vlong_value& x );
  void subtract( vlong_value& x );
  void init( unsigned x );
  void copy( vlong_value& x );
  operator unsigned(); // Unsafe conversion to unsigned
  vlong_value();
  void mul( vlong_value& x, vlong_value& y );
  void divide( vlong_value& x, vlong_value& y, vlong_value& rem );
};

class vlong // very long integer - can be used like long
{
public:
  // Standard arithmetic operators
  friend vlong operator +( const vlong& x, const vlong& y );
  friend vlong operator -( const vlong& x, const vlong& y );
  friend vlong operator *( const vlong& x, const vlong& y );
  friend vlong operator /( const vlong& x, const vlong& y );
  friend vlong operator %( const vlong& x, const vlong& y );
  vlong& operator +=( const vlong& x );
  vlong& operator -=( const vlong& x );

  // Standard comparison operators
  friend inline int operator !=( const vlong& x, const vlong& y ){ return x.cf( y ) != 0; }
  friend inline int operator ==( const vlong& x, const vlong& y ){ return x.cf( y ) == 0; };
 // friend inline int operator ==( const vlong& x, const unsigned long & y1 ){ vlong y(y1);return x.cf( y ) == 0; }
  friend inline int operator >=( const vlong& x, const vlong& y ){ return x.cf( y ) >= 0; }
  friend inline int operator <=( const vlong& x, const vlong& y ){ return x.cf( y ) <= 0; }
  friend inline int operator > ( const vlong& x, const vlong& y ){ return x.cf( y ) > 0; }
  friend inline int operator < ( const vlong& x, const vlong& y ){ return x.cf( y ) < 0; }



  ///
  void buftov( const unsigned char * s ,unsigned long size);
  long strtov(const char *s,long base);
  void vtobuf(unsigned char * buf,unsigned long size);

  // Construction and conversion operations

  vlong ( unsigned x=0);
  vlong ( const vlong& x );
  ~vlong();
  operator unsigned ();
  vlong& operator =(const vlong& x);
 
  vlong_value * value;

private:
  int negative;
  int cf( const vlong x ) const;
  void docopy();
  long val(char c	);
  friend class monty;
};

vlong modexp( const vlong & x, const vlong & e, const vlong & m ); // m must be odd
vlong gcd( const vlong &X, const vlong &Y ); // greatest common denominator
vlong modinv( const vlong &a, const vlong &m ); // modular inverse

class CDHKey
{
public:
	CDHKey( void );
	// 64,128,512
	CDHKey( unsigned long _size );
	~CDHKey(void);
	void CreateKeyPair(unsigned char* InterKey,unsigned char* pair=NULL);
	bool Agree(unsigned char *EncryptionKey,unsigned char*RecipientInterKey);
private:
	//设置,将要产生得key得长度,只能是,64,128,512. 缺省是,128;
	void CreateRand(vlong &pguid,long size,unsigned char *InterKey=NULL);
	vlong g;
	vlong n;
	vlong a;

	vlong tInterKey;
	vlong key;
	vlong aInterKey;

	unsigned long size;
};

} // end of namespace ApexChina
#endif
