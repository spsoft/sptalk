/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <stdio.h>
#include <string.h>

#include "spxmppsession.hpp"

#include "spserver/sputils.hpp"

#include "spdict/spdictionary.hpp"

SP_XmppSession :: SP_XmppSession( const char * resource )
{
	snprintf( mResource, sizeof( mResource ), "%s", resource );
	memset( &mSid, 0, sizeof( mSid ) );
}

SP_XmppSession :: ~SP_XmppSession()
{
}

void SP_XmppSession :: setSid( SP_Sid_t sid )
{
	mSid = sid;
}

SP_Sid_t SP_XmppSession :: getSid()
{
	return mSid;
}

const char * SP_XmppSession :: getResource()
{
	return mResource;
}

//---------------------------------------------------------

SP_XmppSessionMap :: SP_XmppSessionMap( const char * username )
{
	mList = new SP_ArrayList();
	snprintf( mUsername, sizeof( mUsername ), "%s", username );
}

SP_XmppSessionMap :: ~SP_XmppSessionMap()
{
	for( int i = 0; i < mList->getCount(); i++ ) {
		delete (SP_XmppSession*)mList->getItem( i );
	}
	delete mList;

	mList = NULL;
}

const char * SP_XmppSessionMap :: getUsername() const
{
	return mUsername;
}

int SP_XmppSessionMap :: find( const char * resource )
{
	int index = -1;

	for( int i = 0; i < mList->getCount(); i++ ) {
		if( 0 == strcmp( resource, ((SP_XmppSession*)mList->getItem( i ))->getResource() ) ) {
			index = i;
		}
	}

	return index;
}

SP_XmppSession * SP_XmppSessionMap :: get( const char * resource )
{
	return get( find( resource ) );
}

SP_XmppSession * SP_XmppSessionMap :: take( const char * resource )
{
	return take( find( resource ) );
}

int SP_XmppSessionMap :: getCount() const
{
	return mList->getCount();
}

void SP_XmppSessionMap :: add( SP_XmppSession * session )
{
	mList->append( session );
}

SP_XmppSession * SP_XmppSessionMap :: get( int index ) const
{
	return (SP_XmppSession*)mList->getItem( index );
}

SP_XmppSession * SP_XmppSessionMap :: take( int index )
{
	return (SP_XmppSession*)mList->takeItem( index );
}

//---------------------------------------------------------

class SP_XmppSessionMapHandler : public SP_DictHandler {
public:
	SP_XmppSessionMapHandler();
	virtual ~SP_XmppSessionMapHandler();

	virtual int compare( const void * item1,
			const void * item2 ) const;
	virtual void destroy( void * item ) const;
};

SP_XmppSessionMapHandler :: SP_XmppSessionMapHandler()
{
}

SP_XmppSessionMapHandler :: ~SP_XmppSessionMapHandler()
{
}

int SP_XmppSessionMapHandler :: compare( const void * item1,
	const void * item2 ) const
{
	SP_XmppSessionMap * s1 = (SP_XmppSessionMap*)item1;
	SP_XmppSessionMap * s2 = (SP_XmppSessionMap*)item2;

	return strcmp( s1->getUsername(), s2->getUsername() );
}

void SP_XmppSessionMapHandler :: destroy( void * item ) const
{
	delete (SP_XmppSessionMap*)item;
}

//---------------------------------------------------------

SP_XmppSessionManager :: SP_XmppSessionManager()
{
	mDict = SP_Dictionary::newInstance( SP_Dictionary::eBTree,
		new SP_XmppSessionMapHandler() );

	pthread_mutex_init( &mMutex, NULL );
}

SP_XmppSessionManager :: ~SP_XmppSessionManager()
{
	pthread_mutex_destroy( &mMutex );
}

SP_XmppSessionManager * SP_XmppSessionManager :: getDefault()
{
	static SP_XmppSessionManager defaultObject;

	return &defaultObject;
}

int SP_XmppSessionManager :: addIfNotExist( const char * username, SP_XmppSession * session )
{
	int ret = -1;

	pthread_mutex_lock( &mMutex );

	SP_XmppSessionMap sessionMap( username );

	SP_XmppSessionMap * target = (SP_XmppSessionMap*)mDict->search( &sessionMap );

	if( NULL == target ) {
		target = new SP_XmppSessionMap( username );
		mDict->insert( target );
	}

	SP_XmppSession * oldSession = target->get( session->getResource() );

	if( NULL == oldSession ) {
		ret = 0;
		target->add( session );
	}

	pthread_mutex_unlock( &mMutex );

	return ret;
}

int SP_XmppSessionManager :: getSid( const char * username, const char * resource, SP_Sid_t * sid )
{
	int ret = -1;

	pthread_mutex_lock( &mMutex );

	SP_XmppSessionMap sessionMap( username );

	SP_XmppSessionMap * target = (SP_XmppSessionMap*)mDict->search( &sessionMap );

	if( NULL != target ) {
		SP_XmppSession * session = target->get( resource );
		if( NULL != session ) {
			ret = 0;
			if( NULL != sid ) * sid = session->getSid();
		}
	}

	pthread_mutex_unlock( &mMutex );

	return ret;
}

int SP_XmppSessionManager :: getSidList( const char * username, SP_SidList * sidList )
{
	int ret = -1;

	pthread_mutex_lock( &mMutex );

	SP_XmppSessionMap sessionMap( username );

	SP_XmppSessionMap * target = (SP_XmppSessionMap*)mDict->search( &sessionMap );

	if( NULL != target ) {
		ret = 0;

		for( int i = 0; i < target->getCount(); i++ ) {
			sidList->add( target->get(i)->getSid() );
		}
	}

	pthread_mutex_unlock( &mMutex );

	return ret;
}

int SP_XmppSessionManager :: remove( const char * username, const char * resource )
{
	int ret = -1;

	pthread_mutex_lock( &mMutex );

	SP_XmppSessionMap sessionMap( username );

	SP_XmppSessionMap * target = (SP_XmppSessionMap*)mDict->search( &sessionMap );

	if( NULL != target ) {
		SP_XmppSession * session = target->take( resource );
		if( NULL != session ) {
			ret = 0;
			delete session;
		}

		if( target->getCount() <= 0 ) {
			mDict->remove( target );
			delete target;
		}
	}

	pthread_mutex_unlock( &mMutex );

	return ret;
}

