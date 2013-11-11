/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Properties of a source document opened to Eclipse editor
 *
 */
package com.nokia.tracecompiler.engine.source;

import java.util.ArrayList;
import java.util.Iterator;

import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceLocation;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.source.SourceDocumentFactory;
import com.nokia.tracecompiler.source.SourceDocumentInterface;
import com.nokia.tracecompiler.source.SourceIterator;
import com.nokia.tracecompiler.source.SourceParser;
import com.nokia.tracecompiler.source.SourceParserException;
import com.nokia.tracecompiler.source.SourcePropertyProvider;
import com.nokia.tracecompiler.source.SourceStringSearch;

/**
 * Properties of a source document which contains trace locations
 * 
 */
public class SourceProperties implements Iterable<TraceLocation> {

	/**
	 * Trace locations within the source
	 */
	private ArrayList<TraceLocation> locations = new ArrayList<TraceLocation>();

	/**
	 * Source parser
	 */
	private SourceParser sourceParser;

	/**
	 * Offset is stored in preProcess and reset in postProcess.
	 */
	private int firstChangedLocation = -1;

	/**
	 * Offset is stored in preProcess and reset in postProcess.
	 */
	private int firstUnchangedLocation = -1;

	/**
	 * The searchers for trace identifiers
	 */
	private ArrayList<SourceStringSearch> searchers = new ArrayList<SourceStringSearch>();

	/**
	 * Start index for calls to parseTrace
	 */
	private int searchStartIndex;

	/**
	 * Read-only flag
	 */
	private boolean readOnly;

	/**
	 * Creates source properties for given source document
	 * 
	 * @param model
	 *            the trace model
	 * @param framework
	 *            the document framework
	 * @param document
	 *            the document
	 */
	SourceProperties(TraceModel model, SourceDocumentFactory framework,
			SourceDocumentInterface document) {
		sourceParser = new SourceParser(framework, document);
		Iterator<SourceParserRule> parsers = model
				.getExtensions(SourceParserRule.class);
		while (parsers.hasNext()) {
			// The rule defines what to search and how to interpret the
			// parameters. It is stored into the searcher as search data
			addParserRule(parsers.next());
		}
	}

	/**
	 * Gets the source parser
	 * 
	 * @return the parser
	 */
	public SourceParser getSourceParser() {
		return sourceParser;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	public Iterator<TraceLocation> iterator() {
		return locations.iterator();
	}

	/**
	 * Gets the file name of this source
	 * 
	 * @return the name
	 */
	public String getFileName() {
		String retval = null;
		if (sourceParser != null) {
			SourceDocumentInterface source = sourceParser.getSource();
			if (source != null) {
				SourcePropertyProvider provider = source.getPropertyProvider();
				if (provider != null) {
					retval = provider.getFileName();
				}
			}
		}
		return retval;
	}

	/**
	 * Sets the read-only flag for this source. Traces cannot be added to
	 * read-only sources, but they can be parsed for data
	 * 
	 * @param readOnly
	 *            the read-only flag
	 */
	void setReadOnly(boolean readOnly) {
		this.readOnly = readOnly;
	}

	/**
	 * Gets the read-only flag
	 * 
	 * @return read-only flag
	 */
	public boolean isReadOnly() {
		return readOnly;
	}

	/**
	 * Source opened notification
	 */
	void sourceOpened() {
		updateTraces(0, sourceParser.getDataLength());
	}

	/**
	 * Parses the document starting from given offset and locates the trace
	 * entries from it. The first unchanged trace entry stops the search
	 * 
	 * @param startOffset
	 *            the offset where to start the search
	 * @param endOffset
	 *            the offset where to end the search
	 */
	private void updateTraces(int startOffset, int endOffset) {
		Iterator<SourceStringSearch> itr = searchers.iterator();
		while (itr.hasNext()) {
			SourceStringSearch searcher = itr.next();
			searcher.resetSearch(startOffset, endOffset);
			updateTraces(endOffset, searcher);
		}
	}

	/**
	 * Uses the given SourceSearch to parse traces
	 * 
	 * @param end
	 *            the offset where parser should stop
	 * @param searcher
	 *            the searcher
	 */
	private void updateTraces(int end, SourceStringSearch searcher) {
		int offset;
		searchStartIndex = 0;
		// If not updating, the entries contents are processed
		do {
			offset = searcher.findNext();
			try {
				if (offset != -1 && offset < end) {
					String tag = isValidTrace(offset, searcher
							.getSearchString().length(), searcher, false);
					if (tag != null) {
						parseTrace(offset, (SourceParserRule) searcher
								.getSearchData(), tag);
					}
				}
			} catch (SourceParserException e) {
				TraceLocation location = new TraceLocation(this, offset,
						offset + 80);
				TraceCompilerEngineGlobals
						.getEvents()
						.postErrorMessage(
								Messages
										.getString("SourceProperties.parsingArrowAtBeginText") + location.getFilePath() + location.getFileName() + Messages.getString("SourceProperties.parsingArrownAtMiddleText") + location.getLineNumber(), null, true); //$NON-NLS-1$ //$NON-NLS-2$
				// If the parameters cannot be parsed, the trace is
				// not added to the array
			}
		} while (offset != -1 && offset < end);
	}

	/**
	 * Parses a trace found from the document and adds it to the document's list
	 * of positions. The position updater keeps the trace location up-to-date.
	 * 
	 * @param offset
	 *            the offset to the trace
	 * @param parserRule
	 *            the parser to be attached to the location
	 * @param locationTag
	 *            the tag of the location
	 * @throws SourceParserException
	 *             if trace cannot be parsed
	 */
	private void parseTrace(int offset, SourceParserRule parserRule,
			String locationTag) throws SourceParserException {
		int arrayIndex = -1;
		// Checks the changed locations. If a matching offset if found, the
		// location is an existing one. In that case the location is not
		// added to the array. If an offset larger than the new offset is
		// found from the array, the location is inserted into that slot. If
		// all locations within the array are smaller than the new offset,
		// the new location is inserted before the first unchanged location.
		// Since the locations in the array are ordered, the checking can
		// always start from the latest location that has been found from
		// the array. The caller of this function must set
		// parseTraceStartIndex to 0 before starting a loop where this
		// function is called. If firstUnchangedLocation is -1, this is the
		// first time the file is being parsed and thus all locations are
		// checked
		boolean found = false;
		int searchEndIndex;
		int newSearchStartIndex = -1;
		if (firstUnchangedLocation >= 0) {
			searchEndIndex = firstUnchangedLocation;
		} else {
			searchEndIndex = locations.size();
		}
		for (int i = searchStartIndex; i < searchEndIndex && !found; i++) {
			TraceLocation location = locations.get(i);
			// Deleted locations are ignored. If a trace was replaced, the
			// new offset will match the offset of the deleted one.
			if (!location.isDeleted()) {
				// If the offset of the trace matches an existing offset,
				// the trace is old one. If the offset within the array is
				// larger than the source offset, the trace found from
				// source is new.
				if (location.getOffset() == offset) {
					found = true;
					// Starts the next search from the value following the
					// trace that was found
					searchStartIndex = i + 1;
					arrayIndex = -1;
				} else if (location.getOffset() > offset) {
					found = true;
					// A new trace will be added into the current index, so
					// the next search will start from the same location as
					// was checked now. The index is updated after the trace has
					// succesfully been created
					newSearchStartIndex = i + 1;
					arrayIndex = i;
				}
			}
		}
		// If trace was not found from the list, the trace is new and all
		// traces following it are also new. The start index is set to point
		// past the first unchanged location and thus the next search will
		// ignore the above loop.
		if (!found) {
			arrayIndex = searchEndIndex;
			searchStartIndex = firstUnchangedLocation + 1;
		}
		if (arrayIndex >= 0) {
			// Creates a new location if it was not found
			ArrayList<String> list = new ArrayList<String>();
			int endOfTrace = sourceParser
					.tokenizeParameters(offset, list, true);

			TraceLocation location = new TraceLocation(this, offset, endOfTrace
					- offset);

			// The parser rules have been associated with the searchers. The
			// parser rule that found the location is associated with the
			// location and used to process its parameters
			location.setTag(locationTag);
			location.setParserRule(parserRule);
			location.setData(list);

			TraceCompilerEngineGlobals
					.getEvents()
					.postInfoMessage(
							Messages
									.getString("SourceProperties.newTraceLocationFoundBeginText") + location.getFilePath() + location.getFileName() + Messages.getString("SourceProperties.newTraceLocationFoundMiddleText") + location.getLineNumber() + Messages.getString("SourceProperties.newTraceLocationFoundEndText") + location.getTraceText(), null); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$

			locations.add(arrayIndex, location);
			// The changed flag is set to newly added traces. If a location
			// is added prior to the first changed location, the index of first
			// changed location needs to be adjusted so that the flag gets
			// cleared in postprocessing. Also the index of first unchanged
			// location needs to be updated to reflect the changed array
			if (firstUnchangedLocation >= 0) {
				location.setContentChanged(true);
				if (arrayIndex < firstChangedLocation) {
					firstChangedLocation = arrayIndex;
				}
				firstUnchangedLocation++;
			}
			// Updates the search start index if trace creation was succesful
			if (newSearchStartIndex >= 0) {
				searchStartIndex = newSearchStartIndex;
			}
		}
	}

	/**
	 * Checks that a trace is valid
	 * 
	 * @param offset
	 *            offset to trace identifier
	 * @param length
	 *            length of trace
	 * @param searcher
	 *            the source searcher
	 * @param checkMainTag
	 *            true if the main search tag needs to be checked, false if only
	 *            the tag suffix is checked
	 * @return the trace tag or null if trace is not valid
	 */
	private String isValidTrace(int offset, int length,
			SourceStringSearch searcher, boolean checkMainTag) {
		String retval = null;
		try {
			int idlen = searcher.getSearchString().length();
			int idend = offset + idlen;
			if (checkMainTag) {
				if (length >= idlen
						&& searcher.isSearchStringMatch(sourceParser.getData(
								offset, idlen))) {
					// The previous character must be a separator or white space
					if (offset == 0
							|| !Character.isJavaIdentifierPart(sourceParser
									.getData(offset - 1))) {
						retval = getSearchTag(offset, idend);
					}
				}
			} else {
				// If main tag is not checked
				retval = getSearchTag(offset, idend);
			}
			retval = verifyTag(searcher, retval, idlen);
		} catch (Exception e) {
			if (TraceCompilerEngineConfiguration.ASSERTIONS_ENABLED) {
				TraceCompilerEngineGlobals.getEvents().postAssertionFailed(
						"Trace validity check failed", e); //$NON-NLS-1$
			}
		}
		return retval;
	}

	/**
	 * Verifies the tag against tag suffixes from parser
	 * 
	 * @param searcher
	 *            the searcher
	 * @param tag
	 *            the tag include main tag and suffix
	 * @param idlen
	 *            the length of the main tag
	 * @return the tag if it is valid, null if not
	 */
	private String verifyTag(SourceStringSearch searcher, String tag, int idlen) {
		if (tag != null) {
			// The trace suffix is verified by the parser. For example, if
			// search data is "SymbianTrace" and the tag found from source
			// is "SymbianTraceData1", the parser checks if "Data1" is a
			// valid trace tag suffix.
			if (!((SourceParserRule) searcher.getSearchData())
					.isAllowedTagSuffix(tag.substring(idlen))) {
				tag = null;
			}
		}
		return tag;
	}

	/**
	 * Gets the search tag between offset and next '(' character
	 * 
	 * @param offset
	 *            the start of tag
	 * @param idend
	 *            the end of tag
	 * @return the tag
	 * @throws SourceParserException
	 *             if parser fails
	 */
	private String getSearchTag(int offset, int idend)
			throws SourceParserException {
		// Locates the parameters starting from trace identifier
		String retval = null;
		SourceIterator srcitr = sourceParser.createIterator(idend - 1,
				SourceParser.SKIP_ALL);
		boolean found = false;
		while (srcitr.hasNext() && !found) {
			char c = srcitr.next();
			if (c == ';') {
				// Trace must have parameters
				found = true;
			} else if (c == '(') {
				found = true;
				// Stores the tag into location
				retval = sourceParser.getData(offset, srcitr.previousIndex()
						- offset + 1);
			} else if (srcitr.hasSkipped()) {
				// White spaces are not allowed within trace tag
				found = true;
			}
		}
		return retval;
	}

	/**
	 * Checks if a trace can be inserted into given location
	 * 
	 * @param offset
	 *            the offset to the location
	 * @return true if location is valid
	 */
	boolean checkInsertLocation(int offset) {
		boolean retval = true;
		try {
			offset = sourceParser.findStartOfLine(offset, false, true);
			if (sourceParser.isInExcludedArea(offset)) {
				retval = false;
			}
		} catch (SourceParserException e) {
			retval = false;
		}
		return retval;
	}

	/**
	 * Adds a new parser
	 * 
	 * @param rule
	 *            the new parser rule
	 */
	void addParserRule(SourceParserRule rule) {
		SourceStringSearch searcher = sourceParser.startStringSearch(rule
				.getSearchTag(), 0, -1, SourceParser.MATCH_WORD_BEGINNING
				| SourceParser.SKIP_ALL);
		searcher.setSearchData(rule);
		searchers.add(searcher);
	}

}
