/* UDP.H
 * 
 * Portions Copyright (c) 1997-1999 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/*
 * Udp protocol header.
 * Per RFC 768, September, 1981.
 */

#ifndef	_NETINET_UDP_H
#define	_NETINET_UDP_H

#ifdef	__cplusplus
extern "C" {
#endif
/**
@publishedAll
@released
*/
struct udphdr {
	u_short	uh_sport;		/* source port */
	u_short	uh_dport;		/* destination port */
	short	uh_ulen;		/* udp length */
	u_short	uh_sum;			/* udp checksum */
};

#ifdef	__cplusplus
}
#endif

#endif	/* _NETINET_UDP_H */
