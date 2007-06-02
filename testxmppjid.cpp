/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */



#include <stdio.h>

#include "spxmppjid.hpp"

int main( int argc, char * argv[] )
{
	char * jidArray [] = {
		"user@domain.com/resouce", "user@domain.com",
		"domain.com", "domain.com/resouce",
		"user@", "user@domain.com/"
	};

	for( int i = 0; i < (int)( sizeof( jidArray ) / sizeof( jidArray[0] ) ); i++ ) {
		SP_XmppJid jid( jidArray[i] );

		printf( "%s -- bard: <%s>, full: <%s>\n", jidArray[i],
			jid.getBareJid(), jid.getFullJid() );
	}

	return 0;
}

