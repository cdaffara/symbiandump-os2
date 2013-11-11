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
* Source document abstraction
*
*/
package com.nokia.tracecompiler.source;

/**
 * Source document abstraction
 * 
 */
public interface SourceDocumentInterface {

	/**
	 * Gets the property provider interface. This can return null if the
	 * document framework does not support source properties
	 * 
	 * @return the property provider
	 */
	public SourcePropertyProvider getPropertyProvider();

	/**
	 * Gets a subset of document data
	 * 
	 * @param start
	 *            the start offset
	 * @param length
	 *            the data length
	 * @return the data
	 * @throws SourceParserException
	 *             if parameters are not valid
	 */
	public String get(int start, int length) throws SourceParserException;

	/**
	 * Gets a character
	 * 
	 * @param offset
	 *            the offset
	 * @return the character
	 * @throws SourceParserException
	 *             if offset is not valid
	 */
	public char getChar(int offset) throws SourceParserException;

	/**
	 * Gets the data length
	 * 
	 * @return the length
	 */
	public int getLength();

	/**
	 * Maps an offset to line number
	 * 
	 * @param offset
	 *            the offset
	 * @return the line number
	 * @throws SourceParserException
	 *             if offset is not valid
	 */
	public int getLineOfOffset(int offset) throws SourceParserException;

	/**
	 * Replaces data from the document
	 * 
	 * @param offset
	 *            offset to removed data
	 * @param length
	 *            length of removed data
	 * @param newText
	 *            new data
	 * @throws SourceParserException
	 *             if parameters are not valid
	 */
	public void replace(int offset, int length, String newText)
			throws SourceParserException;

	/**
	 * Adds a location to this source.
	 * 
	 * @param location
	 *            the location to be added
	 */
	public void addLocation(SourceLocationInterface location);

	/**
	 * Removes a location from this source.
	 * 
	 * @param location
	 *            the location to be removed
	 */
	public void removeLocation(SourceLocationInterface location);

	/**
	 * Gets the owner of this source
	 * 
	 * @return the owner
	 */
	public Object getOwner();

	/**
	 * Sets the owner of this source
	 * 
	 * @param owner
	 *            the owner
	 */
	public void setOwner(Object owner);

}
