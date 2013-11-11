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
* Trace API parser plug-in
*
*/
package com.nokia.tracecompiler.plugin;

import com.nokia.tracecompiler.engine.TraceLocation;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;

/**
 * Trace API parser plug-in
 * 
 */
public interface TraceAPIParser {

	/**
	 * Gets the tag to be located from source
	 * 
	 * @return the tag
	 */
	public String getSourceTag();

	/**
	 * Gets the list of suffixes that are allowed with the source tag
	 * 
	 * @return the suffixes
	 */
	public String[] getTagSuffixes();

	/**
	 * Verifies the validity of given location
	 * 
	 * @param location
	 *            the location to be checked
	 * @return error code from TraceCompilerErrorCodes
	 */
	public TraceCompilerErrorCode checkLocationValidity(TraceLocation location);

	/**
	 * Gets the name of the group where locations created by this parser belong
	 * 
	 * @return the location group
	 */
	public String getLocationGroup();

}
