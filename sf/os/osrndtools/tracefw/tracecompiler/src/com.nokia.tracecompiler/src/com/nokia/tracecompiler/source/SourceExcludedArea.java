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
* Comment or string ares are added to the excluded list
*
*/
package com.nokia.tracecompiler.source;

/**
 * Comment or string ares are added to the excluded list
 * 
 */
public class SourceExcludedArea extends SourceLocationBase {

	/**
	 * Line comment type
	 */
	public static final int LINE_COMMENT = 1; // CodForChk_Dis_Magic

	/**
	 * Multiline comment type
	 */
	public static final int MULTILINE_COMMENT = 2; // CodForChk_Dis_Magic

	/**
	 * String type
	 */
	public static final int STRING = 3; // CodForChk_Dis_Magic

	/**
	 * Character type
	 */
	public static final int CHARACTER = 4; // CodForChk_Dis_Magic

	/**
	 * Preprocessor definition area
	 */
	public static final int PREPROCESSOR_DEFINITION = 5; // CodForChk_Dis_Magic

	/**
	 * Type of area
	 */
	private int type;

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the parser where this area belongs
	 * @param offset
	 *            offset to the area
	 * @param type
	 *            type of area
	 */
	SourceExcludedArea(SourceParser parser, int offset, int type) {
		super(parser, offset);
		this.type = type;
	}

	/**
	 * Gets the type of this area
	 * 
	 * @return the type
	 */
	public int getType() {
		return type;
	}

}
