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
* Rule interface for tags to be searched from source files
*
*/
package com.nokia.tracecompiler.engine.source;

import java.util.List;

import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceObjectRule;
import com.nokia.tracecompiler.project.TraceLocationParser;

/**
 * Rule interface for tags to be searched from source files
 * 
 */
public interface SourceParserRule extends TraceObjectRule {

	/**
	 * Parameter conversion result
	 */
	public class ParameterConversionResult {

		/**
		 * Parameter name
		 */
		public String name;

		/**
		 * Parameter type
		 */
		public String type;

		/**
		 * Parameter extensions
		 */
		public List<TraceModelExtension> extensions;
	}

	/**
	 * Trace location conversion result
	 */
	public class TraceConversionResult {

		/**
		 * Name of the group where the trace goes to
		 */
		public String group;

		/**
		 * Name for the trace
		 */
		public String name;

		/**
		 * Text for the trace
		 */
		public String text;

		/**
		 * List of parameters
		 */
		public List<ParameterConversionResult> parameters;

		/**
		 * Extensions for the trace
		 */
		public List<TraceModelExtension> extensions;

	}

	/**
	 * Gets the name of this parser
	 * 
	 * @return the name
	 */
	public String getName();

	/**
	 * Gets the source tag to be located from source
	 * 
	 * @return the tag name
	 */
	public String getSearchTag();

	/**
	 * Checks if the trace tag suffix is allowed
	 * 
	 * @param tagSuffix
	 *            the tag to be checked
	 * @return true if allowed, false if not
	 */
	public boolean isAllowedTagSuffix(String tagSuffix);

	/**
	 * Parses a parameter list found from source
	 * 
	 * @param tag
	 *            the location tag
	 * @param list
	 *            list of parameters
	 * @return the parameter list
	 * @throws TraceCompilerException
	 *             if parameter list is not valid
	 */
	public SourceParserResult parseParameters(String tag, List<String> list)
			throws TraceCompilerException;

	/**
	 * Gets the location parser interface
	 * 
	 * @return the location parser
	 */
	public TraceLocationParser getLocationParser();

}
