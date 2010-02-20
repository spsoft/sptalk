/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>

#include "spserver/spserver.hpp"

#include "spxmpphandler.hpp"

#include "spxmppfiledb.hpp"
#include "spxmppconfig.hpp"

int main( int argc, char * argv[] )
{
	int port = 5222, maxThreads = 10;

	extern char *optarg ;
	int c ;

	while( ( c = getopt ( argc, argv, "p:t:h:v" )) != EOF ) {
		switch ( c ) {
			case 'p' :
				port = atoi( optarg );
				break;
			case 't':
				maxThreads = atoi( optarg );
				break;
			case 'h':
				SP_XmppConfig::getDefault()->setHost( optarg );
				break;
			case '?' :
			case 'v' :
				printf( "Usage: %s [-p <port>] [-t <threads>] [-h <domain>]\n", argv[0] );
				exit( 0 );
		}
	}

#ifdef LOG_PERROR
	openlog( "sptalkd", LOG_CONS | LOG_PID | LOG_PERROR, LOG_USER );
#else
	openlog( "sptalkd", LOG_CONS | LOG_PID, LOG_USER );
#endif

	const char * baseDir = "spool";
	mkdir( baseDir, 0770 );

	SP_XmppPersistManager::getDefault()->setUserDao( new SP_XmppFileUserDao( baseDir ) );
	SP_XmppPersistManager::getDefault()->setRosterDao( new SP_XmppFileRosterDao( baseDir ) );

	SP_Server server( "", port, new SP_XmppHandlerFactory() );

	server.setTimeout( 3600 );
	server.setMaxThreads( maxThreads );
	server.setReqQueueSize( 100, "Sorry, server is busy now!\n" );

	server.runForever();

	closelog();

	return 0;
}

