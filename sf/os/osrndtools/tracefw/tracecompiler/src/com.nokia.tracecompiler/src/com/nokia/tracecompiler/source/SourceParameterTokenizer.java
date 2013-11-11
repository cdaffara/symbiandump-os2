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
* Parameter tokenizer is used to parse function parameters lists
*
*/
package com.nokia.tracecompiler.source;

import java.util.List;

/**
 * Parameter tokenizer is used to parse function parameters lists
 * 
 */
public class SourceParameterTokenizer {

	/**
	 * The source parser
	 */
	private SourceParser parser;

	/**
	 * The offset where to start the tokenizer
	 */
	private int offset;

	/**
	 * Slip next whitespace
	 */
	private boolean skipNextWhiteSpace;

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the source parser
	 * @param offset
	 *            offset to the start of parameter
	 */
	public SourceParameterTokenizer(SourceParser parser, int offset) {
		this.parser = parser;
		this.offset = offset;
	}

	/**
	 * Parses a list of parameters (a, b, c) and stores the values into the
	 * given list.
	 * 
	 * @param list
	 *            the list of parameters
	 * @param findSeparator
	 *            if true, the processing stops after ';' or '{' character. If
	 *            false, processing stops after ')' at end of parameters
	 * @return index at end of parameters
	 * @throws SourceParserException
	 *             if processing fails
	 */
	public int tokenize(List<String> list, boolean findSeparator)
			throws SourceParserException {
		TokenizerSearchData data = new TokenizerSearchData();
		data.itr = parser.createIterator(offset, SourceParser.SKIP_WHITE_SPACES
				| SourceParser.SKIP_COMMENTS);
		boolean bracketsOpened = false;
		boolean finished = false;
		while (data.itr.hasNext() && !finished) {
			data.value = data.itr.next();
			if (!data.inQuotes && data.value == '\"') {
				data.inQuotes = true;
				data.hasData = true;
			} else if (data.inQuotes) {
				processInQuotesChar(data);
			} else if (data.complete) {
				processEndOfParametersChar(data);
			} else if (data.value == '(') {
				bracketsOpened = true;
				processOpeningBracket(data);
			} else if (data.value == ',' || data.value == ')') {
				processCommaOrClosingBracket(list, data);
			} else if (data.value == ';' && data.openBracketCount != 0 || data.value == '{'
					|| data.value == '}' ) {
				throw new SourceParserException(
						SourceErrorCodes.UNEXPECTED_PARAMETER_SEPARATOR);
			} else {
				// Raises a flag that there is some data. processOpeningBracket
				// no longer interprets the next bracket as opening bracket
				if (data.openBracketCount > 0) {
					data.hasData = true;
				}
			}
			finished = ((data.complete && !findSeparator) || (data.endFound && findSeparator));
			if (bracketsOpened && data.openBracketCount == 0) {
				data.complete = true;
				break;
			}
		}
		if (!data.complete) {
			throw new SourceParserException(
					SourceErrorCodes.UNEXPECTED_END_OF_FILE);
		}
		if (data.openBracketCount != 0) {
			throw new SourceParserException(SourceErrorCodes.BRACKET_MISMATCH);
		}
		
		
		return data.itr.currentIndex() + 1;
	}

	/**
	 * Parses list of parameters with types (int a, int b, int c) and stores the
	 * values into the given list.
	 * 
	 * @param list
	 *            the list of parameters
	 * @return index at end of parameters
	 * @throws SourceParserException
	 *             if processing fails
	 */
	public int tokenizeTyped(List<SourceParameter> list) 
			throws SourceParserException {
		TokenizerSearchData data = new TokenizerSearchData();

		try {
			data.itr = parser
					.createIterator(offset, SourceParser.SKIP_WHITE_SPACES
							| SourceParser.SKIP_COMMENTS);
			data.sourceParameter = new SourceParameter();
			while (data.itr.hasNext() && !data.complete) {
				data.value = data.itr.next();

				// Check if there was array start or end character and then
				// space. It would mean that the parameter continues and more
				// should be parsed.
				if (skipNextWhiteSpace) {
					skipNextWhiteSpace = false;
					if (data.itr.hasSkipped()) {
						data.value = data.itr.next();
					}
				}

				if (data.value == '\"') {
					throw new SourceParserException(
							SourceErrorCodes.UNEXPECTED_QUOTE_CHARACTER);
				} else if (data.value == '(') {
					processOpeningBracket(data);
				} else if (data.value == ',' || data.value == ')') {
					processCommaOrClosingBracket(list, data);
				} else if (data.value == ';' || data.value == '{'
						|| data.value == '}') {
					data.complete = true;
					// Array start or end character.
				} else if (data.value == '<' || data.value == '>') {
					skipNextWhiteSpace = true;
				} else if (data.itr.hasSkipped() && data.openBracketCount > 0) {
					processNameValueSeparator(data);
				}
			}
			if (!data.complete) {
				throw new SourceParserException(
						SourceErrorCodes.UNEXPECTED_END_OF_FILE);
			}
		} catch (SourceParserException e) {
			// Resets all source locations if parser fails
			for (int i = 0; i < list.size(); i++) {
				list.get(i).getSourceLocation().dereference();
			}
			throw e;
		}
		return data.itr.currentIndex() + 1;
	}

	/**
	 * Processes a separator character and updates the current SourceParameter
	 * object in the search data
	 * 
	 * @param data
	 *            the search data
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processNameValueSeparator(TokenizerSearchData data)
			throws SourceParserException {
		// If the parameter is empty, the previous index will point
		// to index preceeding tagStartIndex
		int previous = data.itr.previousIndex();
		if (previous >= data.tagStartIndex) {
			int endIndex = previous + 1;
			if (data.sourceParameter.getType() == null) {
				processNameValueSeparatorNoType(data, endIndex);
			} else if (data.sourceParameter.getName() == null) {
				processNameValueSeparatorNoName(data, endIndex);
			}
			data.tagStartIndex = data.itr.currentIndex();
		}
	}

	/**
	 * Processes a name-value separator when there is no name
	 * 
	 * @param data
	 *            the search data
	 * @param endIndex
	 *            the end index of the parameters
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processNameValueSeparatorNoName(TokenizerSearchData data,
			int endIndex) throws SourceParserException {
		String name = parser.getSource().get(data.tagStartIndex,
				endIndex - data.tagStartIndex);
		boolean startFound = false;
		int start = 0;
		int end = name.length();
		for (int i = 0; i < name.length(); i++) {
			char c = name.charAt(i);
			if (c == '&' || c == '*') {
				if (c == '&') {
					data.sourceParameter.setReference();
				} else {
					data.sourceParameter.addPointer();
				}
				if (!startFound) {
					start++;
				} else {
					end--;
				}
			} else {
				startFound = true;
			}
		}
		name = name.substring(start, end);
		if (name.length() > 0) {
			if (isParameterTypeQualifier(name)) {
				// Qualifiers between type and name are ignored
				// For example TInt const* aValue
			} else {
				data.sourceParameter.setName(name);
			}
		}
	}

	/**
	 * Processes a name-value separator when there is no value
	 * 
	 * @param data
	 *            the search data
	 * @param endIndex
	 *            the end index of the parameters
	 * @throws SourceParserException
	 *             if processing fails
	 */
	private void processNameValueSeparatorNoType(TokenizerSearchData data,
			int endIndex) throws SourceParserException {
		String type = parser.getSource().get(data.tagStartIndex,
				endIndex - data.tagStartIndex);
		if (isParameterTypeQualifier(type)) {
			data.sourceParameter.addQualifier(type);
		} else {
			for (int i = type.length() - 1; i >= 0; i--) {
				if (type.charAt(i) == '&') {
					data.sourceParameter.setReference();
					if (i == 0) {
						type = ""; //$NON-NLS-1$
					}
				} else if (type.charAt(i) == '*') {
					data.sourceParameter.addPointer();
					if (i == 0) {
						type = ""; //$NON-NLS-1$
					}
				} else {
					if (i != type.length() - 1) {
						type = type.substring(0, i + 1);
					}
					i = -1;
				}
			}
			if (type.length() > 0) {
				// Remove spaces
				type = type.replace(" ", ""); //$NON-NLS-1$ //$NON-NLS-2$
				data.sourceParameter.setType(type);
			}
		}
	}

	/**
	 * Checks if parameter type if a qualifier or not
	 * 
	 * @param type
	 *            the type to be checked
	 * @return true if qualifier, false if not
	 */
	private boolean isParameterTypeQualifier(String type) {
		boolean retval = false;
		for (String element : SourceConstants.PARAMETER_QUALIFIERS) {
			if (type.equals(element)) {
				retval = true;
			}
		}
		return retval;
	}

	/**
	 * Processes a parameter separator or closing bracket
	 * 
	 * @param list
	 *            the list of existing parameters
	 * @param data
	 *            the search data
	 * @throws SourceParserException
	 *             if invalid character is encountered
	 */
	@SuppressWarnings("unchecked")
	private void processCommaOrClosingBracket(List list,
			TokenizerSearchData data) throws SourceParserException {
		// This method is called from both tokenize functions. One uses
		// List<String> and other List<SourceParameter>
		// Thus this uses List and @SuppressWarnings
		if (data.value == ')') {
			data.openBracketCount--;
		}
		
		if (data.value == ',') {
			//we may have the case of OstTraceDef1( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDef1, "EOstTraceDef1 - %u" , f(a,b));
			//when processing the comma in f(a,b) that should not count as parameter separator
			if (data.openBracketCount >1 ) {
				return;
			}
		}
		
		if (data.value == ',' || data.openBracketCount == 0) {
			//we have another parameter
			if (data.sourceParameter != null) {
				// If processing typed parameter list, the name and type are
				// stored into a SourceParameter object, which is then added
				// to list
				processNameValueSeparator(data);
				if (data.sourceParameter.getType() != null) {
					SourceLocation location = new SourceLocation(parser,
							data.paramStartIndex, data.itr.currentIndex()
									- data.paramStartIndex);
					data.sourceParameter.setSourceLocation(location);
					list.add(data.sourceParameter);
					data.sourceParameter = new SourceParameter();
				}
			} else {
				// In this case the list contains strings.
				int previous = data.itr.previousIndex();
				String tracepoint = ""; //$NON-NLS-1$
				if (previous >= data.tagStartIndex) {
					int endIndex = data.itr.previousIndex() + 1;
					tracepoint = parser.getSource().get(data.tagStartIndex,
							endIndex - data.tagStartIndex);
					list.add(tracepoint);
				} else {
					list.add(tracepoint);
				}	
			}
			
			// In case like below we have parsed all parameters and data is completed if next character after ')' is ':' 
			// and open bracket count is 0:
			// 		CNpeSendData::CNpeSendData(RMeDriver* aDriver, TUint16 aMaxMsgLength): CActive(EPriorityStandard),
			//				iDriver(aDriver),
			//				iMaxMsgLength(aMaxMsgLength)
			if (data.itr.hasNext()){
				char nextChar = data.itr.peek();
				
				if (data.value == ')' &&  nextChar == ':' && data.openBracketCount == 0) {
					data.complete = true;
				}
			}
			
			if (data.value == ',') {
				data.tagStartIndex = data.itr.nextIndex();
				data.paramStartIndex = data.tagStartIndex;
			}
		}
	}

	/**
	 * Processes an opening bracket
	 * 
	 * @param data
	 *            the search data
	 */
	private void processOpeningBracket(TokenizerSearchData data) {
		data.openBracketCount++;
		if (!data.hasData && data.openBracketCount == 1 ) {
			// The number of initial '(' characters is stored. The
			// parameters are assumed to end when the corresponding ')'
			// character is encountered
			data.tagStartIndex = data.itr.nextIndex();
			data.paramStartIndex = data.tagStartIndex;
			data.initialBracketCount = data.openBracketCount;
		}
	}

	/**
	 * Process a character when in quotes
	 * 
	 * @param data
	 *            the search data
	 */
	private void processInQuotesChar(TokenizerSearchData data) {
		if (data.value == '\"' && data.previousValue != '\\') {
			data.inQuotes = false;
		}
		data.previousValue = data.value;
	}

	/**
	 * Processes a character found after the closing bracket
	 * 
	 * @param data
	 *            the data
	 * @throws SourceParserException
	 *             if invalid characters are found
	 */
	private void processEndOfParametersChar(TokenizerSearchData data)
			throws SourceParserException {
		if (data.value == ';' || data.value == '{') {
			data.endFound = true;
		} else if (data.value == ')') {
			data.openBracketCount--;
		} else if (!Character.isWhitespace(data.value)) {
			throw new SourceParserException(SourceErrorCodes.BRACKET_MISMATCH);
		}
	}

}
