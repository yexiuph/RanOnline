#pragma once

namespace seqrandom
{
	BOOL init ( WORD wdetail=4, WORD wcopy=4 );

	WORD get ();
	float getpercent ();
};
