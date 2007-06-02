/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

#include "spxmppfiledb.hpp"
#include "spxmppentity.hpp"

#include "spxmlnode.hpp"
#include "spdomparser.hpp"

static SP_XmlDomParser * loadFile( const char * path )
{
	SP_XmlDomParser * parser = NULL;

	assert( NULL == strstr( path, "null"  ) );

	int fd = open( path, O_RDWR );
	if( fd >= 0 ) {
		struct stat aStat;
		if( 0 == fstat( fd, &aStat ) ) {
			char * buffer = (char*)malloc( aStat.st_size + 1 );
			assert( NULL != buffer );
			assert( aStat.st_size == read( fd, buffer, aStat.st_size ) );
			buffer[ aStat.st_size ] = '\0';

			parser = new SP_XmlDomParser();
			parser->append( buffer, aStat.st_size );

			free( buffer );
		}

		close( fd );
	} else {
		syslog( LOG_WARNING, "cannot open file <%s>", path );
	}

	if( NULL != parser && NULL != parser->getError() ) {
		syslog( LOG_WARNING, "parser xml error <%s>", parser->getError() );

		delete parser;
		parser = NULL;
	}

	return parser;
}

static int saveToFile( const char * path, SP_XmlDomParser * parser )
{
	int ret = -1;

	int fd = open( path, O_RDWR | O_CREAT | O_TRUNC, 0770 );
	if( fd >= 0 ) {
		SP_XmlDomBuffer buffer( parser->getDocument()->getRootElement() );

assert( NULL == strstr( buffer.getBuffer(), "null" ) );

		write( fd, buffer.getBuffer(), buffer.getSize() );
		close( fd );

		ret = 0;
	} else {
		syslog( LOG_WARNING, "cannot open file <%s>", path );
	}

	return ret;
}

static SP_XmlElementNode * findElement( SP_XmlElementNode * element, const char * name )
{
	if( NULL == element ) return NULL;

	if( 0 == strcmp( element->getName(), name ) ) return element;

	SP_XmlElementNode * ret = NULL;

	const SP_XmlNodeList * children = element->getChildren();
	for( int i = 0; i < children->getLength() && NULL == ret; i++ ) {
		const SP_XmlNode * node = children->get( i );

		if( SP_XmlNode::eELEMENT == node->getType() ) {
			ret = findElement( (SP_XmlElementNode*)node, name );
		}
	}

	return ret;
}

static SP_XmlElementNode * findElement( SP_XmlElementNode * element,
		const char * attrName, const char * attrValue )
{
	if( NULL == element ) return NULL;

	const char * value = element->getAttrValue( attrName );
	if( NULL != value && 0 == strcmp( value, attrValue ) ) return element;

	SP_XmlElementNode * ret = NULL;

	const SP_XmlNodeList * children = element->getChildren();
	for( int i = 0; i < children->getLength() && NULL == ret; i++ ) {
		const SP_XmlNode * node = children->get( i );

		if( SP_XmlNode::eELEMENT == node->getType() ) {
			ret = findElement( (SP_XmlElementNode*)node, attrName, attrValue );
		}
	}

	return ret;
}

static const char * getElementContent( SP_XmlElementNode * element,
		const char * name, const char * defValue )
{
	const char * ret = defValue;

	SP_XmlElementNode * target = findElement( element, name );
	if( NULL != target && target->getChildren()->getLength() > 0 ) {
		const SP_XmlNode * node = target->getChildren()->get( 0 );
		if( SP_XmlNode::eCDATA == node->getType() ) {
			ret = ((SP_XmlCDataNode*)node)->getText();
		}
	}

	return ret;
}

static void setElementContent( SP_XmlElementNode * element, const char * content,
	SP_XmlElementNode * parent, const char * name )
{
	if( NULL != element ) {
		if( element->getChildren()->getLength() > 0 ) {
			const SP_XmlNode * node = element->getChildren()->get( 0 );
			if( SP_XmlNode::eCDATA == node->getType() ) {
				((SP_XmlCDataNode*)node)->setText( content );
			}
		} else {
			SP_XmlCDataNode * cdata = new SP_XmlCDataNode();
			cdata->setText( content );
			element->addChild( cdata );
		}
	} else if( NULL != parent ) {
		SP_XmlElementNode * newElement = new SP_XmlElementNode();
		newElement->setName( name );

		SP_XmlCDataNode * cdata = new SP_XmlCDataNode();
		cdata->setText( content );

		newElement->addChild( cdata );

		parent->addChild( newElement );
	}
}

//---------------------------------------------------------

SP_XmppFileUserDao :: SP_XmppFileUserDao( const char * baseDir )
{
	snprintf( mBaseDir, sizeof( mBaseDir ), "%s", baseDir );
}

SP_XmppFileUserDao :: ~SP_XmppFileUserDao()
{
}

SP_XmppUser * SP_XmppFileUserDao :: load( const char * username )
{
	SP_XmppUser * user = NULL;

	char fullPath[ 256 ] = { 0 };
	snprintf( fullPath, sizeof( fullPath ), "%s/%s.xml", mBaseDir, username );

	SP_XmlDomParser * parser = loadFile( fullPath );
	if( NULL != parser ) {
		SP_XmlElementNode * root = parser->getDocument()->getRootElement();

		user = new SP_XmppUser( username );
		user->setPassword( getElementContent( root, "password", "" ) );

		SP_XmlElementNode * query = findElement( root, "xmlns", "jabber:iq:register" );

		if( NULL != query ) {
			user->setName( getElementContent( query, "name", "" ) );
			user->setEmail( getElementContent( query, "email", "" ) );
		}

		delete parser;
	}

	return user;
}

int SP_XmppFileUserDao :: store( SP_XmppUser * user )
{
	char fullPath[ 256 ] = { 0 };
	snprintf( fullPath, sizeof( fullPath ), "%s/%s.xml", mBaseDir, user->getUsername() );

	SP_XmlDomParser * parser = loadFile( fullPath );

	if( NULL == parser ) parser = new SP_XmlDomParser();

	if( NULL == parser->getDocument()->getRootElement() ) {
		SP_XmlElementNode * root = new SP_XmlElementNode();
		root->setName( "xdb" );

		// password
		SP_XmlElementNode * password = new SP_XmlElementNode();
		password->setName( "password" );
		password->addAttr( "xmlns", "jabber:iq:auth" );
		password->addAttr( "xdbns", "jabber:iq:auth" );

		SP_XmlCDataNode * cdataPassword = new SP_XmlCDataNode();
		cdataPassword->setText( user->getPassword() );
		password->addChild( cdataPassword );

		root->addChild( password );

		// query
		SP_XmlElementNode * query = new SP_XmlElementNode();
		query->setName( "query" );
		query->addAttr( "xmlns", "jabber:iq:register" );
		query->addAttr( "xdbns", "jabber:iq:register" );

		// query::name
		SP_XmlElementNode * name = new SP_XmlElementNode();
		name->setName( "name" );
		SP_XmlCDataNode * cdataName = new SP_XmlCDataNode();
		cdataName->setText( user->getName() );
		name->addChild( cdataName );

		query->addChild( name );

		// query::email
		SP_XmlElementNode * email = new SP_XmlElementNode();
		email->setName( "email" );
		SP_XmlCDataNode * cdataEmail = new SP_XmlCDataNode();
		cdataEmail->setText( user->getEmail() );
		email->addChild( cdataEmail );

		query->addChild( email );

		root->addChild( query );

		((SP_XmlDocument*)parser->getDocument())->setRootElement( root );
	} else {
		SP_XmlElementNode * password = findElement(
			parser->getDocument()->getRootElement(), "password" );

		setElementContent( password, user->getPassword(),
			parser->getDocument()->getRootElement(), "password" );

		SP_XmlElementNode * query = findElement(
			parser->getDocument()->getRootElement(), "xmlns", "jabber:iq:register" );

		SP_XmlElementNode * name = findElement( query, "name" );
		SP_XmlElementNode * email = findElement( query, "email" );

		setElementContent( name, user->getName(), query, "name" );
		setElementContent( email, user->getEmail(), query, "email" );
	}

	int ret = saveToFile( fullPath, parser );

	delete parser;

	return ret;
}

int SP_XmppFileUserDao :: remove( const char * username )
{
	char fullPath[ 256 ] = { 0 };
	snprintf( fullPath, sizeof( fullPath ), "%s/%s.xml", mBaseDir, username );

	return unlink( fullPath );
}

int SP_XmppFileUserDao :: updatePassword( const char * username, const char * password )
{
	int ret = -1;

	SP_XmppUser * user = load( username );

	if( NULL != user ) {
		user->setPassword( password );
		ret = store( user );
	}

	return ret;
}

//---------------------------------------------------------

SP_XmppFileRosterDao :: SP_XmppFileRosterDao( const char * baseDir )
{
	snprintf( mBaseDir, sizeof( mBaseDir ), "%s", baseDir );
}

SP_XmppFileRosterDao :: ~SP_XmppFileRosterDao()
{
}

SP_XmppRosterItem * SP_XmppFileRosterDao :: xml2item( SP_XmlElementNode * node,
		const char * username )
{
	SP_XmppRosterItem * item = NULL;

	if( 0 == strcmp( "item", node->getName() ) ) {
		item = new SP_XmppRosterItem( username );

		item->setJid( node->getAttrValue( "jid" ) );
		item->setNickname( node->getAttrValue( "name" ) );
		item->setSubType( node->getAttrValue( "subscription" ) );
	}

	return item;
}

SP_XmppRoster * SP_XmppFileRosterDao :: load( const char * username )
{
	char fullPath[ 256 ] = { 0 };
	snprintf( fullPath, sizeof( fullPath ), "%s/%s.xml", mBaseDir, username );

	SP_XmppRoster * roster = new SP_XmppRoster( username );

	SP_XmlDomParser * parser = loadFile( fullPath );

	if( NULL != parser ) {
		SP_XmlElementNode * query = findElement(
			parser->getDocument()->getRootElement(), "xmlns", "jabber:iq:roster" );

		if( NULL != query ) {
			const SP_XmlNodeList * itemList = query->getChildren();
			for( int i = 0; i < itemList->getLength(); i++ ) {
				const SP_XmlNode * node = itemList->get( i );
				if( SP_XmlNode::eELEMENT == node->getType() ) {
					SP_XmppRosterItem * item = xml2item(
							(SP_XmlElementNode*)node, username );
					if( NULL != item ) roster->append( item );
				}
			}
		}

		delete parser;
	}

	return roster;
}

SP_XmppRosterItem * SP_XmppFileRosterDao :: load( const char * username, const char * jid )
{
	SP_XmppRosterItem * item = NULL;

	char fullPath[ 256 ] = { 0 };
	snprintf( fullPath, sizeof( fullPath ), "%s/%s.xml", mBaseDir, username );

	SP_XmlDomParser * parser = loadFile( fullPath );

	if( NULL != parser ) {
		SP_XmlElementNode * query = findElement(
			parser->getDocument()->getRootElement(), "xmlns", "jabber:iq:roster" );

		SP_XmlElementNode * itemNode = findElement( query, "jid", jid );

		if( NULL != itemNode ) {
			item = xml2item( (SP_XmlElementNode*)itemNode, username );
		}

		delete parser;
	}

	return item;
}

int SP_XmppFileRosterDao :: store( SP_XmppRosterItem * item )
{
	int ret = -1;

	char fullPath[ 256 ] = { 0 };
	snprintf( fullPath, sizeof( fullPath ), "%s/%s.xml",
			mBaseDir, item->getUsername() );

	SP_XmlDomParser * parser = loadFile( fullPath );

	if( NULL != parser ) {
		ret = 0;

		SP_XmlElementNode * query = findElement(
			parser->getDocument()->getRootElement(), "xmlns", "jabber:iq:roster" );

		if( NULL == query ) {
			query = new SP_XmlElementNode();
			query->setName( "query" );
			query->addAttr( "xmlns", "jabber:iq:roster" );
			query->addAttr( "xdbns", "jabber:iq:roster" );

			parser->getDocument()->getRootElement()->addChild( query );
		}

		SP_XmlElementNode * oldItem = findElement( query, "jid", item->getJid() );
		if( NULL == oldItem ) {
			SP_XmlElementNode * itemNode = new SP_XmlElementNode();
			itemNode->setName( "item" );
			itemNode->addAttr( "jid", item->getJid() );
			itemNode->addAttr( "name", item->getNickname() );
			itemNode->addAttr( "subscription", item->getSubType() );

			query->addChild( itemNode );
		} else {
			oldItem->removeAttr( "name" );
			oldItem->addAttr( "name", item->getNickname() );

			oldItem->removeAttr( "subscription" );
			oldItem->addAttr( "subscription", item->getSubType() );
		}

		ret = saveToFile( fullPath, parser );

		delete parser;
	}

	return 0;
}

int SP_XmppFileRosterDao :: remove( const char * username, const char * jid )
{
	int ret = -1;

	char fullPath[ 256 ] = { 0 };
	snprintf( fullPath, sizeof( fullPath ), "%s/%s.xml", mBaseDir, username );

	SP_XmlDomParser * parser = loadFile( fullPath );

	if( NULL != parser ) {
		ret = 0;

		SP_XmlElementNode * query = findElement(
			parser->getDocument()->getRootElement(), "xmlns", "jabber:iq:roster" );

		SP_XmlElementNode * itemNode = findElement( query, "jid", jid );

		if( NULL != itemNode ) {
			const SP_XmlNodeList * children = query->getChildren();
			for( int i = 0; i < children->getLength(); i++ ) {
				if( children->get( i ) == itemNode ) {
					children->take( i );
					break;
				}
			}
			delete itemNode;
		}

		ret = saveToFile( fullPath, parser );

		delete parser;
	}

	return 0;
}

int SP_XmppFileRosterDao :: remove( const char * username )
{
	return 0;
}

