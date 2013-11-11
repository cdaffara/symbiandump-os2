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
* Description: 
*
*/

#ifndef FRMCONST_PARTNER_H_
#define FRMCONST_PARTNER_H_

enum TCursorMove
/**
This enum holds all the allowed movements of the document cursor by this
cursor positioning framework.
@publishedPartner
@released
*/
	{
	/** No cursor movement 
	*/
	ECursNoMovement,
    /** Single character cursor movement to the left 
    */
    ECursLeft,
    /** Single character cursor movement to the right 
    */
    ECursRight,
    /** Line up cursor movement 
    */
    ECursLineUp,
    /** Line down cursor movement 
    */
    ECursLineDown,
    /** Not used, present for backwards compatibility for page movement 
    */
    ECursReserved1,
    ECursReserved2,
    /** Cursor movement to line start 
    */
    ECursLineBegin,
    /** Cursor movement to line end 
    */
    ECursLineEnd
	};

#endif