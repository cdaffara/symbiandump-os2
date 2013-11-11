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
 * Simple Logger
 *
 */
package com.nokia.tracecompiler;



/**
 * Simple logger for errors warnings and infos
 *
 */
public class TraceCompilerLogger {

	/**
	 * Prints errors message to STDERR.
	 * 
	 * @param msg
	 *            the message
	 */
	static public void printError(String msg) {
		if (msg != null) {
			System.err.println(Messages.getString("TraceCompilerLogger.errorPrefix") + msg); //$NON-NLS-1$
			System.err.flush();
		}
	}
	
	/**
	 * Prints warning message to STDERR.
	 * @param msg
	 */
	static public void printWarning(String msg) {
			System.err.println(Messages.getString("TraceCompilerLogger.warningPrefix") + msg); //$NON-NLS-1$
			System.err.flush();
	}

	/**
	 * Prints warning message to STDOUT if verbose is on.
	 * @param msg
	 */
	static public void printInfo(String msg) {
		if (TraceCompilerGlobals.isVebose()) {
			System.out.println(Messages.getString("TraceCompilerLogger.infoPrefix") + msg); //$NON-NLS-1$
			System.out.flush();
		}
	}
	
	/**
	 * Prints warning message to STDOUT.
	 * @param msg
	 */
	static public void printMessage(String msg) {
			System.out.println(msg);
			System.out.flush();
	}
}
