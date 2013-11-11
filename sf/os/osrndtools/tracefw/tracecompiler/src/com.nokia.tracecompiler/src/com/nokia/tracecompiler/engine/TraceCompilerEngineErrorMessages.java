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
 * Error code to error message mapper
 *
 */
package com.nokia.tracecompiler.engine;

import java.io.File;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.FileErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.RangeErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.StringErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceCompilerErrorParameters;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceUtils;

/**
 * Error code to error message mapper
 * 
 */
public final class TraceCompilerEngineErrorMessages {

	/**
	 * Maps an exception to error message
	 * 
	 * @param e
	 *            the exception
	 * @return the message
	 */
	public static String getErrorMessage(TraceCompilerException e) {
		return TraceCompilerEngineErrorMessages.getErrorMessage(
				(TraceCompilerErrorCode) e.getErrorCode(), e
						.getErrorParameters());
	}

	/**
	 * Maps error code to error message
	 * 
	 * @param error
	 *            the error code
	 * @param parameters
	 *            the parameters of the error
	 * @return the message
	 */
	public static String getErrorMessage(TraceCompilerErrorCode error,
			TraceCompilerErrorParameters parameters) {
		// CodForChk_Dis_LengthyFunc
		// CodForChk_Dis_ComplexFunc
		String s;
		StringBuffer sb = new StringBuffer();
		switch (error) {
		case DUPLICATE_GROUP_ID:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.DuplicateGroupID")); //$NON-NLS-1$
			break;
		case DUPLICATE_GROUP_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.DuplicateGroupName")); //$NON-NLS-1$
			break;
		case DUPLICATE_TRACE_ID:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.DuplicateTraceID")); //$NON-NLS-1$
			break;
		case DUPLICATE_TRACE_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.DuplicateTraceName")); //$NON-NLS-1$
			break;
		case DUPLICATE_PARAMETER_ID:
			s = Messages
					.getString("TraceCompilerErrorMessages.DuplicateParameterID"); //$NON-NLS-1$
			sb.append(s);
			break;
		case DUPLICATE_CONSTANT_VALUE:
			s = Messages
					.getString("TraceCompilerErrorMessages.DuplicateConstantValue"); //$NON-NLS-1$
			sb.append(s);
			break;
		case DUPLICATE_CONSTANT_ID:
			s = Messages
					.getString("TraceCompilerErrorMessages.DuplicateConstantID"); //$NON-NLS-1$
			sb.append(s);
			break;
		case DUPLICATE_CONSTANT_TABLE_NAME:
			s = Messages
					.getString("TraceCompilerErrorMessages.DuplicateConstantTableName"); //$NON-NLS-1$
			sb.append(s);
			break;
		case DUPLICATE_CONSTANT_TABLE_ID:
			s = Messages
					.getString("TraceCompilerErrorMessages.DuplicateConstantTableID"); //$NON-NLS-1$
			sb.append(s);
			break;
		case DUPLICATE_PARAMETER_NAME:
			s = Messages
					.getString("TraceCompilerErrorMessages.DuplicateParameterName"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_GROUP_ID:
			createInvalidGroupIDMessage(parameters, sb);
			break;
		case INVALID_TRACE_ID:
			createInvalidTraceIDMessage(parameters, sb);
			break;
		case INVALID_MODEL_PROPERTIES_FOR_EXPORT:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidModelPropertiesForExport"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_MODEL_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidProjectName")); //$NON-NLS-1$
			break;
		case INVALID_GROUP_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidGroupName")); //$NON-NLS-1$
			break;
		case INVALID_TRACE_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceName")); //$NON-NLS-1$
			break;
		case INVALID_PARAMETER_NAME:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidParameterName"); //$NON-NLS-1$
			sb.append(s);
			break;
		case EMPTY_PARAMETER_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.EmptyParameterName")); //$NON-NLS-1$
			break;
		case INVALID_CONSTANT_TABLE_NAME:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidConstantTableName"); //$NON-NLS-1$
			sb.append(s);
			break;
		case CONSTANT_TABLE_NOT_PART_OF_PROJECT:
			s = Messages
					.getString("TraceCompilerErrorMessages.ConstantTableNotPartOfProject"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_TRACE_DATA:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceData")); //$NON-NLS-1$
			break;
		case INVALID_PARAMETER_TYPE:
			createInvalidParameterTypeMessage(parameters, sb);
			break;
		case INVALID_CONSTANT_VALUE:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidConstantValue"); //$NON-NLS-1$
			sb.append(s);
			break;
		case SOURCE_NOT_EDITABLE:
			s = Messages
					.getString("TraceCompilerErrorMessages.SourceNotEditable"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_SOURCE_LOCATION:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidSourceLocation"); //$NON-NLS-1$
			sb.append(s);
			break;
		case UNREACHABLE_TRACE_LOCATION:
			s = Messages
					.getString("TraceCompilerErrorMessages.UnreachableTraceLocation"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_PROJECT_FILE:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceFile")); //$NON-NLS-1$
			break;
		case FILE_NOT_FOUND:
			createFileNotFoundMessage((FileErrorParameters) parameters, sb);
			break;
		case INVALID_PATH:
			createInvalidPathMessage((FileErrorParameters) parameters, sb);
			break;
		case SOURCE_NOT_OPEN:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.SourceNotOpen")); //$NON-NLS-1$
			break;
		case CANNOT_OPEN_PROJECT_FILE:
			s = Messages
					.getString("TraceCompilerErrorMessages.CannotOpenProjectFile"); //$NON-NLS-1$
			sb.append(s);
			break;
		case CANNOT_WRITE_PROJECT_FILE:
			s = Messages
					.getString("TraceCompilerErrorMessages.CannotWriteProjectFile"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PARAMETER_FORMAT_MISMATCH:
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatMismatch"); //$NON-NLS-1$
			sb.append(s);
			break;
		case GROUP_NOT_SELECTED:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.GroupNotSelected")); //$NON-NLS-1$
			break;
		case TRACE_NOT_SELECTED:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.TraceNotSelected")); //$NON-NLS-1$
			break;
		case CONSTANT_TABLE_NOT_SELECTED:
			s = Messages
					.getString("TraceCompilerErrorMessages.ConstantTableNotSelected"); //$NON-NLS-1$
			sb.append(s);
			break;
		case LOCATION_NOT_SELECTED:
			s = Messages
					.getString("TraceCompilerErrorMessages.TraceLocationNotSelected"); //$NON-NLS-1$
			sb.append(s);
			break;
		case CANNOT_DELETE_SELECTED_OBJECT:
			s = Messages
					.getString("TraceCompilerErrorMessages.CannotDeleteSelectedObject"); //$NON-NLS-1$
			sb.append(s);
			break;
		case MODEL_NOT_READY:
			s = Messages
					.getString("TraceCompilerErrorMessages.TraceProjectNotOpen"); //$NON-NLS-1$
			sb.append(s);
			break;
		case NO_TRACE_GROUPS:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.NoTraceGroups")); //$NON-NLS-1$
			break;
		case NOT_ENOUGH_PARAMETERS:
			s = Messages
					.getString("TraceCompilerErrorMessages.NotEnoughParameters"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PARAMETER_ADD_NOT_ALLOWED:
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterAddNotAllowed"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PARAMETER_REMOVE_NOT_ALLOWED:
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterRemoveNotAllowed"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PARAMETER_TEMPLATE_ALREADY_IN_USE:
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterTemplateInUse"); //$NON-NLS-1$
			sb.append(s);
			break;
		case CONSTANT_TABLE_PARSE_FAILED:
			s = Messages
					.getString("TraceCompilerErrorMessages.ConstantTableParseFailed"); //$NON-NLS-1$
			sb.append(s);
			break;
		case UNEXPECTED_EXCEPTION:
			s = Messages
					.getString("TraceCompilerErrorMessages.UnexpectedException"); //$NON-NLS-1$
			sb.append(s);
			break;
		case TRACE_NAME_FORMAT_MISSING_FUNCTION:
			s = Messages
					.getString("TraceCompilerErrorMessages.NameFormatMissingFunction"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_TRACE_TEXT_FORMAT:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceTextFormat"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_TRACE_NAME_FORMAT:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceNameFormat"); //$NON-NLS-1$
			sb.append(s);
			break;
		case NO_FUNCTIONS_TO_INSTRUMENT_WITH_TEMPLATE:
			s = Messages
					.getString("TraceCompilerErrorMessages.NoFunctionsToInstrumentPrefix"); //$NON-NLS-1$
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			s = Messages
					.getString("TraceCompilerErrorMessages.NoFunctionsToInstrumentPostfix"); //$NON-NLS-1$
			sb.append(s);
			break;
		case NO_FUNCTIONS_TO_INSTRUMENT:
			s = Messages
					.getString("TraceCompilerErrorMessages.NoFunctionsToInstrument"); //$NON-NLS-1$
			sb.append(s);
			break;
		case MULTIPLE_ERRORS_IN_OPERATION:
			s = Messages
					.getString("TraceCompilerErrorMessages.MultipleErrorsInOperation"); //$NON-NLS-1$
			sb.append(s);
			break;
		case NO_TRACES_TO_DELETE:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.NoTracesToDelete")); //$NON-NLS-1$
			break;
		case TRACE_DOES_NOT_EXIST:
			s = Messages
					.getString("TraceCompilerErrorMessages.TraceDoesNotExist"); //$NON-NLS-1$
			sb.append(s);
			break;
		case TRACE_NEEDS_CONVERSION:
			s = Messages
					.getString("TraceCompilerErrorMessages.TraceNeedsConversionPrefix"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PARAMETER_COUNT_MISMATCH:
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterCountMismatch"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PARAMETER_COUNT_DOES_NOT_MATCH_API:
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterCountDoesNotMatchApi"); //$NON-NLS-1$
			sb.append(s);
			break;
		case TRACE_HAS_NO_LOCATIONS:
			s = Messages
					.getString("TraceCompilerErrorMessages.TraceHasNoLocations"); //$NON-NLS-1$
			sb.append(s);
			break;
		case TRACE_HAS_MULTIPLE_LOCATIONS:
			s = Messages
					.getString("TraceCompilerErrorMessages.TraceHasMultipleLocations"); //$NON-NLS-1$
			sb.append(s);
			break;
		case LOCATION_PARSER_FAILED:
			s = Messages
					.getString("TraceCompilerErrorMessages.LocationCouldNotBeParsed"); //$NON-NLS-1$
			sb.append(s);
			break;
		case NO_TRACES_TO_EXPORT:
			s = Messages
					.getString("TraceCompilerErrorMessages.NoTracesToExport"); //$NON-NLS-1$
			sb.append(s);
			break;
		case CANNOT_OPEN_SOURCE_FILE:
			s = Messages
					.getString("TraceCompilerErrorMessages.CannotOpenSourceFile"); //$NON-NLS-1$
			sb.append(s);
			break;
		case CANNOT_UPDATE_TRACE_INTO_SOURCE:
			s = Messages
					.getString("TraceCompilerErrorMessages.CannotUpdateTraceIntoSource"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PARAMETER_FORMAT_NOT_SUPPORTED:
			createParameterFormatNotSupportedMessage(parameters, sb);
			break;
		case PARAMETER_FORMAT_NEEDS_EXT_MACRO:
			createParameterFormatNotSupportedInMacroMessage(parameters, sb);
			break;
		case PARAMETER_FORMAT_NOT_SUPPORTED_IN_ARRAY:
			createParameterFormatNotSupportedInArrayMessage(parameters, sb);
			break;
		case PARAMETER_FORMAT_UNNECESSARY_EXT_MACRO:
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatUnnecessaryExtMacro"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PROPERTY_FILE_ELEMENT_NOT_SUPPORTED:
			s = Messages
					.getString("TraceCompilerErrorMessages.PropertyFileElementNotSupportedPrefix"); //$NON-NLS-1$ CodForChk_Dis_LengthyLine
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			s = Messages
					.getString("TraceCompilerErrorMessages.PropertyFileElementNotSupportedPostfix"); //$NON-NLS-1$ CodForChk_Dis_LengthyLine
			sb.append(s);
			break;
		case PROPERTY_FILE_ELEMENT_MISPLACED:
			s = Messages
					.getString("TraceCompilerErrorMessages.PropertyFileElementMisplacedPrefix"); //$NON-NLS-1$
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			s = Messages
					.getString("TraceCompilerErrorMessages.PropertyFileElementMisplacedPostfix"); //$NON-NLS-1$
			sb.append(s);
			break;
		case PROPERTY_FILE_ATTRIBUTE_INVALID:
			s = Messages
					.getString("TraceCompilerErrorMessages.PropertyFileAttributeInvalidPrefix"); //$NON-NLS-1$
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			s = Messages
					.getString("TraceCompilerErrorMessages.PropertyFileAttributeInvalidPostfix"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INSERT_TRACE_DOES_NOT_WORK:
			s = Messages
					.getString("TraceCompilerErrorMessages.InsertTraceDoesNotWork"); //$NON-NLS-1$
			sb.append(s);
			break;
		case NO_CONTEXT_FOR_LOCATION:
			s = Messages
					.getString("TraceCompilerErrorMessages.NoContextForLocation"); //$NON-NLS-1$
			sb.append(s);
			break;
		case CANNOT_PARSE_FUNCTION_PARAMETERS:
			s = Messages
					.getString("TraceCompilerErrorMessages.CannotParseFunctionParameters"); //$NON-NLS-1$
			sb.append(s);
			break;
		case INVALID_PARAMETER_NAME_IN_RETURN_VALUE:
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidParameterNameInReturnValue"); //$NON-NLS-1$
			sb.append(s);
			break;
		case RUN_OUT_OF_GROUP_IDS:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.RunOutOfGroupIDs")); //$NON-NLS-1$
			break;
		case RUN_OUT_OF_TRACE_IDS:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.RunOutOfTraceIDs")); //$NON-NLS-1$
			break;
		case VAR_ARG_LIST_PARAMETER_FOUND:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.VarArgListParameterFound")); //$NON-NLS-1$
			break;
		case INVALID_USAGE_OF_TRACE_STATE_GROUP_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidUsageOfTraceStateGroupName")); //$NON-NLS-1$
			break;
		case INVALID_USAGE_OF_TRACE_PERFORMACE_GROUP_NAME:
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidUsageOfTracePerformanceGroupName")); //$NON-NLS-1$
			break;
		default:
			break;
		}
		return sb.toString();
	}

	/**
	 * Creates invalid parameter type message
	 * 
	 * @param parameters
	 *            the parameters
	 * @param sb
	 *            the message buffer
	 */
	private static void createInvalidParameterTypeMessage(
			TraceCompilerErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters instanceof StringErrorParameters) {
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidParameterTypePrefix"); //$NON-NLS-1$
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			String format = SourceUtils
					.mapNormalTypeToFormat(((StringErrorParameters) parameters).string);
			if (format != null) {
				sb.append(" "); //$NON-NLS-1$
				sb.append(format);
			}
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidParameterTypePostfix"); //$NON-NLS-1$
			sb.append(s);
		} else {
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidParameterType"); //$NON-NLS-1$
			sb.append(s);
		}
	}

	/**
	 * Creates parameter format not supported message
	 * 
	 * @param parameters
	 *            the parameters
	 * @param sb
	 *            the message buffer
	 */
	private static void createParameterFormatNotSupportedMessage(
			TraceCompilerErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters instanceof StringErrorParameters) {
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedPrefix"); //$NON-NLS-1$
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedPostfix"); //$NON-NLS-1$
			sb.append(s);
		} else {
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupported"); //$NON-NLS-1$
			sb.append(s);
		}
	}

	/**
	 * Creates parameter not supported in macro message
	 * 
	 * @param parameters
	 *            the parameters
	 * @param sb
	 *            the message buffer
	 */
	private static void createParameterFormatNotSupportedInMacroMessage(
			TraceCompilerErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters instanceof StringErrorParameters) {
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedInMacroPrefix"); //$NON-NLS-1$ CodForChk_Dis_LengthyLine
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedInMacroPostfix"); //$NON-NLS-1$ CodForChk_Dis_LengthyLine
			sb.append(s);
		} else {
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedInMacro"); //$NON-NLS-1$
			sb.append(s);
		}
	}

	/**
	 * Creates parameter not supported in array message
	 * 
	 * @param parameters
	 *            the parameters
	 * @param sb
	 *            the message buffer
	 */
	private static void createParameterFormatNotSupportedInArrayMessage(
			TraceCompilerErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters instanceof StringErrorParameters) {
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedInArrayPrefix"); //$NON-NLS-1$ CodForChk_Dis_LengthyLine
			sb.append(s);
			sb.append(((StringErrorParameters) parameters).string);
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedInArrayPostfix"); //$NON-NLS-1$ CodForChk_Dis_LengthyLine
			sb.append(s);
		} else {
			s = Messages
					.getString("TraceCompilerErrorMessages.ParameterFormatNotSupportedInArray"); //$NON-NLS-1$
			sb.append(s);
		}
	}

	/**
	 * Creates invalid trace ID message
	 * 
	 * @param parameters
	 *            the parameters
	 * @param sb
	 *            the message buffer
	 */
	private static void createInvalidTraceIDMessage(
			TraceCompilerErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters instanceof RangeErrorParameters) {
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceIDStart"); //$NON-NLS-1$
			sb.append(s);
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceIDMiddle"); //$NON-NLS-1$
			addRangeParameter((RangeErrorParameters) parameters, sb, s);
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceIDEnd")); //$NON-NLS-1$
		} else {
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidTraceID")); //$NON-NLS-1$
		}
	}

	/**
	 * Creates invalid group ID message
	 * 
	 * @param parameters
	 *            the parameters
	 * @param sb
	 *            the message buffer
	 */
	private static void createInvalidGroupIDMessage(
			TraceCompilerErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters instanceof RangeErrorParameters) {
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidGroupIDStart"); //$NON-NLS-1$
			sb.append(s);
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidGroupIDMiddle"); //$NON-NLS-1$
			addRangeParameter((RangeErrorParameters) parameters, sb, s);
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidGroupIDEnd")); //$NON-NLS-1$
		} else {
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidGroupID")); //$NON-NLS-1$
		}
	}

	/**
	 * Adds a range parameter to error buffer
	 * 
	 * @param parameters
	 *            the range
	 * @param sb
	 *            the buffer
	 * @param middleText
	 *            the text between the range
	 */
	private static void addRangeParameter(RangeErrorParameters parameters,
			StringBuffer sb, String middleText) {
		if (parameters.isHex) {
			sb.append(SourceConstants.HEX_PREFIX);
			sb.append(Integer.toHexString(parameters.start));
		} else {
			sb.append(parameters.start);
		}
		sb.append(middleText);
		if (parameters.isHex) {
			sb.append(SourceConstants.HEX_PREFIX);
			sb.append(Integer.toHexString(parameters.end));
		} else {
			sb.append(parameters.end);
		}
	}

	/**
	 * Creates "Invalid directory" message
	 * 
	 * @param parameters
	 *            the message parameters
	 * @param sb
	 *            the string buffer where the message is stored
	 */
	private static void createInvalidPathMessage(
			FileErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters != null) {
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidDirectoryPrefix"); //$NON-NLS-1$
			sb.append(s);
			sb.append(convertPath(parameters.file));
			s = Messages
					.getString("TraceCompilerErrorMessages.InvalidDirectoryPostfix"); //$NON-NLS-1$
			sb.append(s);
		} else {
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.InvalidDirectory")); //$NON-NLS-1$
		}
	}

	/**
	 * Creates "File not found" message
	 * 
	 * @param parameters
	 *            the message parameters
	 * @param sb
	 *            the string buffer where the message is stored
	 */
	private static void createFileNotFoundMessage(
			FileErrorParameters parameters, StringBuffer sb) {
		String s;
		if (parameters != null) {
			s = Messages
					.getString("TraceCompilerErrorMessages.FileDoesNotExistPrefix"); //$NON-NLS-1$
			sb.append(s);
			sb.append(convertPath(parameters.file));
			s = Messages
					.getString("TraceCompilerErrorMessages.FileDoesNotExistPostfix"); //$NON-NLS-1$
			sb.append(s);
		} else {
			sb.append(Messages
					.getString("TraceCompilerErrorMessages.FileDoesNotExist")); //$NON-NLS-1$
		}
	}

	/**
	 * Adds some spaces to path string to allow folding
	 * 
	 * @param path
	 *            the path
	 * @return the converted path
	 */
	public static String convertPath(String path) {
		StringBuffer sb = new StringBuffer();
		int strIndex = -1;
		do {
			strIndex++;
			int lastIndex = strIndex;
			strIndex = path.indexOf(File.separatorChar, strIndex);
			if (strIndex != -1) {
				String sub = path.substring(lastIndex, strIndex);
				if (sub.length() > 0) {
					sb.append(sub);
					sb.append(' ');
					sb.append(File.separatorChar);
					sb.append(' ');
				}
			} else {
				// If the data ends with file separator, lastIndex points to
				// end-of-data. If not, the rest of the data is appended without
				// further white spaces
				if (lastIndex < path.length()) {
					String sub = path.substring(lastIndex);
					sb.append(sub);
				}
			}
		} while (strIndex != -1);
		return sb.toString();
	}

}
