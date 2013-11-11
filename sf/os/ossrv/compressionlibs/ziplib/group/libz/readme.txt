The BLD.INF file in this directory builds only the following binaries:

libzcore.dll
libz.dll

In order to build these dll's against 9.1 OS release, the macro SYMBIAN_ZLIB_BUILD91 must be defined.  This can be temporarily defined in epoc32/include/variant/Symbian_OS_v9.1.hrh.  This causes libzcore_build91.mmp and zlib_build91.mmp to be used, which have backslashes instead of forward slashes for any paths, as required by 9.1 release.

Headers are exported to /epoc32/include/stdapis/

