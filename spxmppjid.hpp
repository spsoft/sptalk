/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spxmppjid_hpp__
#define __spxmppjid_hpp__

#include "spxmppdefine.hpp"

class SP_XmppJid {
public:
	SP_XmppJid();
	SP_XmppJid( const char * jid );
	~SP_XmppJid();

	void setJid( const char * jid );

	void setNode( const char * node );
	const char * getNode() const;

	void setDomain( const char * domain );
	const char * getDomain() const;

	void setResource( const char * resource );
	const char * getResource() const;

	const char * getBareJid() const;
	const char * getFullJid() const;

private:
	SP_XmppJid( SP_XmppJid & );
	SP_XmppJid & operator=( SP_XmppJid & );

	void init( const char * jid );
	void reset();

	void updateJid();

	char mNode[ SP_XMPP_MAX_NODE ], mDomain[ SP_XMPP_MAX_DOMAIN ],
		mResource[ SP_XMPP_MAX_RESOURCE ];
	char mBareJid[ SP_XMPP_MAX_JID ], mFullJid[ SP_XMPP_MAX_JID ];
};

#endif

