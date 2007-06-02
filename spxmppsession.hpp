/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmppsession_hpp__
#define __spxmppsession_hpp__

#include <pthread.h>

#include "spresponse.hpp"

#include "spxmppdefine.hpp"

class SP_ArrayList;
class SP_Dictionary;

class SP_XmppSession {
public:
	SP_XmppSession( const char * resource );
	~SP_XmppSession();

	void setSid( SP_Sid_t sid );
	SP_Sid_t getSid();

	const char * getResource();

private:
	SP_Sid_t mSid;
	char mResource[ SP_XMPP_MAX_RESOURCE ];
};

class SP_XmppSessionMap {
public:
	SP_XmppSessionMap( const char * username );
	~SP_XmppSessionMap();

	const char * getUsername() const;

	SP_XmppSession * get( const char * resource );
	SP_XmppSession * take( const char * resource );

	int getCount() const;
	void add( SP_XmppSession * session );
	SP_XmppSession * get( int index ) const;
	SP_XmppSession * take( int index );

private:
	int find( const char * resource );

	SP_ArrayList * mList;
	char mUsername[ SP_XMPP_MAX_USERNAME ];
};

class SP_XmppSessionManager {
public:
	SP_XmppSessionManager();
	~SP_XmppSessionManager();

	static SP_XmppSessionManager * getDefault();

	// 0 : OK, -1 : conflict
	int addIfNotExist( const char * username, SP_XmppSession * session );

	int remove( const char * username, const char * resource );

	// 0 : OK, -1 : not found
	int getSid( const char * username, const char * resource,
			SP_Sid_t * sid = NULL );

	// 0 : OK, -1 : not found
	int getSidList( const char * username, SP_SidList * sidList );

private:
	SP_Dictionary * mDict;

	pthread_mutex_t mMutex;
};

#endif

