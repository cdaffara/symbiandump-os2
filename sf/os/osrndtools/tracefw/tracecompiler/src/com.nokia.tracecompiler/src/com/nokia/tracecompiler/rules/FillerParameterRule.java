/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Rule that defines a filler parameter
*
*/
package com.nokia.tracecompiler.rules;

/**
 * Rule that defines a filler parameter. A filler does not have a source or view
 * representation, but does have a representation in the header and decoder
 * files. It aligns trace parameters to 32-bit boundaries.
 * 
 */
public interface FillerParameterRule extends HiddenTraceObjectRule {
}
