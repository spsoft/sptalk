/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <stdio.h>

#include "spxmpppersist.hpp"

//---------------------------------------------------------

SP_XmppUserDao :: ~SP_XmppUserDao()
{
}

//---------------------------------------------------------

SP_XmppRosterDao :: ~SP_XmppRosterDao()
{
}

//---------------------------------------------------------

SP_XmppPersistManager :: SP_XmppPersistManager()
{
	mUserDao = NULL;
	mRosterDao = NULL;
}

SP_XmppPersistManager :: ~SP_XmppPersistManager()
{
	if( NULL != mUserDao ) delete mUserDao;
	mUserDao = NULL;

	if( NULL != mRosterDao ) delete mRosterDao;
	mRosterDao = NULL;
}

SP_XmppPersistManager * SP_XmppPersistManager :: getDefault()
{
	static SP_XmppPersistManager defaultObject;

	return &defaultObject;
}

void SP_XmppPersistManager :: setUserDao( SP_XmppUserDao * userDao )
{
	mUserDao = userDao;
}

void SP_XmppPersistManager :: setRosterDao( SP_XmppRosterDao * rosterDao )
{
	mRosterDao = rosterDao;
}

SP_XmppUser * SP_XmppPersistManager :: findUser( const char * username )
{
	return mUserDao->load( username );
}

int SP_XmppPersistManager :: addUser( SP_XmppUser * user )
{
	return mUserDao->store( user );
}

int SP_XmppPersistManager :: addRosterItem( const char * username,
		SP_XmppRosterItem * item )
{
	return mRosterDao->store( item );
}

SP_XmppRoster * SP_XmppPersistManager :: findRoster( const char * username )
{
	return mRosterDao->load( username );
}

SP_XmppRosterItem * SP_XmppPersistManager :: findRosterItem(
		const char * username, const char * jid )
{
	return mRosterDao->load( username, jid );
}

int SP_XmppPersistManager :: delRosterItem( const char * username, const char * jid )
{
	return mRosterDao->remove( username, jid );
}


