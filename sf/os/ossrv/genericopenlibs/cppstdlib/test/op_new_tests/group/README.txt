These tests validate tool-chain changes that prevent intermixing of Symbian's operator 
new semantics and that of the Standard C++.

The test dll_using_lib_new_macro checks if the build system can detect that there is a refernce to 'new' in the static library (built in StdC++ mode - with target type as lib) which is linked in by a dll (built in non-StdC++ mode). This test is expected to give a build error.

The test dll_using_lib_new_nomacro checks if the build system allows to link a static library(built in non-StdC++ mode) that has a reference to 'new' is linked in from a DLL(also built in non-StdC++ mode). This test is expected to build successfully.

The test dll_using_lib_nonew_macro checks if the build system allows to link a static library(built in StdC++ mode - with targettype as lib) that has no reference to 'new' is linked in from a DLL(also built in non-StdC++ mode). This test is expected to build successfully.

The test dll_using_lib_nonew_nomacro checks if the build system allows to link a static library(built in non-StdC++ mode) that has no reference to 'new' from a DLL(also built in non-StdC++ mode). This test is expected to build successfully.

The test dll_using_stdlib_new checks if the build system can detect that there is a refernce to 'new' in the static library (built in StdC++ mode - with target type as stdlib) which is linked in by a dll (built in non-StdC++ mode). This test is expected to give a build error.

The test dll_using_stdlib_nonew checks if the build system allows to link a static library(built in StdC++ mode - with targettype as stdlib) that has no reference to 'new' is linked in from a DLL(also built in non-StdC++ mode). This test is expected to build successfully.

The test exe_using_lib_new_macro checks if the build system can detect that there is a refernce to 'new' in the static library (built in StdC++ mode - with target type as lib) which is linked in by a exe (built in non-StdC++ mode). This test is expected to give a build error.

The test exe_using_lib_new_nomacro checks if the build system allows to link a static library(built in non-StdC++ mode) that has a reference to 'new' is linked in from a exe(also built in non-StdC++ mode). This test is expected to build successfully.

The test exe_using_lib_nonew_macro checks if the build system allows to link a static library(built in StdC++ mode - with targettype as lib) that has no reference to 'new' is linked in from a exe(also built in non-StdC++ mode). This test is expected to build successfully.

The test exe_using_lib_nonew_nomacro checks if the build system allows to link a static library(built in non-StdC++ mode) that has no reference to 'new' from a exe(also built in non-StdC++ mode). This test is expected to build successfully.

The test exe_using_stdlib_new checks if the build system can detect that there is a refernce to 'new' in the static library (built in StdC++ mode - with target type as stdlib) which is linked in by an exe (built in non-StdC++ mode). This test is expected to give a build error.

The test exe_using_stdlib_nonew checks if the build system allows to link a static library(built in StdC++ mode - with targettype as stdlib) that has no reference to 'new' is linked in from an exe(also built in non-StdC++ mode). This test is expected to build successfully.

The test lib_new_macro builds a static library in StdC++ mode (with targettype as LIB). It has a reference to operator new.

The test lib_new_nomacro builds a static library in non-StdC++ mode (with targettype as LIB). It has a reference to operator new.

The test lib_nonew_macro builds a static library in StdC++ mode (with targettype as LIB). It has no reference to operator new.

The test lib_nonew_nomacro builds a static library in non-StdC++ mode (with targettype as LIB). It has no reference to operator new.

The test stddll_using_lib_new_macro checks if the build system allows to link a static library (built in StdC++ mode - with target type as lib) that has a refernce to 'new' can be linked in by an stddll (built in StdC++ mode).

The test stddll_using_lib_new_nomacro checks if the build system can detect that there is a reference to 'new' in a static library that is built in non-StdC++ mode and it is being linked in from an stddll. This is expected to give a build error.

The test stddll_using_lib_nonew_macro checks if the build system allows to have a static library that has no reference to operator new, to be linked in from an stddll.

The test stddll_using_lib_nonew_nomacro checks is the build system allows to have a static library(built in non-StdC++ mode) with no reference to operator new, to be linked in from an stddll.

The test stddll_using_stdlib_new checks is the build system allows to have a static library(built in StdC++ mode) with reference to operator new, to be linked in from an stddll.

The test stddll_using_stdlib_nonew  checks is the build system allows to have a static library(built in StdC++ mode) with no reference to operator new, to be linked in from an stddll.

The test stdexe_using_lib_new_macro checks if the build system allows to link a static library (built in StdC++ mode - with target type as lib) that has a refernce to 'new' can be linked in by an stdexe (built in StdC++ mode).

The test stdexe_using_lib_new_nomacro checks if the build system can detect that there is a reference to 'new' in a static library that is built in non-StdC++ mode and it is being linked in from an stdexe. This is expected to give a build error.

The test stdexe_using_lib_nonew_macro checks if the build system allows to have a static library that has no reference to operator new, to be linked in from an stdexe.

The test stdexe_using_lib_nonew_nomacro checks is the build system allows to have a static library(built in non-StdC++ mode) with no reference to operator new, to be linked in from an stdexe.

The test stdexe_using_stdlib_new checks is the build system allows to have a static library(built in StdC++ mode) with reference to operator new, to be linked in from an stdexe.

The test stdexe_using_stdlib_nonew  checks is the build system allows to have a static library(built in StdC++ mode) with no reference to operator new, to be linked in from an stdexe.

The test stdlib_new builds a static library with target type as stdlib. This library has a reference to operator new.

The test stdlib_nonew builds a static library with target type as stdlib. This library has no reference to operator new.