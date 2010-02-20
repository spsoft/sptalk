/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmppconfig_hpp__
#define __spxmppconfig_hpp__

class SP_XmppConfig {
public:
	SP_XmppConfig();
	~SP_XmppConfig();

	static SP_XmppConfig * getDefault();

	void setHost( const char * host );
	const char * getHost() const;

private:
	char mHost[ 128 ];
};

#endif

