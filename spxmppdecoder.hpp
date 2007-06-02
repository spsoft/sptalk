/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spxmppdecoder_hpp__
#define __spxmppdecoder_hpp__

#include "spmsgdecoder.hpp"

class SP_XmlNodeList;
class SP_XmlElementNode;
class SP_XmlPullParser;
class SP_Buffer;

class SP_XmppDecoder : public SP_MsgDecoder {
public:
	SP_XmppDecoder();

	virtual ~SP_XmppDecoder();

	virtual int decode( SP_Buffer * inBuffer );

	SP_XmlNodeList * getElementList();

	int isClose();

private:
	SP_XmlPullParser * mParser;
	SP_XmlNodeList * mElementList;

	SP_XmlElementNode * mElement;
	int mLevel;

	int mIsClose;
};

#endif

