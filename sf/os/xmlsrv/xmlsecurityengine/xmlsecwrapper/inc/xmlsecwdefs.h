/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Macro definitions used in XmlSec code.       
*
*/

#ifndef _DEFS_H
#define _DEFS_H

#define TEMPLATE (static_cast<xmlDocPtr>(iTemplate))

#define INTERNAL_NODEPTR(aNodeObject) (*reinterpret_cast<xmlNodePtr*>(&(aNodeObject)))
#define INTERNAL_DOCPTR(aDocObject)     (*reinterpret_cast<xmlDocPtr*>(&(aDocObject))) 

#define ENC_CTX (static_cast<xmlSecEncCtxPtr>(iEncCtx))
#define SIG_CTX (static_cast<xmlSecDSigCtxPtr>(iSigCtx))

#endif // _DEFS_H
