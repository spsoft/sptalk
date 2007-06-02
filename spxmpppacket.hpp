/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmpppacket_hpp__
#define __spxmpppacket_hpp__

class SP_XmppJid;

class SP_XmlElementNode;
class SP_XmppRosterItem;

class SP_XmppPacket {
public:
	SP_XmppPacket( SP_XmlElementNode * node );
	virtual ~SP_XmppPacket();

	const char * getId() const;

	SP_XmppJid * getTo();

	SP_XmppJid * getFrom();

	int isType( const char * type );
	const char * getType() const;

protected:
	SP_XmppPacket( SP_XmppPacket & );
	SP_XmppPacket & operator=( SP_XmppPacket & );

	SP_XmppJid * mFromJid, * mToJid;

	SP_XmlElementNode * mElement;

	static SP_XmlElementNode * getChildElement( SP_XmlElementNode * element,
			const char * name );
};

class SP_XmppIQ : public SP_XmppPacket {
public:
	SP_XmppIQ( SP_XmlElementNode * node );
	virtual ~SP_XmppIQ();

	int isQueryNS( const char * ns );

	const char * getQueryParam( const char * name ) const;

	// caller must delete the return value
	SP_XmppRosterItem * toRosterItem( const char * username ) const;

	const char * getQueryNS() const;
};

class SP_XmppPresence : public SP_XmppPacket {
public:
	SP_XmppPresence( SP_XmlElementNode * node );
	virtual ~SP_XmppPresence();

	const char * getStatus() const;
};

#endif

