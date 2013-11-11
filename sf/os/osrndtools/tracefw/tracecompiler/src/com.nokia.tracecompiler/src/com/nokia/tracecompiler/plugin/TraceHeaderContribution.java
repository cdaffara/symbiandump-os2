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
* Trace header contribution
*
*/
package com.nokia.tracecompiler.plugin;

import com.nokia.tracecompiler.model.TraceModelExtension;

/**
 * Interface which can be added to model by plug-in's to contribute data to the
 * trace header
 * 
 */
public interface TraceHeaderContribution extends TraceModelExtension {

	/**
	 * List of possible contributions to the trace header file
	 */
	public enum TraceHeaderContributionType {

		/**
		 * #define statements to be added to the header
		 */
		GLOBAL_DEFINES,

		/**
		 * #include statements to be added to the header
		 */
		GLOBAL_INCLUDES,

		/**
		 * Content for the main header file
		 */
		MAIN_HEADER_CONTENT

	}

	/**
	 * Gets data to be added to the trace header
	 * 
	 * @param type
	 *            the contribution type
	 * @return the data to be added, depends on type
	 */
	public String[] getContribution(TraceHeaderContributionType type);

}
