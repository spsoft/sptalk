/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <stdio.h>
#include <string.h>

#include "spxmpppacket.hpp"
#include "spxmppjid.hpp"
#include "spxmppentity.hpp"

#include "spxml/spxmlnode.hpp"

SP_XmppPacket :: SP_XmppPacket( SP_XmlElementNode * node )
{
	mElement = node;
	mFromJid = mToJid = NULL;
}

SP_XmppPacket :: ~SP_XmppPacket()
{
	if( NULL != mElement ) delete mElement;
	mElement = NULL;

	if( NULL != mFromJid ) delete mFromJid;
	mFromJid = NULL;

	if( NULL != mToJid ) delete mToJid;
	mToJid = NULL;
}

const char * SP_XmppPacket :: getId() const
{
	return mElement->getAttrValue( "id" );
}

SP_XmppJid * SP_XmppPacket :: getTo()
{
	const char * to = mElement->getAttrValue( "to" );
	if( NULL != to ) {
		if( NULL != mToJid ) {
			if( 0 != strcmp( to, mToJid->getFullJid() ) ) {
				mToJid->setJid( to );
			}
		} else {
			mToJid = new SP_XmppJid( to );
		}
	}

	return mToJid;
}

SP_XmppJid * SP_XmppPacket :: getFrom()
{
	const char * from = mElement->getAttrValue( "from" );
	if( NULL != from ) {
		if( NULL != mFromJid ) {
			if( 0 != strcmp( from, mFromJid->getFullJid() ) ) {
				mFromJid->setJid( from );
			}
		} else {
			mFromJid = new SP_XmppJid( from );
		}
	}

	return mFromJid;
}

SP_XmlElementNode * SP_XmppPacket :: getChildElement( SP_XmlElementNode * element,
		const char * name )
{
	SP_XmlElementNode * child = NULL;

	if( NULL == element ) return child;

	const SP_XmlNodeList * children = element->getChildren();
	for( int i = 0; i < children->getLength() && NULL == child; i++ ) {
		const SP_XmlNode * node = children->get( i );
		if( SP_XmlNode::eELEMENT == node->getType() ) {
			if( 0 == strcmp( name, ((SP_XmlElementNode*)node)->getName() ) ) {
				child = (SP_XmlElementNode*)node;
			}
		}
	}

	return child;
}

const char * SP_XmppPacket :: getType() const
{
	return mElement->getAttrValue( "type" );
}

int SP_XmppPacket :: isType( const char * type )
{
	const char * myType = getType();

	return NULL == myType ? 0 : 0 == strcmp( myType, type );
}

//---------------------------------------------------------

SP_XmppIQ :: SP_XmppIQ( SP_XmlElementNode * node )
	: SP_XmppPacket( node )
{
}

SP_XmppIQ :: ~SP_XmppIQ()
{
}

const char * SP_XmppIQ :: getQueryNS() const
{
	SP_XmlElementNode * query = getChildElement( mElement, "query" );

	return NULL == query ? NULL : query->getAttrValue( "xmlns" );
}

int SP_XmppIQ :: isQueryNS( const char * ns )
{
	const char * myNS = getQueryNS();

	return NULL == myNS ? 0 : 0 == strcmp( myNS, ns );
}

const char * SP_XmppIQ :: getQueryParam( const char * name ) const
{
	const char * ret = NULL;

	SP_XmlElementNode * target = getChildElement(
		getChildElement( mElement, "query" ), name );

	if( NULL != target && target->getChildren()->getLength() > 0 ) {
		const SP_XmlNode * node = target->getChildren()->get( 0 );
		if( SP_XmlNode::eCDATA == node->getType() ) {
			ret = ((SP_XmlCDataNode*)node)->getText();
		}
	}

	return ret;
}

SP_XmppRosterItem * SP_XmppIQ :: toRosterItem( const char * username ) const
{
	SP_XmlElementNode * itemNode = getChildElement(
		getChildElement( mElement, "query" ), "item" );

	SP_XmppRosterItem * item = NULL;
	if( NULL != itemNode ) {
		item = new SP_XmppRosterItem( username );

		item->setJid( itemNode->getAttrValue( "jid" ) );
		item->setNickname( itemNode->getAttrValue( "name" ) );
	}

	return item;
}

//---------------------------------------------------------

SP_XmppPresence :: SP_XmppPresence( SP_XmlElementNode * node )
	: SP_XmppPacket( node )
{
}

SP_XmppPresence :: ~SP_XmppPresence()
{
}

const char * SP_XmppPresence :: getStatus() const
{
	const char * ret = NULL;

	SP_XmlElementNode * target = getChildElement( mElement, "status" );

	if( NULL != target && target->getChildren()->getLength() > 0 ) {
		const SP_XmlNode * node = target->getChildren()->get( 0 );
		if( SP_XmlNode::eCDATA == node->getType() ) {
			ret = ((SP_XmlCDataNode*)node)->getText();
		}
	}

	return ret;
}

