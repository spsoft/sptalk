/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "spxmppaction.hpp"

#include "spxmppconfig.hpp"
#include "spxmpppersist.hpp"
#include "spxmppentity.hpp"
#include "spxmpppacket.hpp"
#include "spxmppjid.hpp"
#include "spxmppsession.hpp"

#include "spxml/spxmlnode.hpp"
#include "spxml/spdomparser.hpp"

#include "spserver/spbuffer.hpp"

int SP_XmppAction :: doAuthGet( SP_XmppIQ * iqPacket, SP_Buffer * reply )
{
	int ret = -1;

	char buffer[ 1024 ] = { 0 };

	const char * username = iqPacket->getQueryParam( "username" );

	SP_XmppUser * user = SP_XmppPersistManager::getDefault()->findUser( username );

	if( NULL == user ) {
		snprintf( buffer, sizeof( buffer ), "<iq type=\"error\" id=\"%s\">"
				"<query xmlns=\"jabber:iq:auth\">"
				"<username>%s</username>"
				"</query><error code=\"401\">Unauthorized</error></iq>",
	 			iqPacket->getId(), username );
	} else {
		delete user;

		snprintf( buffer, sizeof( buffer ), "<iq type=\"result\" id=\"%s\">"
				"<query xmlns=\"jabber:iq:auth\"><username>%s</username>"
				"<password/><resource/></query></iq>",
				iqPacket->getId(), username );
		ret = 0;
	}

	reply->append( buffer );

	return ret;
}

int SP_XmppAction :: doAuthSet( SP_XmppIQ * iqPacket, SP_Buffer * reply )
{
	int ret = -1;

	char buffer[ 1024 ] = { 0 };

	const char * username = iqPacket->getQueryParam( "username" );

	SP_XmppUser * user = SP_XmppPersistManager::getDefault()->findUser( username );

	if( NULL == user ) {
		snprintf( buffer, sizeof( buffer ), "<iq type=\"error\" id=\"%s\">"
				"<query xmlns=\"jabber:iq:auth\">"
				"<username>%s</username>"
				"</query><error code=\"401\">Unauthorized</error></iq>",
	 			iqPacket->getId(), username );
	} else {
		const char * password = iqPacket->getQueryParam( "password" );

		if( 0 == strcmp( password, user->getPassword() ) ) {
			snprintf( buffer, sizeof( buffer ), "<iq type=\"result\" id=\"%s\"/>",
				iqPacket->getId() );
			ret = 0;
		} else {
			snprintf( buffer, sizeof( buffer ), "<iq id=\"%s\" type=\"error\">"
				"<error code=\"401\">Unauthorized</error></iq>",
				iqPacket->getId() );
		}
		delete user;
	}

	reply->append( buffer );

	return ret;
}

int SP_XmppAction :: doRegGet( SP_XmppIQ * iqPacket, SP_Buffer * reply )
{
	int ret = -1;

	char buffer[ 1024 ] = { 0 };

	snprintf( buffer, sizeof( buffer ), "<iq type=\"result\" id=\"%s\">"
		"<query xmlns=\"jabber:iq:register\"><instructions>"
		"Choose a username and password for use with this service."
		"Please also provide your email address."
		"</instructions><username/><password/><email/></query></iq>",
		iqPacket->getId() );

	reply->append( buffer );

	return ret;
}

int SP_XmppAction :: doRegSet( SP_XmppIQ * iqPacket, SP_Buffer * reply )
{
	int ret = -1;

	char buffer[ 1024 ] = { 0 };

	const char * username = iqPacket->getQueryParam( "username" );
	const char * password = iqPacket->getQueryParam( "password" );

	SP_XmppUser user( username );
	user.setPassword( password );

	if( 0 == SP_XmppPersistManager::getDefault()->addUser( &user ) ) {
		snprintf( buffer, sizeof( buffer ), "<iq type=\"result\" id=\"%s\"/>",
			iqPacket->getId() );
		ret = 0;
	} else {
		snprintf( buffer, sizeof( buffer ), "<iq type=\"error\" id=\"%s\">"
			"<error>Server internal error</error></iq>",
 			iqPacket->getId() );
	}

	reply->append( buffer );

	return ret;
}

int SP_XmppAction :: doRosterSet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
	SP_XmppJid * fromJid )
{
	int ret = -1;

	char buffer[ 1024 ] = { 0 };

	SP_XmppRosterItem * item = iqPacket->toRosterItem( fromJid->getNode() );

	if( NULL != item ) {
		if( item->isSubType( "remove" ) ) {
			ret = SP_XmppPersistManager::getDefault()->delRosterItem( fromJid->getNode(), item->getJid() );
		} else {
			if( 0 == SP_XmppPersistManager::getDefault()->addRosterItem( fromJid->getNode(), item ) ) {
				ret = 0;
				snprintf( buffer, sizeof( buffer ), "<iq type=\"set\">"
					"<query xmlns=\"jabber:iq:roster\">"
					"<item jid=\"%s\" subscription=\"none\"/></query></iq>"
					"<iq id=\"%s\" type=\"result\" from=\"%s\" to=\"%s\"/>",
					item->getJid(), iqPacket->getId() ? iqPacket->getId() : "",
					fromJid->getFullJid(), fromJid->getFullJid() );
			} else {
				snprintf( buffer, sizeof( buffer ), "<iq type=\"error\" id=\"%s\">"
					"<error>Server internal error</error></iq>",
	 				iqPacket->getId() ? iqPacket->getId() : "" );
			}
		}
		delete item;
	} else {
		snprintf( buffer, sizeof( buffer ), "<iq type=\"error\" id=\"%s\">"
			"<error>Bad packet</error></iq>",
 			iqPacket->getId() );
	}

	reply->append( buffer );

	return ret;
}

int SP_XmppAction :: doRosterGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
	SP_XmppJid * fromJid )
{
	int ret = -1;

	char buffer[ 1024 ] = { 0 };

	SP_XmppRoster * roster = SP_XmppPersistManager::getDefault()->findRoster( fromJid->getNode() );

	if( NULL != roster ) {
		ret = 0;

		if( NULL != iqPacket->getId() ) {
			snprintf( buffer, sizeof( buffer ), "<iq id=\"%s\" type=\"result\" "
				"from=\"%s\"><query xmlns=\"jabber:iq:roster\">",
				iqPacket->getId(), fromJid->getFullJid() );
		} else {
			snprintf( buffer, sizeof( buffer ), "<iq type=\"result\" from=\"%s\">"
				"<query xmlns=\"jabber:iq:roster\">",
				fromJid->getFullJid() );
		}

		reply->append( buffer );

		for( int i = 0; i < roster->getCount(); i++ ) {
			SP_XmppRosterItem * item = roster->getItem( i );
			snprintf( buffer, sizeof( buffer ), "<item jid=\"%s\" name=\"%s\" subscription=\"%s\"/>",
				item->getJid(), item->getNickname(), item->getSubType() );
			reply->append( buffer );
		}

		reply->append( "</query></iq>" );

		delete roster;
	} else {
	}

	return ret;
}

int SP_XmppAction :: doPresenceAsk( SP_XmppPresence * prPacket, SP_Response * response,
	SP_XmppJid * fromJid )
{
	char buffer[ 1024 ] = { 0 };
	char subType[ 64 ] = { "none" };

	// response to sender

	SP_XmppRosterItem * oldItem = SP_XmppPersistManager::getDefault()->findRosterItem(
			fromJid->getNode(), prPacket->getTo()->getBareJid() );

	if( NULL != oldItem ) {
		snprintf( subType, sizeof( subType ), "%s", oldItem->getSubType() );

		snprintf( buffer, sizeof( buffer ), "<iq type=\"set\"><query xmlns=\"jabber:iq:roster\">"
			"<item jid=\"%s\" subscription=\"%s\" ask=\"subscribe\"/></query></iq>",
			prPacket->getTo()->getBareJid(), oldItem->getSubType() );
		delete oldItem;
	} else {
		snprintf( buffer, sizeof( buffer ), "<iq type=\"set\"><query xmlns=\"jabber:iq:roster\">"
			"<item jid=\"%s\" subscription=\"none\" ask=\"subscribe\"/></query></iq>",
			prPacket->getTo()->getBareJid() );
	}

	response->getReply()->getMsg()->append( buffer );

	// forward to recipients

	if( 0 != strcasecmp( subType, "both" ) ) {
		if( NULL != prPacket->getStatus() ) {
			snprintf( buffer, sizeof( buffer ), "<presence from=\"%s\" to=\"%s\" type=\"subscribe\">"
				"<status>%s</status></presence>",
				fromJid->getBareJid(), prPacket->getTo()->getBareJid(), prPacket->getStatus() );
		} else {
			snprintf( buffer, sizeof( buffer ), "<presence from=\"%s\" to=\"%s\" type=\"subscribe\"/>",
				fromJid->getBareJid(), prPacket->getTo()->getBareJid() );
		}

		SP_Sid_t toSid;
		if( 0 == SP_XmppSessionManager::getDefault()->getSid( prPacket->getTo()->getNode(),
				prPacket->getTo()->getResource(), &toSid ) ) {
			SP_Message * msg = new SP_Message();
			msg->getToList()->add( toSid );
			msg->getMsg()->append( buffer );
			response->addMessage( msg );
		} else {
			// TODO: save as offline message
		}
	} else {
		syslog( LOG_NOTICE, "ignore presence ask" );
	}

	return 0;
}

int SP_XmppAction :: doPresenceSub( SP_XmppPresence * prPacket, SP_Response * response,
	SP_XmppJid * fromJid )
{
	int ret = -1;

	char buffer[ 1024 ] = { 0 };

	SP_XmppRosterItem * oldItem = SP_XmppPersistManager::getDefault()->findRosterItem(
			fromJid->getNode(), prPacket->getTo()->getBareJid() );

	if( NULL == oldItem ) {
		oldItem = new SP_XmppRosterItem( fromJid->getNode() );
		oldItem->setJid( prPacket->getTo()->getBareJid() );
		oldItem->setNickname( prPacket->getTo()->getNode() );
		oldItem->setSubType( "from" );
	}

	if( ! oldItem->isSubType( "both" ) ) {
		if( oldItem->isSubType( "none" ) ) oldItem->setSubType( "from" );
		if( oldItem->isSubType( "to" ) ) oldItem->setSubType( "both" );

		if( 0 == SP_XmppPersistManager::getDefault()->addRosterItem( fromJid->getNode(), oldItem ) ) {
			ret = 0;
			// response to sender
			snprintf( buffer, sizeof( buffer ), "<iq type=\"set\">"
				"<query xmlns=\"jabber:iq:roster\">"
				"<item jid=\"%s\" name=\"%s\" subscription=\"%s\"/></query></iq>",
				oldItem->getJid(), oldItem->getNickname(), oldItem->getSubType() );
		} else {
			snprintf( buffer, sizeof( buffer ), "<iq type=\"error\">"
				"<error>Server internal error</error></iq>" );
		}

		response->getReply()->getMsg()->append( buffer );
	}

	delete oldItem;

	if( 0 == ret ) {
		oldItem = SP_XmppPersistManager::getDefault()->findRosterItem(
			prPacket->getTo()->getNode(), fromJid->getBareJid() );

		if( NULL == oldItem ) {
			oldItem = new SP_XmppRosterItem( prPacket->getTo()->getNode() );
			oldItem->setJid( fromJid->getBareJid() );
			oldItem->setNickname( fromJid->getNode() );
			oldItem->setSubType( "to" );
		}

		if( ! oldItem->isSubType( "both" ) ) {
			if( oldItem->isSubType( "none" ) ) oldItem->setSubType( "to" );
			if( oldItem->isSubType( "from" ) ) oldItem->setSubType( "both" );

			if( 0 == SP_XmppPersistManager::getDefault()->addRosterItem( oldItem->getUsername(), oldItem ) ) {
				SP_Message * msg = new SP_Message();
				if( 0 == SP_XmppSessionManager::getDefault()->getSidList(
					prPacket->getTo()->getNode(), msg->getToList() ) ) {

					// forward to recepients
					snprintf( buffer, sizeof( buffer ), "<presence to=\"%s\" type=\"subscribed\" from=\"%s\"/>",
						prPacket->getTo()->getBareJid(), fromJid->getBareJid() );
					msg->getMsg()->append( buffer );

					snprintf( buffer, sizeof( buffer ), "<iq type=\"set\"><query xmlns=\"jabber:iq:roster\">"
						"<item jid=\"%s\" subscription=\"%s\"/></query></iq>",
						fromJid->getBareJid(), oldItem->getSubType() );
					msg->getMsg()->append( buffer );

					response->addMessage( msg );	
				} else {
					// TODO
					delete msg;	
				}
			} else {
				// TODO: server internal error
			}
		}

		delete oldItem;
	}

	return 0;
}

int SP_XmppAction :: doPresenceUnsub( SP_XmppPresence * prPacket, SP_Response * response,
	SP_XmppJid * fromJid )
{
	char buffer[ 1024 ] = { 0 };

	SP_XmppRosterItem * fromItem = SP_XmppPersistManager::getDefault()->findRosterItem(
		fromJid->getNode(), prPacket->getTo()->getBareJid() );
	SP_XmppRosterItem * toItem = SP_XmppPersistManager::getDefault()->findRosterItem(
		prPacket->getTo()->getNode(), fromJid->getBareJid() );

	if( NULL != fromItem ) {
		fromItem->setSubType( "from" );
		SP_XmppPersistManager::getDefault()->addRosterItem( fromJid->getNode(), fromItem );
		delete fromItem;
	}

	if( NULL != toItem ) {
		toItem->setSubType( "to" );
		SP_XmppPersistManager::getDefault()->addRosterItem( prPacket->getTo()->getNode(), toItem );

		snprintf( buffer, sizeof( buffer ), "<iq type='set'><query xmlns='jabber:iq:roster'>"
			"<item jid='%s' subscription='none' name='%s'/></query></iq>",
			toItem->getJid(), toItem->getNickname() );
		response->getReply()->getMsg()->append( buffer );

		delete toItem;
	}

	SP_Message * msg = new SP_Message();
	if( 0 == SP_XmppSessionManager::getDefault()->getSidList(
			prPacket->getTo()->getNode(), msg->getToList() ) ) {
		snprintf( buffer, sizeof( buffer ), "<presence from='%s' to='%s' type='unsubscribe'/>",
			fromJid->getBareJid(),  prPacket->getTo()->getBareJid() );
		msg->getMsg()->append( msg );
		response->addMessage( msg );
	}

	return 0;
}

int SP_XmppAction :: doPresencePub( SP_XmppPresence * prPacket, SP_Response * response,
	SP_XmppJid * fromJid )
{
	char buffer[ 1024 ] = { 0 };

	// broadcast
	if( NULL == prPacket->getTo() ) {
		SP_XmppRoster * roster = SP_XmppPersistManager::getDefault()->findRoster( fromJid->getNode() );

		if( NULL != roster ) {
			for( int i = 0; i < roster->getCount(); i++ ) {
				SP_XmppRosterItem * item = roster->getItem( i );

				if( ! item->isSubType( "from" ) && ! item->isSubType( "both" ) ) continue;

				SP_XmppJid toJid( item->getJid() );

				SP_Message * msg = new SP_Message();
				if( 0 == SP_XmppSessionManager::getDefault()->getSidList( toJid.getNode(), msg->getToList() ) ) {
					snprintf( buffer, sizeof( buffer ), "<presence from=\"%s\" to=\"%s\">"
						"<status>%s</status></presence>",
						fromJid->getFullJid(), item->getJid(),
						prPacket->getStatus() ? prPacket->getStatus() : "available" );
					msg->getMsg()->append( buffer );
					response->addMessage( msg );
				} else {
					// contact is offline, not need to notify
					delete msg;
				}
			}

			delete roster;
		}
	}

	return 0;
}

int SP_XmppAction :: doMessageChat( SP_XmppPacket * packet, SP_XmlElementNode * node,
		SP_Response * response, SP_XmppJid * fromJid )
{
	node->removeAttr( "from" );
	node->addAttr( "from", fromJid->getFullJid() );

	SP_Sid_t toSid;
	if( 0 == SP_XmppSessionManager::getDefault()->getSid( packet->getTo()->getNode(),
			packet->getTo()->getResource(), &toSid ) ) {
		SP_XmlDomBuffer domBuffer( node );

		SP_Message * msg = new SP_Message();
		msg->getToList()->add( toSid );
		msg->getMsg()->append( domBuffer.getBuffer() );
		response->addMessage( msg );
	} else {
		// TODO
	}

	return 0;
}

int SP_XmppAction :: doAgentsGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
	SP_XmppJid * fromJid )
{
	char buffer[ 1024 ] = { 0 };

	snprintf( buffer, sizeof( buffer ), "<iq id=\"%s\" to=\"%s\" type=\"result\" from=\"%s\">"
		"<query xmlns=\"jabber:iq:agents\"></query></iq>",
		iqPacket->getId(), fromJid->getFullJid(), iqPacket->getTo()->getBareJid() );

	reply->append( buffer );

	return 0;
}

int SP_XmppAction :: doBrowseGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
	SP_XmppJid * fromJid )
{
	char buffer[ 1024 ] = { 0 };

	snprintf( buffer, sizeof( buffer ), "<iq id=\"%s\" to=\"%s\" type=\"result\" from=\"%s\"/>",
		iqPacket->getId(), fromJid->getFullJid(), iqPacket->getTo()->getBareJid() );

	reply->append( buffer );

	return 0;
}

int SP_XmppAction :: doPrivateGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
	SP_XmppJid * fromJid )
{
	char buffer[ 1024 ] = { 0 };

	snprintf( buffer, sizeof( buffer ), "<iq id=\"%s\" type=\"result\" from=\"%s\">"
		"<query xmlns=\"jabber:iq:private\"><roster xmlns=\"roster:delimiter\"/></query></iq>",
		iqPacket->getId(), fromJid->getFullJid() );

	reply->append( buffer );

	return 0;
}

int SP_XmppAction :: doUnavailableGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
	SP_XmppJid * fromJid )
{
	char buffer[ 1024 ] = { 0 };

	snprintf( buffer, sizeof( buffer ), "<iq id=\"%s\" type=\"error\" "
		"from=\"%s\" to=\"%s\"><query xmlns=\"%s\"/>"
		"<error code='503'>Service Unavailable</error></iq>",
		iqPacket->getId(), fromJid->getBareJid(),
		fromJid->getFullJid(), iqPacket->getQueryNS() );

	reply->append( buffer );

	return 0;
}

