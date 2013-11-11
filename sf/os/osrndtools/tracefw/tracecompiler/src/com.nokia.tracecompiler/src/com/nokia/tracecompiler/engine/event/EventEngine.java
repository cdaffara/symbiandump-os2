/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Implements TraceCompilerEvents to forward events to the event router plug-in
*
*/
package com.nokia.tracecompiler.engine.event;

import com.nokia.tracecompiler.TraceCompilerGlobals;
import com.nokia.tracecompiler.TraceCompilerLogger;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorMessages;
import com.nokia.tracecompiler.engine.TraceCompilerEngineEvents;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceLocationBase;

/**
 * Implements TraceCompilerEvents to forward events to the event router plug-in
 * 
 */
public final class EventEngine implements TraceCompilerEngineEvents {

	/**
	 * Info event type
	 */
	public final static int INFO = 1; // CodForChk_Dis_Magic

	/**
	 * Warning event type
	 */
	public final static int WARNING = 2; // CodForChk_Dis_Magic

	/**
	 * Error event type
	 */
	public final static int ERROR = 3; // CodForChk_Dis_Magic

	/**
	 * Critical assertion failure event type
	 */
	public final static int ASSERT_CRITICAL = 4; // CodForChk_Dis_Magic

	/**
	 * Non-critical assertion failure event type
	 */
	public final static int ASSERT_NORMAL = 5; // CodForChk_Dis_Magic
	

	/**
	 * indicates if an error happened for all files
	 */
	private boolean globalProcessingError = false;
	


	/**
	 * Constructor
	 * 
	 * @param model
	 *            the trace model
	 */
	public EventEngine(TraceModel model) {
	}

	/**
	 * check if an error happened
	 * 
	 * @return the error
	 */
	public boolean hasErrorHappened() {
		return globalProcessingError;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEvents#
	 *      postError(com.nokia.tracecompiler.engine.TraceCompilerException)
	 */
	public void postError(TraceCompilerException exception) {
		String message = TraceCompilerEngineErrorMessages.getErrorMessage(exception);
		if (message != null && message.length() > 0) {
			postErrorMessage(message, exception.getErrorSource(), exception.isEventWantedToPost());
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEvents#postErrorMessage(java.lang.String, java.lang.Object, boolean)
	 */
	public void postErrorMessage(String message, Object source, boolean postEvent) {
		
		if (postEvent) {
			postEvent(ERROR, message, source);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEvents#
	 *      postWarningMessage(java.lang.String, java.lang.Object)
	 */
	public void postWarningMessage(String message, Object source) {
		postEvent(WARNING, message, source);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEvents#
	 *      postInfoMessage(java.lang.String, java.lang.Object)
	 */
	public void postInfoMessage(String message, Object source) {
		postEvent(INFO, message, source);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEvents#
	 *      postAssertionFailed(java.lang.String, java.lang.Object)
	 */
	public void postAssertionFailed(String message, Object source) {
		message = "Assertion failed. Reason: " //$NON-NLS-1$
				+ message;
		postEvent(ASSERT_NORMAL, message, source);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEvents#
	 *      postCriticalAssertionFailed(java.lang.String, java.lang.Object)
	 */
	public void postCriticalAssertionFailed(String message, Object source) throws TraceCompilerException {
		message = "Critical assertion failure, the project has been closed. Reason: " //$NON-NLS-1$
				+ message;
		// Closes the trace project and shows an error message
		TraceCompilerEngineGlobals.getTraceCompiler().closeProject();
		postEvent(ASSERT_CRITICAL, message, source);
	}


	/**
	 * Print an event to console
	 * @param type
	 *            event type
	 * @param message
	 *            event message
	 * @param source
	 *            event source
	 */
	private void postEvent(int type, String message, Object source) {
		switch (type) {
		case INFO:
				if (TraceCompilerGlobals.isVebose()) {
					printEvent(type, message, source);

					// This means that the component has been processed. Let's
					// print one space to ensure that parse_mmp.pl will print
					// out something.
				} else if (message.contains(
						"Dictionary File written")) { //$NON-NLS-1$
					System.out.print(" "); //$NON-NLS-1$
				} else if (!message
						.contains("No files were written")) { //$NON-NLS-1$
					// If we get "no files written" message, let's dismiss it ->
					// parse_mmp.pl won't print out anything
					printEvent(type, message, source);
				}
			break;
		case WARNING:
		case ERROR:
		case ASSERT_NORMAL:
		case ASSERT_CRITICAL:
			printEvent(type, message, source);
			break;
		}		
	}

	/**
	 * Prints event
	 * @param type error type
	 * @param message
	 *            event message
	 * @param source
	 *            event source
	 */
	private void printEvent(int type, String message, Object source) {
		if (message == null) {
			message = Messages.getString("EventEngine.unknowError"); //$NON-NLS-1$
		}
		String fullMessage = ""; //$NON-NLS-1$

		
		if (source instanceof SourceLocationBase) {
			SourceLocationBase location = (SourceLocationBase) source;
			fullMessage = location.getFileName() + Messages.getString("EventEngine.lineNumberPrefix") + location.getLineNumber() + SourceConstants.COLON + SourceConstants.SPACE; //$NON-NLS-1$
		} else if (source instanceof String) {
			message = (String) source;
		}
		fullMessage = fullMessage + message;
		
		switch (type) {
		case INFO:
			TraceCompilerLogger.printInfo(fullMessage);
			break;
		case WARNING:
		case ASSERT_NORMAL:
			TraceCompilerLogger.printWarning(fullMessage);
			break;
		case ERROR:
		case ASSERT_CRITICAL:
			globalProcessingError = true;
			TraceCompilerLogger.printError(fullMessage);
			break;
		default:
			break;
		}
	}
	

}
