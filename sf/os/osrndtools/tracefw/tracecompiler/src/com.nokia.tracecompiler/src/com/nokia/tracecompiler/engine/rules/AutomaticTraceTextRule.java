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
* Rule which specifies that trace does not have user-defined text
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.source.SourceContext;

/**
 * Rule which specifies that trace does not have user-defined text
 * 
 */
public abstract class AutomaticTraceTextRule extends RuleBase {

	/**
	 * Formats a trace according to given source context
	 * 
	 * @param context
	 *            the context
	 * @return the trace
	 */
	public abstract String formatTrace(SourceContext context);

}
