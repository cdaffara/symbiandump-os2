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
* SourceIterator can be used to traverse through the source file character at a time
*
*/
package com.nokia.tracecompiler.source;

/**
 * SourceIterator can be used to traverse through the source file character at a
 * time.
 * 
 */
public class SourceIterator {

	/**
	 * The source parser
	 */
	private SourceParser parser;

	/**
	 * Index of next excluded area
	 */
	private int nextExcludedIndex;

	/**
	 * The type of next excluded area
	 */
	private int nextExcludedAreaType;

	/**
	 * Offset to the start of next excluded area
	 */
	private int nextExcludedStart;

	/**
	 * Offset to the end of next excluded area
	 */
	private int nextExcludedEnd;

	/**
	 * Index of next character to be fetched
	 */
	private int nextIndex;

	/**
	 * Index of character returned by latest call to next
	 */
	private int currentIndex;

	/**
	 * Index of character returned by previous call to next
	 */
	private int previousIndex;

	/**
	 * Search flags
	 */
	private int flags;

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            source parser
	 * @param startIndex
	 *            the index where to start
	 * @param flags
	 *            the iterator flags
	 */
	SourceIterator(SourceParser parser, int startIndex, int flags) {
		SourceDocumentInterface source = parser.getSource();
		if (startIndex >= source.getLength()
				&& ((flags & SourceParser.BACKWARD_SEARCH) != 0)) {
			nextIndex = source.getLength() - 1;
		} else {
			nextIndex = startIndex;
		}
		this.parser = parser;
		this.flags = flags;
		if (hasNext()) {
			boolean forward = (flags & SourceParser.BACKWARD_SEARCH) == 0;
			nextExcludedIndex = parser.findExcludedAreaIndex(nextIndex);
			if (nextExcludedIndex < 0) {
				nextExcludedIndex = -1 - nextExcludedIndex;
				if (forward) {
					// Update increments the index, so it must be moved behind
					// the start of search
					nextExcludedIndex--;
				}
			}
			// Increments / decrements the next excluded area according to
			// search direction. If direction is backward, this decrements the
			// index. In that case the above initialization has selected the
			// index after the start of search index. If direction is forward,
			// this increments the index.
			updateExcludedIndex();
			// After the excluded index has been set, the white spaces and
			// comments are skipped
			if (forward) {
				forwardSeekNext();
			} else {
				backwardSeekNext();
			}
			previousIndex = startIndex;
			currentIndex = startIndex;
		}
	}

	/**
	 * Determines if there are more characters to process
	 * 
	 * @return true if iterator has more characters
	 */
	public boolean hasNext() {
		return (flags & SourceParser.BACKWARD_SEARCH) == 0 ? nextIndex < parser
				.getSource().getLength() : nextIndex >= 0;
	}

	/**
	 * Gets the next character from this iterator
	 * 
	 * @return the next character
	 * @throws SourceParserException
	 *             if there are no more characters
	 */
	public char next() throws SourceParserException {
		char ret;
		previousIndex = currentIndex;
		currentIndex = nextIndex;
		if ((flags & SourceParser.BACKWARD_SEARCH) == 0) {
			ret = forwardNext();
		} else {
			ret = backwardNext();
		}
		return ret;
	}

	/**
	 * Returns next character moving forward
	 * 
	 * @return the character
	 * @throws SourceParserException
	 *             if there are no more characters
	 */
	private char forwardNext() throws SourceParserException {
		char c = parser.getSource().getChar(nextIndex);
		nextIndex++;
		forwardSeekNext();
		return c;
	}

	/**
	 * Skips to next index
	 */
	private void forwardSeekNext() {
		// Skips over the excluded area if the index enters one
		boolean didSkip;
		SourceDocumentInterface source = parser.getSource();
		try {
			do {
				didSkip = false;
				if (nextIndex >= nextExcludedStart && nextExcludedStart != -1) {
					// Skips if applicable. Otherwise just updates the next
					// excluded
					// area variables
					if (isExcluded()) {
						nextIndex = nextExcludedEnd;
					}
					updateExcludedIndex();
				}
				if ((flags & SourceParser.SKIP_WHITE_SPACES) != 0) {
					// Skips over white spaces
					boolean wspFound = true;
					do {
						// If a white space is skipped, the excluded area check
						// needs to be done again. didSkip flag controls that
						if (nextIndex < source.getLength()
								&& Character.isWhitespace(source
										.getChar(nextIndex))) {
							nextIndex++;
							didSkip = true;
						} else {
							wspFound = false;
						}
					} while (wspFound);
				}
			} while (didSkip);
		} catch (SourceParserException e) {
			// The exception must not be thrown out of this function
		}
	}

	/**
	 * Returns next character moving backward
	 * 
	 * @return the character
	 * @throws SourceParserException
	 *             if there are no more characters
	 */
	private char backwardNext() throws SourceParserException {
		char c = parser.getSource().getChar(nextIndex);
		nextIndex--;
		backwardSeekNext();
		return c;
	}

	/**
	 * Skips to previous index
	 */
	private void backwardSeekNext() {
		// Skips over the excluded area if the index enters one
		boolean didSkip;
		SourceDocumentInterface source = parser.getSource();
		try {
			do {
				didSkip = false;
				if (nextIndex <= nextExcludedEnd - 1) {
					// Skips if applicable. Otherwise just updates the next
					// excluded
					// area variables
					if (isExcluded()) {
						nextIndex = nextExcludedStart - 1;
					}
					updateExcludedIndex();
				}
				if ((flags & SourceParser.SKIP_WHITE_SPACES) != 0) {
					boolean wspFound = true;
					do {
						// If a white space is skipped, the excluded area check
						// needs to be done again. didSkip flag controls that
						if (nextIndex >= 0
								&& Character.isWhitespace(source
										.getChar(nextIndex))) {
							nextIndex--;
							didSkip = true;
						} else {
							wspFound = false;
						}
					} while (wspFound);
				}
			} while (didSkip);
		} catch (SourceParserException e) {
			// The exception must not be thrown out of this function
		}
	}

	/**
	 * Updates the excluded area index
	 */
	private void updateExcludedIndex() {
		if ((flags & SourceParser.BACKWARD_SEARCH) == 0) {
			nextExcludedIndex++;
		} else {
			nextExcludedIndex--;
		}
		// Updates the values using the next excluded area
		if (nextExcludedIndex >= 0
				&& nextExcludedIndex < parser.getExcludedAreas().size()) {
			SourceExcludedArea p = parser.getExcludedAreas().get(
					nextExcludedIndex);
			nextExcludedStart = p.getOffset();
			nextExcludedEnd = p.getOffset() + p.getLength();
			nextExcludedAreaType = p.getType();
		} else {
			nextExcludedStart = -1;
			nextExcludedEnd = -1;
		}
	}

	/**
	 * Returns the index where the next character will be fetched
	 * 
	 * @return the index
	 */
	public int nextIndex() {
		return nextIndex;
	}

	/**
	 * Gets the index of the character returned by last call to next
	 * 
	 * @return the index
	 */
	public int currentIndex() {
		return currentIndex;
	}

	/**
	 * Gets the index that preceeded the latest call to next
	 * 
	 * @return the index
	 */
	public int previousIndex() {
		return previousIndex;
	}

	/**
	 * Gets the next character but does not move the iterator
	 * 
	 * @return the next character
	 * @throws SourceParserException
	 *             if there are no more characters
	 */
	public char peek() throws SourceParserException {
		return parser.getSource().getChar(nextIndex);
	}

	/**
	 * Determines if the iterator skipped over characters during last call to
	 * next
	 * 
	 * @return true if skipped, false otherwise
	 */
	public boolean hasSkipped() {
		return (flags & SourceParser.BACKWARD_SEARCH) == 0 ? currentIndex > previousIndex + 1
				: currentIndex < previousIndex - 1;
	}

	/**
	 * Checks if the next area is skipped
	 * 
	 * @return true is skipped
	 */
	private boolean isExcluded() {
		return isExcluded(nextExcludedAreaType);
	}

	/**
	 * Checks if the given type is skipped
	 * 
	 * @param type
	 *            the type
	 * @return true is skipped
	 */
	private boolean isExcluded(int type) {
		return SourceParser.isExcluded(type, flags);
	}

}
