/* $OpenLDAP: pkg/ldap/include/ldap_features.nt,v 1.8.2.1 2003/02/09 17:02:17 kurt Exp $ */
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
 * LDAP Features
 */
#ifndef _LDAP_FEATURES_H
#define _LDAP_FEATURES_H 1

/* OpenLDAP API version macros */
#define LDAP_VENDOR_VERSION			0
#define LDAP_VENDOR_VERSION_MAJOR	2
#define LDAP_VENDOR_VERSION_MINOR	X
#define LDAP_VENDOR_VERSION_PATCH	X

/*
** WORK IN PROGRESS!
**
** OpenLDAP reentrancy/thread-safeness should be dynamically
** checked using ldap_get_option().
**
** The -lldap implementation may or may not be:
**		LDAP_API_FEATURE_THREAD_SAFE
**
** The preprocessor flag LDAP_API_FEATURE_X_OPENLDAP_REENTRANT can
** be used to determine if -lldap is LDAP_API_FEATURE_THREAD_SAFE at
** compile time.
**
** The -lldap_r implementation is always THREAD_SAFE but
** may also be:
**		LDAP_API_FEATURE_SESSION_THREAD_SAFE
**		LDAP_API_FEATURE_OPERATION_THREAD_SAFE
**
** The preprocessor flag LDAP_API_FEATURE_X_OPENLDAP_THREAD_SAFE
** can be used to determine if -lldap_r is availalbe at compile
** time.  You must define LDAP_THREAD_SAFE if and only if you
** link with -lldap_r.
**
** If you fail to define LDAP_THREAD_SAFE when linking with
** -lldap_r or define LDAP_THREAD_SAFE when linking with -lldap,
** provided header definations and declarations may be incorrect.
**
*/

/* is -lldap reentrant or not */
/* #undef LDAP_API_FEATURE_X_OPENLDAP_REENTRANT */

/* is threadsafe version of -lldap (ie: -lldap_r) *available* or not */
#define LDAP_API_FEATURE_X_OPENLDAP_THREAD_SAFE LDAP_VENDOR_VERSION

/* LDAP v2 Referrals */
#define LDAP_API_FEATURE_X_OPENLDAP_V2_REFERRALS LDAP_VENDOR_VERSION

/* LDAP Server Side Sort. */
#define LDAP_API_FEATURE_SERVER_SIDE_SORT 1000

/* LDAP Virtual List View. Version = 1000 + draft revision.
 * VLV requires Server Side Sort control.
 */
#define LDAP_API_FEATURE_VIRTUAL_LIST_VIEW 1000

#endif /* LDAP_FEATURES */
