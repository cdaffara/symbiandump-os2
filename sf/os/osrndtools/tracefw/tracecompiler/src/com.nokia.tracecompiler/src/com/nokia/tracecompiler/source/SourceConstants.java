/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Constants related to source files
*
*/
package com.nokia.tracecompiler.source;

/**
 * Constants related to source files
 * 
 */
public interface SourceConstants {

	/**
	 * Space character as string
	 */
	final String SPACE = " "; //$NON-NLS-1$

	/**
	 * Quote character as string
	 */
	final String QUOTE = "\""; //$NON-NLS-1$

	/**
	 * '\' character as string
	 */
	final String BACKSLASH = "\\"; //$NON-NLS-1$

	/**
	 * '_' character as string
	 */
	final String UNDERSCORE = "_"; //$NON-NLS-1$

	/**
	 * Double underscore for header guards
	 */
	final String DOUBLE_UNDERSCORE = "__"; //$NON-NLS-1$

	/**
	 * Semicolon character as string
	 */
	final String SEMICOLON = ";"; //$NON-NLS-1$

	/**
	 * Colon character as string
	 */
	final String COLON = ":"; //$NON-NLS-1$

	/**
	 * Period character as string
	 */
	final String PERIOD = "."; //$NON-NLS-1$

	/**
	 * Tilde character as string
	 */
	final String TILDE = "~"; //$NON-NLS-1$

	/**
	 * Opening brace as string
	 */
	final String OPENING_BRACE = "{"; //$NON-NLS-1$

	/**
	 * Closing brace as string
	 */
	final String CLOSING_BRACE = "}"; //$NON-NLS-1$

	/**
	 * Asterisk character
	 */
	final String ASTERISK = "*"; //$NON-NLS-1$

	/**
	 * Space character
	 */
	final char SPACE_CHAR = ' ';

	/**
	 * Colon character
	 */
	final char COLON_CHAR = ':';
	
	/**
	 * Quote character
	 */
	final char QUOTE_CHAR = '\"';

	/**
	 * '\' character
	 */
	final char BACKSLASH_CHAR = '\\';

	/**
	 * '/' character
	 */
	final char FORWARD_SLASH_CHAR = '/';

	/**
	 * '_' character
	 */
	final char UNDERSCORE_CHAR = '_';

	/**
	 * '.' character
	 */
	final char PERIOD_CHAR = '.';

	/**
	 * Line separator
	 */
	final String LINE_FEED = System.getProperty("line.separator"); //$NON-NLS-1$

	/**
	 * Header extension (.h)
	 */
	final String HEADER_EXTENSION = ".h"; //$NON-NLS-1$

	/**
	 * #define
	 */
	final String DEFINE = "#define"; //$NON-NLS-1$

	/**
	 * #undef
	 */
	final String UNDEF = "#undef"; //$NON-NLS-1$

	/**
	 * #include
	 */
	final String INCLUDE = "#include"; //$NON-NLS-1$

	/**
	 * #ifdef
	 */
	final String IFDEF = "#ifdef"; //$NON-NLS-1$

	/**
	 * #ifndef
	 */
	final String IFNDEF = "#ifndef"; //$NON-NLS-1$

	/**
	 * #if
	 */
	final String IF = "#if"; //$NON-NLS-1$

	/**
	 * defined
	 */
	final String DEFINED = "defined"; //$NON-NLS-1$

	/**
	 * #else
	 */
	final String ELSE = "#else"; //$NON-NLS-1$

	/**
	 * #endif
	 */
	final String ENDIF = "#endif"; //$NON-NLS-1$

	/**
	 * Or
	 */
	final String OR = "||"; //$NON-NLS-1$

	/**
	 * Return statement
	 */
	final String RETURN = "return"; //$NON-NLS-1$

	/**
	 * One step up in path ("../")
	 */
	final String PATH_UP = "../"; //$NON-NLS-1$

	/**
	 * This path ("./")
	 */
	final String THIS_PATH = "./"; //$NON-NLS-1$

	/**
	 * inline
	 */
	final String INLINE = "inline"; //$NON-NLS-1$

	/**
	 * void
	 */
	final String VOID = "void"; //$NON-NLS-1$

	/**
	 * char
	 */
	final String CHAR = "char"; //$NON-NLS-1$

	/**
	 * short
	 */
	final String SHORT = "short"; //$NON-NLS-1$

	/**
	 * int
	 */
	final String INT = "int"; //$NON-NLS-1$

	/**
	 * long
	 */
	final String LONG = "long"; //$NON-NLS-1$

	/**
	 * unsigned
	 */
	final String UNSIGNED = "unsigned"; //$NON-NLS-1$

	/**
	 * Parameter type qualifier list
	 */
	final String[] PARAMETER_QUALIFIERS = { "const", //$NON-NLS-1$
			"volatile", UNSIGNED }; //$NON-NLS-1$

	/**
	 * No parameters
	 */
	final String NO_PARAMETERS = "()"; //$NON-NLS-1$

	/**
	 * Start of parameters
	 */
	final String START_PARAMETERS = "( "; //$NON-NLS-1$

	/**
	 * Start of parameters with quote
	 */
	final String START_PARAMETERS_QUOTE = "( \""; //$NON-NLS-1$

	/**
	 * Closing parenthesis with non-quoted last parameter
	 */
	final String END_PARAMETERS = " )"; //$NON-NLS-1$

	/**
	 * Closing parenthesis with quoted last parameter
	 */
	final String END_PARAMETERS_QUOTE = "\" )"; //$NON-NLS-1$

	/**
	 * Separator between end of quoted parameter and start of non-quoted
	 * parameter
	 */
	final String QUOTE_PARAMETER_SEPARATOR = "\", "; //$NON-NLS-1$

	/**
	 * Separator between two parameters without quotes
	 */
	final String PARAMETER_SEPARATOR = ", "; //$NON-NLS-1$

	/**
	 * Separator within for
	 */
	final String FOR_SEPARATOR = "; "; //$NON-NLS-1$

	/**
	 * Separator between end of non-quoted parameter and start of quoted
	 * parameter
	 */
	final String PARAMETER_SEPARATOR_QUOTE = ", \""; //$NON-NLS-1$

	/**
	 * if and the opening parenthesis
	 */
	final String START_IF = "if ( "; //$NON-NLS-1$

	/**
	 * Sizeof and the opening parenthesis
	 */
	final String START_SIZEOF = "sizeof ( "; //$NON-NLS-1$

	/**
	 * for and the opening parenthesis
	 */
	final String START_FOR = "for ( "; //$NON-NLS-1$

	/**
	 * Assignment operation with spaces
	 */
	final String ASSIGN_WITH_SPACES = " = "; //$NON-NLS-1$

	/**
	 * Assignment operation with out spaces
	 */
	final String ASSIGN_WITH_OUT_SPACES = "="; //$NON-NLS-1$
	
	/**
	 * Less of equals for if-statements
	 */
	final String LESS_OR_EQUAL_THAN = " <= "; //$NON-NLS-1$

	/**
	 * Less for if statements
	 */
	final String LESS_THAN = " < "; //$NON-NLS-1$

	/**
	 * Starting bracket for array
	 */
	final String START_ARRAY = "[ "; //$NON-NLS-1$

	/**
	 * Ending bracket for array
	 */
	final String END_ARRAY = " ]"; //$NON-NLS-1$

	/**
	 * Pointer and space
	 */
	final String POINTER = "* "; //$NON-NLS-1$

	/**
	 * Add operation with spaces
	 */
	final String ADD = " + "; //$NON-NLS-1$

	/**
	 * Modulo operation with spaces
	 */
	final String MOD = " % "; //$NON-NLS-1$

	/**
	 * Subtract with assignment
	 */
	final String SUBTRACT_ASSIGN = " -= "; //$NON-NLS-1$

	/**
	 * Add with assignment
	 */
	final String ADD_ASSIGN = " += "; //$NON-NLS-1$

	/**
	 * Not equal check
	 */
	final String NOT_EQUALS = " != "; //$NON-NLS-1$

	/**
	 * Equal check
	 */
	final String EQUALS = " == "; //$NON-NLS-1$

	/**
	 * Increment operation
	 */
	final String INCREMENT = "++"; //$NON-NLS-1$

	/**
	 * Subtract operation with spaces
	 */
	final String SUBTRACT = " - "; //$NON-NLS-1$

	/**
	 * class
	 */
	final String CLASS = "class"; //$NON-NLS-1$

	/**
	 * public
	 */
	final String PUBLIC = "public"; //$NON-NLS-1$

	/**
	 * private
	 */
	final String PRIVATE = "private"; //$NON-NLS-1$

	/**
	 * Prefix for hex numbers
	 */
	final String HEX_PREFIX = "0x"; //$NON-NLS-1$

	/**
	 * Prefix for hex numbers, upper case
	 */
	final String HEX_PREFIX_U = "0X"; //$NON-NLS-1$

	/**
	 * Prefix for octal numbers
	 */
	final String OCTAL_PREFIX = "0"; //$NON-NLS-1$

	/**
	 * 64-bit integer postfix
	 */
	final String I64_POSTFIX = "i64"; //$NON-NLS-1$

	/**
	 * Size of long integer
	 */
	final int LONG_SIZE = 64; // CodForChk_Dis_Magic

	/**
	 * Size of integer
	 */
	final int INT_SIZE = 32; // CodForChk_Dis_Magic

	/**
	 * Size of short
	 */
	final int SHORT_SIZE = 16; // CodForChk_Dis_Magic

	/**
	 * Size of byte
	 */
	final int BYTE_SIZE = 8; // CodForChk_Dis_Magic

	/**
	 * Array type prefix
	 */
	final String OST_ARRAY_TYPE_PREFIX = "const TOstArray< "; //$NON-NLS-1$

	/**
	 * Array type postfix
	 */
	final String OST_ARRAY_TYPE_POSTFIX = " >&"; //$NON-NLS-1$
	
	/**
	 * Variable argument list indicator
	 */
	final String VARIABLE_ARG_LIST_INDICATOR = "..."; //$NON-NLS-1$

}
