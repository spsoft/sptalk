/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "spxmppjid.hpp"

SP_XmppJid :: SP_XmppJid()
{
	reset();
}

SP_XmppJid :: SP_XmppJid( const char * jid )
{
	reset();
	init( jid );
}

SP_XmppJid :: ~SP_XmppJid()
{
	reset();
}

void SP_XmppJid :: reset()
{
	memset( mNode, 0, sizeof( mNode ) );
	memset( mDomain, 0, sizeof( mDomain ) );
	memset( mResource, 0, sizeof( mResource ) );
	memset( mBareJid, 0, sizeof( mBareJid ) );
	memset( mFullJid, 0, sizeof( mFullJid ) );
}

void SP_XmppJid :: init( const char * jid )
{
	size_t len = 0;
	const char * atPos = strchr( jid, '@' );
	const char * slashPos = strchr( jid, '/' );

	if( NULL != atPos ) {
		// <node>@[<domain>/<resource>]
		len = atPos - jid + 1;
		len = len > sizeof( mNode ) ? sizeof( mNode ) : len;
		strncpy( mNode, jid, len - 1 );
		mNode[ len - 1 ] = '\0';

		if( NULL != slashPos ) {
			// <node>@<domain>/[<resource>]
			len = slashPos - atPos - 1 + 1;
			len = len > sizeof( mDomain ) ? sizeof( mDomain ) : len;
			strncpy( mDomain, atPos + 1, len - 1 );
			mDomain[ len - 1 ] = '\0';
		} else {
			// <node>@<domain>
			strncpy( mDomain, atPos + 1, sizeof( mDomain ) - 1 );
		}
	} else {
		if( NULL != slashPos ) {
			// <domain>/[<resouce>]
			len = slashPos - jid + 1;
			len = len > sizeof( mDomain ) ? sizeof( mDomain ) : len;
			strncpy( mDomain, jid, len - 1 );
			mDomain[ len - 1 ] = '\0';
		} else {
			// <domain>
			strncpy( mDomain, jid, sizeof( mDomain ) - 1 );
		}
	}

	if( NULL != slashPos ) {
		strncpy( mResource, slashPos + 1, sizeof( mResource ) - 1 );
	}

	updateJid();
}

void SP_XmppJid :: setJid( const char * jid )
{
	reset();
	init( jid );
}

void SP_XmppJid :: setNode( const char * node )
{
	snprintf( mNode, sizeof( mNode ), "%s", node );
	updateJid();
}

const char * SP_XmppJid :: getNode() const
{
	return mNode;
}

void SP_XmppJid :: setDomain( const char * domain )
{
	snprintf( mDomain, sizeof( mDomain ), "%s", domain );
	updateJid();
}

const char * SP_XmppJid :: getDomain() const
{
	return mDomain;
}

void SP_XmppJid :: setResource( const char * resource )
{
	snprintf( mResource, sizeof( mResource ), "%s", resource );
	updateJid();
}

const char * SP_XmppJid :: getResource() const
{
	return mResource;
}

void SP_XmppJid :: updateJid()
{
	snprintf( mBareJid, sizeof( mBareJid ), "%s%s%s",
		mNode[0] ? mNode : "", mNode[0] && mDomain[0] ? "@" : "",
		mDomain[0] ? mDomain : "" );

	snprintf( mFullJid, sizeof( mFullJid ), "%s%s%s%s%s",
		mNode, mNode[0] && mDomain[0] ? "@" : "",
		mDomain, mResource[0] ? "/" : "", mResource );
}

const char * SP_XmppJid :: getBareJid() const
{
	return mBareJid;
}

const char * SP_XmppJid :: getFullJid() const
{
	return mFullJid;
}

