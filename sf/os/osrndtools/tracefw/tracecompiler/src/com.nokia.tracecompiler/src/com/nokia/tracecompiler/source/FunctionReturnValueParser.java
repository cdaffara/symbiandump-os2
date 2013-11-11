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
* Parser for function return values
*
*/
package com.nokia.tracecompiler.source;

import java.util.Collections;
import java.util.List;

/**
 * Parser for function return values
 * 
 */
class FunctionReturnValueParser {

	/**
	 * Source parser
	 */
	private SourceParser parser;

	/**
	 * Comparator for source return values
	 */
	private PositionArrayComparator comparator = new PositionArrayComparator();

	/**
	 * Macros representing return statement
	 */
	private List<String> returnExtensions;

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the source parser
	 */
	FunctionReturnValueParser(SourceParser parser) {
		this.parser = parser;
	}

	/**
	 * Parses the return values of given source context
	 * 
	 * @param context
	 *            the context to be parsed
	 * @param list
	 *            the list of return values
	 */
	void parseReturnValues(SourceContext context, List<SourceReturn> list) {
		String statement = SourceConstants.RETURN;
		try {
			parseReturnValues(context, list, statement);
		} catch (SourceParserException e) {
		}
		if (returnExtensions != null && returnExtensions.size() > 0) {
			for (int i = 0; i < returnExtensions.size(); i++) {
				try {
					parseReturnValues(context, list, returnExtensions.get(i));
				} catch (SourceParserException e) {
				}
			}
			Collections.sort(list, comparator);
		}
		if (context.isVoid()) {
			boolean addToEnd = true;
			// If there were no return statements, the trace is added to end
			// Otherwise the last return statement needs to be checked. If it is
			// at the end of the function, the return trace is not added to
			// the end
			if (!list.isEmpty()) {
				SourceReturn ret = list.get(list.size() - 1);
				SourceIterator itr = parser.createIterator(ret.getOffset()
						+ ret.getLength() + 1, SourceParser.SKIP_ALL);
				try {
					itr.next();
					// If the next character after return statement is the end
					// of function, the return is not added
					if (itr.currentIndex() == context.getOffset()
							+ context.getLength() - 1) {
						addToEnd = false;
					}
				} catch (SourceParserException e) {
					addToEnd = false;
				}
			}
			if (addToEnd) {
				list.add(new SourceReturn(parser, context.getOffset()
						+ context.getLength() - 1, 0));
			}
		}
	}

	/**
	 * Parses the return values of given source context that use the given
	 * return statement
	 * 
	 * @param context
	 *            the context to be parsed
	 * @param list
	 *            the list of return values
	 * @param statement
	 *            the return statement
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void parseReturnValues(SourceContext context,
			List<SourceReturn> list, String statement)
			throws SourceParserException {
		FunctionReturnValueSearchData data = new FunctionReturnValueSearchData();
		int offset = context.getOffset();
		int end = offset + context.getLength();
		SourceSearch search = parser.startStringSearch(statement, offset, end,
				SourceParser.MATCH_WHOLE_WORD | SourceParser.SKIP_ALL);
		boolean looping = true;
		do {
			data.index = search.findNext();
			if (data.index != -1) {
				data.itr = parser.createIterator(data.index
						+ statement.length(), SourceParser.SKIP_ALL);
				locateReturnStatement(data);
				if (data.endOffset != -1 && data.startOffset != -1) {
					SourceReturn ret = createReturnStatement(data);
					list.add(ret);
				} else {
					// End of return statement missing
					looping = false;
				}
			} else {
				looping = false;
			}
		} while (looping);
	}

	/**
	 * Locates the start and end offsets for the return statement
	 * 
	 * @param data
	 *            the search data
	 * @throws SourceParserException
	 *             if parser fails
	 */
	private void locateReturnStatement(FunctionReturnValueSearchData data)
			throws SourceParserException {
		data.startOffset = -1;
		data.endOffset = -1;
		boolean found = false;
		boolean colonAllowed = false;
		while (data.itr.hasNext() && !found) {
			char c = data.itr.next();
			if (c == ';') {
				data.endOffset = data.itr.previousIndex() + 1;
				found = true;
			} else if (c == '}') {
				found = true;
			} else if (c == '?') {
				colonAllowed = true;
			} else if (c == ':') {
				if (data.itr.hasNext() && data.itr.peek() == ':') {
					// Skips over ::
					c = data.itr.next();
				} else {
					if (colonAllowed) {
						colonAllowed = false;
					} else {
						data.endOffset = data.itr.previousIndex() + 1;
						found = true;
					}
				}
			}
			if (data.startOffset == -1) {
				data.startOffset = data.itr.currentIndex();
			}
		}
	}

	/**
	 * Creates a return statement
	 * 
	 * @param data
	 *            the parser data
	 * @return the new statement
	 * @throws SourceParserException
	 *             if parser fails
	 */
	private SourceReturn createReturnStatement(
			FunctionReturnValueSearchData data) throws SourceParserException {
		SourceReturn ret = new SourceReturn(parser, data.startOffset,
				data.endOffset - data.startOffset);
		if (checkTag(data.startOffset, data.endOffset - data.startOffset)) {
			ret.setTagHazard();
		}
		if (checkPreviousChar(data.index - 1)) {
			ret.setPreviousCharHazard();
		}
		return ret;
	}

	/**
	 * Checks if the tag is hazardous
	 * 
	 * @param start
	 *            start offset
	 * @param length
	 *            tag length
	 * @return true if there is a problem
	 */
	private boolean checkTag(int start, int length) {
		boolean hazard = false;
		boolean previous = false;
		// Function calls and increment / decrement operators are not safe
		for (int i = start; i < start + length && !hazard; i++) {
			char c = parser.getData(i);
			if (c == '(') {
				// If return statement is within brackets, it is not hazardous
				if (i != start || parser.getData(start + length - 1) != ')') {
					hazard = true;
				}
			} else if (c == '?') {
				hazard = true;
			} else if (c == '-' || c == '+') {
				if (previous) {
					hazard = true;
				} else {
					previous = true;
				}
			} else {
				previous = false;
			}
		}
		return hazard;
	}

	/**
	 * Checks if previous character is hazardous
	 * 
	 * @param index
	 *            the index
	 * @return true if hazard, false if not
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private boolean checkPreviousChar(int index) throws SourceParserException {
		boolean hazard = false;
		SourceIterator previtr = parser.createIterator(index,
				SourceParser.SKIP_ALL | SourceParser.BACKWARD_SEARCH);
		char prevchar = previtr.next();
		if (prevchar != ';' && prevchar != '{' && prevchar != '}') {
			hazard = true;
		}
		return hazard;
	}

	/**
	 * Finds the last return statement from the given context
	 * 
	 * @param context
	 *            the context
	 * @return the index to beginning of the return statement
	 */
	int findLast(SourceContext context) {
		String statement = SourceConstants.RETURN;
		int retval = findLast(context, statement);
		if (returnExtensions != null && returnExtensions.size() > 0) {
			int res;
			for (int i = 0; i < returnExtensions.size(); i++) {
				res = findLast(context, returnExtensions.get(i));
				if (res > retval) {
					retval = res;
				}
			}
		}
		if (retval == -1) {
			retval = context.getOffset() + context.getLength();
		}
		return retval;
	}

	/**
	 * Finds the last return statement from the given context
	 * 
	 * @param context
	 *            the context
	 * @param statement
	 *            the statement to be searched
	 * @return the index to beginning of the return statement
	 */
	private int findLast(SourceContext context, String statement) {
		// TODO: Backwards string search
		int start = context.getOffset();
		int end = start + context.getLength();
		SourceSearch search = parser.startStringSearch(statement, start, end,
				SourceParser.MATCH_WHOLE_WORD | SourceParser.SKIP_ALL);
		int index = 0;
		int retval = -1;
		do {
			index = search.findNext();
			if (index != -1) {
				retval = index;
			}
		} while (index != -1);
		return retval;
	}
}
