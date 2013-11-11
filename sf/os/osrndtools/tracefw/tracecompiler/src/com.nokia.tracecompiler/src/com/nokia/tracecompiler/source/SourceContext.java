/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Source context is a representation of a range of source code
*
*/
package com.nokia.tracecompiler.source;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

/**
 * Source context is a representation of a range of source code. For example a
 * function in source code could be represented with a context that has the name
 * of the function
 * 
 */
public class SourceContext extends SourceLocationBase implements ParsedType {

	/**
	 * No proposal
	 */
	public static final int NONE = 0; // CodForChk_Dis_Magic

	/**
	 * Start of function proposal type
	 */
	public static final int START_OF_FUNCTION = 1; // CodForChk_Dis_Magic

	/**
	 * End of function proposal type
	 */
	public static final int END_OF_FUNCTION = 2; // CodForChk_Dis_Magic

	/**
	 * Full selection proposal type
	 */
	public static final int FULL_SELECTION = 3; // CodForChk_Dis_Magic

	/**
	 * Class name
	 */
	private String className;

	/**
	 * Function name
	 */
	private String functionName;

	/**
	 * Index to start of parameters
	 */
	private int parameterStart;

	/**
	 * List of return types
	 */
	private ArrayList<String> returnTypes;

	/**
	 * Creates a new SourceContext starting from given offset
	 * 
	 * @param parser
	 *            the parser owning this context
	 * @param offset
	 *            the offset
	 */
	SourceContext(SourceParser parser, int offset) {
		super(parser, offset);
	}

	/**
	 * Gets the class name of this context
	 * 
	 * @return the class name
	 */
	public String getClassName() {
		return className;
	}

	/**
	 * Gets the function name of this context
	 * 
	 * @return the function name
	 */
	public String getFunctionName() {
		return functionName;
	}

	/**
	 * Sets the class name. If the class name contains pointer or reference
	 * characters, they are added to the return statements list
	 * 
	 * @param name
	 *            the new class name
	 */
	void setClassName(String name) {
		className = removePtrAndRef(name);
	}

	/**
	 * Sets the function name. If the function name contains pointer or
	 * reference characters, they are added to the return statements list
	 * 
	 * @param name
	 *            the new function name
	 */
	void setFunctionName(String name) {
		functionName = removePtrAndRef(name);
	}

	/**
	 * Removes the * and & characters from the given tag and adds them as return
	 * modifiers
	 * 
	 * @param name
	 *            the name
	 * @return the name with * and & removed
	 */
	private String removePtrAndRef(String name) {
		String retval = name;
		if (name != null) {
			int start = 0;
			int end = name.length();
			boolean found = true;
			while (start < end && found) {
				char c = name.charAt(start);
				if (c == '*' || c == '&') {
					addReturnModifier(c);
					start++;
				} else {
					found = false;
				}
			}
			found = true;
			while (end > start && found) {
				char c = name.charAt(end - 1);
				if (c == '*' || c == '&') {
					addReturnModifier(c);
					end--;
				} else {
					found = false;
				}
			}
			retval = name.substring(start, end);
		}
		return retval;
	}

	/**
	 * Sets the index for start of parameters
	 * 
	 * @param index
	 *            the index
	 */
	void setParametersStartIndex(int index) {
		parameterStart = index;
	}

	/**
	 * Adds a return type string to the context
	 * 
	 * @param type
	 *            the return type
	 */
	void addReturnType(String type) {
		// Constructors and destructor do not have return types
		if (className != null && functionName!= null && !className.equals(functionName)
				&& !functionName.equals(SourceConstants.TILDE + className)) {
			type = removePtrAndRef(type);
			if (type.length() > 0) {
				if (returnTypes == null) {
					returnTypes = new ArrayList<String>();
				}
				returnTypes.add(type);
			}
		}
	}

	/**
	 * Adds a return type modifier to the list of return types
	 * 
	 * @param c
	 *            the modifier
	 */
	private void addReturnModifier(char c) {
		if (returnTypes == null) {
			returnTypes = new ArrayList<String>();
		}
		returnTypes.add(String.valueOf(c));
	}

	/**
	 * Gets the return types of this context
	 * 
	 * @return the return types
	 */
	Iterator<String> getReturnTypes() {
		List<String> list;
		if (returnTypes != null) {
			list = returnTypes;
		} else {
			list = Collections.emptyList();
		}
		return list.iterator();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.ParsedType#hasQualifier(java.lang.String)
	 */
	public boolean hasQualifier(String type) {
		// All types and qualifiers are in the returnTypes list
		return typeEquals(type);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.ParsedType#typeEquals(java.lang.String)
	 */
	public boolean typeEquals(String type) {
		return (returnTypes != null && returnTypes.contains(type));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.ParsedType#isPointer()
	 */
	public boolean isPointer() {
		boolean retval = false;
		if (returnTypes != null) {
			for (int i = 0; i < returnTypes.size() && !retval; i++) {
				if (returnTypes.get(i).equals(SourceConstants.ASTERISK)) {
					retval = true;
				}
			}
		}
		return retval;
	}

	/**
	 * Checks if return types list contains "void"
	 * 
	 * @return true if void, false if not
	 */
	public boolean isVoid() {
		return (returnTypes == null || returnTypes
				.contains(SourceConstants.VOID));
	}

	/**
	 * Parses the parameters of this source context
	 * 
	 * @param parameterList
	 *            the list where the parameters are stored
	 * @throws SourceParserException
	 *             if parsing fails
	 */
	public void parseParameters(List<SourceParameter> parameterList)
			throws SourceParserException {
		if (getParser() != null) {
			getParser().parseFunctionParameters(parameterStart, parameterList);
		} else {
			throw new SourceParserException(
					SourceErrorCodes.CONTEXT_MISSING_OWNER);
		}
	}

	/**
	 * Parses the return statements of this source context
	 * 
	 * @param returnList
	 *            the list where the return values are stored
	 * @throws SourceParserException
	 *             if parsing fails
	 */
	public void parseReturnValues(List<SourceReturn> returnList)
			throws SourceParserException {
		if (getParser() != null) {
			getParser().parseReturnValues(this, returnList);
		} else {
			throw new SourceParserException(
					SourceErrorCodes.CONTEXT_MISSING_OWNER);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		StringBuffer sb = new StringBuffer();
		if (className != null) {
			sb.append(getClassName());
		}
		sb.append("::"); //$NON-NLS-1$
		sb.append(getFunctionName());
		return sb.toString();
	}

}
