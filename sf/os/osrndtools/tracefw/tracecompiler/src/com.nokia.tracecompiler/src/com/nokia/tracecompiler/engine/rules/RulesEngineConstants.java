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
* Properties associated with formatting rules
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.rules.osttrace.OstTraceFormatRule;
import com.nokia.tracecompiler.engine.rules.osttrace.OstTraceParserRule;
import com.nokia.tracecompiler.engine.source.SourceParserRule;
import com.nokia.tracecompiler.project.TraceProjectAPI;

/**
 * Constants for rules engine
 * 
 */
interface RulesEngineConstants {

	/**
	 * Trace parsers
	 */
	SourceParserRule[] TRACE_PARSERS = { new OstTraceParserRule() };

	/**
	 * Printf parsers
	 */
	String[] PRINTF_PARSERS = { "RDebug::Print", //$NON-NLS-1$
			"Kern::Printf" //$NON-NLS-1$
	};

	/**
	 * List of supported API's
	 */
	TraceProjectAPI[] TRACE_APIS = { new OstTraceFormatRule() };

	/**
	 * Persistent extensions
	 */
	ClassNameWrapper[] PERSISTENT_EXTENSIONS = {
			new ClassNameWrapper(ParameterTypeMappingRule.STORAGE_NAME,
					ParameterTypeMappingRule.class),
			new ClassNameWrapper(ArrayParameterRuleImpl.STORAGE_NAME,
					ArrayParameterRuleImpl.class) };

}
