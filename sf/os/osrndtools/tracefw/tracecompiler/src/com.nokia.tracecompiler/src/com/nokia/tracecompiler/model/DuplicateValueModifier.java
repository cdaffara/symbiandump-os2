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
* Base class for value modifiers
*
*/
package com.nokia.tracecompiler.model;

/**
 * Base class for value modifiers
 * 
 */
abstract class DuplicateValueModifier implements TraceObjectModifier {

	/**
	 * Duplicate tag for values
	 */
	private static final String DUPLICATE_TAG = "DUP"; //$NON-NLS-1$

	/**
	 * Duplicate tag separator
	 */
	private static final char DUPLICATE_SEPARATOR = '_';

	/**
	 * Value
	 */
	private String value;

	/**
	 * Changed flag
	 */
	private boolean changed;

	/**
	 * Constructor
	 * 
	 * @param value
	 *            the value
	 */
	DuplicateValueModifier(String value) {
		this.value = value;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectUtils.TraceObjectModifier#getData()
	 */
	public String getData() {
		return value;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectUtils.TraceObjectModifier#hasChanged()
	 */
	public boolean hasChanged() {
		return changed;
	}

	/**
	 * Gets the separator character
	 * 
	 * @return the char
	 */
	protected char getSeparator() {
		return DUPLICATE_SEPARATOR;
	}

	/**
	 * Processes the name and modifies if there are duplicates
	 */
	void processName() {
		int index = getStartIndex(value);
		TraceObject object;
		do {
			object = findObject(value);
			if (object != null) {
				changed = true;
				value = appendIndexToString(value, index);
				index++;
			}
		} while (object != null);
	}

	/**
	 * Gets the start index from given text
	 * 
	 * @param value
	 *            the value to be parsed
	 * @return the start index
	 */
	private int getStartIndex(String value) {
		String valueStr = null;
		if (value.startsWith(DUPLICATE_TAG)) {
			for (int i = DUPLICATE_TAG.length(); i < value.length(); i++) {
				char c = value.charAt(i);
				if (!Character.isDigit(c)) {
					valueStr = value.substring(DUPLICATE_TAG.length(), i);
					i = value.length();
				}
			}
		}
		int retval = -1;
		if (valueStr != null && valueStr.length() > 0) {
			retval = Integer.parseInt(valueStr);
		}
		return retval;
	}

	/**
	 * Appends an index value to given string replacing existing index if it
	 * exists
	 * 
	 * @param value
	 *            the string
	 * @param index
	 *            the index
	 * @return the changed string
	 */
	private String appendIndexToString(String value, int index) {
		StringBuffer newValue = new StringBuffer();
		if (index >= 0) {
			String indexStr = String.valueOf(index);
			newValue.append(DUPLICATE_TAG);
			newValue.append(String.valueOf(index + 1));
			newValue.append(getSeparator());
			newValue.append(value.substring(DUPLICATE_TAG.length()
					+ indexStr.length() + 1)); // Add separator
		} else {
			newValue.append(DUPLICATE_TAG);
			newValue.append("1"); //$NON-NLS-1$
			newValue.append(getSeparator());
			newValue.append(value);
		}
		return newValue.toString();
	}

	/**
	 * Gets the duplicate modifier from beginning of given text
	 * 
	 * @param text
	 *            the text
	 * @return the modifier
	 */
	static String getModifier(String text) {
		String retval = null;
		if (text.startsWith(DUPLICATE_TAG)) {
			for (int i = DUPLICATE_TAG.length(); i < text.length(); i++) {
				char c = text.charAt(i);
				if (!Character.isDigit(c)) {
					retval = text.substring(0, i + 1); // Add separator
					i = text.length();
				}
			}
		}
		return retval;
	}

	/**
	 * Finds the object from model
	 * 
	 * @param value
	 *            the value
	 * @return the object or null if not found
	 */
	abstract TraceObject findObject(String value);

}