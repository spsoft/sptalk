/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <assert.h>

#include "spxmppdecoder.hpp"

#include "spxml/spxmlparser.hpp"
#include "spxml/spxmlnode.hpp"
#include "spxml/spxmlevent.hpp"

#include "spserver/spbuffer.hpp"

SP_XmppDecoder :: SP_XmppDecoder()
{
	mParser = new SP_XmlPullParser();
	mElementList = new SP_XmlNodeList();

	mElement = NULL;
	mLevel = 0;
	mIsClose = 0;
}

SP_XmppDecoder :: ~SP_XmppDecoder()
{
	if( NULL != mParser ) delete mParser;
	mParser = NULL;

	if( NULL != mElementList ) delete mElementList;
	mElementList = NULL;

	if( NULL != mElement ) delete mElement;
	mElement = NULL;
}

int SP_XmppDecoder :: decode( SP_Buffer * inBuffer )
{
	mParser->append( (char*)inBuffer->getBuffer(), inBuffer->getSize() );
	inBuffer->reset();

	for( SP_XmlPullEvent * event = mParser->getNext();
			NULL != event; event = mParser->getNext() ) {

		switch( event->getEventType() ) {
			case SP_XmlPullEvent::eStartTag:
			{
				SP_XmlElementNode * element =
						new SP_XmlElementNode( (SP_XmlStartTagEvent*)event );
				if( 0 == mLevel ) {
					mElementList->append( element );
				} else {
					if( NULL != mElement ) mElement->addChild( element );
					mElement = element;
				}
				mLevel++;
				break;
			}
			case SP_XmlPullEvent::eEndTag:
			{
				mLevel--;

				if( 0 == mLevel ) {
					SP_XmlElementNode * element = new SP_XmlElementNode();
					element->setName( ((SP_XmlEndTagEvent*)event)->getText() );
					mElementList->append( element );
					mIsClose = 1;
				} else if( 1 == mLevel ) {
					assert( NULL != mElement && NULL == mElement->getParent() );
					mElementList->append( mElement );
					mElement = NULL;
				} else {
					SP_XmlNode * parent = (SP_XmlNode*)mElement->getParent();
					assert( NULL != parent );
					mElement = static_cast<SP_XmlElementNode*>((SP_XmlNode*)parent);
				}
				delete event;
				break;
			}
			case SP_XmlPullEvent::eCData:
			{
				if( NULL != mElement ) {
					mElement->addChild( new SP_XmlCDataNode( (SP_XmlCDataEvent*)event ) );
				} else {
					delete event;
				}
				break;
			}
			default: // ignore eStartDocument/eEndDocument/eDocDecl/eDocType/eComment
				delete event;
				break;
		}
	}

	return mElementList->getLength() > 0 ? eOK : eMoreData;
}

SP_XmlNodeList * SP_XmppDecoder :: getElementList()
{
	return mElementList;
}

int SP_XmppDecoder :: isClose()
{
	return mIsClose;
}

