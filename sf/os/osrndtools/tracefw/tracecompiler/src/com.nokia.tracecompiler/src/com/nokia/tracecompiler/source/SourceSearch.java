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
* Base class for source search classes
*
*/
package com.nokia.tracecompiler.source;

import java.util.List;

/**
 * Base class for source search classes.
 * 
 */
public abstract class SourceSearch {

	/**
	 * Source parser
	 */
	protected final SourceParser parser;

	/**
	 * Current index within data during search
	 */
	protected int searchIndex;

	/**
	 * End index for search
	 */
	protected int searchEnd;

	/**
	 * Index of the next excluded area during search.
	 */
	protected int searchExcludedIndex;

	/**
	 * Search flags
	 */
	protected int flags;

	/**
	 * Creates a new search
	 * 
	 * @param parser
	 *            the parser containing the source
	 * @param startOffset
	 *            offset to the start of search
	 * @param endOffset
	 *            offset to end of search or -1 for rest of the document
	 * @param flags
	 *            search flags
	 */
	protected SourceSearch(SourceParser parser, int startOffset, int endOffset,
			int flags) {
		this.parser = parser;
		this.flags = flags;
		resetSearch(startOffset, endOffset);
	}

	/**
	 * Resets the current search with a new offset
	 * 
	 * @param startOffset
	 *            the offset to the start of search
	 * @param endOffset
	 *            offset to end of search or -1 for rest of the document
	 */
	public void resetSearch(int startOffset, int endOffset) {
		searchIndex = startOffset;
		searchEnd = endOffset == -1 ? parser.getDataLength() : endOffset;
		// Calculates the starting position for the search based on the
		// offset and excluded areas
		int excluded = parser.findExcludedAreaIndex(startOffset);
		if (excluded >= 0) {
			SourceExcludedArea area = parser.getExcludedAreas().get(excluded);
			searchIndex = area.getOffset() + area.getLength();
			if (excluded < parser.getExcludedAreas().size() - 1) {
				searchExcludedIndex = excluded + 1;
			} else {
				searchExcludedIndex = -1;
			}
		} else {
			// If binarySearch returns < 0, startIndex was between some
			// excluded areas.
			searchExcludedIndex = -1 - excluded;
			if (searchExcludedIndex > parser.getExcludedAreas().size()) {
				searchExcludedIndex = -1;
			}
		}
		skipNonExcludedAreas();
	}

	/**
	 * Changes the index to point behind an excluded area if current index is
	 * within one.
	 */
	protected void skipExcludedArea() {
		// If data index is within an excluded area, it is moved to end of
		// it and the next excluded area is selected
		boolean changedExcludedArea;
		do {
			// If the excluded area changes, the flag is set and the next
			// excluded area is also checked. Otherwise the excluded area index
			// gets left behind if the search index jumps past multiple excluded
			// areas
			changedExcludedArea = false;
			if (searchExcludedIndex >= 0
					&& searchExcludedIndex < parser.getExcludedAreas().size()) {
				SourceExcludedArea area = parser.getExcludedAreas().get(
						searchExcludedIndex);
				if (area.getOffset() <= searchIndex) {
					// If the search offset has skipped past an excluded area,
					// the index is not changed
					if (area.getOffset() + area.getLength() > searchIndex) {
						searchIndex = area.getOffset() + area.getLength();
					}
					searchExcludedIndex++;
					skipNonExcludedAreas();
					changedExcludedArea = true;
				}
			}
		} while (changedExcludedArea);
	}

	/**
	 * Skips past excluded areas which are not actually excluded due to flags
	 */
	private void skipNonExcludedAreas() {
		if (searchExcludedIndex >= 0
				&& searchExcludedIndex < parser.getExcludedAreas().size()) {
			boolean notExcluded = true;
			do {
				// If an excluded area is not excluded due to flags,
				// the excluded area index is moved past that area
				SourceExcludedArea area = parser.getExcludedAreas().get(
						searchExcludedIndex);
				if (!SourceParser.isExcluded(area.getType(), flags)) {
					searchExcludedIndex++;
				} else {
					notExcluded = false;
				}
			} while (notExcluded
					&& searchExcludedIndex < parser.getExcludedAreas().size());
		}
	}

	/**
	 * Returns the next occurence
	 * 
	 * @return the index or -1
	 */
	public abstract int findNext();

	/**
	 * Finds all occurences
	 * 
	 * @param list
	 *            the list where the data is stored
	 * @throws SourceParserException
	 *             if search fails
	 */
	public abstract void findAll(List<Integer> list)
			throws SourceParserException;

}