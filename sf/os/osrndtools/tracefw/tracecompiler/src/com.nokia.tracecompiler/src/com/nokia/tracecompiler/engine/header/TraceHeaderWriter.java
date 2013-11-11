/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Writes the TraceHeader to a file
 *
 */
package com.nokia.tracecompiler.engine.header;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.nokia.tracecompiler.TraceCompilerLogger;
import com.nokia.tracecompiler.engine.LocationListBase;
import com.nokia.tracecompiler.engine.LocationProperties;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceLocation;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.source.SourceFormatter;
import com.nokia.tracecompiler.engine.source.TraceFormattingRule;
import com.nokia.tracecompiler.engine.source.TraceParameterFormattingRule;
import com.nokia.tracecompiler.file.FileCompareOutputStream;
import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.plugin.TraceFormatConstants;
import com.nokia.tracecompiler.plugin.TraceHeaderContribution;
import com.nokia.tracecompiler.plugin.TraceAPIFormatter.TraceFormatType;
import com.nokia.tracecompiler.plugin.TraceHeaderContribution.TraceHeaderContributionType;
import com.nokia.tracecompiler.rules.FillerParameterRule;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceExcludedArea;
import com.nokia.tracecompiler.source.SourceParser;
import com.nokia.tracecompiler.source.SourceUtils;
import com.nokia.tracecompiler.source.SymbianConstants;
import com.nokia.tracecompiler.utils.TraceCompilerVersion;

/**
 * Writes the TraceHeader to a file
 * 
 */
final class TraceHeaderWriter {

	/**
	 * return type text of a generated OstTraceGenx function.
	 */
	private static final String INLINE_TBOOL = "inline TBool";

	/**
	 * open bracket in a type cast
	 */
	private static final String BEGINCAST = "OBR"; //$NON-NLS-1$
	
	/**
	 * closing bracket in a type cast
	 */
	private static final String ENDCAST = "CBR"; //$NON-NLS-1$

	/**
	 * REF replaces & in a function guard
	 */
	private static final String REF = "REF"; //$NON-NLS-1$

	/**
	 * & character
	 */
	private static final String AMPERSANT = "&"; //$NON-NLS-1$

	/**
	 * closing bracket
	 */
	private static final String CLOSING_BRACKET = ")"; //$NON-NLS-1$

	/**
	 * open bracket
	 */
	private static final String OPEN_BRACKET = "("; //$NON-NLS-1$

	/**
	 * TUint32 definition
	 */
	private static final String TUINT32_DEF = "TUint32 "; //$NON-NLS-1$

	/**
	 * TInt32 definition
	 */
	private static final String TINT32_DEF = "TInt32 "; //$NON-NLS-1$

	/**
	 * TUint definition
	 */
	private static final String TUINT_DEF = "TUint "; //$NON-NLS-1$

	/**
	 * TInt definition
	 */
	private static final String TINT_DEF = "TInt "; //$NON-NLS-1$

	/**
	 * Length variable defined flag
	 */
	private boolean lenghtVariableDefined = false;
	
	
	/**
	 * List of dynamic elements that can be used from the templates
	 * 
	 */
	enum HeaderTemplateElementType {

		/**
		 * Licence text
		 */
		LICENCE_TEXT,
		
		/**
		 * TraceCompiler version number
		 */
		TRACE_COMPILER_VERSION,

		/**
		 * Header guard based on file name
		 */
		HEADER_GUARD,

		/**
		 * Opening brace
		 */
		OPEN_BRACE,

		/**
		 * Closing brace
		 */
		CLOSE_BRACE,

		/**
		 * New line and indent based on open brace count
		 */
		NEW_LINE,

		/**
		 * Writes currentTraceFormatted
		 */
		FORMATTED_TRACE,

		/**
		 * Adds all closing braces except the one that closes the function
		 */
		CLOSE_EXTRA_BRACES,

		/**
		 * Type of current parameter
		 */
		PARAMETER_TYPE,

		/**
		 * Name of current parameter
		 */
		PARAMETER_NAME,

		/**
		 * Name of current trace
		 */
		TRACE_NAME,

		/**
		 * ID of current trace
		 */
		TRACE_ID_HEX,

		/**
		 * Calls a function to add the trace buffer initialization code
		 */
		BUILD_TRACE_BUFFER_CHECK,

		/**
		 * Calls a function to add the function body
		 */
		TRACE_FUNCTION_BODY,

		/**
		 * Calls a function to add function parameters
		 */
		TRACE_FUNCTION_PARAMETERS,

		/**
		 * Writes the fixedBufferSize member variable
		 */
		FIXED_BUFFER_SIZE,

		/**
		 * Writes the dynamicBufferSize member variable
		 */
		DYNAMIC_BUFFER_SIZE,

		/**
		 * Index of the parameter being processed
		 */
		PARAMETER_INDEX
	}

	/**
	 * Group ID shift bits
	 */
	private static final int GROUP_SHIFT = 16; // CodForChk_Dis_Magic

	/**
	 * Number of bytes in parameter
	 */
	private static final int BYTES_IN_PARAMETER = 4; // CodForChk_Dis_Magic

	/**
	 * Indent
	 */
	private static final String INDENT = "    "; //$NON-NLS-1$

	/**
	 * The header file to be updated
	 */
	private TraceHeader header;

	/**
	 * Output stream for the header
	 */
	private OutputStream headerOutput;

	/**
	 * Temporary flag that specifies if a trace requires a trace buffer or it
	 * can be represented by the default trace macros
	 */
	private boolean buildTraceBuffer;

	/**
	 * Temporary variable for fixed size
	 */
	private int fixedBufferSize;

	/**
	 * Dynamic size flag
	 */
	private boolean dynamicBufferSizeFlag;

	/**
	 * Flag which is set it trace needs #endif for __KERNEL_MODE__
	 */
	private boolean needsKernelEndif;

	/**
	 * Used via HeaderTemplateElementType.FORMATTED_TRACE
	 */
	private String currentTraceFormatted;

	/**
	 * Type of current parameter
	 */
	private String currentParameterType;

	/**
	 * Name of current parameter
	 */
	private String currentParameterName;

	/**
	 * Index of current parameter
	 */
	private int currentParameterIndex;

	/**
	 * Number of opened brackets
	 */
	private int openBraceCount;

	/**
	 * Trace being processed
	 */
	private Trace currentTrace;

	/**
	 * Parameter being processed
	 */
	private TraceParameter currentParameter;

	/**
	 * List of trace functions already in the header
	 */
	private ArrayList<String> traceDeclarations = new ArrayList<String>();

	/**
	 * Number of sequential new lines
	 */
	private int newLineCount;

	/**
	 * Number of allowed sequential new lines
	 */
	private int maxNewLines;

	/**
	 * Indicates that writing a function to the header file is going
	 */
	private boolean firstOpenBraceFound;
	
	/**
	 * boolean indication that we are buffering a function text
	 */
	private boolean bufferingFunction;

	/**
	 * While writing a function to the header file, it's gathered to this
	 * member. The member is then checked if the function parameters contain
	 * TInt or TUint values. If so, the function is replicated so that TInt is
	 * replaced by TInt32 and TUint with TUint32.
	 */
	private StringBuilder functionText = new StringBuilder();

	/**
	 * Number of brackets seen when writing a function. When it gets to 0, the
	 * function in previousFunction variable is complete and can be written.
	 */
	private int numberOfBrackets;
		
	/**
	 * string to hold the function guard
	 */
	private String ostTraceGenxFunGuard;

	/**
	 * Creates a new header writer
	 * 
	 * @param header
	 *            the header to be written
	 */
	TraceHeaderWriter(TraceHeader header) {
		this.header = header;
	}

	/**
	 * Writes the header
	 * 
	 * @return true if header was written, false if it matched the existing
	 *         header
	 * @throws TraceCompilerException
	 *             if writing fails
	 */
	boolean write() throws TraceCompilerException {
		boolean headerWritten = false;
		try {
			openBraceCount = 0;
			createHeader();
			writeTemplate(HeaderTemplate.HEADER_TEMPLATE);
			headerWritten = closeHeader();
		} catch (IOException e) {
			e.printStackTrace();
			throw new TraceCompilerException(
					TraceCompilerErrorCode.CANNOT_WRITE_PROJECT_FILE, e);
		} finally {
			traceDeclarations.clear();
			if (headerOutput != null) {
				try {
					headerOutput.close();
				} catch (IOException e) {
				}
			}
			headerOutput = null;
		}
		return headerWritten;
	}

	/**
	 * Creates the header file
	 * 
	 * @throws IOException
	 *             if creation fails
	 */
	private void createHeader() throws IOException {
		File file = new File(header.getAbsolutePath());
		if (file.exists()) {
			// If header exists, data is written to a byte array and compared
			// with existing file. If they are the same, the file is not
			// updated.
			headerOutput = new FileCompareOutputStream(file);
		} else {
			// If header does not exist, the data is written directly to file
			headerOutput = FileUtils.createOutputStream(file);
		}
	}

	/**
	 * Closes the header file. If data was written to a byte buffer this
	 * compares the contents of the buffer with the existing file and re-writes
	 * the file if contents do not match.
	 * 
	 * @return true if header was written, false if it matched the existing
	 *         header
	 * @throws IOException
	 *             if closing fails
	 */
	private boolean closeHeader() throws IOException {
		boolean headerWritten = true;
		if (headerOutput instanceof FileCompareOutputStream) {
			headerWritten = ((FileCompareOutputStream) headerOutput)
					.writeFile();
		}
		headerOutput.close();
		headerOutput = null;
		return headerWritten;
	}

	/**
	 * Writes a template to the stream
	 * 
	 * @param template
	 *            the template
	 * @throws IOException
	 *             if writing fails
	 */
	void writeTemplate(Object[] template) throws IOException {
		for (Object o : template) {
			if (o instanceof String) {
				write((String) o);
			} else if (o instanceof TraceHeaderContributionType) {
				writeHeaderContributions((TraceHeaderContributionType) o);
			} else if (o instanceof HeaderTemplateElementType) {
				writeTemplateElement((HeaderTemplateElementType) o);
			} else if (o instanceof Object[]) {
				// Template within template
				writeTemplate((Object[]) o);
			} else if (o instanceof TemplateChoice) {
				TemplateChoice choice = (TemplateChoice) o;
				// Gets the array index from template
				Class<? extends TemplateCheckBase> c = choice.getChoiceClass();
				try {
					// Creates a switch-case object based on array index
					TemplateCheckBase check = c.newInstance();
					check.setWriter(this);
					// Gets the case from the switch-case object and uses it to
					// get the correct template
					if (check.check()) {
						writeTemplate(choice.getTrueTemplate());
					} else {
						writeTemplate(choice.getFalseTemplate());
					}
				} catch (InstantiationException e) {
				} catch (IllegalAccessException e) {
				}
			} else if (o instanceof TemplateIterator) {
				Class<? extends TemplateIteratorEntry> c = ((TemplateIterator) o)
						.getIteratorClass();
				try {
					// Creates an iterator object based on array index
					TemplateIteratorEntry itr = c.newInstance();
					itr.setWriter(this);
					itr.iterate(((TemplateIterator) o).getTemplate());
				} catch (InstantiationException e) {
				} catch (IllegalAccessException e) {
				}
			} else if (o instanceof SetNewLineCount) {
				maxNewLines = ((SetNewLineCount) o).getLineCount();
			} else if (o instanceof TraceFormatType) {
				// Stores the formatted trace, but does not write anything
				// HeaderTemplateElementType.FORMATTED_TRACE writes the trace
				currentTraceFormatted = SourceFormatter.formatTrace(
						currentTrace, (TraceFormatType) o);
			}
		}
	}

	/**
	 * Writes an element from the HeaderTemplateElementType enumeration
	 * 
	 * @param type
	 *            the element type
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeTemplateElement(HeaderTemplateElementType type)
			throws IOException { // CodForChk_Dis_ComplexFunc
		switch (type) {
		case NEW_LINE:
			writeNewLine();
			break;
		case OPEN_BRACE:
			writeOpenBrace();
			break;
		case CLOSE_BRACE:
			writeCloseBrace();
			break;
		case PARAMETER_INDEX:
			write(String.valueOf(currentParameterIndex));
			break;
		case PARAMETER_TYPE:
			write(currentParameterType);
			break;
		case PARAMETER_NAME:
			write(currentParameterName);
			break;
		case FORMATTED_TRACE:
			writeFormattedTrace(currentTraceFormatted);
			break;
		case TRACE_NAME:
			write(currentTrace.getName());
			break;
		case TRACE_ID_HEX:
			writeTraceID();
			break;
		case FIXED_BUFFER_SIZE:
			write(String.valueOf(fixedBufferSize));
			break;
		case TRACE_FUNCTION_BODY:
			writeFunctionBody(currentTrace);
			break;
		case TRACE_FUNCTION_PARAMETERS:
			writeParameter(currentParameter);
			break;
		case BUILD_TRACE_BUFFER_CHECK:
			buildTraceBuffer = traceNeedsBuffer(currentTrace);
			break;
		case HEADER_GUARD:
			write(SourceUtils.createHeaderGuard(header.getFileName()));
			break;
		case TRACE_COMPILER_VERSION:
			write(TraceCompilerVersion.getVersion());
			break;
		case CLOSE_EXTRA_BRACES:
			while (openBraceCount > 1) {
				writeCloseBrace();
			}
			break;
		case LICENCE_TEXT:
		    writeLicence();
		    break;
		}
	}

	/**
	 * Writes the trace ID to header
	 * 
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeTraceID() throws IOException {
		int gid = currentTrace.getGroup().getID() << GROUP_SHIFT;
		write(Integer.toHexString(gid | currentTrace.getID()));
		ComplexHeaderRule rule = currentTrace
				.getExtension(ComplexHeaderRule.class);
		if (rule != null) {
			String ext = rule.getTraceIDDefineExtension();
			if (ext != null) {
				write(ext);
			}
		}
	}

	/**
	 * Writes the header contributions from plug-in's
	 * 
	 * @param type
	 *            the contribution type
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeHeaderContributions(TraceHeaderContributionType type)
			throws IOException {
		Iterator<String> contributions = getContributions(type);
		boolean written = false;
		while (contributions.hasNext()) {
			writeContribution(contributions.next(), type);
			written = true;
		}
		if (written) {
			writeNewLine();
		}
	}

	/**
	 * Gets a list of contributions from plug-in's
	 * 
	 * @param type
	 *            the contribution type
	 * @return the contributions
	 */
	private Iterator<String> getContributions(TraceHeaderContributionType type) {
		Iterator<TraceHeaderContribution> contributions = header.getOwner()
				.getExtensions(TraceHeaderContribution.class);
		ArrayList<String> list = new ArrayList<String>();
		while (contributions.hasNext()) {
			String[] s = contributions.next().getContribution(type);
			if (s != null) {
				for (String element : s) {
					list.add(element);
				}
			}
		}
		return list.iterator();
	}

	/**
	 * Writes a contribution to the stream
	 * 
	 * @param contribution
	 *            the contribution
	 * @param type
	 *            the contribution type
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeContribution(String contribution,
			TraceHeaderContributionType type) throws IOException {
		switch (type) {
		case GLOBAL_DEFINES:
			writeDefine(contribution);
			break;
		case GLOBAL_INCLUDES:
			writeSystemInclude(contribution);
			break;
		case MAIN_HEADER_CONTENT:
			// Handled by HeaderEngine
			break;
		}
	}
	
	/**
	 * write start of function guard
	 * @throws IOException
	 */
	private void writeStartFunctionGuard() throws IOException {
		Pattern p = Pattern.compile("inline\\s+TBool\\s+([^\\(]+)\\s*\\((.*)\\)\\s*\\{"); //$NON-NLS-1$
		String guard = null;
		String functionName = null;
		
		// Get the function definition line
		int startIndex = functionText.indexOf(SourceConstants.OPENING_BRACE) + 1;
		String funcDef = functionText.substring(0, startIndex);
		
		Matcher m = p.matcher(funcDef);
		if (m.matches()) {
			//get function name
			functionName = m.group(1);
			if (functionName == null || functionName.length() == 0) {
				throw new IOException(Messages.getString("TraceHeader.internalError1")); //$NON-NLS-1$
			}
			//get raw parameters
			String parameters = m.group(2);
			if (parameters == null || parameters.length() == 0) {//there must be at least TraceID
				throw new IOException(Messages.getString("TraceHeader.internalError2")); //$NON-NLS-1$
			}
			
			functionName = functionName.trim();
			parameters = parameters.trim();
			//remove parameters names
			guard = parameters.replaceAll("(\\S+,)|(\\S+\\s*$)", ""); //$NON-NLS-1$ //$NON-NLS-2$
			//replace repeated spaces by one space
			guard = guard.replaceAll("\\s+", SourceConstants.SPACE).trim(); //$NON-NLS-1$
			//replace space by underscore
			guard = guard.replace(SourceConstants.SPACE, SourceConstants.UNDERSCORE);
			//replace ampersant by REF
			guard = guard.replace(AMPERSANT, REF);
			//replace ( by OBR
			guard = guard.replace(OPEN_BRACKET, BEGINCAST);
			//replace ) by CBR
			guard = guard.replace(CLOSING_BRACKET, ENDCAST);
		} else {
			throw new IOException(Messages.getString("TraceHeader.internalError3")); //$NON-NLS-1$
		}
		
		guard = SourceConstants.DOUBLE_UNDERSCORE 
				+ functionName.toUpperCase()
				+ SourceConstants.UNDERSCORE
				+ guard.toUpperCase()
				+ SourceConstants.DOUBLE_UNDERSCORE;
		
		ostTraceGenxFunGuard = guard;
		write( SourceConstants.IFNDEF + SourceConstants.SPACE_CHAR + ostTraceGenxFunGuard);
		write(SourceConstants.LINE_FEED);
		write( SourceConstants.DEFINE +SourceConstants.SPACE_CHAR + ostTraceGenxFunGuard);
		write(SourceConstants.LINE_FEED);
		write(SourceConstants.LINE_FEED);
	}

	/**
	 * write end of function guard
	 * @throws IOException
	 */
	private void writeEndFunctionGuard() throws IOException {
		if (ostTraceGenxFunGuard != null) {
			write(SourceConstants.LINE_FEED);
			write(SourceConstants.LINE_FEED);
			write(SourceConstants.ENDIF 
					+ SourceConstants.SPACE_CHAR
					+ SourceConstants.FORWARD_SLASH_CHAR
					+ SourceConstants.FORWARD_SLASH_CHAR
					+ SourceConstants.SPACE_CHAR
					+ ostTraceGenxFunGuard);
			write(SourceConstants.LINE_FEED);
		}
	}
	/**
	 * Writes the function body to the stream
	 * 
	 * @param trace
	 *            the trace to be written
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeFunctionBody(Trace trace) throws IOException {
		writeTraceBufferAllocation(trace);
		writeTemplate(HeaderTemplate.PARAMETERS_TEMPLATE);
		// If buffer is not used (single descriptor parameter), trace line is
		// already written in template
		if (isTraceBufferBuilt()) {
			writeTraceLine(trace);
		}
	}

	/**
	 * Writes trace buffer allocation code to the function
	 * 
	 * @param trace
	 *            the trace to be written
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeTraceBufferAllocation(Trace trace) throws IOException {
		// If buffer is not used (single descriptor parameter), this function
		// does nothing
		if (isTraceBufferBuilt()) {
			Iterator<TraceParameter> parameters = trace.getParameters();
			lenghtVariableDefined = false;
			int fixedSizeParametersTotalSize = 0;
			while (parameters.hasNext()) {
				TraceParameter parameter = parameters.next();
				TraceParameterFormattingRule sourceRule = parameter
						.getExtension(TraceParameterFormattingRule.class);
				if (sourceRule == null || sourceRule.isShownInSource()
						|| sourceRule instanceof FillerParameterRule) {
					// Fillers do not increment parameter index
					if (!(sourceRule instanceof FillerParameterRule)) {
						currentParameterIndex++;
					}
					int paramSize = SourceUtils
							.mapParameterTypeToSize(parameter);
					// calculateParameterSize returns 0 for dynamic parameters,
					// but 4 extra bytes need to be reserved for the length
					if (paramSize == 0) {
						paramSize = BYTES_IN_PARAMETER;
					}
					fixedBufferSize += paramSize;
					fixedSizeParametersTotalSize += paramSize;
					if (SourceUtils.isParameterSizeDynamic(parameter)) {

						// Define length variable only once
						if (lenghtVariableDefined == false) {
							writeTemplate(HeaderTemplate.LENGTH_VARIABLE_DEFINITION_TEMPLATE);
							lenghtVariableDefined = true;
						}

						// Increase length variable if needed
						// This is needed in case that there has been fixed size
						// parameter
						// before dynamic parameter
						if (fixedSizeParametersTotalSize - paramSize > 0) {
							fixedSizeParametersTotalSize -= paramSize;
							writeTemplate(HeaderTemplate.LENGTH_VARIABLE_INCREASE_TEMPLATE_BEGIN);
							write(String.valueOf(fixedSizeParametersTotalSize));
							writeTemplate(HeaderTemplate.LENGTH_VARIABLE_INCREASE_TEMPLATE_END);
						}

						fixedSizeParametersTotalSize = 0;

						writeTemplate(HeaderTemplate.DYNAMIC_PARAMETER_LENGTH_TEMPLATE);
						dynamicBufferSizeFlag = true;
					}
				}
			}
			writeTemplate(HeaderTemplate.BUFFER_ALLOCATION_TEMPLATE);
			currentParameterIndex = 0;
		}
	}

	/**
	 * Writes the given parameter to the header
	 * 
	 * @param parameter
	 *            the parameter
	 * @throws IOException
	 *             if writing fails
	 */
	void writeParameter(TraceParameter parameter) throws IOException {
		TraceParameterFormattingRule sourceRule = parameter
				.getExtension(TraceParameterFormattingRule.class);
		if (sourceRule == null || sourceRule.isShownInSource()
				|| sourceRule instanceof FillerParameterRule) {
			String paramType = SourceUtils
					.mapParameterTypeToSymbianType(parameter);
			if (SourceUtils.isParameterSizeDynamic(parameter)) {
				currentParameterIndex++;
				currentParameterName = SymbianConstants.PARAMETER_DECLARATION_PREFIX
						+ currentParameterIndex;
				writeTemplate(HeaderTemplate.DYNAMIC_PARAMETER_TEMPLATE);
			} else {
				currentParameterType = paramType;
				if (sourceRule instanceof FillerParameterRule) {
					currentParameterName = "0"; //$NON-NLS-1$
					writeTemplate(HeaderTemplate.FIXED_PARAMETER_TEMPLATE);
				} else {
					currentParameterIndex++;
					currentParameterName = SymbianConstants.PARAMETER_DECLARATION_PREFIX
							+ currentParameterIndex;
					if (lenghtVariableDefined == true) {
						writeTemplate(HeaderTemplate.FIXED_PARAMETER_TEMPLATE_WITH_LENGTH_CHECK);
					} else {
						writeTemplate(HeaderTemplate.FIXED_PARAMETER_TEMPLATE);
					}
				}
			}
		}
	}

	/**
	 * Writes the trace line to the function
	 * 
	 * @param trace
	 *            the trace to be written
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeTraceLine(Trace trace) throws IOException {
		StringBuffer sb;
		StringBuffer bufferData = new StringBuffer();
		StringBuffer lengthData = new StringBuffer();
		if (isBufferSizeDynamic()) {
			sb = writeBufferedTraceLine(trace, bufferData, lengthData);
		} else {
			// If buffer size is 4, the buffer can be traced using the
			// the 32-bit parameter trace macro instead of data macro
			if (fixedBufferSize / BYTES_IN_PARAMETER == 1) {
				sb = writePackedTraceLine(trace, bufferData);
			} else {
				sb = writeBufferedTraceLine(trace, bufferData, lengthData);
			}
		}
		int index = sb.indexOf(TraceFormatConstants.DATA_BUFFER_FORMAT);
		if (index >= 0) {
			sb.replace(index, index
					+ TraceFormatConstants.DATA_BUFFER_FORMAT.length(),
					bufferData.toString());
		}
		index = sb.indexOf(TraceFormatConstants.DATA_LENGTH_FORMAT);
		if (index >= 0) {
			sb.replace(index, index
					+ TraceFormatConstants.DATA_LENGTH_FORMAT.length(),
					lengthData.toString());
		}
		String s = sb.toString();
		write("retval = "); //$NON-NLS-1$
		writeFormattedTrace(s);
		writeNewLine();
	}

	/**
	 * Writes a trace line when the parameters can be fitted into a direct API
	 * call
	 * 
	 * @param trace
	 *            the trace
	 * @param bufferData
	 *            the buffer
	 * @return formatted trace
	 */
	private StringBuffer writePackedTraceLine(Trace trace,
			StringBuffer bufferData) {
		StringBuffer sb;
		// The formatting rule is used to get the API macro
		sb = new StringBuffer(SourceFormatter.formatTrace(trace,
				TraceFormatType.TRACE_PACKED));
		TraceFormattingRule rule = trace
				.getExtension(TraceFormattingRule.class);
		if (rule == null) {
			rule = trace.getModel().getExtension(TraceFormattingRule.class);
		}
		int index = sb.indexOf(TraceFormatConstants.PARAM_COUNT_FORMAT);
		if (index >= 0) {
			// Single parameter is supported
			sb.replace(index, index
					+ TraceFormatConstants.PARAM_COUNT_FORMAT.length(), rule
					.mapParameterCountToSource(trace, 1));
		}
		bufferData.append("*( ( TUint32* )ptr )"); //$NON-NLS-1$
		return sb;
	}

	/**
	 * Writes a trace line when the trace contains more data that the API
	 * supports
	 * 
	 * @param trace
	 *            the trace
	 * @param bufferData
	 *            the trace buffer
	 * @param lengthData
	 *            the trace length buffer
	 * @return the formatted trace
	 */
	private StringBuffer writeBufferedTraceLine(Trace trace,
			StringBuffer bufferData, StringBuffer lengthData) {
		StringBuffer sb;
		// Buffer parameter
		// *( ( TUint32* )ptr ), *( ( TUint32* )( ptr + 4 ) ), ..., ptr + x
		sb = new StringBuffer(SourceFormatter.formatTrace(trace,
				TraceFormatType.TRACE_BUFFER));
		if (isTraceBufferBuilt()) {
			bufferData.append("ptr"); //$NON-NLS-1$
			if (isBufferSizeDynamic()) {
				// In case of dynamic buffer, the length has been calculated
				// into length variable
				lengthData.append("length"); //$NON-NLS-1$
			} else {
				// Fixed size case
				lengthData.append(String.valueOf(fixedBufferSize));
			}
		} else {
			// In case of no-buffer, the size variable contain the data size
			bufferData.append("ptr"); //$NON-NLS-1$
			lengthData.append("size"); //$NON-NLS-1$
		}
		return sb;
	}

	/**
	 * Writes a formatted trace to the stream. This removes the newline from the
	 * trace if it exists
	 * 
	 * @param trace
	 *            the trace
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeFormattedTrace(String trace) throws IOException {
		if (trace.endsWith(SourceConstants.LINE_FEED)) {
			write(trace.substring(0, trace.length()
					- SourceConstants.LINE_FEED.length()));
		} else {
			write(trace);
		}
	}

	/**
	 * Increases indent and writes a new line, brace, new line combination
	 * 
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeOpenBrace() throws IOException {
		openBraceCount++;
		writeNewLine();
		write(SourceConstants.OPENING_BRACE);
		writeNewLine();
	}

	/**
	 * Write brace, decreases indent and writes a new line
	 * 
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeCloseBrace() throws IOException {
		write(SourceConstants.CLOSING_BRACE);
		openBraceCount--;
		writeNewLine();
	}
	
	/**
	 * write licence Text
	 * @throws IOException if write fails
	 */
	private void writeLicence() throws IOException {
		String licence = null;
		SourceParser parser = null;
		//first get any of the traces belonging to this header
		TraceModel model = header.getOwner().getModel();
		for (TraceGroup group : model) {
			for (Trace trace : group) {
				Iterator<LocationListBase> itr = trace.getExtensions(LocationListBase.class);
				// The trace must have at least one location that belong to this header
				while (itr.hasNext() && parser == null) {
					LocationListBase list = itr.next();
					for (LocationProperties loc : list) {
						if (isValidTraceForHeader(loc.getFileName())) {
								parser = ((TraceLocation)loc).getParser();
								break;
						}
					}
				}
				if (parser!= null)  {
					break;
				}
			}
			if (parser != null) {
				break;
			}
		}
		
		if (parser!= null) {
		List<SourceExcludedArea> excludedAreas = parser.getExcludedAreas();
		//try to find licence from the source
		if (!excludedAreas.isEmpty()) {
			SourceExcludedArea sourceExcludedArea = excludedAreas.get(0);
			int offset = sourceExcludedArea.getOffset();
			int type = sourceExcludedArea.getType();
			int length = sourceExcludedArea.getLength();
			if (offset == 0 && type == SourceExcludedArea.MULTILINE_COMMENT) {
				String data = sourceExcludedArea.getParser().getData(offset, length);
				if (data.contains("Copyright")) { //$NON-NLS-1$
					// licence found write it
					TraceCompilerLogger.printInfo("Add Licence text from: " + sourceExcludedArea.getFileName() + " to : " + header.getAbsolutePath()); //$NON-NLS-1$
					licence = data;
					write(licence);
				}
			}	
		}
		}
		
		if (licence == null) {
			//get default licence from the licence file
			licence = TraceCompilerEngineGlobals.getDefaultLicence(true);
			
			if(licence != null) {
				TraceCompilerLogger.printInfo("Add default EPL Licence to : " + header.getAbsolutePath()); //$NON-NLS-1$
				write(licence);
			}
		}
	}

	/**
	 * Writes a new line and indent to the stream
	 * 
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeNewLine() throws IOException {
		int newLines = newLineCount;
		while (newLines < maxNewLines) {
			write(SourceConstants.LINE_FEED);
			for (int i = 0; i < openBraceCount; i++) {
				write(INDENT);
			}
			newLines++;
		}
		newLineCount = maxNewLines;
	}

	/**
	 * Writes a define to stream
	 * 
	 * @param name
	 *            the name for the define
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeDefine(String name) throws IOException {
		write(SourceConstants.DEFINE);
		write(SourceConstants.SPACE);
		write(name);
		writeNewLine();
	}

	/**
	 * Writes include to header
	 * 
	 * @param name
	 *            the header name
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeSystemInclude(String name) throws IOException {
		write(SourceConstants.INCLUDE);
		write(SourceConstants.SPACE);
		write("<"); //$NON-NLS-1$
		write(name);
		write(">"); //$NON-NLS-1$
		writeNewLine();
	}

	/**
	 * Writes data to a stream
	 * 
	 * @param data
	 *            the string of data
	 * @throws IOException
	 *             if writing fails
	 */
	private void write(String data) throws IOException {
		// Check if function starts
		if (data.contains(INLINE_TBOOL) || bufferingFunction) {
			bufferingFunction = true;
			functionText.append(data);
		} else {
			headerOutput.write(data.getBytes());
		}
			newLineCount = 0;

		//try to duplicate function if the current function processing is complete and duplicate is needed.
		writeAndDuplicateFunction(data);
	}

	/**
	 * Duplicates the function if needed
	 * 
	 * @param data
	 *            data String
	 * @throws IOException
	 *             if writing fails
	 */
	private void writeAndDuplicateFunction(String data) throws IOException {
			// This assumes there is only one start or end bracket in one line!
			if (data.contains(SourceConstants.OPENING_BRACE)) {
				firstOpenBraceFound = true;
				numberOfBrackets++;
			} else if (data.contains(SourceConstants.CLOSING_BRACE)) {
				numberOfBrackets--;

				// Function ends
				if (numberOfBrackets == 0 && firstOpenBraceFound) {
					firstOpenBraceFound = false;
					bufferingFunction = false;
					//write start function guard
					writeStartFunctionGuard();
					//write the function
					headerOutput.write(functionText.toString().getBytes());
					//write end function guard
					writeEndFunctionGuard();

					//process duplicate if needed
					// Get the function definition line
					int startIndex = functionText.indexOf(SourceConstants.OPENING_BRACE);
					String funcDef = functionText.substring(0, startIndex);

					// Replace TInt with TInt32 and TUint with TUint32 from the
					// header and write the function back again
					if (funcDef.contains(TINT_DEF)
							|| funcDef.contains(TUINT_DEF)) {
						//replace and duplicate
						funcDef = funcDef.replace(TINT_DEF, TINT32_DEF);
						funcDef = funcDef.replace(TUINT_DEF, TUINT32_DEF);
						functionText.replace(0, startIndex, funcDef);
						
						//write start function guard for duplicate
						write(SourceConstants.LINE_FEED);
						write(SourceConstants.LINE_FEED);
						writeStartFunctionGuard();
						
						//write duplicate function
						headerOutput.write(functionText.toString().getBytes());
						
						//write end function guard for duplicate
						writeEndFunctionGuard();
					}

					functionText.setLength(0);
				}
			}
	}

	/**
	 * Checks if a trace needs a buffer or it can be represented with regular
	 * trace macros.
	 * 
	 * @param trace
	 *            the trace
	 * @return true if trace needs a buffer
	 */
	private boolean traceNeedsBuffer(Trace trace) {
		// A single dynamic parameter can be passed through the regular
		// API macros. In that case the parameter length is determined
		// by the trace message length
		Iterator<TraceParameter> parameters = trace.getParameters();
		boolean needsBuffer = false;
		boolean dynamicFound = false;
		while (parameters.hasNext() && !needsBuffer) {
			TraceParameter parameter = parameters.next();
			if (isParameterVisible(parameter)) {
				if (SourceUtils.isParameterSizeDynamic(parameter)) {
					if (dynamicFound) {
						needsBuffer = true;
					} else {
						dynamicFound = true;
					}
				} else {
					needsBuffer = true;
				}
			}
		}
		return needsBuffer;
	}

	/**
	 * Checks if a parameter is visible
	 * 
	 * @param parameter
	 *            the parameter to be checked
	 * @return true if visible, false if not
	 */
	private boolean isParameterVisible(TraceParameter parameter) {
		boolean retval;
		TraceParameterFormattingRule sourceRule = parameter
				.getExtension(TraceParameterFormattingRule.class);
		if (sourceRule == null || sourceRule.isShownInSource()) {
			retval = true;
		} else {
			retval = false;
		}
		return retval;
	}

	/**
	 * Gets the formatted trace
	 * 
	 * @return the trace
	 */
	String getCurrentTraceFormatted() {
		return currentTraceFormatted;
	}

	/**
	 * Checks if trace is already formatted to header
	 * 
	 * @return true if formatted
	 */
	boolean isTraceFormatDuplicate() {
		boolean retval;
		if (traceDeclarations.contains(currentTraceFormatted)) {
			retval = true;
		} else {
			traceDeclarations.add(currentTraceFormatted);
			retval = false;
		}
		return retval;
	}

	/**
	 * Checks if the buffer size for current trace is fixed
	 * 
	 * @return the flag
	 */
	boolean isTraceBufferFixed() {
		return fixedBufferSize != 0;
	}

	/**
	 * Checks if the buffer is built
	 * 
	 * @return the flag
	 */
	boolean isTraceBufferBuilt() {
		return buildTraceBuffer;
	}

	/**
	 * Gets the current trace
	 * 
	 * @return the trace
	 */
	Trace getCurrentTrace() {
		return currentTrace;
	}

	/**
	 * Gets the header
	 * 
	 * @return the header
	 */
	TraceHeader getHeader() {
		return header;
	}

	/**
	 * Starts writing a trace
	 * 
	 * @param trace
	 *            the trace
	 * @return true if trace can be written, false if not
	 */
	boolean startTrace(Trace trace) {
		boolean validTrace = false;
		Iterator<LocationListBase> itr = trace
				.getExtensions(LocationListBase.class);
		// The trace must have at least one location that belong to this header
		while (itr.hasNext() && !validTrace) {
			LocationListBase list = itr.next();
			for (LocationProperties loc : list) {
				validTrace = isValidTraceForHeader(loc.getFileName());
				if (validTrace) {
					break;
				}
			}
		}
		if (validTrace) {
			currentTrace = trace;
			fixedBufferSize = 0;
			dynamicBufferSizeFlag = false;
			buildTraceBuffer = false;
			currentTraceFormatted = null;
			currentParameterName = null;
			currentParameterType = null;
			currentParameterIndex = 0;
		}
		return validTrace;
	}

	/**
	 * Checks if the location belongs to this header
	 * 
	 * @param locFileName
	 *            the location
	 * @return true if location belongs here
	 */
	private boolean isValidTraceForHeader(String locFileName) {
		boolean valid = false;
		if (locFileName != null) {
			int index = locFileName.lastIndexOf('.');
			if (index >= 0) {
				locFileName = locFileName.substring(0, index);
			}
			if (locFileName.equals(header.getProjectName())) {
				valid = true;
			}
		}
		return valid;
	}

	/**
	 * Starts writing a parameter
	 * 
	 * @param parameter
	 *            the parameter
	 */
	void startParameter(TraceParameter parameter) {
		currentParameterName = null;
		currentParameterType = null;
		currentParameter = parameter;
	}

	/**
	 * Sets the kernel mode #endif needed flag
	 * 
	 * @param flag
	 *            the flag
	 */
	void setKernelModeEndifNeeded(boolean flag) {
		needsKernelEndif = flag;
	}

	/**
	 * Gets the kernel mode endif needed flag
	 * 
	 * @return the flag
	 */
	boolean isKernelModeEndifNeeded() {
		return needsKernelEndif;
	}

	/**
	 * Returns the dynamic buffer size flag
	 * 
	 * @return true if buffer size is dynamic
	 */
	boolean isBufferSizeDynamic() {
		return dynamicBufferSizeFlag;
	}

	/**
	 * Checks if current parameter needs alignment
	 * 
	 * @return true if needed
	 */
	boolean isParameterAlignmentNeeded() {
		return SourceUtils.isParameterAlignementNeeded(currentParameter
				.getType());
	}

}
