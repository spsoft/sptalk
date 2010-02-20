/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "spxmppconfig.hpp"

SP_XmppConfig :: SP_XmppConfig()
{
	memset( mHost, 0, sizeof( mHost ) );
	gethostname( mHost, sizeof( mHost ) );
}

SP_XmppConfig :: ~SP_XmppConfig()
{
}

SP_XmppConfig * SP_XmppConfig :: getDefault()
{
	static SP_XmppConfig defaultObject;

	return &defaultObject;
}

void SP_XmppConfig :: setHost( const char * host )
{
	snprintf( mHost, sizeof( mHost ), "%s", host );
}

const char * SP_XmppConfig :: getHost() const
{
	return mHost;
}

