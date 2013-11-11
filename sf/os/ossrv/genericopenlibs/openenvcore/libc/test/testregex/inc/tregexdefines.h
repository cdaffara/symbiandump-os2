/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/





#define IPEXPR    "([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})"
#define EMEXPR    ".*@.*\\.([a-zA-Z]{1,3})$"
#define EMEXPR1   ".^{0,9}[a-z]"
//search for an email address
#define EMEXPR2   "\b[A-Z0-9._%-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b"
//phone number with area code 264
#define EMEXPR3   "\b1?264[)- ]*\d{3}[- ]*\d{4}\b "
//seraching for copy right statements
#define EMEXPR4 "(copyright +(©|\(c\)|&copy;) +\d{4})( *[-,] *\d{4})*"
#define EMEXPR5  "\b[-.+_\da-z]++@[-.\da-z]+\.(?:com|net|org)\b"
//strip domain from url (http:\\www.google .com) not the(http://www.google.com/products/bob?gmail.ckj)
#define EMEXPR6  "http://[^/]*/"
// validates a simple URL like http://www.google.com , ftp://www.soso.com
#define EMEXPR7   "^[http|ftp|wap|https]{3,5}:\//\www\.\w*\.[com|net]{2,3}$"
//to search for multiple email addresses

