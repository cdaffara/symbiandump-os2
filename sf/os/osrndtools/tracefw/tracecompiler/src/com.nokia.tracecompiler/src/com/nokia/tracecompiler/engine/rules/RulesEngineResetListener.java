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
* Model reset listener
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceModelResetListener;
import com.nokia.tracecompiler.project.TraceProjectAPI;

/**
 * Model reset listener
 * 
 */
final class RulesEngineResetListener implements TraceModelResetListener {

	/**
	 * Rule engine
	 */
	private final RulesEngine engine;

	/**
	 * Trace model
	 */
	private final TraceModel model;

	/**
	 * Constructor
	 * 
	 * @param engine
	 *            rule engine
	 * @param model
	 *            the trace model
	 */
	RulesEngineResetListener(RulesEngine engine, TraceModel model) {
		this.engine = engine;
		this.model = model;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelResetListener#modelResetting()
	 */
	public void modelResetting() {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelResetListener#modelReset()
	 */
	public void modelReset() {
		model.removeExtensions(TraceProjectAPI.class);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelResetListener#modelValid(boolean)
	 */
	public void modelValid(boolean valid) throws TraceCompilerException {
		if (valid) {
			model.startProcessing();
			try {
				// Creates the trace API if it does not exist yet
				engine.setDefaultTraceAPI();
				// Adds filler parameters to correct places
				for (TraceGroup group : model) {
					for (Trace trace : group) {
						engine.checkFillerParameters(trace);
					}
				}
			} finally {
				model.processingComplete();
			}
		}
	}
}