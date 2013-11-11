About the WBXML Token Dictionaries

WBXML can use tokens to represent tag names, attribute names, attribute
names combined with part or all of the attribute value, and part or all
of an attribute value.

The cXML code needs internal tables that map tokens to strings and strings
to tokens.  To simplify the generation of these tables (dictionaries) a
simple data file giving token-string pairs is processed by a dictionary
creator utility program which outputs the required .h and .c files.

The dictionary creator program source code is in dict_creator.c and it
is a stand-alone program.  To compile it with the MS C compiler use the
command: cl /Zi /W4 /WX dict_creator.c

If you encounter following errors about lib.c can't be found or stdio.h is 
not found in path, you need to set up system var:

INCLUDE: C:\progra~1\micros~3\VC98\Include
LIB: C:\progra~1\micros~3\VC98\lib

To get dict_creator usage info run dict_creator with no arguments.

Files with extension ".dict" are data input files.

Example runs:

dict_creator xhtml_1_0.dict xhtml_1_0_dict.c nw_xhtml_xhtml_1_0_tokens.h
dict_creator wml_1_3.dict wml_1_3_dict.c nw_wml1x_wml_1_3_tokens.h

dict_creator si.dict si_dict.c si_dict.h


You have to get through dict_creator without errors and then you have to
get through the C compiler on the output.  If the output doesn't compile,
it may be because dict_creator did something wrong.  Dict_creator has to
make a few transformations on the input to create C variable names and it
is possible (though we hope never encountered in use) that this produces
output that won't compile.

END
