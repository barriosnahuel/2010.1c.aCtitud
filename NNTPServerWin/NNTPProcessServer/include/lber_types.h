/* $OpenLDAP: pkg/ldap/include/lber_types.nt,v 1.10.2.1 2003/03/03 17:10:03 kurt Exp $ */
/*
 * Copyright 1998-2003 The OpenLDAP Foundation, Redwood City, California, USA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted only
 * as authorized by the OpenLDAP Public License.  A copy of this
 * license is available at http://www.OpenLDAP.org/license.html or
 * in file LICENSE in the top-level directory of the distribution.
 */

/*
 * LBER types for Windows NT (and Win32)
 * copied by setup.mak to lber_types.h (when confingure is not used).
 */

#ifndef _LBER_TYPES_H
#define _LBER_TYPES_H

#include <ldap_cdefs.h>

LDAP_BEGIN_DECL

/*
 * NT types:
 *
 *	bitsof(short) == 2
 *	bitsof(int)	== 4
 *	bitsof(long) == 4	(or larger)
 *
 *  typedef unsigned int size_t;
 *	typedef unsigned int SOCKET;
 *
 *	we use native C types to avoid sucking in system headers
 */

/* LBER boolean, enum, integers - 32 bits or larger*/
#define LBER_INT_T	int

/* LBER tags - 32 bits or larger */
#define LBER_TAG_T	long

/* LBER socket descriptor */
#define LBER_SOCKET_T	unsigned int

/* LBER lengths - 32 bits or larger*/
#define LBER_LEN_T		long

/* ------------------------------------------------------------ */

/* booleans, enumerations, and integers */
typedef LBER_INT_T ber_int_t;

/* signed and unsigned versions */
typedef signed LBER_INT_T ber_sint_t;
typedef unsigned LBER_INT_T ber_uint_t;

/* tags */
typedef unsigned LBER_TAG_T ber_tag_t;

/* "socket" descriptors */
typedef LBER_SOCKET_T ber_socket_t;

/* lengths */
typedef unsigned LBER_LEN_T ber_len_t;

/* signed lengths */
typedef signed LBER_LEN_T ber_slen_t;

LDAP_END_DECL

#endif /* _LBER_TYPES_H */
