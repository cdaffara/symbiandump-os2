/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Internal file for definitions used in shaping and positioning text
*
*/


/**
 @file
 @internalTechnology
*/

#ifndef SHAPERPARAMS_H_
#define SHAPERPARAMS_H_

#include <e32cmn.h>

/** To be TPckged up for EFbsShapeText message. */
class TShapeMessageParameters
	{
public:
	/** The start of the text to be drawn in the buffer passed. */
	TInt iStart;
	/** The end of the text to be drawn in the buffer passed. */
	TInt iEnd;
	/** The script code. */
	TInt iScript;
	/** The language code. */
	TInt iLanguage;
	};

#endif
