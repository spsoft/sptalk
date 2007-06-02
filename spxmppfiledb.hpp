/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmppfiledb_hpp__
#define __spxmppfiledb_hpp__

#include "spxmpppersist.hpp"

class SP_XmlElementNode;

class SP_XmppFileUserDao : public SP_XmppUserDao {
public:
	SP_XmppFileUserDao( const char * baseDir );
	virtual ~SP_XmppFileUserDao();

	virtual SP_XmppUser * load( const char * username );
	virtual int store( SP_XmppUser * user );
	virtual int remove( const char * username );
	virtual int updatePassword( const char * username, const char * password );

private:
	char mBaseDir[ 128 ];
};

class SP_XmppFileRosterDao : public SP_XmppRosterDao {
public:
	SP_XmppFileRosterDao( const char * baseDir );
	virtual ~SP_XmppFileRosterDao();

	virtual SP_XmppRoster * load( const char * username );

	virtual SP_XmppRosterItem * load( const char * username, const char * jid );
	virtual int store( SP_XmppRosterItem * item );
	virtual int remove( const char * username, const char * jid );
	virtual int remove( const char * username );

private:
	static SP_XmppRosterItem * xml2item( SP_XmlElementNode * node, const char * username );

	char mBaseDir[ 128 ];
};

#endif

