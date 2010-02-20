/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "spxmppentity.hpp"

#include "spserver/sputils.hpp"  // in spserver, for SP_ArrayList

SP_XmppUser :: SP_XmppUser( const char * username )
{
	snprintf( mUsername, sizeof( mUsername ), "%s", username );

	memset( mPassword, 0, sizeof( mPassword ) );
	memset( mName, 0, sizeof( mName ) );
	memset( mEmail, 0, sizeof( mEmail ) );
	mCreateTime = time( &mModTime );
}

SP_XmppUser :: ~SP_XmppUser()
{
}

void SP_XmppUser :: setUsername( const char * username )
{
	snprintf( mUsername, sizeof( mUsername ), "%s", username );
}

const char * SP_XmppUser :: getUsername() const
{
	return mUsername;
}

void SP_XmppUser :: setPassword( const char * password )
{
	snprintf( mPassword, sizeof( mPassword ), "%s", password );
}

const char * SP_XmppUser :: getPassword() const
{
	return mPassword;
}

void SP_XmppUser :: setName( const char * name )
{
	snprintf( mName, sizeof( mName ), "%s", name );
}

const char * SP_XmppUser :: getName() const
{
	return mName;
}

void SP_XmppUser :: setEmail( const char * email )
{
	snprintf( mEmail, sizeof( mEmail ), "%s", email );
}

const char * SP_XmppUser :: getEmail() const
{
	return mEmail;
}

void SP_XmppUser :: setCreateTime( time_t createTime )
{
	mCreateTime = createTime;
}

time_t SP_XmppUser :: getCreateTime() const
{
	return mCreateTime;
}

void SP_XmppUser :: setModTime( time_t modTime )
{
	mModTime = modTime;
}

time_t SP_XmppUser :: getModTime() const
{
	return mModTime;
}

//---------------------------------------------------------

SP_XmppRosterItem :: SP_XmppRosterItem( const char * username )
{
	snprintf( mUsername, sizeof( mUsername ), "%s", username );
	memset( mJid, 0, sizeof( mJid ) );
	memset( mNickname, 0, sizeof( mNickname ) );

	snprintf( mSubType, sizeof( mSubType ), "none" );
}

SP_XmppRosterItem :: ~SP_XmppRosterItem()
{
}

void SP_XmppRosterItem :: setUsername( const char * username )
{
	snprintf( mUsername, sizeof( mUsername ), "%s", username );
}

const char * SP_XmppRosterItem :: getUsername() const
{
	return mUsername;
}

void SP_XmppRosterItem :: setJid( const char * jid )
{
	jid = jid ? jid : "";
	snprintf( mJid, sizeof( mJid ), "%s", jid );
}

const char * SP_XmppRosterItem :: getJid() const
{
	return mJid;
}

void SP_XmppRosterItem :: setNickname( const char * nickname )
{
	nickname = nickname ? nickname : "";
	snprintf( mNickname, sizeof( mNickname ), "%s", nickname );
}

const char * SP_XmppRosterItem :: getNickname() const
{
	return mNickname;
}

void SP_XmppRosterItem :: setSubType( const char * subType )
{
	snprintf( mSubType, sizeof( mSubType ), "%s", subType );
}

const char * SP_XmppRosterItem :: getSubType() const
{
	return mSubType;
}

int SP_XmppRosterItem :: isSubType( const char * subType )
{
	return 0 == strcmp( mSubType, subType );
}

//---------------------------------------------------------

SP_XmppRoster :: SP_XmppRoster( const char * username )
{
	mList = new SP_ArrayList();
}

SP_XmppRoster :: ~SP_XmppRoster()
{
	for( int i = 0; i < mList->getCount(); i++ ) {
		delete (SP_XmppRosterItem*)mList->getItem(i);
	}

	delete mList;
	mList = NULL;
}

void SP_XmppRoster :: append( SP_XmppRosterItem * item )
{
	mList->append( item );
}

int SP_XmppRoster :: getCount()
{
	return mList->getCount();
}

SP_XmppRosterItem * SP_XmppRoster :: getItem( int index )
{
	return (SP_XmppRosterItem*)mList->getItem( index );
}

SP_XmppRosterItem * SP_XmppRoster :: takeItem( int index )
{
	return (SP_XmppRosterItem*)mList->takeItem( index );
}

int SP_XmppRoster :: findByJid( const char * jid )
{
	int index = -1;

	for( int i = 0; i < mList->getCount() && index < 0; i++ ) {
		SP_XmppRosterItem * item = (SP_XmppRosterItem*)mList->getItem(i);
		if( 0 == strcmp( jid, item->getJid() ) ) index = i;
	}

	return index;
}

SP_XmppRosterItem * SP_XmppRoster :: getItem( const char * jid )
{
	return (SP_XmppRosterItem*)mList->getItem( findByJid( jid ) );
}

SP_XmppRosterItem * SP_XmppRoster :: takeItem( const char * jid )
{
	return (SP_XmppRosterItem*)mList->takeItem( findByJid( jid ) );
}

