1. base_header_test1 declares the StdC++ operator new and includes e32cmn.h
2. base_header_test2 includes e32cmn.h and then declares the StdC++ operator new (Just to make sure that ordering is 
	not a problem)
3. base_header_test3 involves 2 source files. One of them includes the StdC++ header and the other includes e32cmn.h.
	The macro must be defined for each source file and hence both should pick either from e32cmn or the StdC++ new.
4. base_header_test4, base_header_test5 & base_header_test6 are same as base_header_test1, base_header_test2 &
	base_header_test3 resp. except that the former are in SymbianC++ mode while the latter in StdC++ mode.
