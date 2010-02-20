/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmppaction_hpp__
#define __spxmppaction_hpp__

class SP_XmppPacket;
class SP_XmppIQ;
class SP_XmppPresence;
class SP_XmppJid;

class SP_Response;
class SP_Buffer;

class SP_XmlElementNode;

class SP_XmppAction {
public:

	// iq packet
	static int doAuthGet( SP_XmppIQ * iqPacket, SP_Buffer * reply );
	static int doAuthSet( SP_XmppIQ * iqPacket, SP_Buffer * reply );

	static int doRegGet( SP_XmppIQ * iqPacket, SP_Buffer * reply );
	static int doRegSet( SP_XmppIQ * iqPacket, SP_Buffer * reply );

	static int doRosterSet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
		SP_XmppJid * fromJid );
	static int doRosterGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
		SP_XmppJid * fromJid );

	static int doAgentsGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
		SP_XmppJid * fromJid );

	static int doBrowseGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
		SP_XmppJid * fromJid );

	static int doPrivateGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
		SP_XmppJid * fromJid );

	static int doUnavailableGet( SP_XmppIQ * iqPacket, SP_Buffer * reply,
		SP_XmppJid * fromJid );

	// presence packet
	static int doPresenceAsk( SP_XmppPresence * prPacket, SP_Response * response,
		SP_XmppJid * fromJid );

	static int doPresenceSub( SP_XmppPresence * prPacket, SP_Response * response,
		SP_XmppJid * fromJid );

	static int doPresenceUnsub( SP_XmppPresence * prPacket, SP_Response * response,
		SP_XmppJid * fromJid );

	static int doPresencePub( SP_XmppPresence * prPacket, SP_Response * response,
		SP_XmppJid * fromJid );

	// message packet
	static int doMessageChat( SP_XmppPacket * packet, SP_XmlElementNode * node,
		SP_Response * response, SP_XmppJid * fromJid );

private:
	SP_XmppAction();
	~SP_XmppAction();
};

#endif

