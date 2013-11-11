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
 * Error codes for TraceCompiler
 *
 */
package com.nokia.tracecompiler.engine;

import com.nokia.tracecompiler.model.TraceCompilerErrorParameters;
import com.nokia.tracecompiler.model.TraceCompilerException.TraceCompilerExceptionCode;

/**
 * Error codes for TraceCompiler
 * 
 */
public interface TraceCompilerEngineErrorCodes {

	/**
	 * Error parameters for a file-related errors
	 */
	public class FileErrorParameters extends TraceCompilerErrorParameters {

		/**
		 * Path to the file
		 */
		public String file;

	}

	/**
	 * Error parameters for errors related to names, titles etc.
	 */
	public class StringErrorParameters extends TraceCompilerErrorParameters {

		/**
		 * String
		 */
		public String string;

	}

	/**
	 * Error parameters for integer values that must be between two values
	 */
	public class RangeErrorParameters extends TraceCompilerErrorParameters {

		/**
		 * Start of range
		 */
		public int start;

		/**
		 * End of range
		 */
		public int end;

		/**
		 * Hex / decimal flag
		 */
		public boolean isHex;

	}

	/**
	 * Error codes for TraceCompiler
	 */
	public enum TraceCompilerErrorCode implements TraceCompilerExceptionCode {

		/**
		 * Success
		 */
		OK,

		/**
		 * Trace ID was duplicate
		 */
		DUPLICATE_TRACE_ID,

		/**
		 * Group ID was duplicate
		 */
		DUPLICATE_GROUP_ID,

		/**
		 * Trace name was duplicate
		 */
		DUPLICATE_TRACE_NAME,

		/**
		 * Trace name was duplicate
		 */
		DUPLICATE_GROUP_NAME,

		/**
		 * Trace ID was not valid. Parameters for this error are either
		 * RangeErrorParameters or null (no range)
		 */
		INVALID_TRACE_ID,

		/**
		 * Group ID was not valid. Parameters for this error are either
		 * RangeErrorParameters or null (no range)
		 */
		INVALID_GROUP_ID,

		/**
		 * Name was not valid
		 */
		INVALID_TRACE_NAME,

		/**
		 * Name was not valid
		 */
		INVALID_GROUP_NAME,

		/**
		 * Trace string was not valid
		 */
		INVALID_TRACE_DATA,

		/**
		 * Cannot edit source file
		 */
		SOURCE_NOT_EDITABLE,

		/**
		 * Source location was not valid
		 */
		INVALID_SOURCE_LOCATION,

		/**
		 * Unreachable trace location
		 */
		UNREACHABLE_TRACE_LOCATION,

		/**
		 * Tried to perform group operation when group was not selected
		 */
		GROUP_NOT_SELECTED,

		/**
		 * Tried to perform trace operation when trace was not selected
		 */
		TRACE_NOT_SELECTED,

		/**
		 * Not enough parameters in trace
		 */
		NOT_ENOUGH_PARAMETERS,

		/**
		 * Unexpected exception occurred
		 */
		UNEXPECTED_EXCEPTION,

		/**
		 * Trace model is not ready
		 */
		MODEL_NOT_READY,

		/**
		 * No trace groups
		 */
		NO_TRACE_GROUPS,

		/**
		 * Parameter name is not valid
		 */
		INVALID_PARAMETER_NAME,

		/**
		 * Model properties are not valid
		 */
		INVALID_MODEL_PROPERTIES_FOR_EXPORT,

		/**
		 * Model name is not valid
		 */
		INVALID_MODEL_NAME,

		/**
		 * Trace project file cannot be parsed
		 */
		INVALID_PROJECT_FILE,

		/**
		 * File was not found. This error is related to FileErrorParameters
		 * class
		 */
		FILE_NOT_FOUND,

		/**
		 * Path was not valid. This is related to FileErrorParameters class
		 */
		INVALID_PATH,

		/**
		 * Constant value was duplicate
		 */
		DUPLICATE_CONSTANT_VALUE,

		/**
		 * Constant name was already in use
		 */
		DUPLICATE_CONSTANT_TABLE_NAME,

		/**
		 * Constant table name was not valid
		 */
		INVALID_CONSTANT_TABLE_NAME,

		/**
		 * Constant table not part of project
		 */
		CONSTANT_TABLE_NOT_PART_OF_PROJECT,

		/**
		 * Constant ID was already in use
		 */
		DUPLICATE_CONSTANT_ID,

		/**
		 * Constant value was not valid
		 */
		INVALID_CONSTANT_VALUE,

		/**
		 * Parameter name was already in use
		 */
		DUPLICATE_PARAMETER_NAME,

		/**
		 * Parameter type was not supported
		 */
		INVALID_PARAMETER_TYPE,

		/**
		 * Parameters cannot be added
		 */
		PARAMETER_ADD_NOT_ALLOWED,

		/**
		 * Parameters cannot be removed
		 */
		PARAMETER_REMOVE_NOT_ALLOWED,

		/**
		 * Parameter name was empty. This is generated when automatically adding
		 * function parameters to new trace and a parameter does not have a name
		 * in the declaration.
		 */
		EMPTY_PARAMETER_NAME,

		/**
		 * Parameter template was already used by another parameter
		 */
		PARAMETER_TEMPLATE_ALREADY_IN_USE,

		/**
		 * Tried to remove trace location when one was not selected
		 */
		LOCATION_NOT_SELECTED,

		/**
		 * Cannot add a constant, since constant table was not selected
		 */
		CONSTANT_TABLE_NOT_SELECTED,

		/**
		 * The selected object was not valid target for delete operation
		 */
		CANNOT_DELETE_SELECTED_OBJECT,

		/**
		 * Trace text format is not valid
		 */
		INVALID_TRACE_TEXT_FORMAT,

		/**
		 * Trace name format is not valid
		 */
		INVALID_TRACE_NAME_FORMAT,

		/**
		 * Missing function name from trace name format
		 */
		TRACE_NAME_FORMAT_MISSING_FUNCTION,

		/**
		 * No traces to delete
		 */
		NO_TRACES_TO_DELETE,

		/**
		 * Source file is not open
		 */
		SOURCE_NOT_OPEN,

		/**
		 * Failed to parse a constant table from source
		 */
		CONSTANT_TABLE_PARSE_FAILED,

		/**
		 * Cannot open project file
		 */
		CANNOT_OPEN_PROJECT_FILE,

		/**
		 * Cannot write project file
		 */
		CANNOT_WRITE_PROJECT_FILE,

		/**
		 * Parameter ID was already in use
		 */
		DUPLICATE_PARAMETER_ID,

		/**
		 * Constant table ID was already in use
		 */
		DUPLICATE_CONSTANT_TABLE_ID,

		/**
		 * Instrumenter function list was empty
		 */
		NO_FUNCTIONS_TO_INSTRUMENT,

		/**
		 * Instrumenter function list was empty. Related to
		 * StringErrorParameters
		 */
		NO_FUNCTIONS_TO_INSTRUMENT_WITH_TEMPLATE,

		/**
		 * Multiple errors during an operation
		 */
		MULTIPLE_ERRORS_IN_OPERATION,

		/**
		 * Printf parameter parameter format specification did not match the
		 * parameter count
		 */
		PARAMETER_FORMAT_MISMATCH,

		/**
		 * A trace does not exist
		 */
		TRACE_DOES_NOT_EXIST,

		/**
		 * Trace needs to be converted to correct API
		 */
		TRACE_NEEDS_CONVERSION,

		/**
		 * Parameter count of location and trace do not match
		 */
		PARAMETER_COUNT_MISMATCH,

		/**
		 * Parameter count in location does not match the API macro
		 */
		PARAMETER_COUNT_DOES_NOT_MATCH_API,

		/**
		 * Location parser failed
		 */
		LOCATION_PARSER_FAILED,

		/**
		 * Trace is not referenced from source files
		 */
		TRACE_HAS_NO_LOCATIONS,

		/**
		 * Trace is used from multiple locations in source files
		 */
		TRACE_HAS_MULTIPLE_LOCATIONS,

		/**
		 * Project was empty when export was called
		 */
		NO_TRACES_TO_EXPORT,

		/**
		 * Source file cannot be opened
		 */
		CANNOT_OPEN_SOURCE_FILE,

		/**
		 * Trace in source cannot be updated
		 */
		CANNOT_UPDATE_TRACE_INTO_SOURCE,

		/**
		 * Parameter format is not supported
		 */
		PARAMETER_FORMAT_NOT_SUPPORTED,

		/**
		 * Parameter format is not supported by default macros
		 */
		PARAMETER_FORMAT_NEEDS_EXT_MACRO,

		/**
		 * Ext macro usage is not necessary with
		 */
		PARAMETER_FORMAT_UNNECESSARY_EXT_MACRO,

		/**
		 * Parameter format cannot be used as an array
		 */
		PARAMETER_FORMAT_NOT_SUPPORTED_IN_ARRAY,

		/**
		 * Property file element was unknown
		 */
		PROPERTY_FILE_ELEMENT_NOT_SUPPORTED,

		/**
		 * Property file element was in wrong place
		 */
		PROPERTY_FILE_ELEMENT_MISPLACED,

		/**
		 * Property file attribute was not valid
		 */
		PROPERTY_FILE_ATTRIBUTE_INVALID,

		/**
		 * Inserting a trace into source will produce incorrect results
		 */
		INSERT_TRACE_DOES_NOT_WORK,

		/**
		 * Trace found from source is not in valid source context
		 */
		NO_CONTEXT_FOR_LOCATION,

		/**
		 * Function parameters could not be parsed
		 */
		CANNOT_PARSE_FUNCTION_PARAMETERS,

		/**
		 * Parameter name is not valid exit trace return parameter name
		 */
		INVALID_PARAMETER_NAME_IN_RETURN_VALUE,

		/**
		 * Run out of Group IDs
		 */
		RUN_OUT_OF_GROUP_IDS,

		/**
		 * Run out of Trace IDs
		 */
		RUN_OUT_OF_TRACE_IDS,

		/**
		 * Cannot add function parameters if there is var arg list
		 */
		VAR_ARG_LIST_PARAMETER_FOUND,

		/**
		 * TRACE_STATE group name was used in non state trace
		 */
		INVALID_USAGE_OF_TRACE_STATE_GROUP_NAME,

		/**
		 * TRACE_PERFORMANCE group name was used in non performance trace
		 */
		INVALID_USAGE_OF_TRACE_PERFORMACE_GROUP_NAME
	}

}
