#pragma once

static bool while_assert ( bool a )
{
	GASSERT ( a && "while_limit: exceeded iteration limit" );
	return (a);
}

#define UNIQUE_VAR(x) safety_limit##x
#define _while_limit(a,b,c) \
	GASSERT(b>0"while_limit: limit is zero or negative");\
	int UNIQUE_VAR(c)=b;\
	while(a&&while_assert(--UNIQUE_VAR(c)>=0))

#define while_limit(a,b) _while_limit(a,b,__COUNTER__)
