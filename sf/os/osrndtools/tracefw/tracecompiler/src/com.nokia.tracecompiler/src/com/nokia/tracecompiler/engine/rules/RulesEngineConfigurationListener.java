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
* Configuration listener
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.TraceCompilerEngineConfigurationListener;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;

/**
 * Configuration listener for rule engine monitors the trace formatting
 * 
 */
final class RulesEngineConfigurationListener implements
		TraceCompilerEngineConfigurationListener {

	/**
	 * Rules engine
	 */
	private RulesEngine engine;

	/**
	 * Constructor
	 * 
	 * @param engine
	 *            the rules engine
	 */
	RulesEngineConfigurationListener(RulesEngine engine) {
		this.engine = engine;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfigurationListener#configurationCreated()
	 */
	public void configurationCreated() {
		if (TraceCompilerEngineConfiguration.ALLOW_FORMAT_CHANGE) {
			engine.traceAPIChanged(TraceCompilerEngineGlobals.getConfiguration()
					.getText(TraceCompilerEngineConfiguration.FORMATTER_NAME));
		}
	}
}
