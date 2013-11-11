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
 * TraceCompilerGlobals global constants
 *
 */
package com.nokia.tracecompiler;

/**
 * TraceCompiler Global constants
 *
 */
public class TraceCompilerGlobals {

	/**
	 * Verbose output. Verbose is usually checked to print Infos
	 */
	private static boolean VERBOSE = false;
	
	/**
	 * KepGoing is used to decide whether we continue processing on errors
	 */
	private static boolean KEEPGOING =  true;
	
	//disallow instances
	private TraceCompilerGlobals() {
		// singleton
	}

	/**
	 * set verbose mode
	 * @param verbose
	 */
	public static void setVerbose(boolean verbose) {
		VERBOSE = verbose;
	}
	
	/**
	 * is vebose mode on/off
	 * @return boolean
	 */
	public static boolean isVebose() {
		return VERBOSE;
	}
	
	/**
	 * set keep going mode
	 * @param keepgoing
	 */
	public static void setKeepGoing(boolean keepgoing) {
		KEEPGOING = keepgoing;
	}
	
	/**
	 * is keepgoing mode on/off
	 * @return boolean
	 */
	public static boolean keepGoing() {
		return KEEPGOING;
	}
}
