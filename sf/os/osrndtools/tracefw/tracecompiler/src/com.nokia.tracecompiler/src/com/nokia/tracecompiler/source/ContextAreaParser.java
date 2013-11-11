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
* Parser for source contexts
*
*/
package com.nokia.tracecompiler.source;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

/**
 * Parser for source contexts
 * 
 */
class ContextAreaParser {

	/**
	 * Source parser
	 */
	private SourceParser parser;

	/**
	 * List of source file contexts
	 */
	protected ArrayList<SourceContext> contextAreas = new ArrayList<SourceContext>();

	/**
	 * Comparator for array sorting and searching
	 */
	private PositionArrayComparator arrayComparator = new PositionArrayComparator();

	/**
	 * "usingnamespace" text
	 */
	private static final String USINGNAMESPACE = "usingnamespace"; //$NON-NLS-1$

	/**
	 * Start index of "using" substring in "usingnamespace" string
	 */
	private static final int START_INDEX_OF_USING_SUBSTRING = 0; // CodForChk_Dis_Magic

	/**
	 * End index of "using" substring in "usingnamespace" string
	 */
	private static final int END_INDEX_OF_USING_SUBSTRING = 5; // CodForChk_Dis_Magic

	/**
	 * Start index of "namespace" substring in "usingnamespace" string
	 */
	private static final int START_INDEX_OF_NAMESPACE_SUBSTRING = 5; // CodForChk_Dis_Magic

	/**
	 * End index of "namespace" substring in "usingnamespace" string
	 */
	private static final int END_INDEX_OF_NAMESPACE_SUBSTRING = 14; // CodForChk_Dis_Magic

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the source parser
	 */
	ContextAreaParser(SourceParser parser) {
		this.parser = parser;
	}

	/**
	 * Resets the context areas
	 */
	void reset() {
		contextAreas.clear();
	}

	/**
	 * Returns the context at given offset
	 * 
	 * @param offset
	 *            the offset to the source data
	 * @return the context at the offset or null if no context exists
	 * @throws SourceParserException
	 *             if parser fails
	 */
	SourceContext parseAndGet(int offset) throws SourceParserException {
		if (contextAreas.isEmpty()) {
			parseAll();
		}
		int index = find(offset);
		SourceContext context = null;
		if (index >= 0) {
			context = contextAreas.get(index);
		}
		return context;
	}

	/**
	 * Gets the context areas. If the areas have not been parsed, this parses
	 * them
	 * 
	 * @return the areas
	 * @throws SourceParserException
	 *             if parser fails
	 */
	Iterator<SourceContext> parseAndGetAll() throws SourceParserException {
		if (contextAreas.isEmpty()) {
			parseAll();
		}
		return contextAreas.iterator();
	}

	/**
	 * Gets the context area list. This does not parse the areas
	 * 
	 * @return the list of context areas
	 */
	List<SourceContext> getContextList() {
		return contextAreas;
	}

	/**
	 * Finds the array index of the context area which contains the offset. If
	 * none of the areas contain the offset, returns negative integer indicating
	 * the index of the context area following the offset
	 * 
	 * @param offset
	 *            the offset to the data
	 * @return the context area index
	 */
	int find(int offset) {
		return Collections.binarySearch(contextAreas, new SourceLocationBase(
				parser, offset), arrayComparator);
	}

	/**
	 * Builds the context array
	 * 
	 * @throws SourceParserException
	 *             if parser fails
	 */
	void parseAll() throws SourceParserException { // CodForChk_Dis_ComplexFunc
		contextAreas.clear();
		char value;
		
		int inBrackets = 0;
		int inContext = 0;
		int inNamespace = 0;

		int usingIndex = START_INDEX_OF_USING_SUBSTRING;
		int usingKeywordEnd = 0;
		int namespaceIndex = START_INDEX_OF_NAMESPACE_SUBSTRING;
		int nameSpaceKeywordEnd = 0;
		int previousIndexBeforeNamespace = 0;
		boolean checkNextCharacter = false;

		SourceContext context = null;
		SourceIterator itr = parser.createIterator(0, SourceParser.SKIP_ALL);

		while (itr.hasNext()) {
			value = itr.next();

			// Next character check is need only if we have found "namespace"
			// text
			if (checkNextCharacter) {

				// Next character after "namespace" text should be space.
				// Because we have skipped spaces, current index should be
				// bigger than nameSpaceKeywordEnd + 1. If it is not space then
				// we are not inside namespace
				if (itr.currentIndex() - nameSpaceKeywordEnd < 2) { // CodForChk_Dis_Magic
					inNamespace--;
				}
				checkNextCharacter = false;
			}

			// Check is character part of "using" text
			if (value == USINGNAMESPACE.charAt(usingIndex)) {
				usingIndex++;
			} else {

				// Character not part of "using" text -> reset usingIndex
				usingIndex = START_INDEX_OF_USING_SUBSTRING;
			}

			// Check that did we found "using" text
			if (usingIndex == END_INDEX_OF_USING_SUBSTRING) {
				usingKeywordEnd = itr.currentIndex();
				usingIndex = START_INDEX_OF_USING_SUBSTRING;
			}

			// Check is character part of "namespace" text
			if (value == USINGNAMESPACE.charAt(namespaceIndex)) {
				if (previousIndexBeforeNamespace == 0) {
					previousIndexBeforeNamespace = itr.previousIndex();
				}
				namespaceIndex++;
			} else {

				// Character not part of "namespace" text -> reset
				// previousIndexBeforeNamespace and namespaceIndex
				previousIndexBeforeNamespace = 0;
				namespaceIndex = START_INDEX_OF_NAMESPACE_SUBSTRING;
			}

			// Check that did we found "namespace" text
			if (namespaceIndex == END_INDEX_OF_NAMESPACE_SUBSTRING) {
				nameSpaceKeywordEnd = itr.currentIndex();

				// If there was "using" text just before "namespace" text, then
				// namespace is defined like: "using namespace foo;" and we are
				// not going inside namespace brackets
				if (usingKeywordEnd != previousIndexBeforeNamespace) {
					inNamespace++;
					checkNextCharacter = true;
				}
				namespaceIndex = START_INDEX_OF_NAMESPACE_SUBSTRING;
			}

			if (value == '{') {
				inBrackets++;

				// Check that are we inside namespace or context
				if (inBrackets > inNamespace) {
					inContext++;
					if (inContext == 1) {
						int start = itr.currentIndex() + 1;
						context = new SourceContext(parser, start);

						// Includes the '{' character into the context
						if (!createContext(context, start - 2)) { // CodForChk_Dis_Magic
							context = null;
						}
					}
				}
			} else if (value == '}') {
				// Check that are we exiting from context or namespace
				if (inBrackets == inNamespace) {
					inNamespace--;
				} else {
					inContext--;
					if (inContext == 0 && context != null) {
						context.setLength(itr.currentIndex() + 1
								- context.getOffset());
						contextAreas.add(context);
					}
				}

				inBrackets--;
			}
		}
	}

	/**
	 * Sets the data to the source context
	 * 
	 * @param context
	 *            the source context to be updated
	 * @param offset
	 *            the index preceeding the '{' character
	 * @return true if valid, false otherwise
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private boolean createContext(SourceContext context, int offset)
			throws SourceParserException {
		ContextSearchData data = new ContextSearchData();
		data.itr = parser.createIterator(offset, SourceParser.BACKWARD_SEARCH
				| SourceParser.SKIP_ALL);
		data.context = context;
		while (data.itr.hasNext() && !data.finished) {
			char c = data.itr.next();
			// Function start or stop character or statement separator breaks
			// the search in normal case. In case of nested class separator
			// character breaks the search.
			if (c == ';' || c == '}' || c == '{'
					|| (c == ':' && data.itr.peek() == ':')
					&& data.classStartIndex != -1) {
				processContextTerminator(context, data, false);
			} else if (!data.parametersFound) {
				processParametersNotFoundCharacter(data, c);
			} else if (c == ')' || c == '(' || c == ','
					|| (c == ':' && data.itr.peek() != ':')) {
				// Constructor member initializer list may contain brackets, ','
				// and ':'. When one of the characters from member initializer
				// list is encountered, this assumes that the previous
				// one was not the actual function parameter list yet. All
				// variables are reset in that case
				data.parametersFound = false;
				data.functionEndIndex = -1;
				data.functionStartIndex = -1;
				data.classEndIndex = -1;
				processParametersNotFoundCharacter(data, c);
			} else if (data.functionEndIndex == -1) {
				processFunctionNameNotFoundCharacter(data, c);
			} else if (data.functionStartIndex == -1) {
				processFunctionNameCharacter(context, data, c);
			} else if (data.classEndIndex == -1) {
				processClassNameNotFoundCharacter(data);
			} else if (data.classStartIndex == -1) {
				processClassNameCharacter(context, data, c);
			} else {
				processReturnTypeCharacter(context, data);
			}
		}
		if (!data.finished) {
			processContextTerminator(context, data, true);
		}
		return data.valid;
	}

	/**
	 * Processes a character after class and function names have been found
	 * 
	 * @param context
	 *            the context
	 * @param data
	 *            the search data
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processReturnTypeCharacter(SourceContext context,
			ContextSearchData data) throws SourceParserException {
		if (data.itr.hasSkipped()) {
			// Collects all return type candidates to the context
			addReturnType(context, data.itr.previousIndex(),
					data.returnEndIndex);
			data.returnEndIndex = data.itr.currentIndex();
		}
	}

	/**
	 * Processes a character after function name has been found, but class name
	 * has not yet been found
	 * 
	 * @param data
	 *            the search flags
	 */
	private void processClassNameNotFoundCharacter(ContextSearchData data) {
		// After start of function and the separator has been found, the
		// next character marks the end of class name
		data.classEndIndex = data.itr.currentIndex() + 1;
	}

	/**
	 * Parses a character which belongs to the class name
	 * 
	 * @param context
	 *            the source context to be parsed
	 * @param data
	 *            the context search parameters
	 * @param c
	 *            the character
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processClassNameCharacter(SourceContext context,
			ContextSearchData data, char c) throws SourceParserException {
		if (data.itr.hasSkipped() || (c == ':' && data.itr.peek() == ':')) {
			// Start of class name is found when iterator skips over
			// white space or comment characters or in case of nested class
			// separator character has been found
			context.setFunctionName(parser.getSource().get(
					data.functionStartIndex,
					data.functionEndIndex - data.functionStartIndex));
			data.classStartIndex = data.itr.previousIndex();
			data.returnEndIndex = data.itr.currentIndex();
			context.setClassName(parser.getSource().get(data.classStartIndex,
					data.classEndIndex - data.classStartIndex));

			// In case of nested class skips over the second ':'
			if (c == ':' && data.itr.peek() == ':') {
				data.itr.next();
			}
		}
	}

	/**
	 * Processes a character while within function name
	 * 
	 * @param context
	 *            the source context under processing
	 * @param data
	 *            the context search flags
	 * @param c
	 *            the character
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processFunctionNameCharacter(SourceContext context,
			ContextSearchData data, char c) throws SourceParserException {
		// After end of function has been found the separator character
		// marks the start of function
		if (c == ':') {
			if (data.itr.hasNext() && data.itr.peek() == ':') {
				data.functionStartIndex = data.itr.previousIndex();
				context.setFunctionName(parser.getSource().get(
						data.functionStartIndex,
						data.functionEndIndex - data.functionStartIndex));
				// Skips over the second ':'
				data.itr.next();
			} else {
				// Only one ':' character -> Invalid
				data.finished = true;
			}
		} else if (data.itr.hasSkipped()) {
			// If the iterator skipped over some characters and the next
			// character is not ':' the function is a non-member
			data.functionStartIndex = data.itr.previousIndex();
			context.setFunctionName(parser.getSource().get(
					data.functionStartIndex,
					data.functionEndIndex - data.functionStartIndex));
			// Class name indices are set so parser does not search for them
			data.classStartIndex = data.itr.previousIndex();
			data.classEndIndex = data.itr.previousIndex();
			data.returnEndIndex = data.itr.currentIndex();
		}
	}

	/**
	 * Processes a character when function name has not yet been found
	 * 
	 * @param data
	 *            the search flags
	 * @param c
	 *            the character to be processed
	 */
	private void processFunctionNameNotFoundCharacter(ContextSearchData data,
			char c) {
		// The next character after parameters is the end of function
		if (c == ':') {
			data.finished = true;
		}
		data.functionEndIndex = data.itr.currentIndex() + 1;
	}

	/**
	 * Checks if the character is '(' or ')' and updates the parametersFound
	 * flag accordingly
	 * 
	 * @param data
	 *            the search data
	 * @param c
	 *            the current character
	 */
	private void processParametersNotFoundCharacter(ContextSearchData data,
			char c) {
		if (c == ')') {
			data.inParameters++;
		} else if (c == '(') {
			data.inParameters--;
			if (data.inParameters == 0) {
				data.context.setParametersStartIndex(data.itr.currentIndex());
				data.parametersFound = true;
			}
		}
	}

	/**
	 * Processes a context terminating character
	 * 
	 * @param context
	 *            the context under processing
	 * @param data
	 *            the search data
	 * @param startOfFile
	 *            context was terminated due to start of file
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processContextTerminator(SourceContext context,
			ContextSearchData data, boolean startOfFile)
			throws SourceParserException {
		int offset = startOfFile ? data.itr.currentIndex() : data.itr
				.previousIndex();
		if (data.classStartIndex != -1) {
			addReturnType(context, offset, data.returnEndIndex);
			data.valid = true;
		} else if (data.classEndIndex != -1) {
			context.setClassName(parser.getSource().get(offset,
					data.classEndIndex - offset));
			data.valid = true;
		} else if (data.functionEndIndex != -1) {
			context.setFunctionName(parser.getSource().get(offset,
					data.functionEndIndex - offset));
			data.valid = true;
		}
		// Finished flag is set. If function name was not found, the valid flag
		// remains false
		data.finished = true;
	}

	/**
	 * Adds a return type to the context
	 * 
	 * @param context
	 *            the context to be searched
	 * @param start
	 *            the start index
	 * @param end
	 *            the end index
	 * @throws SourceParserException
	 *             if return type cannot be added
	 */
	private void addReturnType(SourceContext context, int start, int end)
			throws SourceParserException {
		context.addReturnType(parser.getSource().get(start, end - start + 1));
	}
}
