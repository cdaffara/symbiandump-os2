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
* String search algorithm
*
*/
package com.nokia.tracecompiler.source;

import java.util.List;

/**
 * String search algorithm
 * 
 */
public class SourceStringSearch extends SourceSearch {

	/**
	 * String to be searched from the source.
	 */
	private String searchString;

	/**
	 * Number of different characters in source file alphabet. Assumes that
	 * 8-bit encoding is used
	 */
	private final static int CHARACTER_COUNT = 256; // CodForChk_Dis_Magic

	/**
	 * Mask for bad character shifts (0xFF)
	 */
	private static final int BAD_CHARACTER_MASK = 0xFF; // CodForChk_Dis_Magic

	/**
	 * String search shifts.
	 */
	private int[] badCharacterShifts = new int[CHARACTER_COUNT];

	/**
	 * User data associated with search
	 */
	private Object searchData;

	/**
	 * Creates a new string search
	 * 
	 * @param parser
	 *            the parser containing the source to be searched
	 * @param searchString
	 *            the string to be searched
	 * @param startOffset
	 *            offset to the start of search
	 * @param endOffset
	 *            offset to end of search or -1 for rest of the document
	 * @param flags
	 *            the search flags
	 */
	public SourceStringSearch(SourceParser parser, String searchString,
			int startOffset, int endOffset, int flags) {
		super(parser, startOffset, endOffset, flags);
		setSearchString(searchString);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceSearch#findNext()
	 */
	@Override
	public int findNext() {
		try {
			SourceDocumentInterface source = parser.getSource();
			int stringIndex = searchString.length() - 1;
			boolean found = false;
			while (!found && searchIndex + stringIndex < searchEnd) {
				char dataChar;
				char searchChar;
				boolean match = false;
				do {
					dataChar = source.getChar(searchIndex + stringIndex);
					searchChar = searchString.charAt(stringIndex);
					if ((flags & SourceParser.IGNORE_CASE) != 0) {
						searchChar = Character.toLowerCase(searchChar);
						dataChar = Character.toLowerCase(dataChar);
					}
					if (searchChar == '?' || searchChar == dataChar) {
						match = true;
						stringIndex--;
					} else {
						match = false;
					}
				} while (match && stringIndex >= 0);
				// If string was not found, resets index and skips according
				// to the shift table
				if (stringIndex < 0) {
					found = true;
					// Checks the previous character if match beginning is set
					if ((flags & SourceParser.MATCH_WORD_BEGINNING) != 0) {
						if ((searchIndex > 0)
								&& isPartOfWord(source.getChar(searchIndex - 1))) {
							found = false;
						}
					}
					// Checks the character after data if match end is set
					if (found && ((flags & SourceParser.MATCH_WORD_END) != 0)) {
						if (((searchIndex + searchString.length()) < source
								.getLength())
								&& isPartOfWord(source.getChar(searchIndex
										+ searchString.length()))) {
							found = false;
						}
					}
				}
				if (!found) {
					int diff = searchString.length() - 1 - stringIndex;
					stringIndex = searchString.length() - 1;
					int skip;

					if (dataChar > badCharacterShifts.length - 1) {
						skip = 1;
					} else {
						skip = badCharacterShifts[dataChar] - diff;
						if (skip <= 0) {
							skip = 1;
						}
					}

					searchIndex += skip;
					skipExcludedArea();
				}
			}
			if (!found) {
				searchIndex = -1;
			}
		} catch (SourceParserException e) {
			searchIndex = -1;
		}
		int ret = searchIndex;
		if (searchIndex != -1) {
			searchIndex += searchString.length();
		}
		return ret;
	}

	/**
	 * Checks if the character is part of a word
	 * 
	 * @param c
	 *            the character to be checked
	 * @return true if part of word
	 */
	private boolean isPartOfWord(char c) {
		return Character.isLetterOrDigit(c) || c == '_';
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceSearch#findAll(java.util.List)
	 */
	@Override
	public void findAll(List<Integer> list) throws SourceParserException {
		resetSearch(0, -1);
		int index;
		do {
			index = findNext();
			if (index > 0) {
				list.add(new Integer(index));
			}
		} while (index > 0);
	}

	/**
	 * Changes the search string
	 * 
	 * @param searchString
	 *            the string to be searched
	 */
	public void setSearchString(String searchString) {
		this.searchString = searchString;
		int length = searchString.length();
		// Elements not found in the string get the maximum shift distance
		for (int i = 0; i < badCharacterShifts.length; i++) {
			badCharacterShifts[i] = searchString.length();
		}
		// Characters of the search string are mapped into the
		// shift distances array. If a character is found multiple
		// times, the smallest shitft distance is stored
		for (int i = 0; i < searchString.length() - 1; i++) {
			if ((flags & SourceParser.IGNORE_CASE) != 0) {
				badCharacterShifts[Character
						.toLowerCase(searchString.charAt(i))
						& BAD_CHARACTER_MASK] = length - i - 1;

			} else {
				badCharacterShifts[searchString.charAt(i) & BAD_CHARACTER_MASK] = length
						- i - 1;
			}
		}
	}

	/**
	 * Returns the search string
	 * 
	 * @return the string
	 */
	public String getSearchString() {
		return searchString;
	}

	/**
	 * Checks if the given string matches the search string
	 * 
	 * @param string
	 *            the string
	 * @return true if they match
	 */
	public boolean isSearchStringMatch(String string) {
		int index = 0;
		boolean match = true;
		while (index < string.length() && match) {
			char dataChar = string.charAt(index);
			char searchChar = searchString.charAt(index);
			if ((flags & SourceParser.IGNORE_CASE) != 0) {
				searchChar = Character.toLowerCase(searchChar);
				dataChar = Character.toLowerCase(dataChar);
			}
			if (searchChar == '?' || searchChar == dataChar) {
				index++;
			} else {
				match = false;
			}
		}
		return match;
	}

	/**
	 * Sets the user variable associated with this searcher
	 * 
	 * @param data
	 *            the variable
	 */
	public void setSearchData(Object data) {
		searchData = data;
	}

	/**
	 * Gets the user variable associated with this searcher
	 * 
	 * @return the variable
	 */
	public Object getSearchData() {
		return searchData;
	}

}