/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmpphandler_hpp__
#define __spxmpphandler_hpp__

#include "spserver/sphandler.hpp"
#include "spserver/spresponse.hpp"

class SP_XmppJid;

class SP_XmppHandler : public SP_Handler {
public:
	SP_XmppHandler();
	virtual ~SP_XmppHandler();

	virtual int start( SP_Request * request, SP_Response * response );

	// return -1 : terminate session, 0 : continue
	virtual int handle( SP_Request * request, SP_Response * response );

	virtual void error( SP_Response * response );

	virtual void timeout( SP_Response * response );

	virtual void close();

private:
	SP_Sid_t mSid;
	SP_XmppJid * mJid;	

	static int mMsgSeq;
};

class SP_XmppCompletionHandler : public SP_CompletionHandler {
public:
	SP_XmppCompletionHandler();
	~SP_XmppCompletionHandler();
	virtual void completionMessage( SP_Message * msg );
};

class SP_XmppHandlerFactory : public SP_HandlerFactory {
public:
	SP_XmppHandlerFactory();
	virtual ~SP_XmppHandlerFactory();

	virtual SP_Handler * create() const;

	virtual SP_CompletionHandler * createCompletionHandler() const;
};

#endif

