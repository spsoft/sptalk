/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <string.h>
#include <stdio.h>

#include "spxmpphandler.hpp"
#include "spxmppdecoder.hpp"
#include "spxmppjid.hpp"
#include "spxmpppacket.hpp"
#include "spxmppconfig.hpp"
#include "spxmppaction.hpp"
#include "spxmppsession.hpp"

#include "sprequest.hpp"
#include "spresponse.hpp"
#include "spbuffer.hpp"

#include "spxmlnode.hpp"
#include "spdomparser.hpp"

//---------------------------------------------------------

int SP_XmppHandler :: mMsgSeq = 0;

SP_XmppHandler :: SP_XmppHandler()
{
	memset( &mSid, 0, sizeof( mSid ) );

	mJid = new SP_XmppJid();
}

SP_XmppHandler :: ~SP_XmppHandler()
{
	if( NULL != mJid ) delete mJid;
	mJid = NULL;
}

int SP_XmppHandler :: start( SP_Request * request, SP_Response * response )
{
	request->setMsgDecoder( new SP_XmppDecoder() );

	mSid = response->getFromSid();

	char buffer[ 1024 ] = { 0 };
	snprintf( buffer, sizeof( buffer ),
			"<stream:stream "
			"xmlns:stream=\"http://etherx.jabber.org/streams\" "
			"id=\"%d.%d\" xmlns=\"jabber:client\" from=\"%s\">",
			mSid.mKey, mSid.mSeq, SP_XmppConfig::getDefault()->getHost() );
	response->getReply()->getMsg()->append( buffer );
	response->getReply()->setCompletionKey( ++mMsgSeq );

	return 0;
}

int SP_XmppHandler :: handle( SP_Request * request, SP_Response * response )
{
	SP_XmppDecoder * decoder = (SP_XmppDecoder*)request->getMsgDecoder();
	SP_XmlNodeList * list = decoder->getElementList();

	for( ; list->getLength() > 0; ) {
		SP_XmlElementNode * node = (SP_XmlElementNode*)list->take( 0 );
		SP_XmlDomBuffer domBuffer( node );
		//printf( "========================= process\n%s", domBuffer.getBuffer() );

		if( 0 == strcmp( "iq", node->getName() ) ) {
			SP_XmppIQ iqPacket( node );

			if( iqPacket.isType( "get" ) && iqPacket.isQueryNS( "jabber:iq:auth" ) ) {
				SP_XmppAction::doAuthGet( &iqPacket, response->getReply()->getMsg() );
			} else if( iqPacket.isType( "set" ) && iqPacket.isQueryNS( "jabber:iq:auth" ) ) {
				if( 0 == SP_XmppAction::doAuthSet( &iqPacket, response->getReply()->getMsg() ) ) {
					mJid->setNode( iqPacket.getQueryParam( "username" ) );
					mJid->setDomain( SP_XmppConfig::getDefault()->getHost() );
					mJid->setResource( iqPacket.getQueryParam( "resource" ) );

					SP_XmppSession * session = new SP_XmppSession( mJid->getResource() );
					session->setSid( response->getFromSid() );
					SP_XmppSessionManager::getDefault()->addIfNotExist(
						mJid->getNode(), session );
				}
			} else if( iqPacket.isType( "get" ) && iqPacket.isQueryNS( "jabber:iq:register" ) ) {
				SP_XmppAction::doRegGet( &iqPacket, response->getReply()->getMsg() );
			} else if( iqPacket.isType( "set" ) && iqPacket.isQueryNS( "jabber:iq:register" ) ) {
				SP_XmppAction::doRegSet( &iqPacket, response->getReply()->getMsg() );
			} else if( iqPacket.isType( "set" ) && iqPacket.isQueryNS( "jabber:iq:roster" ) ) {
				SP_XmppAction::doRosterSet( &iqPacket, response->getReply()->getMsg(), mJid );
			} else if( iqPacket.isType( "get" ) && iqPacket.isQueryNS( "jabber:iq:roster" ) ) {
				SP_XmppAction::doRosterGet( &iqPacket, response->getReply()->getMsg(), mJid );
			} else if( iqPacket.isType( "get" ) && iqPacket.isQueryNS( "jabber:iq:agents" ) ) {
				SP_XmppAction::doAgentsGet( &iqPacket, response->getReply()->getMsg(), mJid );
			} else if( iqPacket.isType( "get" ) && iqPacket.isQueryNS( "jabber:iq:browse" ) ) {
				SP_XmppAction::doBrowseGet( &iqPacket, response->getReply()->getMsg(), mJid );
			} else if( iqPacket.isType( "get" ) && iqPacket.isQueryNS( "jabber:iq:private" ) ) {
				SP_XmppAction::doPrivateGet( &iqPacket, response->getReply()->getMsg(), mJid );
			} else {
				SP_XmppAction::doUnavailableGet( &iqPacket, response->getReply()->getMsg(), mJid );
				printf( "unavailable : %s\n", domBuffer.getBuffer() );
			}
		} else if( 0 == strcmp( "presence", node->getName() ) ) {
			printf( "presence: %s\n", domBuffer.getBuffer() );

			SP_XmppPresence prPacket( node );

			if( prPacket.isType( "subscribe" ) ) {
				SP_XmppAction::doPresenceAsk( &prPacket, response, mJid );
			} else if( prPacket.isType( "subscribed" ) ) {
				SP_XmppAction::doPresenceSub( &prPacket, response, mJid );
			} else if( prPacket.isType( "unsubscribe" ) ) {
				SP_XmppAction::doPresenceUnsub( &prPacket, response, mJid );
			} else {
				//SP_XmppAction::doPresencePub( &prPacket, response, mJid );
				printf( "pub : %s\n", domBuffer.getBuffer() );
			}
		} else if( 0 == strcmp( "message", node->getName() ) ) {
			SP_XmppPacket packet( node );
			if( packet.isType( "chat" ) ) {
				SP_XmppAction::doMessageChat( &packet, node, response, mJid );
			} else {
				printf( "ignore: %s\n", domBuffer.getBuffer() );
			}
		} else {
			printf( "ignore: %s\n", domBuffer.getBuffer() );
			delete node;
		}
	}

	if( decoder->isClose() ) response->getReply()->getMsg()->append( "</stream:stream>" );

	return 0;
}

void SP_XmppHandler :: error( SP_Response * response )
{
}

void SP_XmppHandler :: timeout( SP_Response * response )
{
}

void SP_XmppHandler :: close()
{
	SP_XmppSessionManager::getDefault()->remove(
		mJid->getNode(), mJid->getResource() );
}

//---------------------------------------------------------

SP_XmppCompletionHandler :: SP_XmppCompletionHandler()
{
}

SP_XmppCompletionHandler :: ~SP_XmppCompletionHandler()
{
}

void SP_XmppCompletionHandler :: completionMessage( SP_Message * msg )
{
#if 1
	printf( "message completed { completion key : %d }\n", msg->getCompletionKey() );

	printf( "\tsuccess {" );
	for( int i = 0; i < msg->getSuccess()->getCount(); i++ ) {
		printf( " %d", msg->getSuccess()->get( i ).mKey );
	}
	printf( "}\n" );

	printf( "\tfailure {" );
	for( int i = 0; i < msg->getFailure()->getCount(); i++ ) {
		printf( " %d", msg->getFailure()->get( i ).mKey );
	}
	printf( "}\n" );
#endif

	delete msg;
}

//---------------------------------------------------------

SP_XmppHandlerFactory :: SP_XmppHandlerFactory()
{
}

SP_XmppHandlerFactory :: ~SP_XmppHandlerFactory()
{
}

SP_Handler * SP_XmppHandlerFactory :: create() const
{
	return new SP_XmppHandler();
}

SP_CompletionHandler * SP_XmppHandlerFactory :: createCompletionHandler() const
{
	return new SP_XmppCompletionHandler();
}

