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
* Parser for comments and strings
*
*/
package com.nokia.tracecompiler.source;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Parser for comments and strings
 * 
 */
final class ExcludedAreaParser {

	/**
	 * The check range is used to limit the effect of unterminated ' in code
	 */
	private static final int CHAR_CHECK_RANGE = 3; // CodForChk_Dis_Magic

	/**
	 * Source parser
	 */
	private SourceParser parser;

	/**
	 * List of source file areas that are not used in search
	 */
	private ArrayList<SourceExcludedArea> excludedAreas = new ArrayList<SourceExcludedArea>();

	/**
	 * Comparator for array sorting and searching
	 */
	private PositionArrayComparator arrayComparator = new PositionArrayComparator();

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the source parser
	 */
	ExcludedAreaParser(SourceParser parser) {
		this.parser = parser;
	}

	/**
	 * Resets the excluded areas
	 */
	void reset() {
		excludedAreas.clear();
	}

	/**
	 * Finds the array index of the excluded area which contains the offset. If
	 * none of the areas contain the offset, returns negative integer indicating
	 * the index of the excluded area following the offset
	 * 
	 * @param offset
	 *            the offset to the data
	 * @return the excluded area index
	 */
	int find(int offset) {
		return Collections.binarySearch(excludedAreas, new SourceLocationBase(
				parser, offset), arrayComparator);
	}

	/**
	 * Finds the excluded source file areas. Excluded areas include comments and
	 * quoted strings. Overwrites possible old areas.
	 * 
	 * @throws SourceParserException
	 *             if processing fails
	 */
	void parseAll() throws SourceParserException {
		excludedAreas.clear();
		ExcludedAreaSearchData data = new ExcludedAreaSearchData();
		int length = parser.getSource().getLength();
		SourceExcludedArea lastarea = parse(data, length);
		if (data.inString || data.inChar || data.inComment
				|| data.inLineComment || data.inPreprocessor) {
			lastarea.setLength(parser.getSource().getLength()
					- lastarea.getOffset());
			excludedAreas.add(lastarea);
		}
	}

	/**
	 * Parses the excluded areas of source
	 * 
	 * @param data
	 *            the search data
	 * @param length
	 *            the length of data to be parsed
	 * @return the last area
	 * @throws SourceParserException
	 *             if parser fails
	 */
	private SourceExcludedArea parse(ExcludedAreaSearchData data, int length)
			throws SourceParserException {
		SourceExcludedArea area = null;
		while (data.index < length) {
			data.value = parser.getSource().getChar(data.index++);
			// Line comments end at end-of-line
			if (data.inLineComment) {
				processInLineComment(data, area);
			} else if (data.inComment) {
				processInComment(data, area);
			} else if (data.inPreprocessor) {
				processInPreprocessor(data, area);
			} else if (data.inString) {
				processInString(data, area);
			} else if (data.inChar) {
				processInChar(data, area);
			} else if (data.value == '/' && data.index < length) {
				area = createCommentArea(data);
			} else if (data.value == '\"') {
				area = createStringArea(data);
			} else if (data.value == '\'') {
				area = createCharArea(data);
			} else if (data.value == '#'
					&& (data.index == 1 || parser.getSource().getChar(
							data.index - 2) == '\n')) { // CodForChk_Dis_Magic
				area = createPreprocessorArea(data);
			}
		}
		return area;
	}

	/**
	 * Gets the excluded area that contains given offset
	 * 
	 * @param offset
	 *            the offset to the area
	 * @return the area or null if offset does not hit any area
	 */
	SourceExcludedArea getArea(int offset) {
		SourceExcludedArea retval;
		int index = find(offset);
		if (index >= 0) {
			retval = excludedAreas.get(index);
		} else {
			retval = null;
		}
		return retval;
	}

	/**
	 * Gets the list of excluded areas
	 * 
	 * @return the list of areas
	 */
	List<SourceExcludedArea> getAreas() {
		return excludedAreas;
	}

	/**
	 * Processes a quote (') character marking start of character area
	 * 
	 * @param data
	 *            the search flags
	 * @return the new area
	 */
	private SourceExcludedArea createCharArea(ExcludedAreaSearchData data) {
		SourceExcludedArea area;
		data.inChar = true;
		area = new SourceExcludedArea(parser, data.index - 1,
				SourceExcludedArea.CHARACTER);
		return area;
	}

	/**
	 * Processes a double quote (") character marking start of string area
	 * 
	 * @param data
	 *            the search flags
	 * @return the new area
	 */
	private SourceExcludedArea createStringArea(ExcludedAreaSearchData data) {
		SourceExcludedArea area;
		data.inString = true;
		area = new SourceExcludedArea(parser, data.index - 1,
				SourceExcludedArea.STRING);
		return area;
	}

	/**
	 * Processes a forward slash (/) character marking start of comment
	 * 
	 * @param data
	 *            the search flags
	 * @return the comment object
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private SourceExcludedArea createCommentArea(ExcludedAreaSearchData data)
			throws SourceParserException {
		SourceExcludedArea area;
		char next = parser.getSource().getChar(data.index);
		if (next == '/') {
			data.inLineComment = true;
			area = new SourceExcludedArea(parser, data.index - 1,
					SourceExcludedArea.LINE_COMMENT);
			data.index++;
		} else if (next == '*') {
			data.inComment = true;
			area = new SourceExcludedArea(parser, data.index - 1,
					SourceExcludedArea.MULTILINE_COMMENT);
			data.index++;
		} else {
			area = null;
		}
		return area;
	}

	/**
	 * Processes a preprocessor definition
	 * 
	 * @param data
	 *            the search flags
	 * @return the preprocessor area representation
	 */
	private SourceExcludedArea createPreprocessorArea(
			ExcludedAreaSearchData data) {
		SourceExcludedArea area = new SourceExcludedArea(parser,
				data.index - 1, SourceExcludedArea.PREPROCESSOR_DEFINITION);
		data.inPreprocessor = true;
		return area;
	}

	/**
	 * Processes a character that belongs to '' area
	 * 
	 * @param data
	 *            the search flags
	 * @param area
	 *            the area under processing
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processInChar(ExcludedAreaSearchData data,
			SourceExcludedArea area) throws SourceParserException {
		// The check range is used to limit the effect of unterminated '
		if ((data.value == '\'' && parser.getSource().getChar(data.index - 2) != '\\') // CodForChk_Dis_Magic
				|| data.index - area.getOffset() > CHAR_CHECK_RANGE) {
			data.inChar = false;
			area.setLength(data.index - area.getOffset());
			excludedAreas.add(area);
		}
	}

	/**
	 * Processes a character that belongs to "" area
	 * 
	 * @param data
	 *            the search flags
	 * @param area
	 *            the area under processing
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processInString(ExcludedAreaSearchData data,
			SourceExcludedArea area) throws SourceParserException {
		// Strings end with " unless escaped with \" (except \\")
		if (data.value == '\"') {
			if (parser.getSource().getChar(data.index - 2) != '\\' // CodForChk_Dis_Magic
					|| parser.getSource().getChar(data.index - 3) == '\\') { // CodForChk_Dis_Magic
				data.inString = false;
				area.setLength(data.index - area.getOffset());
				excludedAreas.add(area);
			}
		}
	}

	/**
	 * Processes a character that belongs to multi-line comment
	 * 
	 * @param data
	 *            the search flags
	 * @param area
	 *            the area under processing
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processInComment(ExcludedAreaSearchData data,
			SourceExcludedArea area) throws SourceParserException {
		// Comments end with */
		if (data.value == '*') {
			if (data.index < parser.getSource().getLength()
					&& parser.getSource().getChar(data.index) == '/') {
				data.index++;
				data.inComment = false;
				area.setLength(data.index - area.getOffset());
				excludedAreas.add(area);
			}
		}
	}

	/**
	 * Processes a character that belongs to line comment
	 * 
	 * @param data
	 *            the search flags
	 * @param area
	 *            the area under processing
	 */
	private void processInLineComment(ExcludedAreaSearchData data,
			SourceExcludedArea area) {
		if (data.value == '\n') {
			data.inLineComment = false;
			area.setLength(data.index - area.getOffset());
			excludedAreas.add(area);
		}
	}

	/**
	 * Processes a character that belongs to preprocessor definition
	 * 
	 * @param data
	 *            the search flags
	 * @param area
	 *            the area under processing
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processInPreprocessor(ExcludedAreaSearchData data,
			SourceExcludedArea area) throws SourceParserException {
		if (data.value == '\n') {
			char prev = parser.getSource().getChar(data.index - 2); // CodForChk_Dis_Magic
			char prev2 = parser.getSource().getChar(data.index - 3); // CodForChk_Dis_Magic
			if (!((prev == '\\') || (prev == '\r' && prev2 == '\\'))) {
				data.inPreprocessor = false;
				area.setLength(data.index - area.getOffset());
				excludedAreas.add(area);
			}
		}
	}
}
