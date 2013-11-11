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
* Description: Helper functions for internal use in Wrapper.       
*
*/

#ifndef C_INTERNALUTILS_H
#define C_INTERNALUTILS_H

/**
 * Checks the error flag, and if the flag is set: leaves and resets it
 *
 * @lib XmlSecWrapper.dll
 * @since S60 v3.2 
 */ 
void XmlSecErrorFlagTestL();

/**
 * Cleanup node pointer
 *
 * @lib XmlSecWrapper.dll
 * @since S60 v3.2 
 */ 
void LibxmlNodeCleanup(TAny* aNodePtr );
    
#endif // C_INTERNALUTILS_H
