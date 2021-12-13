#include "pch.h"
#include "dhkey.h"
#include <stdlib.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  g128 "98A4D112C3EBA27AD9EA76938B20CD5F"
#define  n128 "A18C92492EE8893EA8618812BE9C934F"

#define  g64 "CE0835F012503743"
#define  n64 "E09082D102CAC731"

#define  g512 "BDE8B97AF6E4AE453287D6A21CD9820DDC25C400D16EB99B4F25EF1A59B4250EF22E4368142572E9270CAFBF773B28825E29B62C0F6644D4754FACC9DABC0FB9"
#define  n512 "B4FEF503267797287E837152A997495F6AA340E9C60092DE7DF9339B803D2902CBF28286F5A37E92F5FE34D056BF8A327FED2F61D61E53FA2B7A463BD6D39C37"

namespace ApexChina
{

unsigned flex_unit::get( unsigned i ) const
{
  if ( i >= n ) return 0;
  return a[i];
}

void flex_unit::clear()
{
   n = 0;
}

flex_unit::flex_unit()
{
  z = 0;
  a = 0;
  n = 0;
}

flex_unit::~flex_unit()
{
  unsigned i=z;
  while (i) { i-=1; a[i] = 0; } // burn
  delete [] a;
}

void flex_unit::reserve( unsigned x )
{
  if (x > z)
  {
    unsigned * na = new unsigned[x];
    for (unsigned i=0;i<n;i+=1) na[i] = a[i];
    delete [] a;
    a = na;
    z = x;
  }
}

void flex_unit::set( unsigned i, unsigned x )
{
  if ( i < n )
  {
    a[i] = x;
    if (x==0) while (n && a[n-1]==0) n-=1; // normalise
  }
  else if ( x )
  {
    reserve(i+1);
    for (unsigned j=n;j<i;j+=1) a[j] = 0;
    a[i] = x;
    n = i+1;
  }
}

// Macros for doing double precision multiply
#define BPU ( 8*sizeof(unsigned) )       // Number of bits in an unsigned
#define lo(x) ( (x) & ((1<<(BPU/2))-1) ) // lower half of unsigned
#define hi(x) ( (x) >> (BPU/2) )         // upper half
#define lh(x) ( (x) << (BPU/2) )         // make upper half

void flex_unit::fast_mul( flex_unit &x, flex_unit &y, unsigned keep )
{
  // *this = (x*y) % (2**keep)
  unsigned i,limit = (keep+BPU-1)/BPU; // size of result in words
  reserve(limit); for (i=0; i<limit; i+=1) a[i] = 0;
  unsigned min = x.n; if (min>limit) min = limit;
  for (i=0; i<min; i+=1)
  {
    unsigned m = x.a[i];
    unsigned c = 0; // carry
    unsigned min = i+y.n; if (min>limit) min = limit;
	unsigned j=0;
    for (j=i; j<min; j+=1 )
    {
      // This is the critical loop
      // Machine dependent code could help here
      // c:a[j] = a[j] + c + m*y.a[j-i];
      unsigned w, v = a[j], p = y.a[j-i];
      v += c; c = ( v < c );
      w = lo(p)*lo(m); v += w; c += ( v < w );
      w = lo(p)*hi(m); c += hi(w); w = lh(w); v += w; c += ( v < w );
      w = hi(p)*lo(m); c += hi(w); w = lh(w); v += w; c += ( v < w );
      c += hi(p) * hi(m);
      a[j] = v;
    }
    while ( c && j<limit )
    {
      a[j] += c;
      c = a[j] < c;
      j += 1;
    }
  }

  // eliminate unwanted bits
  keep %= BPU; if (keep) a[limit-1] &= (1<<keep)-1;

   // calculate n
  while (limit && a[limit-1]==0) limit-=1;
  n = limit;
};

vlong_value::operator unsigned()
{
  return get(0);
}

int vlong_value::is_zero() const
{
  return n==0;
}

int vlong_value::test( unsigned i ) const
{ return ( get(i/BPU) & (1<<(i%BPU)) ) != 0; }

unsigned vlong_value::bits() const
{
  unsigned x = n*BPU;
  while (x && test(x-1)==0) x -= 1;
  return x;
}

int vlong_value::cf( vlong_value& x ) const
{
  if ( n > x.n ) return +1;
  if ( n < x.n ) return -1;
  unsigned i = n;
  while (i)
  {
    i -= 1;
    if ( get(i) > x.get(i) ) return +1;
    if ( get(i) < x.get(i) ) return -1;
  }
  return 0;
}

void vlong_value::shl()
{
  unsigned carry = 0;
  unsigned N = n; // necessary, since n can change
  for (unsigned i=0;i<=N;i+=1)
  {
    unsigned u = get(i);
    set(i,(u<<1)+carry);
    carry = u>>(BPU-1);
  }
}

void vlong_value::shr()
{
  unsigned carry = 0;
  unsigned i=n;
  while (i)
  {
    i -= 1;
    unsigned u = get(i);
    set(i,(u>>1)+carry);
    carry = u<<(BPU-1);
  }
}

void vlong_value::shr( unsigned x )
{
  unsigned delta = x/BPU; x %= BPU;
  for (unsigned i=0;i<n;i+=1)
  {
    unsigned u = get(i+delta);
    if (x)
    {
      u >>= x;
      u += get(i+delta+1) << (BPU-x);
    }
    set(i,u);
  }
}

void vlong_value::add( vlong_value & x )
{
  unsigned carry = 0;
  unsigned max = n; if (max<x.n) max = x.n;
  reserve(max);
  for (unsigned i=0;i<max+1;i+=1)
  {
    unsigned u = get(i);
    u = u + carry; carry = ( u < carry );
    unsigned ux = x.get(i);
    u = u + ux; carry += ( u < ux );
    set(i,u);
  }
}

void vlong_value::subtract( vlong_value & x )
{
  unsigned carry = 0;
  unsigned N = n;
  for (unsigned i=0;i<N;i+=1)
  {
    unsigned ux = x.get(i);
    ux += carry;
    if ( ux >= carry )
    {
      unsigned u = get(i);
      unsigned nu = u - ux;
      carry = nu > u;
      set(i,nu);
    }
  }
}

void vlong_value::init( unsigned x )
{
  clear();
  set(0,x);
}

void vlong_value::copy( vlong_value& x )
{
  clear();
  unsigned i=x.n;
  while (i) { i -= 1; set( i, x.get(i) ); }
}

vlong_value::vlong_value()
{
  share = 0;
}

void vlong_value::mul( vlong_value& x, vlong_value& y )
{
  fast_mul( x, y, x.bits()+y.bits() );
}

void vlong_value::divide( vlong_value& x, vlong_value& y, vlong_value& rem )
{
  init(0);
  rem.copy(x);
  vlong_value m,s;
  m.copy(y);
  s.init(1);
  while ( rem.cf(m) > 0 )
  {
    m.shl();
    s.shl();
  }
  while ( rem.cf(y) >= 0 )
  {
    while ( rem.cf(m) < 0 )
    {
      m.shr();
      s.shr();
    }
    rem.subtract( m );
    add( s );
  }
}

// Implementation of vlong

void vlong::docopy()
{
  if ( value->share )
  {
    value->share -= 1;
    vlong_value * nv = new vlong_value;
    nv->copy(*value);
    value = nv;
  }
}

int vlong::cf( const vlong x ) const
{
  int neg = negative && !value->is_zero();
  if ( neg == (x.negative && !x.value->is_zero()) )
    return value->cf( *x.value );
  else if ( neg ) return -1;
  else return +1;
}

vlong::vlong (unsigned x)
{
  value = new vlong_value;
  negative = 0;
  value->init(x);
}

vlong::vlong ( const vlong& x ) // copy constructor
{
  negative = x.negative;
  value = x.value;
  value->share += 1;
}

vlong& vlong::operator =(const vlong& x)
{
  if ( value->share ) value->share -=1; else delete value;
  value = x.value;
  value->share += 1;
  negative = x.negative;
  return *this;
}

vlong::~vlong()
{
  if ( value->share ) value->share -=1; else delete value;
}

vlong::operator unsigned () // conversion to unsigned
{
  return *value;
}

vlong& vlong::operator +=(const vlong& x)
{
  if ( negative == x.negative )
  {
    docopy();
    value->add( *x.value );
  }
  else if ( value->cf( *x.value ) >= 0 )
  {
    docopy();
    value->subtract( *x.value );
  }
  else
  {
    vlong tmp = *this;
    *this = x;
    *this += tmp;
  }
  return *this;
}

vlong& vlong::operator -=(const vlong& x)
{
  if ( negative != x.negative )
  {
    docopy();
    value->add( *x.value );
  }
  else if ( value->cf( *x.value ) >= 0 )
  {
    docopy();
    value->subtract( *x.value );
  }
  else
  {
    vlong tmp = *this;
    *this = x;
    *this -= tmp;
    negative = 1 - negative;
  }
  return *this;
}

vlong operator +( const vlong& x, const vlong& y )
{
  vlong result = x;
  result += y;
  return result;
}

vlong operator -( const vlong& x, const vlong& y )
{
  vlong result = x;
  result -= y;
  return result;
}

vlong operator *( const vlong& x, const vlong& y )
{
  vlong result;
  result.value->mul( *x.value, *y.value );
  result.negative = x.negative ^ y.negative;
  return result;
}

vlong operator /( const vlong& x, const vlong& y )
{
  vlong result;
  vlong_value rem;
  result.value->divide( *x.value, *y.value, rem );
  result.negative = x.negative ^ y.negative;
  return result;
}

vlong operator %( const vlong& x, const vlong& y )
{
  vlong result;
  vlong_value divide;
  divide.divide( *x.value, *y.value, *result.value );
  result.negative = x.negative; // not sure about this?
  return result;
}

vlong gcd( const vlong &X, const vlong &Y )
{
  vlong x=X, y=Y;
  while (1)
  {
    if ( y ==(vlong) 0 ) return x;
    x = x % y;
    if ( x == (vlong)0 ) return y;
    y = y % x;
  }
}

vlong modinv( const vlong &a, const vlong &m ) // modular inverse
// returns i in range 1..m-1 such that i*a = 1 mod m
// a must be in range 1..m-1
{
  vlong j=1,i=0,b=m,c=a,x,y;
  while ( c != (vlong)0 )
  {
    x = b / c;
    y = b - x*c;
    b = c;
    c = y;
    y = j;
    j = i - j*x;
    i = y;
  }
  if ( i < (vlong)0 )
    i += m;
  return i;
}

class monty // class for montgomery modular exponentiation
{
  vlong R,R1,m,n1;
  vlong T,k;   // work registers
  unsigned N;  // bits for R
  void mul( vlong &x, const vlong &y );
public:
  vlong exp( const vlong &x, const vlong &e );
  monty( const vlong &M );
};

monty::monty( const vlong &M )
{
  m = M;
  N = 0; R = 1; while ( R < M ) { R += R; N += 1; }
  R1 = modinv( R-m, m );
  n1 = R - modinv( m, R );
}

void monty::mul( vlong &x, const vlong &y )
{
  // T = x*y;
  T.value->fast_mul( *x.value, *y.value, N*2 );

  // k = ( T * n1 ) % R;
  k.value->fast_mul( *T.value, *n1.value, N );

  // x = ( T + k*m ) / R;
  x.value->fast_mul( *k.value, *m.value, N*2 );
  x += T;
  x.value->shr( N );

  if (x>=m) x -= m;
}

vlong monty::exp( const vlong &x, const vlong &e )
{
  vlong result = R-m, t = ( x * R ) % m;
  unsigned bits = e.value->bits();
  unsigned i = 0;
  while (1)
  {
    if ( e.value->test(i) )
      mul( result, t);
    i += 1;
    if ( i == bits ) break;
    mul( t, t );
  }
  return ( result * R1 ) % m;
}

vlong modexp( const vlong & x, const vlong & e, const vlong & m )
{
  monty me(m);
  return me.exp( x,e );
}
long 
vlong::val(
	char c
	)
{
	switch (c)
	{
		case '0':
			return (0);
		case '1':
			return (1);
		case '2':
			return (2);
		case '3':
			return (3);
		case '4':
			return (4);
		case '5':
			return (5);
		case '6':
			return (6);
		case '7':
			return (7);
		case '8':
			return (8);
		case '9':
			return (9);
		case 'A':
		case 'a':
			return (10);
		case 'B':
		case 'b':
			return (11);
		case 'C':
		case 'c':
			return (12);
		case 'D':
		case 'd':
			return (13);
		case 'E':
		case 'e':
			return (14);
		case 'F':
		case 'f':
			return (15);
		default:
			return(0);
	}
}
long vlong::strtov(const char *s,long base)
{
	    register long v;
        register long cnt = 0;
        register char c;

        if (base < 0) base = -base;
        if ((!base) || (base > 16))
                return(0);
        *this=(vlong)0;

        c = s[cnt++];
        while (((v = val(c)) < base) && ((v) || (c == '0')) )
        {
                *this= (*this)*(vlong)base + (vlong)v;
                c = s[cnt++];
        }
        cnt --;
        return(cnt);
}
void vlong::buftov( const unsigned char * s ,unsigned long size)
{
  *this=(vlong)0;
  for(unsigned long i=0;i<size/8;i++)
  {
    *this = (*this) *(vlong)256 + (vlong)s[i];
  }
}
void vlong::vtobuf(unsigned char * buf,unsigned long size)
{
	unsigned long bytes=size/8;
	memset((void*)buf,0,size/8);
	if(bytes==value->n*4)
	{
		memcpy((void*)buf,(void*)value->a,bytes);
		for(unsigned long i=0;i<bytes/2;i++)
		{
			unsigned char t=buf[i];
			buf[i]=buf[bytes-i-1];
			buf[bytes-i-1]=t;
		}
	}
}
CDHKey::CDHKey( void )
{
	g.strtov( g128, 16 );
	n.strtov( n128, 16 );
	size=128;
	srand( (unsigned) time( NULL ) );
}

CDHKey::CDHKey( unsigned long _size )
{
	if ( (_size==64) || (_size==512) )
	{
		if (_size==64)
		{
			g.strtov( g64,16 );
			n.strtov( n64,16 );
		}
		if (_size==512)
		{
			g.strtov( g512, 16 );
			n.strtov( n512, 16 );
		}
		size=_size;
	}
	else
	{
		g.strtov(g128,16);
		n.strtov(n128,16);
		size=128;
	}
	srand( (unsigned)time( NULL ) );

}
CDHKey::~CDHKey(void)
{
}

void CDHKey::CreateRand( vlong & pguid,long size,unsigned char *InterKey )
{
	unsigned long bytes=size/8;
	unsigned char *p=new unsigned char[bytes+1];
	p[bytes]=0;
	for(unsigned long i=0;i<bytes;i++)
	{
		p[i]=(rand()%255+1);
	}
	pguid.buftov(p,size);
	if(NULL!=InterKey)
		memcpy(InterKey,p,bytes);
	delete []p;
}

void CDHKey::CreateKeyPair(
	unsigned char *InterKey,
	unsigned char* pair)
{
	for (int j=0; j<100; j++)
	{
		memset( InterKey,0,size/8 );
		CreateRand( a,size );
		tInterKey=modexp(g,a,n);
		tInterKey.vtobuf(InterKey,size);
		if(NULL==pair)
			return;
		bool ifsame=true;
		for(int i=0;i<(int)size/8;i++)
		{
			if(InterKey[i]!=pair[i])
			{
				ifsame=false;
				break;
			}

		}
		if(!ifsame)
			return;
	}
}
bool CDHKey::Agree(unsigned char *EncryptionKey,unsigned char *RecipientInterKey)
{
	memset(EncryptionKey,0,size/8);
	aInterKey.buftov(RecipientInterKey,size);
	key=modexp(aInterKey,a,n);
	if(key<(vlong)1000)
		return false;
	key.vtobuf(EncryptionKey,size);
	return true;
}

};