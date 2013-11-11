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
* Kernel #endif flag check
*
*/
package com.nokia.tracecompiler.engine.header;

/**
 * Trace activation length check template element
 * 
 */
public final class CheckFormattedTraceExists extends TemplateCheckBase {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.header.TemplateCheckBase#check()
	 */
	@Override
	boolean check() {
		return writer.getCurrentTraceFormatted().length() > 0;
	}

}