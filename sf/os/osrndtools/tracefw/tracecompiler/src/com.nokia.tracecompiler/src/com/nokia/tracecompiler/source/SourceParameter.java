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
* Representation of a parameter parsed from source
*
*/
package com.nokia.tracecompiler.source;

import java.util.ArrayList;

/**
 * Representation of a parameter parsed from source
 * 
 */
public class SourceParameter implements ParsedType {

	/**
	 * Parameter qualifiers
	 */
	private ArrayList<String> qualifiers;

	/**
	 * Parameter type
	 */
	private String type;

	/**
	 * Parameter name
	 */
	private String name;

	/**
	 * Flag specifying if parameter is reference
	 */
	private boolean isReference;

	/**
	 * Number of pointers in parameter
	 */
	private int pointerCount;

	/**
	 * The location of this parameter in source
	 */
	private SourceLocation location;

	/**
	 * Gets the parameter type
	 * 
	 * @return the type
	 */
	public String getType() {
		return type;
	}

	/**
	 * Gets the parameter name or null if parameter does not have a name
	 * 
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	/**
	 * Checks if parameter is pointer (*) or pointer reference (*&)
	 * 
	 * @return true if pointer, false if not
	 */
	public boolean isPointer() {
		return pointerCount > 0;
	}

	/**
	 * Gets the number of pointers
	 * 
	 * @return the number of pointers
	 */
	public int getPointerCount() {
		return pointerCount;
	}

	/**
	 * Checks if a parameter is referece (&) or pointer reference (*&)
	 * 
	 * @return true if reference, false if not
	 */
	public boolean isReference() {
		return isReference;
	}

	/**
	 * Checks if paraemter is pointer (*), but not pointer reference (*&). Equal
	 * to isPointer() && !isReference()
	 * 
	 * @return true if pointer, but not reference
	 */
	public boolean isPlainPointer() {
		return pointerCount > 0 && !isReference;
	}

	/**
	 * Checks if parameter is reference (&), but not pointer reference (*&).
	 * Equal to !isPointer() && isReference()
	 * 
	 * @return true if reference to value
	 */
	public boolean isPlainReference() {
		return pointerCount == 0 && isReference;
	}

	/**
	 * Checks if parameter is a reference to a pointer (*&). Equal to
	 * isPointer() && isReference()
	 * 
	 * @return true if reference to a pointer
	 */
	public boolean isPointerReference() {
		return pointerCount > 0 && isReference;
	}

	/**
	 * Checks if parameter is pointer (*) or reference (&). Equal to isPointer() ||
	 * isReference()
	 * 
	 * @return true if pointer or reference
	 */
	public boolean isPointerOrReference() {
		return pointerCount > 0 || isReference;
	}

	/**
	 * Checks if parameter type equals given type
	 * 
	 * @param type
	 *            the type to be checked
	 * @return true if types match
	 */
	public boolean typeEquals(String type) {
		return this.type != null && type != null && this.type.equals(type);
	}

	/**
	 * Returns the source reference of this parameter
	 * 
	 * @return the source reference
	 */
	public SourceLocation getSourceLocation() {
		return location;
	}

	/**
	 * Sets the parameter type
	 * 
	 * @param type
	 *            the new type
	 */
	void setType(String type) {
		this.type = type;
	}

	/**
	 * Sets the parameter name
	 * 
	 * @param name
	 *            the new name
	 */
	void setName(String name) {
		this.name = name;
	}

	/**
	 * Sets the reference flag
	 */
	void setReference() {
		isReference = true;
	}

	/**
	 * Increments the pointer count
	 */
	void addPointer() {
		pointerCount++;
	}

	/**
	 * Adds a qualifier to this parameter
	 * 
	 * @param type
	 *            the qualifier type
	 */
	void addQualifier(String type) {
		if (qualifiers == null) {
			qualifiers = new ArrayList<String>();
		}
		qualifiers.add(type);
	}

	/**
	 * Sets the source location of this parameter
	 * 
	 * @param location
	 *            the location
	 */
	void setSourceLocation(SourceLocation location) {
		this.location = location;
	}

	/**
	 * Checks if a qualifier exists
	 * 
	 * @param qualifier
	 *            the qualifier to be checked
	 * @return true if it exists
	 */
	public boolean hasQualifier(String qualifier) {
		boolean retval;
		if (qualifiers != null) {
			retval = qualifiers.contains(qualifier);
		} else {
			retval = false;
		}
		return retval;
	}

}
