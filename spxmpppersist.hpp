/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmpppersist_hpp__
#define __spxmpppersist_hpp__

class SP_XmppUser;
class SP_XmppRosterItem;
class SP_XmppRoster;

class SP_XmppUserDao {
public:
	virtual ~SP_XmppUserDao();

	virtual SP_XmppUser * load( const char * username ) = 0;
	virtual int store( SP_XmppUser * user ) = 0;
	virtual int remove( const char * username ) = 0;
	virtual int updatePassword( const char * username, const char * password ) = 0;
};

class SP_XmppRosterDao {
public:
	virtual ~SP_XmppRosterDao();

	virtual SP_XmppRoster * load( const char * username ) = 0;

	virtual SP_XmppRosterItem * load( const char * username, const char * jid ) = 0;
	virtual int store( SP_XmppRosterItem * item ) = 0;
	virtual int remove( const char * username, const char * jid ) = 0;
	virtual int remove( const char * username ) = 0;
};

class SP_XmppPersistManager {
public:
	SP_XmppPersistManager();
	~SP_XmppPersistManager();

	static SP_XmppPersistManager * getDefault();

	void setUserDao( SP_XmppUserDao * userDao );
	void setRosterDao( SP_XmppRosterDao * rosterDao );

	SP_XmppUser * findUser( const char * username );
	int addUser( SP_XmppUser * user );

	int addRosterItem( const char * username, SP_XmppRosterItem * item );
	int delRosterItem( const char * username, const char * jid );
	SP_XmppRoster * findRoster( const char * username );

	SP_XmppRosterItem * findRosterItem( const char * username, const char * jid );

private:
	SP_XmppUserDao * mUserDao;
	SP_XmppRosterDao * mRosterDao;
};

#endif

