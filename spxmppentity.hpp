/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmppentity_hpp__
#define __spxmppentity_hpp__

#include <time.h>

#include "spxmppdefine.hpp"

class SP_ArrayList;

class SP_XmppUser {
public:
	SP_XmppUser( const char * username );
	~SP_XmppUser();

	void setUsername( const char * username );
	const char * getUsername() const;

	void setPassword( const char * password );
	const char * getPassword() const;

	void setName( const char * name );
	const char * getName() const;

	void setEmail( const char * email );
	const char * getEmail() const;

	void setCreateTime( time_t createTime );
	time_t getCreateTime() const;

	void setModTime( time_t modTime );
	time_t getModTime() const;

private:
	SP_XmppUser( SP_XmppUser & );
	SP_XmppUser & operator=( SP_XmppUser & );

	char mUsername[ SP_XMPP_MAX_USERNAME ],
		mPassword[ SP_XMPP_MAX_PASSWORD ],
		mName[ SP_XMPP_MAX_NAME ], mEmail[ SP_XMPP_MAX_EMAIL ];
	time_t mCreateTime, mModTime;
};

class SP_XmppRosterItem {
public:
	SP_XmppRosterItem( const char * username );
	~SP_XmppRosterItem();

	void setUsername( const char * username );
	const char * getUsername() const;

	void setJid( const char * jid );
	const char * getJid() const;

	void setNickname( const char * nickname );
	const char * getNickname() const;

	void setSubType( const char * subType );
	const char * getSubType() const;
	int isSubType( const char * subType );

private:
	char mUsername[ SP_XMPP_MAX_USERNAME ], mJid[ SP_XMPP_MAX_JID ];
	char mNickname[ SP_XMPP_MAX_NAME ];
	char mSubType[ 32 ];
};

class SP_XmppRoster {
public:
	SP_XmppRoster( const char * username );
	~SP_XmppRoster();

	void append( SP_XmppRosterItem * item );

	int getCount();

	SP_XmppRosterItem * getItem( int index );
	SP_XmppRosterItem * takeItem( int index );

	SP_XmppRosterItem * getItem( const char * jid );
	SP_XmppRosterItem * takeItem( const char * jid );

private:
	int findByJid( const char * jid );

	SP_ArrayList * mList;
};

#endif

