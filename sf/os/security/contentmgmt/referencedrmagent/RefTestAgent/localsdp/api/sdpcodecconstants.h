/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SdpCodecConstants.h
* Part of       : SDPCodec
* Constants defination file 
* Version       : 1.0 
*
*/




#ifndef __SDPCODECCONSTANTS_H__
#define __SDPCODECCONSTANTS_H__

/**
* @file
* @publishedAll
* @released
*/


// Delimiter constants
const TText8 KDotChar = '.';
const TText8 KColonChar = ':';
const TText8 KSPChar = ' ';  
const TText8 KEqualChar = '=';
const TText8 KSlashChar = '/';
const TText8 KHyphenChar = '-';
const TText8 KLFChar = '\n';
const TText8 KCRChar = '\r';
const TText8 KEofChar = '\0';

_LIT8(KDotStr, ".");
_LIT8(KColonStr, ":");
_LIT8(KSPStr, " ");
_LIT8(KEqualStr, "=");
_LIT8(KSlashStr, "/");
_LIT8(KHyphenStr, "-");
_LIT8(KCRLFStr, "\r\n");
_LIT8(KLFStr, "\n");
_LIT8(KCRStr, "\r");
_LIT8(KEofStr, "\0");
_LIT8(KValidFQDNChars, 
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-");

#endif
