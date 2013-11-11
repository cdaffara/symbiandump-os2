/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/** @file
@publishedAll
@released
*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 *		PROPRIETARY NOTICE (Combined)
 *
 *  This source code is unpublished proprietary information
 *  constituting, or derived under license from AT&T's Unix(r) System V.
 *  In addition, portions of such source code were derived from Berkeley
 *  4.3 BSD under license from the Regents of the University of
 *  California.
 *
 *
 *
 *		Copyright Notice
 *
 *  Notice of copyright on this source code product does not indicate
 *  publication.
 *
 *	(c) 1986,1987,1988,1989  Sun Microsystems, Inc.
 *	(c) 1983,1984,1985,1986,1987,1988,1989  AT&T.
 *		All rights reserved.
 */

#ifndef	_NETINET_ARP_H
#define	_NETINET_ARP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <sys/types.h>
#include <libc/netinet/net_types.h>
/**
Address Resolution Protocol.

See RFC 826 for protocol description.  ARP packets are variable
in size; the arphdr structure defines the fixed-length portion.
Protocol type values are the same as those for 10 Mb/s Ethernet.
It is followed by the variable-sized fields ar_sha, arp_spa,
arp_tha and arp_tpa in that order, according to the lengths
specified.  Field names used correspond to RFC 826.
*/
struct	arphdr {
	u_short	ar_hrd;		/* format of hardware address */
#define	ARPHRD_ETHER 	1	/* ethernet hardware address */
	u_short	ar_pro;		/* format of protocol address */
	u_char	ar_hln;		/* length of hardware address */
	u_char	ar_pln;		/* length of protocol address */
	u_short	ar_op;		/* one of: */
#define	ARPOP_REQUEST	1	/* request to resolve address */
#define	ARPOP_REPLY	2	/* response to previous request */
#define	REVARP_REQUEST	3	/* Reverse ARP request */
#define	REVARP_REPLY	4	/* Reverse ARP reply */
	/*
	 * The remaining fields are variable in size,
	 * according to the sizes above, and are defined
	 * as appropriate for specific hardware/protocol
	 * combinations.  (E.g., see <netinet/if_ether.h>.)
	 */
#ifdef	notdef
	u_char	ar_sha[];	/* sender hardware address */
	u_char	ar_spa[];	/* sender protocol address */
	u_char	ar_tha[];	/* target hardware address */
	u_char	ar_tpa[];	/* target protocol address */
#endif	/* notdef */
};

/**
Ethernet Address Resolution Protocol.

See RFC 826 for protocol description.  Structure below is adapted
to resolving internet addresses.  Field names used correspond to
RFC 826.
*/
struct	ether_arp {
	struct	arphdr ea_hdr;		/* fixed-size header */
	struct ether_addr arp_sha;	/* sender hardware address */
	u_char	arp_spa[4];		/* sender protocol address */
	struct ether_addr arp_tha;	/* target hardware address */
	u_char	arp_tpa[4];		/* target protocol address */
};

/**
ARP ioctl request
*/
struct arpreq {
	struct	sockaddr arp_pa;		/* protocol address */
	struct	sockaddr arp_ha;		/* hardware address */
	int	arp_flags;			/* flags */
};


#ifdef	__cplusplus
}
#endif

#endif	/* _NETINET_ARP_H */
