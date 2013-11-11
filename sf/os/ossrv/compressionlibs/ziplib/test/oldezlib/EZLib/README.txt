This is an epoc port of zlib (http://www.cdrom.com/pub/infozip/zlib/)

Changes made in getting zlib lib to compile.

General Changes:
------------------

All .c files renamed to .cpp

K&R function defintions are rewritten to conform with ansi C.



ZLib.h:
---------
Since all our files are going to be .cpp files we don't need the extern c stuff so thats been commented out.
At the start of zlib.h, the macros ZEXPORT, ZEXPORTVA are set to nothing.  ZEXTERN is set to IMPORT_C.
At the end of zlib.h ZEXTERN is set to nothing.  This is nasty and hacky but it involes the least
amount of changes to the zlib code.  Anyway it can go if we decide not to export to c interface.
Every exported function has had EXPORT_C appended to its definition.  This affects many files.  Again
if we decide not to export the c interface these changes can be reversed.


ZUtil.h:
-----------
Includes <e32std.h> at the top.
Commented out bit of code which checks moew msvc and messes redefines fdopen as _fdopen.  Lines 139



deflate.h:
------------

Added a STATIC_CAST to first two lines of _tr_tally_dist

delate.cpp:
----------

#included <e32def.h>

Altered C style casts in INSERT_STRING macro to STATIC_CAST and added some more casts

Lines 1041 & 1211 commented out.  They contained trhe name of a macro in a piece of code which was
#defed out of the compliation process.  Unforntately makmake didn't like it.

MSDOS & MACOS? specific stuff has been commented out.  I guess this should be replace with an appropriate
#ifdef but I'm not sure what this should be yet.

Added line 284 
s->status = INIT_STATE;
to fix zlib bug where a memory leak can occur when zalloc fails


infblock.cpp
-------------

This file has been modified to fix a bug in zlib 1.1.3 which can cause a crash when ZALLOC fails.
Essentially what happens is that in certain circumstances s->sub.trees.blens can be deleted twice.
I have altered infblock.cpp with a patch sent to me by jloup@gzip.org which fixes this problem.  I guess
this fix will appear in zlib 1.1.4.

trees.h:
----------
The array sizes specified for _dist_code & _length_code have been removed from the square brackets as
it conflicts with another extern definition which appears elsewhere in the code.

trees.cpp:
------------

#included <e32def.h>

STATIC_CASTS added on lines 594, 607, 679, 730, 225


inftrees.cpp:
--------------
commented out unused formal parameter z in inflate_trees_fixed at line 399.  Seems to be some global
data in here.  Something is going to have to be done about it.


gzio.cpp:
----------
Swapped around includes for zconf.h and stdio.h so we don't get warnings about multiple defintions of 
NULL.
gzprintf uses to much stack for epoc.  Compliation gives a failure to link with _chkstk.  Rather than 
reducing the size of the buffer, I'm going to allocate the buffer on the heap.


zconf.h
-----------
Commented out lines that include unistd.h

Global Variables:
-------------------
Since EPOC cannot support writeable global data some modifications were required to the global variables
in zlib.  Conveniently, however, all of the global variables which were no const did not need to be so
and I believe their non const status was dues to backward compatibility with older compilers.  The 
following global variables were made read only to facilitate a marm build.

inftrees.cpp:
-------------

fixed_td
fixed_tl
inflate_mask

trees.cpp:
----------

static_bl_desc
static_d_desc
static_l_desc
table	
z_verbose	


deflate.cpp:
-------------
delateInit2::my_version


Changes made to remove dependency on the c standard library.
-------------------------------------------------------------

zutil.h 
----------
the following macros are redefined to make use of the Epoc32 Mem class, zmemcpy, zmemcmp,
zmemzero.

zutil.cpp
----------
zcalloc and zfree now call memory functions defined in User.





