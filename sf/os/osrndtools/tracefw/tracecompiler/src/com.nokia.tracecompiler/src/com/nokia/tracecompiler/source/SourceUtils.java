/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Static utility functions related to source files
*
*/
package com.nokia.tracecompiler.source;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.StringErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceConstantTable;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.rules.ArrayParameterRule;

/**
 * Static utility functions related to source files
 * 
 */
public class SourceUtils {

	/**
	 * Tag for array types
	 */
	private static final String ARRAY_TAG = "[]"; //$NON-NLS-1$

	/**
	 * Start tag for printf format specifier
	 */
	private static final String START_TAG = "%"; //$NON-NLS-1$

	/**
	 * Regular expression optional element tag
	 */
	private static final String OPTIONAL_TAG = "?"; //$NON-NLS-1$

	/**
	 * Optional parameter pattern. Quoted from Wikipedia:
	 * 
	 * <strong> Parameter can be omitted or can be: 'n$' Where n is the number
	 * of the parameter to display using this format specifier, allowing the
	 * parameters provided to be output multiple times, using varying format
	 * specifiers or in different orders. This is a POSIX extension and not in
	 * C99.</strong>
	 * 
	 * This has not been implemented -> Currently format specifier count must
	 * match parameter count
	 */
	private final static String PARAMETER_PATTERN = "(\\d+\\$)"; //$NON-NLS-1$

	/**
	 * Optional flags pattern. Quoted from Wikipedia:
	 * 
	 * <strong> Flags can be zero or more (in any order) of:
	 * <ul>
	 * <li>'+' : Causes printf to always denote the sign '+' or '-' of a number
	 * (the default is to omit the sign for positive numbers). Only applicable
	 * to numeric types.
	 * <li>'-' : Causes printf to left-align the output of this placeholder (the
	 * default is to right-align the output).
	 * <li>'#' : Alternate form. For 'g' and 'G', trailing zeros are not
	 * removed. For 'f', 'F', 'e', 'E', 'g', 'G', the output always contains a
	 * decimal point. For 'o', 'x', and 'X', a 0, 0x, and 0X, respectively, is
	 * prepended to non-zero numbers.
	 * <li>' ' : Causes printf to left-pad the output with spaces until the
	 * required length of output is attained. If combined with '0' (see below),
	 * it will cause the sign to become a space when positive, but the remaining
	 * characters will be zero-padded
	 * <li>'0' : Causes printf to use '0' (instead of spaces) to left fill a
	 * fixed length field. For example (assume i = 3) printf("%2d", i) results
	 * in " 3", while printf("%02d", i) results in "03"
	 * </ul>
	 * </strong>
	 */
	private final static String FLAGS_PATTERN = "([-+# 0])"; //$NON-NLS-1$

	/**
	 * Optional width pattern. Quoted from Wikipedia:
	 * 
	 * <strong>Width can be omitted or be any of:
	 * <ul>
	 * <li>a number : Causes printf to pad the output of this placeholder with
	 * spaces until it is at least number characters wide. If number has a
	 * leading '0', then padding is done with '0' characters.
	 * <li>'*' : Causes printf to pad the output until it is n characters wide,
	 * where n is an integer value stored in the a function argument just
	 * preceding that represented by the modified type. For example
	 * printf("%*d", 5, 10) will result in "10" being printed with a width of
	 * 5.</strong>
	 * </ul>
	 * </strong>
	 * 
	 * '*' has not been implemented -> Currently format specifier count must
	 * match parameter count
	 */
	private final static String WIDTH_PATTERN = "(\\d+|\\*)"; //$NON-NLS-1$

	/**
	 * Optional precision pattern. Quoted from Wikipedia:
	 * 
	 * Precision can be omitted or be any of: <strong>
	 * <ul>
	 * <li>a number : For non-integral numeric types, causes the decimal portion
	 * of the output to be expressed in at least number digits. For the string
	 * type, causes the output to be truncated at number characters.
	 * <li>'*' : Same as the above, but uses an integer value in the intaken
	 * argument to determine the number of decimal places or maximum string
	 * length. For example, printf("%.*s", 3, "abcdef") will result in "abc"
	 * being printed.
	 * </ul>
	 * </strong> If the precision is zero, nothing is printed for the
	 * corresponding argument.
	 * 
	 * '*' has not been implemented -> Currently format specifier count must
	 * match parameter count
	 */
	private final static String PRECISION_PATTERN = "(\\.(\\d+|\\*))"; //$NON-NLS-1$

	/**
	 * Optional length pattern. Quoted from Wikipedia:
	 * 
	 * Length can be omitted or be any of: <strong>
	 * <ul>
	 * <li>'hh' : For integer types, causes printf to expect an int sized
	 * integer argument which was promoted from a char.
	 * <li>'h' : For integer types, causes printf to expect a int sized integer
	 * argument which was promoted from a short.
	 * <li>'l' : (ell) For integer types, causes printf to expect a long sized
	 * integer argument.
	 * <li>'ll' : (ell ell) For integer types, causes printf to expect a long
	 * long sized integer argument.
	 * <li>'L' : For floating point types, causes printf to expect a long double
	 * argument.
	 * <li>'z' : For integer types, causes printf to expect a size_t sized
	 * integer argument.
	 * <li>'j' : For integer types, causes printf to expect a intmax_t sized
	 * integer argument.
	 * <li>'t' : For integer types, causes printf to expect a ptrdiff_t sized
	 * integer argument.
	 * </ul>
	 * </strong>
	 */
	private final static String LENGTH_PATTERN = "([lh]?[hHlLZjt])"; //$NON-NLS-1$

	/**
	 * Type pattern. Quoted from Wikipedia:
	 * 
	 * <strong> type can be any of:
	 * <ul>
	 * <li>'d', 'i' : Print an int as a signed decimal number. '%d' and '%i' are
	 * synonymous for output, but are different when used with scanf() for
	 * input.
	 * <li>'u' : Print decimal unsigned int.
	 * <li>'f', 'F' : Print a double in normal (fixed-point) notation.
	 * <li>'e', 'E' : Print a double value in standard form ([-]d.ddd
	 * e[+/-]ddd).
	 * <li>'g', 'G' : Print a double in either normal or exponential notation,
	 * whichever is more appropriate for its magnitude. 'g' uses lower-case
	 * letters, 'G' uses upper-case letters. This type differs slightly from
	 * fixed-point notation in that insignificant zeroes to the right of the
	 * decimal point are not included. Also, the decimal point is not included
	 * on whole numbers.
	 * <li>'x', 'X' : Print an unsigned int as a hexadecimal number. 'x' uses
	 * lower-case letters and 'X' uses upper-case.
	 * <li>'o' : Print an unsigned int in octal.
	 * <li>'s' : Print a character string.
	 * <li>'c' : Print a char (character).
	 * <li>'p' : Print a void * (pointer to void) in an implementation-defined
	 * format.
	 * <li>'n' : Write number of characters successfully written so far into an
	 * integer pointer parameter.
	 * <li>'%' : Print a literal '%' character (this type doesn't accept any
	 * flags, width, precision or length).
	 * </ul>
	 * </strong>
	 * 
	 * The pattern itself accepts all characters and the validity check is done
	 * in {@link #mapFormatToParameterType mapFormatToType}
	 */
	private final static String TYPE_PATTERN = "([a-zA-Z%])"; //$NON-NLS-1$

	/**
	 * Regular expression pattern for printf
	 * 
	 * %[parameter][flags][width][.precision][length]type
	 */
	private static final String STANDARD_PRINTF_PATTERN = PARAMETER_PATTERN
			+ OPTIONAL_TAG + FLAGS_PATTERN + OPTIONAL_TAG + WIDTH_PATTERN
			+ OPTIONAL_TAG + PRECISION_PATTERN + OPTIONAL_TAG + LENGTH_PATTERN
			+ OPTIONAL_TAG + TYPE_PATTERN;

	/**
	 * Regular expression pattern for Open System Trace printf extensions
	 * %{Type}, %{Array[]}
	 */
	private static final String EXTENSION_PRINTF_PATTERN = "\\{[\\w_]+(\\[\\])?\\}"; //$NON-NLS-1$

	/**
	 * Regular expression pattern for printf
	 */
	public static final String PRINTF_PATTERN = START_TAG + "((" //$NON-NLS-1$
			+ STANDARD_PRINTF_PATTERN + ")|(" //$NON-NLS-1$
			+ EXTENSION_PRINTF_PATTERN + "))"; //$NON-NLS-1$

	/**
	 * The pattern for printf-formatted trace text
	 */
	public final static Pattern traceTextPattern = Pattern
			.compile(PRINTF_PATTERN);

	/**
	 * The pattern for parameter length in printf specifier
	 */
	public final static Pattern lengthPattern = Pattern.compile(LENGTH_PATTERN);

	/**
	 * Hidden constructor
	 */
	private SourceUtils() {
	}

	/**
	 * Creates a header guard
	 * 
	 * @param fileName
	 *            the name of the file
	 * @return the header guard
	 */
	public static String createHeaderGuard(String fileName) {
		StringBuffer sb = new StringBuffer();
		String uname = fileName.replace(SourceConstants.PERIOD_CHAR,
				SourceConstants.UNDERSCORE_CHAR).toUpperCase();
		sb.append(SourceConstants.IFNDEF);
		sb.append(SourceConstants.SPACE);
		sb.append(SourceConstants.DOUBLE_UNDERSCORE);
		sb.append(uname);
		sb.append(SourceConstants.DOUBLE_UNDERSCORE);
		sb.append(SourceConstants.LINE_FEED);
		sb.append(SourceConstants.DEFINE);
		sb.append(SourceConstants.SPACE);
		sb.append(SourceConstants.DOUBLE_UNDERSCORE);
		sb.append(uname);
		sb.append(SourceConstants.DOUBLE_UNDERSCORE);
		return sb.toString();
	}

	/**
	 * Checks the validity of name
	 * 
	 * @param name
	 *            the name
	 * @return true if valid
	 */
	public static boolean isValidName(String name) {
		boolean retval;
		if (name != null && name.length() > 0) {
			retval = true;
			if (!isValidNameStartChar(name.charAt(0))) {
				retval = false;
			} else {
				for (int i = 1; i < name.length() && retval; i++) {
					retval = isValidNameChar(name.charAt(i));
				}
			}
		} else {
			retval = false;
		}
		return retval;
	}

	/**
	 * Checks the validity of parameter name
	 * 
	 * @param name
	 *            the name
	 * @return true if valid
	 */
	public static boolean isValidParameterName(String name) {
		boolean retval;
		if (name != null && name.length() > 0) {
			retval = true;
			if (!isValidNameStartChar(name.charAt(0))) {
				retval = false;
			} else {
				for (int i = 1; i < name.length() && retval; i++) {

					// Check validity of the character
					char c = name.charAt(i);
					retval = (isValidNameChar(c) || isValidSpecialChar(c));
				}
			}
		} else {
			retval = false;
		}
		return retval;
	}

	/**
	 * Checks special character validity
	 * 
	 * @param c
	 *            character
	 * @return true if valid
	 */
	private static boolean isValidSpecialChar(char c) {
		boolean retval = false;
		// Check if the character is allowed
		if (c == '.' || c == '-' || c == '>' || c == '(' || c == ')'
				|| c == '[' || c == ']' || c == ' ' || c == '&' || c == '*') {
			retval = true;
		}
		return retval;
	}

	/**
	 * Checks start-of-name character validity
	 * 
	 * @param c
	 *            character
	 * @return true if valid
	 */
	private static boolean isValidNameStartChar(char c) {
		// Ascii characters and underscore are allowed
		return (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A) // CodForChk_Dis_Magic
				|| c == 0x5F; // CodForChk_Dis_Magic
	}

	/**
	 * Checks name character validity
	 * 
	 * @param c
	 *            character
	 * @return true if valid
	 */
	private static boolean isValidNameChar(char c) {
		// Numbers are allowed in addition to start characters
		return isValidNameStartChar(c) || (c >= 0x30 && c <= 0x39); // CodForChk_Dis_Magic
	}

	/**
	 * Maps a Symbian type to one of the TraceParameter types
	 * 
	 * @param parsedType
	 *            the type parsed from source
	 * @return the parameter type
	 */
	public static TypeMapping mapSymbianTypeToParameterType(
			ParsedType parsedType) {
		String type = null;
		// The type map contains Symbian types
		for (int i = 0; i < SymbianConstants.PARAMETER_TYPE_MAP.length
				&& type == null; i++) {
			if (parsedType
					.typeEquals(SymbianConstants.PARAMETER_TYPE_MAP[i][0])) {
				type = SymbianConstants.PARAMETER_TYPE_MAP[i][1];
			}
		}
		if (type != null) {
			if (parsedType.hasQualifier(SourceConstants.UNSIGNED)) {
				type = convertToUnsigned(type);
			}
		}
		TypeMapping retval = new TypeMapping(type);
		if (type != null) {
			// Value or a reference can be added to source as is
			// Points needs to be cast to HEX32
			if (parsedType.isPointer()) {
				retval.type = TraceParameter.HEX32;
				retval.needsCasting = true;
			} else {
				// TUint32 needs to be cast to TUint and TInt32 to TInt.
				// Otherwise there will be some problems with extension
				// headers
				if (parsedType.typeEquals(SymbianConstants.TUINT32)
						|| parsedType.typeEquals(SymbianConstants.TINT32)) {
					retval.needsCasting = true;
				}
			}
		} else if (parsedType.isPointer()) {
			// Unrecognized pointer types are cast to Hex32
			retval.type = TraceParameter.HEX32;
			retval.needsCasting = true;
		} else {
			// Unrecognized value types are passed as pointer and cast to Hex32
			retval.type = TraceParameter.HEX32;
			retval.valueToPointer = true;
			retval.needsCasting = true;
		}
		return retval;
	}

	/**
	 * Maps the type of a parameter to a Symbian type
	 * 
	 * @param parameter
	 *            the parameter
	 * @return the parameter type as string
	 */
	public static String mapParameterTypeToSymbianType(TraceParameter parameter) {
		String retval;
		ArrayParameterRule rule = parameter
				.getExtension(ArrayParameterRule.class);
		String type = parameter.getType();
		TraceConstantTable table = parameter.getModel()
				.findConstantTableByName(type);
		if (table != null) {
			type = table.getType();
		}
		if (rule != null) {
			retval = mapArrayTypeToSymbianType(type);
		} else {
			retval = mapBasicTypeToSymbianType(type);
		}
		return retval;
	}

	/**
	 * Maps a basic parameter type to Symbian type
	 * 
	 * @param type
	 *            the parameter type
	 * @return the Symbian type
	 */
	public static String mapBasicTypeToSymbianType(String type) {
		String retval;
		// Unsigned and hex both use TUint types
		// Signed uses TInt types
		if (type.equals(TraceParameter.POINTER)) {
			retval = SymbianConstants.CONST_TANY_PTR;
		} else if (type.equals(TraceParameter.SDEC32)) {
			retval = SymbianConstants.TINT;
		} else if (type.equals(TraceParameter.UDEC32)
				|| type.equals(TraceParameter.OCT32)
				|| type.equals(TraceParameter.HEX32)) {
			retval = SymbianConstants.TUINT;
		} else if (type.equals(TraceParameter.SDEC16)) {
			retval = SymbianConstants.TINT16;
		} else if (type.equals(TraceParameter.UDEC16)
				|| type.equals(TraceParameter.OCT16)
				|| type.equals(TraceParameter.HEX16)) {
			retval = SymbianConstants.TUINT16;
		} else if (type.equals(TraceParameter.SDEC8)) {
			retval = SymbianConstants.TINT8;
		} else if (type.equals(TraceParameter.UDEC8)
				|| type.equals(TraceParameter.OCT8)
				|| type.equals(TraceParameter.HEX8)) {
			retval = SymbianConstants.TUINT8;
		} else if (type.equals(TraceParameter.SDEC64)
				|| type.equals(TraceParameter.TIME)) {
			retval = SymbianConstants.TINT64;
		} else if (type.equals(TraceParameter.UDEC64)
				|| type.equals(TraceParameter.OCT64)
				|| type.equals(TraceParameter.HEX64)) {
			retval = SymbianConstants.TUINT64;
		} else if (type.equals(TraceParameter.ASCII)) {
			retval = SymbianConstants.CONST_TDESC8_REF;
		} else if (type.equals(TraceParameter.UNICODE)) {
			retval = SymbianConstants.CONST_TDESC16_REF;
		} else if (type.equals(TraceParameter.FLOAT_EXP)
				|| type.equals(TraceParameter.FLOAT_FIX)
				|| type.equals(TraceParameter.FLOAT_OPT)) {
			retval = SymbianConstants.TREAL;
		} else {
			retval = SymbianConstants.TANY_PTR;
		}
		return retval;
	}

	/**
	 * Maps an array parameter type to Symbian type
	 * 
	 * @param type
	 *            the parameter type
	 * @return the Symbian type
	 */
	public static String mapArrayTypeToSymbianType(String type) {
		String basic = mapBasicTypeToSymbianType(type);
		String retval = SourceConstants.OST_ARRAY_TYPE_PREFIX + basic
				+ SourceConstants.OST_ARRAY_TYPE_POSTFIX;
		return retval;
	}

	/**
	 * Maps a format specifier into parameter type
	 * 
	 * @param formatSpecifier
	 *            the format specifier
	 * @return the parameter type or null if not recognized. If the type is one
	 *         of the supported TraceParameter types, the string contains the
	 *         integer value of the type
	 * @throws TraceCompilerException
	 *             if format specifier is not valid
	 */
	public static FormatMapping mapFormatToParameterType(String formatSpecifier)
			throws TraceCompilerException {
		String type;
		boolean array = false;
		boolean basic = false;
		boolean extended = false;
		int len = formatSpecifier.length();
		// Extension format is checked first: %{x}
		if (len > 3 && formatSpecifier.charAt(1) == '{' // CodForChk_Dis_Magic
				&& formatSpecifier.charAt(len - 1) == '}') {
			// Extension format can be an array: %{x[]}
			// In that case it could also be a basic type
			if (len > 3 + ARRAY_TAG.length() // CodForChk_Dis_Magic
					&& formatSpecifier.charAt(len - 3) == '[' // CodForChk_Dis_Magic
					&& formatSpecifier.charAt(len - 2) == ']') { // CodForChk_Dis_Magic
				type = formatSpecifier.substring(2, len - 1 // CodForChk_Dis_Magic
						- ARRAY_TAG.length());
				array = true;
				if (isStringType(type)) {
					StringErrorParameters param = new StringErrorParameters();
					param.string = type;
					throw new TraceCompilerException(
							TraceCompilerErrorCode.PARAMETER_FORMAT_NOT_SUPPORTED_IN_ARRAY,
							param, null);
				}
			} else {
				type = formatSpecifier.substring(2, len - 1); // CodForChk_Dis_Magic
			}
			extended = !isBasicType(type);
		} else {
			basic = true;
			type = formatSpecifier;
		}
				
		if (basic) {
			type = mapBasicFormatToType(formatSpecifier);
		}
		if (type == null) {
			StringErrorParameters params = new StringErrorParameters();
			params.string = formatSpecifier;
			throw new TraceCompilerException(
					TraceCompilerErrorCode.PARAMETER_FORMAT_NOT_SUPPORTED,
					params, null);
		}
		FormatMapping retval = new FormatMapping(type);
		retval.isArray = array;
		if (extended) {
			// In case of extended types, a constant table can still be
			// represented with normal trace macros.
			TraceConstantTable table = TraceCompilerEngineGlobals.getTraceModel()
					.findConstantTableByName(retval.type);
			if (table != null) {
				if (!array && isSimpleType(table.getType())) {
					retval.isSimple = true;
				}
			} else {
				// Extended type must be found from the property file
				StringErrorParameters params = new StringErrorParameters();
				params.string = formatSpecifier;
				throw new TraceCompilerException(
						TraceCompilerErrorCode.PARAMETER_FORMAT_NOT_SUPPORTED,
						params, null);
			}
		} else if (!retval.isArray) {
			retval.isSimple = isSimpleType(type);
		}
				
		return retval;
	}

	/**
	 * Maps basic format specifies to parameter type
	 * 
	 * @param formatSpecifier
	 *            the format specifies
	 * @return the type
	 * @throws TraceCompilerException
	 *             if mapping cannot be done
	 */
	private static String mapBasicFormatToType(String formatSpecifier)
			throws TraceCompilerException { // CodForChk_Dis_ComplexFunc
		String type;
		int paramLength = SourceUtils
				.mapFormatToParameterLength(formatSpecifier);
		char formatChar = formatSpecifier.charAt(formatSpecifier.length() - 1);
		switch (formatChar) {
		case 'd':
			type = SourceUtils.mapSignedToParameterType(paramLength);
			break;
		case 'x':
		case 'X':
			type = SourceUtils.mapHexToParameterType(paramLength);
			break;
		case 'u':
			type = SourceUtils.mapUnsignedToParameterType(paramLength);
			break;
		case 'o':
			type = SourceUtils.mapOctalToParameterType(paramLength);
			break;
		case 's':
			type = TraceParameter.ASCII;
			break;
		case 'S': // Symbian extension
			type = TraceParameter.UNICODE;
			break;
		case 'c':
			type = TraceParameter.SDEC8;
			break;
		case 'p':
			type = TraceParameter.POINTER;
			break;
		case 'f':
		case 'F':
			type = TraceParameter.FLOAT_FIX;
			break;
		case 'e':
		case 'E':
			type = TraceParameter.FLOAT_EXP;
			break;
		case 'g':
		case 'G':
			type = TraceParameter.FLOAT_OPT;
			break;
		default:
			type = null;
		}

		return type;
	}

	/**
	 * Maps a parameter type to format string
	 * 
	 * @param parameter
	 *            the parameter
	 * @return the format string
	 */
	public static String mapParameterTypeToFormat(TraceParameter parameter) {
		String tag;
		if (parameter.getExtension(ArrayParameterRule.class) != null) {
			tag = mapArrayTypeToFormat(parameter.getType());
		} else {
			tag = mapNormalTypeToFormat(parameter.getType());
		}
		return tag;
	}

	/**
	 * Maps an array type to basic type
	 * 
	 * @param arrayType
	 *            the array type
	 * @return the basic type or null if original type is not array type
	 */
	public static String mapArrayTypeToBasicType(String arrayType) {
		String retval;
		if (arrayType.endsWith(ARRAY_TAG)) {
			retval = arrayType.substring(0, arrayType.length()
					- ARRAY_TAG.length());
		} else {
			retval = null;
		}
		return retval;
	}

	/**
	 * Parses a numeric value from source
	 * 
	 * @param number
	 *            the number as string
	 * @return the value
	 */
	public static int parseNumberFromSource(String number) {
		int ret;
		String low = number.toLowerCase();
		int radix;
		if (low.startsWith(SourceConstants.HEX_PREFIX)) {
			radix = 16; // CodForChk_Dis_Magic
			low = low.substring(SourceConstants.HEX_PREFIX.length());
		} else if (low.startsWith(SourceConstants.OCTAL_PREFIX)
				&& low.length() > SourceConstants.OCTAL_PREFIX.length()
				&& Character.isDigit(low.charAt(SourceConstants.OCTAL_PREFIX
						.length()))) {
			radix = 8; // CodForChk_Dis_Magic
			low = low.substring(SourceConstants.OCTAL_PREFIX.length());
		} else {
			radix = 10; // CodForChk_Dis_Magic
		}
		if (low.endsWith(SourceConstants.I64_POSTFIX)) {
			low = low.substring(0, low.length()
					- SourceConstants.I64_POSTFIX.length());
		}
		if (low.length() > 0) {
			// Removes U / L characters from the end of value
			int index = low.length() - 1;
			boolean complete = false;
			do {
				char c = low.charAt(index);
				if (c == 'u' || c == 'l') {
					index--;
				} else {
					complete = true;
				}
			} while (!complete && index >= 0);
			if (index < low.length() - 1 && index > 0) {
				low = low.substring(0, index + 1);
			}
			ret = Integer.parseInt(low, radix);
		} else {
			ret = 0;
		}
		return ret;
	}

	/**
	 * Calculates the size of parameter
	 * 
	 * @param parameter
	 *            the parameter
	 * @return the parameter size or 0 if the size is not known at compile time
	 */
	public static int mapParameterTypeToSize(TraceParameter parameter) {
		int retval;
		ArrayParameterRule rule = parameter
				.getExtension(ArrayParameterRule.class);
		if (rule != null) {
			// Array parameters are dynamic
			retval = 0;
		} else {
			String type = parameter.getType();
			TraceConstantTable table = parameter.getModel()
					.findConstantTableByName(type);
			if (table != null) {
				type = table.getType();
			}
			retval = mapParameterTypeToSize(type);
		}
		return retval;
	}

	/**
	 * Calculates the size of parameter type
	 * 
	 * @param type
	 *            the parameter type
	 * @return the parameter size or 0 if size is not known at compile time
	 */
	public static int mapParameterTypeToSize(String type) {
		int retval;
		if (type.equals(TraceParameter.HEX32)
				|| type.equals(TraceParameter.UDEC32)
				|| type.equals(TraceParameter.SDEC32)
				|| type.equals(TraceParameter.OCT32)
				|| type.equals(TraceParameter.POINTER)) {
			retval = 4; // CodForChk_Dis_Magic
		} else if (type.equals(TraceParameter.HEX16)
				|| type.equals(TraceParameter.UDEC16)
				|| type.equals(TraceParameter.SDEC16)
				|| type.equals(TraceParameter.OCT16)) {
			retval = 2; // CodForChk_Dis_Magic
		} else if (type.equals(TraceParameter.HEX8)
				|| type.equals(TraceParameter.UDEC8)
				|| type.equals(TraceParameter.SDEC8)
				|| type.equals(TraceParameter.OCT8)) {
			retval = 1;
		} else if (type.equals(TraceParameter.HEX64)
				|| type.equals(TraceParameter.UDEC64)
				|| type.equals(TraceParameter.SDEC64)
				|| type.equals(TraceParameter.OCT64)
				|| type.equals(TraceParameter.FLOAT_EXP)
				|| type.equals(TraceParameter.FLOAT_FIX)
				|| type.equals(TraceParameter.FLOAT_OPT)) {
			retval = 8; // CodForChk_Dis_Magic
		} else {
			retval = 0;
		}
		return retval;
	}

	/**
	 * Removes printf formatting from trace text
	 * 
	 * @param text
	 *            the text to be converted
	 * @return the new text
	 */
	public static String removePrintfFormatting(String text) {
		Matcher matcher = traceTextPattern.matcher(text);
		return matcher.replaceAll(""); //$NON-NLS-1$
	}

	/**
	 * Converts the given type to unsigned type
	 * 
	 * @param type
	 *            the type
	 * @return unsigned type
	 */
	private static String convertToUnsigned(String type) {
		if (type.equals(TraceParameter.SDEC32)) {
			type = TraceParameter.UDEC32;
		} else if (type.equals(TraceParameter.SDEC16)) {
			type = TraceParameter.UDEC16;
		} else if (type.equals(TraceParameter.SDEC8)) {
			type = TraceParameter.UDEC8;
		} else if (type.equals(TraceParameter.SDEC64)) {
			type = TraceParameter.UDEC64;
		}
		return type;
	}

	/**
	 * Maps a normal parameter type for format character
	 * 
	 * @param type
	 *            the parameter type
	 * @return the format character
	 */
	public static String mapNormalTypeToFormat(String type) { // CodForChk_Dis_ComplexFunc
		String tag;
		if (type.equals(TraceParameter.SDEC32)) {
			tag = "%d"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.POINTER)) {
			tag = "%p"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.HEX32)) {
			tag = "%x"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.UDEC32)) {
			tag = "%u"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.OCT32)) {
			tag = "%o"; //$NON-NLS-1$			
		} else if (type.equals(TraceParameter.SDEC16)) {
			tag = "%hd"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.HEX16)) {
			tag = "%hx"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.UDEC16)) {
			tag = "%hu"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.OCT16)) {
			tag = "%ho"; //$NON-NLS-1$			
		} else if (type.equals(TraceParameter.SDEC8)) {
			tag = "%hhd"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.HEX8)) {
			tag = "%hhx"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.UDEC8)) {
			tag = "%hhu"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.OCT8)) {
			tag = "%hho"; //$NON-NLS-1$			
		} else if (type.equals(TraceParameter.SDEC64)) {
			tag = "%Ld"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.HEX64)) {
			tag = "%Lx"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.UDEC64)) {
			tag = "%Lu"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.OCT64)) {
			tag = "%Lo"; //$NON-NLS-1$			
		} else if (type.equals(TraceParameter.ASCII)) {
			tag = "%s"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.UNICODE)) {
			tag = "%S"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.FLOAT_FIX)) {
			tag = "%f"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.FLOAT_EXP)) {
			tag = "%e"; //$NON-NLS-1$
		} else if (type.equals(TraceParameter.FLOAT_OPT)) {
			tag = "%g"; //$NON-NLS-1$
		} else {
			tag = "%{" //$NON-NLS-1$
					+ type + "}"; //$NON-NLS-1$
		}
		return tag;
	}

	/**
	 * Maps an array parameter type to format string
	 * 
	 * @param type
	 *            the parameter type
	 * @return the format string
	 */
	public static String mapArrayTypeToFormat(String type) {
		String tag = "%{" //$NON-NLS-1$
				+ type + "[]}"; //$NON-NLS-1$
		return tag;
	}

	/**
	 * Maps format specifier to parameter length
	 * 
	 * @param formatSpecifier
	 *            the specifier
	 * @return the length
	 * @throws TraceCompilerException
	 *             if length is not valid
	 */
	public static int mapFormatToParameterLength(String formatSpecifier)
			throws TraceCompilerException {
		Matcher matcher = SourceUtils.lengthPattern.matcher(formatSpecifier);
		int paramLength = 0;
		if (matcher.find()) {
			String length = matcher.group();

			if (length.length() == 2) { // CodForChk_Dis_Magic
				if (length.charAt(0) == 'h' && length.charAt(1) == 'h') {
					paramLength = SourceConstants.BYTE_SIZE;
				} else if (length.charAt(0) == 'l' && length.charAt(1) == 'l') {
					paramLength = SourceConstants.LONG_SIZE;
				} else {
					StringErrorParameters params = new StringErrorParameters();
					params.string = formatSpecifier;
					throw new TraceCompilerException(
							TraceCompilerErrorCode.PARAMETER_FORMAT_NOT_SUPPORTED,
							params, null);
				}
			} else if (length.length() == 1) {
				switch (length.charAt(0)) {
				case 'h':
					paramLength = SourceConstants.SHORT_SIZE;
					break;
				case 'l':
					paramLength = SourceConstants.INT_SIZE;
					break;
				case 'L':
					paramLength = SourceConstants.LONG_SIZE;
					break;
				default:
					StringErrorParameters params = new StringErrorParameters();
					params.string = formatSpecifier;
					throw new TraceCompilerException(
							TraceCompilerErrorCode.PARAMETER_FORMAT_NOT_SUPPORTED,
							params, null);
				}
			} else {
				throw new TraceCompilerException(
						TraceCompilerErrorCode.INVALID_TRACE_TEXT_FORMAT, null,
						formatSpecifier);
			}
		}

		return paramLength;
	}

	/**
	 * Maps signed parameter length to type
	 * 
	 * @param paramLength
	 *            the length
	 * @return the type
	 */
	private static String mapSignedToParameterType(int paramLength) {
		String retval;
		if (paramLength == SourceConstants.BYTE_SIZE) {
			retval = TraceParameter.SDEC8;
		} else if (paramLength == SourceConstants.SHORT_SIZE) {
			retval = TraceParameter.SDEC16;
		} else if (paramLength == SourceConstants.LONG_SIZE) {
			retval = TraceParameter.SDEC64;
		} else {
			retval = TraceParameter.SDEC32;
		}
		return retval;
	}

	/**
	 * Maps unsigned parameter length to type
	 * 
	 * @param paramLength
	 *            the length
	 * @return the type
	 */
	public static String mapUnsignedToParameterType(int paramLength) {
		String retval;
		if (paramLength == SourceConstants.BYTE_SIZE) {
			retval = TraceParameter.UDEC8;
		} else if (paramLength == SourceConstants.SHORT_SIZE) {
			retval = TraceParameter.UDEC16;
		} else if (paramLength == SourceConstants.LONG_SIZE) {
			retval = TraceParameter.UDEC64;
		} else {
			retval = TraceParameter.UDEC32;
		}
		return retval;
	}

	/**
	 * Maps hex parameter length to type
	 * 
	 * @param paramLength
	 *            the length
	 * @return the type
	 */
	public static String mapHexToParameterType(int paramLength) {
		String retval;
		if (paramLength == SourceConstants.BYTE_SIZE) {
			retval = TraceParameter.HEX8;
		} else if (paramLength == SourceConstants.SHORT_SIZE) {
			retval = TraceParameter.HEX16;
		} else if (paramLength == SourceConstants.LONG_SIZE) {
			retval = TraceParameter.HEX64;
		} else {
			retval = TraceParameter.HEX32;
		}
		return retval;
	}

	/**
	 * Maps octal parameter length to type
	 * 
	 * @param paramLength
	 *            the length
	 * @return the type
	 */
	public static String mapOctalToParameterType(int paramLength) {
		String retval;
		if (paramLength == SourceConstants.BYTE_SIZE) {
			retval = TraceParameter.OCT8;
		} else if (paramLength == SourceConstants.SHORT_SIZE) {
			retval = TraceParameter.OCT16;
		} else if (paramLength == SourceConstants.LONG_SIZE) {
			retval = TraceParameter.OCT64;
		} else {
			retval = TraceParameter.OCT32;
		}

		return retval;
	}

	/**
	 * Checks if the parameter can be represented with default trace macros
	 * 
	 * @param parameter
	 *            the parameter
	 * @return true if parameter can be represented with default trace macros
	 */
	public static boolean isSimpleType(TraceParameter parameter) {
		boolean retval;
		if (parameter.getExtension(ArrayParameterRule.class) != null) {
			// Arrays are always complex types
			retval = false;
		} else {
			String type = parameter.getType();
			TraceConstantTable table = parameter.getModel()
					.findConstantTableByName(type);
			if (table != null) {
				type = table.getType();
			}
			retval = isSimpleType(type);
		}
		return retval;
	}

	/**
	 * Simple type is 32-bit integer
	 * 
	 * @param type
	 *            the type
	 * @return true if simple, false if not
	 */
	private static boolean isSimpleType(String type) {
		return type.equals(TraceParameter.SDEC32)
				|| type.equals(TraceParameter.UDEC32)
				|| type.equals(TraceParameter.OCT32)
				|| type.equals(TraceParameter.HEX32);
	}

	/**
	 * String type is either ascii or unicode
	 * 
	 * @param type
	 *            the type
	 * @return true if string, false if not
	 */
	private static boolean isStringType(String type) {
		return type.equals(TraceParameter.ASCII)
				|| type.equals(TraceParameter.UNICODE);
	}

	/**
	 * Basic type is any of the built-in TraceParameter types
	 * 
	 * @param type
	 *            the type
	 * @return true if basic, false if not
	 */
	private static boolean isBasicType(String type) {
		return isSimpleType(type) || type.equals(TraceParameter.SDEC8)
				|| type.equals(TraceParameter.SDEC16)
				|| type.equals(TraceParameter.UDEC8)
				|| type.equals(TraceParameter.UDEC16)
				|| type.equals(TraceParameter.OCT16)
				|| type.equals(TraceParameter.HEX8)
				|| type.equals(TraceParameter.HEX16)
				|| type.equals(TraceParameter.SDEC64)
				|| type.equals(TraceParameter.UDEC64)
				|| type.equals(TraceParameter.OCT64)
				|| type.equals(TraceParameter.HEX64)
				|| type.equals(TraceParameter.ASCII)
				|| type.equals(TraceParameter.UNICODE)
				|| type.equals(TraceParameter.FLOAT_EXP)
				|| type.equals(TraceParameter.FLOAT_FIX)
				|| type.equals(TraceParameter.FLOAT_OPT)
				|| type.equals(TraceParameter.POINTER);
	}

	/**
	 * Checks if parameter size is dynamic
	 * 
	 * @param parameter
	 *            the parameter to be checked
	 * @return true if dynamic size
	 */
	public static boolean isParameterSizeDynamic(TraceParameter parameter) {
		String type = parameter.getType();
		ArrayParameterRule rule = parameter
				.getExtension(ArrayParameterRule.class);
		return rule != null || type.equals(TraceParameter.ASCII)
				|| type.equals(TraceParameter.UNICODE);
	}

	/**
	 * Checks if alignment is needed
	 * 
	 * @param type
	 *            the parameter type
	 * @return true if alignment is needed
	 */
	public static boolean isParameterAlignementNeeded(String type) {
		int size = SourceUtils.mapParameterTypeToSize(type);
		boolean retval = false;
		if (size == 1 || size == 2) { // CodForChk_Dis_Magic
			// 8 and 16-bit parameters need alignment
			retval = true;
		} else if (isStringType(type)) {
			retval = true;
		}
		return retval;
	}

}
