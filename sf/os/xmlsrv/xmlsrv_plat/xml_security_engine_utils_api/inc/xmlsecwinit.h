/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       Functions used to initialize and close component.
*
*/







#ifndef _INIT_H
#define _INIT_H

#include <e32base.h>

/**
* Initializes XmlSecWrapper data. It should be used before using the wrapper.
*
* @since S60 v3.2
*/
IMPORT_C void XmlSecInitializeL();
    
/**
* Closes XmlSecWrapper. It should be used after finishing work with wrapper.
*
* @since S60 v3.2
* @param aGlobalState Pointer to global state
*/
IMPORT_C void XmlSecCleanup(TAny* aGlobalState = NULL);
    
/**
* Initializes XmlSecWrapper data and pushes it to cleanup stack.
* It should be used before using the wrapper.
*
* @since S60 v3.2
*/
IMPORT_C void XmlSecPushL();
    
/**
* Closes XmlSecWrapper.
* It should be used after finishing work with wrapper.
*
* @since S60 v3.2
*/
IMPORT_C void XmlSecPopAndDestroy();
    
#endif // _INIT_H
