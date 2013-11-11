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
* Header file template definition
*
*/
package com.nokia.tracecompiler.engine.header;

import com.nokia.tracecompiler.engine.header.TraceHeaderWriter.HeaderTemplateElementType;
import com.nokia.tracecompiler.plugin.TraceAPIFormatter.TraceFormatType;
import com.nokia.tracecompiler.plugin.TraceHeaderContribution.TraceHeaderContributionType;
import com.nokia.tracecompiler.source.SymbianConstants;

/**
 * Header file template definition
 * 
 */
interface HeaderTemplate {
	
	/**
	 * Tag written to trace header
	 */
	String HEADER_COMMENT_CREATED_BY = "// Created by TraceCompiler"; //$NON-NLS-1$
	
	/**
	 * Empty template
	 */
	Object[] EMPTY_TEMPLATE = {};

	/**
	 * Sets line count to 1
	 */
	Object SET_LINES_PACKED = new SetNewLineCount(1);

	/**
	 * Sets line count to 2
	 */
	Object SET_LINES_MIDDLE = new SetNewLineCount(2); // CodForChk_Dis_Magic

	/**
	 * Sets line count to 3
	 */
	Object SET_LINES_LOOSE = new SetNewLineCount(3); // CodForChk_Dis_Magic

	/**
	 * Template for empty trace macros
	 */
	Object[] EMPTY_TRACE_FUNCTION_EXISTS_TEMPLATE = {
			HeaderTemplateElementType.NEW_LINE, "#define ", //$NON-NLS-1$
			HeaderTemplateElementType.FORMATTED_TRACE };

	/**
	 * Empty trace macro template switch. This formats the trace using
	 * TraceFormatType.EmptyPreprocessorDefinition and checks if the definition
	 * has already been written using CheckIsTraceFormatDuplicate switch
	 */
	Object[] EMPTY_TRACE_FUNCTION_TEMPLATE = {
			TraceFormatType.EMPTY_MACRO,
			new TemplateChoice(CheckIsTraceFormatDuplicate.class,
					EMPTY_TEMPLATE, EMPTY_TRACE_FUNCTION_EXISTS_TEMPLATE), };

	/**
	 * Template for trace activation check
	 */
	Object[] TRACE_FUNCTION_ACTIVATION_CHECK = {
			"TBool retval = ", //$NON-NLS-1$
			TraceFormatType.TRACE_ACTIVATION,
			// Checks if the TraceActivation format exists
			new TemplateChoice(CheckFormattedTraceExists.class, new Object[] {
					HeaderTemplateElementType.FORMATTED_TRACE, ";", //$NON-NLS-1$
					HeaderTemplateElementType.NEW_LINE, "if ( retval )", //$NON-NLS-1$
					HeaderTemplateElementType.OPEN_BRACE }, new Object[] {
					"EFalse;", HeaderTemplateElementType.NEW_LINE }), //$NON-NLS-1$
	};

	/**
	 * Template if no activation check is needed
	 */
	Object[] TRACE_FUNCTION_NO_ACTIVATION_CHECK = { "TBool retval;", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Template for trace functions
	 */
	Object[] TRACE_FUNCTION_EXISTS_TEMPLATE = {
			SET_LINES_PACKED,
			HeaderTemplateElementType.BUILD_TRACE_BUFFER_CHECK,
			// If the trace contains TDesC16 parameter, __KERNEL_MODE__ flag is
			// added to it
			new TemplateChoice(CheckIsKernelModeFlagNeeded.class, new Object[] {
					"#ifndef __KERNEL_MODE__", //$NON-NLS-1$
					HeaderTemplateElementType.NEW_LINE }, EMPTY_TEMPLATE),
			"inline TBool ", //$NON-NLS-1$
			HeaderTemplateElementType.FORMATTED_TRACE,
			HeaderTemplateElementType.OPEN_BRACE,
			// If the trace is buffered activation check is written
			new TemplateChoice(CheckIsTraceBufferBuilt.class,
					TRACE_FUNCTION_ACTIVATION_CHECK,
					TRACE_FUNCTION_NO_ACTIVATION_CHECK),
			HeaderTemplateElementType.TRACE_FUNCTION_BODY, // Parameters + API
			HeaderTemplateElementType.CLOSE_EXTRA_BRACES,
			"return retval;", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			HeaderTemplateElementType.CLOSE_BRACE,
			// The __KERNEL_MODE__ #ifdef added above needs to be closed
			new TemplateChoice(
					CheckIsKernelModeEndifNeeded.class,
					new Object[] { "#endif", HeaderTemplateElementType.NEW_LINE }, //$NON-NLS-1$
					EMPTY_TEMPLATE), SET_LINES_LOOSE,
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Trace function template switch. This formats the trace using
	 * TraceFormatType.Header and checks if the definition has already been
	 * written using TRACE_FORMAT_EXISTS_TEMPLATE_INDEX switch
	 */
	Object[] TRACE_FUNCTION_TEMPLATE = {
			TraceFormatType.HEADER,
			new TemplateChoice(CheckIsTraceFormatDuplicate.class,
					EMPTY_TEMPLATE, TRACE_FUNCTION_EXISTS_TEMPLATE) };

	/**
	 * Template for trace ID's
	 */
	Object[] TRACE_ID_TEMPLATE = {
			"#define ", //$NON-NLS-1$
			HeaderTemplateElementType.TRACE_NAME,
			" 0x", //$NON-NLS-1$
			HeaderTemplateElementType.TRACE_ID_HEX,
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Main header template
	 */
	Object[] HEADER_TEMPLATE = {
			SET_LINES_PACKED,
			HeaderTemplateElementType.LICENCE_TEXT,
			HeaderTemplateElementType.NEW_LINE,
			HEADER_COMMENT_CREATED_BY, " ", //$NON-NLS-1$
			HeaderTemplateElementType.TRACE_COMPILER_VERSION,
			HeaderTemplateElementType.NEW_LINE,
			"// DO NOT EDIT, CHANGES WILL BE LOST", //$NON-NLS-1$
			SET_LINES_MIDDLE,
			HeaderTemplateElementType.NEW_LINE,
			HeaderTemplateElementType.HEADER_GUARD,
			HeaderTemplateElementType.NEW_LINE,
			SET_LINES_PACKED,
			TraceHeaderContributionType.GLOBAL_DEFINES,
			SET_LINES_MIDDLE,
			HeaderTemplateElementType.NEW_LINE,
			SET_LINES_PACKED,
			TraceHeaderContributionType.GLOBAL_INCLUDES,
			SET_LINES_MIDDLE,
			HeaderTemplateElementType.NEW_LINE,
			SET_LINES_PACKED,
			new TemplateIterator(IteratorTraces.class, TRACE_ID_TEMPLATE),
			SET_LINES_LOOSE,
			HeaderTemplateElementType.NEW_LINE,
			// Trace functions
			new TemplateIterator(IteratorComplexTraces.class,
					TRACE_FUNCTION_TEMPLATE),
			// End of header guard
			SET_LINES_MIDDLE, "#endif", HeaderTemplateElementType.NEW_LINE, //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE, "// End of file", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Template for parameter with fixed size
	 */
	Object[] FIXED_PARAMETER_TEMPLATE = { "*( ( ", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_TYPE, "* )ptr ) = ", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_NAME, ";", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE, "ptr += sizeof ( ", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_TYPE, " );", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Template for parameter with fixed size and length check
	 */
	Object[] FIXED_PARAMETER_TEMPLATE_WITH_LENGTH_CHECK = {
			"// Check that there are enough space to next parameter", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"if ((length + sizeof ( ", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_TYPE,
			" )) <= KOstMaxDataLength)", //$NON-NLS-1$
			HeaderTemplateElementType.OPEN_BRACE,
			FIXED_PARAMETER_TEMPLATE,
			"length += sizeof ( ", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_TYPE,
			" );", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			HeaderTemplateElementType.CLOSE_BRACE};	
	
	/**
	 * Template for writing 32-bit alignment for parameters
	 */
	Object[] DYNAMIC_PARAMETER_ALIGNMENT_TEMPLATE = {
			"// Fillers are written to get 32-bit alignment", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"while ( length", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_INDEX,
			"++ < lengthAligned", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_INDEX,
			" )", //$NON-NLS-1$
			HeaderTemplateElementType.OPEN_BRACE,
			"*ptr++ = 0;", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			HeaderTemplateElementType.CLOSE_BRACE };

	/**
	 * Template for parameter with dynamic size
	 */
	Object[] DYNAMIC_PARAMETER_TEMPLATE = {
			"if (length", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_INDEX,
			" > 0)", //$NON-NLS-1$
			HeaderTemplateElementType.OPEN_BRACE,
			"// Number of elements is written before data", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"// In case of Unicode string, number of elements is half of length", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"*( ( TUint32* )ptr ) = length", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_INDEX,
			" / (", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_NAME,
			".Size() / ", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_NAME,
			".Length());", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"ptr += sizeof ( TUint32 );", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"memcpy( ptr, ", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_NAME,
			".Ptr(), length", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_INDEX,
			" );", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"ptr += length", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_INDEX,
			";", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			new TemplateChoice(CheckParameterNeedsAlignment.class,
					DYNAMIC_PARAMETER_ALIGNMENT_TEMPLATE, EMPTY_TEMPLATE),
			HeaderTemplateElementType.NEW_LINE,
			"length += sizeof ( TUint32 ) + lengthAligned", //$NON-NLS-1$
			HeaderTemplateElementType.PARAMETER_INDEX,
			";", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,			
			HeaderTemplateElementType.CLOSE_BRACE,
			"else if (length + sizeof ( TUint32 ) <= KOstMaxDataLength)",  //$NON-NLS-1$
			HeaderTemplateElementType.OPEN_BRACE,
			HeaderTemplateElementType.NEW_LINE,
			"*( ( TUint32* )ptr ) = 0;",  //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"ptr += sizeof ( TUint32 );",  //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"length += sizeof ( TUint32 );",  //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			HeaderTemplateElementType.CLOSE_BRACE};

	/**
	 * Template to allocate the buffer for parameters if buffer size is fixed
	 */
	Object[] FIXED_BUFFER_ALLOCATION_TEMPLATE = { "TUint8 data[ ", //$NON-NLS-1$
			HeaderTemplateElementType.FIXED_BUFFER_SIZE, " ];", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE, "TUint8* ptr = data;", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE};

	/**
	 * Template to allocate the buffer for parameters if buffer size is dynamic
	 */
	Object[] DYNAMIC_BUFFER_ALLOCATION_TEMPLATE = {
			// KOstMaxDataLength is defined in opensystemtrace.h
			"TUint8 data[ KOstMaxDataLength ];", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE, "TUint8* ptr = data;", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE, "// Set length to zero and calculate it againg", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE, "// when adding parameters", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE, "length = 0;", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Template to allocate buffer for trace data
	 */
	Object[] BUFFER_ALLOCATION_TEMPLATE = { new TemplateChoice(
			CheckIsBufferSizeDynamic.class, DYNAMIC_BUFFER_ALLOCATION_TEMPLATE,
			FIXED_BUFFER_ALLOCATION_TEMPLATE) };

	/**
	 * Template to length variable definition
	 */
	Object[] LENGTH_VARIABLE_DEFINITION_TEMPLATE = {	
			"TInt length = 0;", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Beging template to length variable increase
	 */
	Object[] LENGTH_VARIABLE_INCREASE_TEMPLATE_BEGIN = {
			"// Increase length because of fixed size parameters", //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE,
			"length += "};	 //$NON-NLS-1$

	/**
	 * End template to length variable increase
	 */
	Object[] LENGTH_VARIABLE_INCREASE_TEMPLATE_END = {	
			";",	 //$NON-NLS-1$	
			HeaderTemplateElementType.NEW_LINE };
	
	/**
	 * Template to calculate the length for a dynamic parameter
	 */
	Object[] DYNAMIC_PARAMETER_LENGTH_TEMPLATE = {
	"// Check that parameter lenght is not too long", //$NON-NLS-1$
	HeaderTemplateElementType.NEW_LINE,
	"TInt length", //$NON-NLS-1$
	HeaderTemplateElementType.PARAMETER_INDEX,
	" = ", //$NON-NLS-1$
	SymbianConstants.PARAMETER_DECLARATION_PREFIX,
	HeaderTemplateElementType.PARAMETER_INDEX, ".Size();", //$NON-NLS-1$
	HeaderTemplateElementType.NEW_LINE,
		"if ((length + length", //$NON-NLS-1$
	HeaderTemplateElementType.PARAMETER_INDEX,
	" + sizeof ( TUint32 )) > KOstMaxDataLength)", //$NON-NLS-1$
	HeaderTemplateElementType.OPEN_BRACE,
	"length", //$NON-NLS-1$
	HeaderTemplateElementType.PARAMETER_INDEX,	
	" = KOstMaxDataLength - (length + sizeof ( TUint32 ));", //$NON-NLS-1$	
	HeaderTemplateElementType.NEW_LINE,
	HeaderTemplateElementType.CLOSE_BRACE,
	HeaderTemplateElementType.NEW_LINE,
	"TInt lengthAligned", //$NON-NLS-1$
	HeaderTemplateElementType.PARAMETER_INDEX,
	" = ( length", //$NON-NLS-1$
	HeaderTemplateElementType.PARAMETER_INDEX,
	" + 3 ) & ~3;", //$NON-NLS-1$
	HeaderTemplateElementType.NEW_LINE,	
	"if (lengthAligned", //$NON-NLS-1$
	HeaderTemplateElementType.PARAMETER_INDEX,
	" > 0)", //$NON-NLS-1$
	HeaderTemplateElementType.OPEN_BRACE,
	"length = length + sizeof ( TUint32 ) + lengthAligned", //$NON-NLS-1$
	HeaderTemplateElementType.PARAMETER_INDEX,
	";", //$NON-NLS-1$
	HeaderTemplateElementType.NEW_LINE,
	HeaderTemplateElementType.CLOSE_BRACE,
	HeaderTemplateElementType.NEW_LINE };

	/**
	 * Template for the optimized single dynamic parameter case, where buffer is
	 * not needed
	 */
	Object[] SINGLE_DYNAMIC_PARAMETER_TEMPLATE = {
			"TInt size = ", //$NON-NLS-1$
		    SymbianConstants.PARAMETER_DECLARATION_PREFIX, "1.Size();", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "// BTrace assumes that parameter size is atleast 4 bytes", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "if (size % 4 == 0)", //$NON-NLS-1$
		    HeaderTemplateElementType.OPEN_BRACE,
		    "TUint8* ptr = ( TUint8* )aParam1.Ptr();", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "// Data is written directly and length is determined from trace message length", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, size );", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    HeaderTemplateElementType.CLOSE_BRACE,
		    HeaderTemplateElementType.NEW_LINE,
		    "else", //$NON-NLS-1$
		    HeaderTemplateElementType.OPEN_BRACE,
		    "TUint8 data[ KOstMaxDataLength ];", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "TUint8* ptr = data;", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "if (size > KOstMaxDataLength)", //$NON-NLS-1$
		    HeaderTemplateElementType.OPEN_BRACE,
		    "size = KOstMaxDataLength;", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    HeaderTemplateElementType.CLOSE_BRACE,
		    HeaderTemplateElementType.NEW_LINE,
		    "TInt sizeAligned = ( size + 3 ) & ~3;", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,		    
		    "memcpy( ptr, ", //$NON-NLS-1$
		    SymbianConstants.PARAMETER_DECLARATION_PREFIX, "1.Ptr(), size );", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "ptr += size;", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "// Fillers are written to get 32-bit alignment", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "while ( size++ < sizeAligned )", //$NON-NLS-1$
		    HeaderTemplateElementType.OPEN_BRACE,
		    "*ptr++ = 0;", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    HeaderTemplateElementType.CLOSE_BRACE,
			HeaderTemplateElementType.NEW_LINE,
		    "ptr -= sizeAligned;", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "size = sizeAligned;", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "// Data is written directly and length is determined from trace message length", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    "retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, size );", //$NON-NLS-1$
		    HeaderTemplateElementType.NEW_LINE,
		    HeaderTemplateElementType.CLOSE_BRACE,
			HeaderTemplateElementType.NEW_LINE};

	/**
	 * Template for buffered parameters
	 */
	Object[] BUFFERED_PARAMETERS_TEMPLATE = { HeaderTemplateElementType.TRACE_FUNCTION_PARAMETERS };

	/**
	 * Template when the trace buffer needs to be allocated
	 */
	Object[] BUILD_TRACE_BUFFER_TEMPLATE = {
			new TemplateIterator(IteratorParameters.class,
					BUFFERED_PARAMETERS_TEMPLATE),
			"ptr -= ", //$NON-NLS-1$
			new TemplateChoice(CheckIsBufferSizeDynamic.class,
					new Object[] { "length;" }, new Object[] { //$NON-NLS-1$
					HeaderTemplateElementType.FIXED_BUFFER_SIZE, ";" }), //$NON-NLS-1$
			HeaderTemplateElementType.NEW_LINE };

	/**
	 * Template for trace parameters.
	 */
	Object[] PARAMETERS_TEMPLATE = { new TemplateChoice(
			CheckIsTraceBufferBuilt.class, BUILD_TRACE_BUFFER_TEMPLATE,
			SINGLE_DYNAMIC_PARAMETER_TEMPLATE) };

}
