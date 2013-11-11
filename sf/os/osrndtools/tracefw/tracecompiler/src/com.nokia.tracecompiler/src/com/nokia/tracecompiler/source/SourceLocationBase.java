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
* Base class for locations
*
*/
package com.nokia.tracecompiler.source;

/**
 * Base class for locations
 * 
 */
public class SourceLocationBase {

	/**
	 * Source parser
	 */
	private SourceParser parser;

	/**
	 * Position abstraction
	 */
	private SourceLocationInterface position;

	/**
	 * Line number is cached and updated when changes occur
	 */
	private int currentLine = -1;

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the source parser
	 * @param offset
	 *            the offset to the location
	 */
	protected SourceLocationBase(SourceParser parser, int offset) {
		this.parser = parser;
		position = parser.getDocumentFramework()
				.createLocation(this, offset, 0);
	}

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the source parser
	 * @param offset
	 *            the offset to the location
	 * @param length
	 *            the location length
	 */
	protected SourceLocationBase(SourceParser parser, int offset, int length) {
		this.parser = parser;
		position = parser.getDocumentFramework().createLocation(this, offset,
				length);
	}

	/**
	 * Gets the offset
	 * 
	 * @return offset
	 */
	public final int getOffset() {
		return position.getOffset();
	}

	/**
	 * Sets the offset
	 * 
	 * @param offset
	 *            new offset
	 */
	public final void setOffset(int offset) {
		position.setOffset(offset);
	}

	/**
	 * Gets the length
	 * 
	 * @return length
	 */
	public final int getLength() {
		return position.getLength();
	}

	/**
	 * Sets the length
	 * 
	 * @param length
	 *            the length
	 */
	public final void setLength(int length) {
		position.setLength(length);
	}

	/**
	 * Returns deleted flag
	 * 
	 * @return the flag
	 */
	public final boolean isDeleted() {
		return position.isDeleted();
	}

	/**
	 * Sets the deleted flag
	 */
	public final void delete() {
		position.delete();
	}

	/**
	 * Gets the location interface
	 * 
	 * @return the location interface
	 */
	final SourceLocationInterface getLocation() {
		return position;
	}

	/**
	 * Gets the source parser
	 * 
	 * @return the parser
	 */
	public SourceParser getParser() {
		return parser;
	}

	/**
	 * Resets the source parser
	 */
	protected void resetParser() {
		parser = null;
	}

	/**
	 * Gets the line number of this location
	 * 
	 * @return line number
	 */
	public int getLineNumber() {
		// Line number is set to -1 when notifyUpdate is called
		if (parser != null) {
			if (currentLine == -1) {
				currentLine = parser.getLineNumber(getOffset());
			}
		} else {
			currentLine = -1;
		}
		return currentLine;
	}

	/**
	 * Resets the line number
	 */
	protected void resetLineNumber() {
		currentLine = -1;
	}

	/**
	 * Gets the source file name
	 * 
	 * @return the file name
	 */
	public String getFileName() {
		String retval = null;
		if (parser != null) {
			SourceDocumentInterface owner = parser.getSource();
			if (owner != null) {
				SourcePropertyProvider propertyProvider = owner
						.getPropertyProvider();
				if (propertyProvider != null) {
					retval = propertyProvider.getFileName();
				}
			}
		}
		return retval;
	}

	/**
	 * Gets the source file path
	 * 
	 * @return the path
	 */
	public String getFilePath() {
		String retval = null;
		if (parser != null) {
			SourceDocumentInterface owner = parser.getSource();
			if (owner != null) {
				SourcePropertyProvider propertyProvider = owner
						.getPropertyProvider();
				if (propertyProvider != null) {
					retval = propertyProvider.getFilePath();
				}
			}
		}
		return retval;
	}

}
