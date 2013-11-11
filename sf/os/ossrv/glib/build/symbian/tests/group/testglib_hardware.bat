@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

copy z:\valid-1.gmarkup c:\
copy z:\iochannel-test-infile c:\
copy z:\casemap.txt c:\
copy z:\casefold.txt c:\
copy z:\utf8.txt c:\
copy z:\collate-1.unicode c:\
copy z:\NormalizationTest.txt c:\
copy z:\4096-random-bytes c:\
copy z:\valid-01.xbel c:\

utf8_validate.exe
utf8_pointer.exe
uri_test.exe
unicode_normalize.exe c:\NormalizationTest.txt 10
unicode_encoding.exe
unicode_collate.exe -key c:\collate-1.unicode
unicode_caseconv.exe
type1_test.exe
tree1_test.exe
timeloop_closure.exe
timeloop.exe
threadtests.exe
threadpool_test.exe
thread_test.exe
testingbase64.exe
testing.exe
testgobject.exe
testglib.exe
testgdate.exe
string.exe
strfuncs.exe
spawn_test.exe
slist_test.exe
slice_threadinit.exe
slice_test.exe 1 Gfc~ 512 1023444
slice_concurrent.exe
slice_color.exe 512 2048 2 16
singleton.exe
signal1.exe
shell_test.exe
sequence_test.exe
scannerapi.exe
rem testgdateparser.exe// _ manual test
relation_test.exe
regex_test.exe -noisy
references.exe
rand.exe
queue_test.exe
qsort_test.exe
properties2.exe
properties.exe
printf.exe
patterntest.exe
paramspec_test.exe
override.exe
option_context.exe
onceinit.exe
objects2.exe
objects.exe
node_test.exe
module_test.exe
markup_test.exe c:\valid-1.gmarkup
markup_subparser.exe
markup_escape_test.exe
markup_collect.exe -v
mapping_test.exe
mainloop_test.exe
list_test.exe
keyfile.exe
iochannel_test.exe
ifaceproperties.exe
ifaceinit.exe
ifaceinherit.exe
ifacecheck.exe
hash_test.exe
gvalue_test.exe
gobject_query.exe -s -i -b -r -n froots tree
glib_genmarshal.exe
gio_test.exe
fileutils.exe
file_test.exe
errorcheck_mutex_test.exe lock_locked_mutex()
env_test.exe
dynamictype.exe
dirname_test.exe
defaultiface.exe
date_test.exe
convert_test.exe
completion_test.exe
closures.exe
child_test.exe
checksum_test.exe
bookmarkfile_test.exe c:\valid-01.xbel
bit_test.exe
base64_test.exe
atomic_test.exe
asyncqueue_test.exe
array_test.exe
accumulator.exe

//newly added exes
array1_test.exe
array2_test.exe
app_name.exe
at_exit.exe
base_name.exe
bookmarkfile_test1.exe c:\valid-01.xbel
list_env.exe
majorversion_test.exe
nullify_ptr.exe
test_utils.exe
utf8_string.exe
uri_funcs.exe
unichar_test.exe

//from BC
tutf8.exe 
tnode.exe 
tunichar.exe 
tmisc.exe 
tslist.exe 
ttrash.exe 
ttree.exe 
tmanual.exe 
tgstring.exe 
tscanner.exe 
toption.exe 
tthread.exe 
tasyncqueue.exe 
closure-marshal.exe 
paramspec-test.exe 
asyncqueue-test.exe 
utf8-pointer.exe 
extra_tests.exe  
child-test1.exe 
child-test2.exe 
option-test.exe 
printf-test.exe 
string-test.exe 
strtod-test.exe 
gvalue-test.exe 
byte_array_test.exe 
dir_test.exe 
misc_test.exe 
cache_test.exe 
dataset_test.exe 
keyfile_test.exe 
mem_test.exe 
datalist_test.exe 
hook_test.exe 
log_test.exe 
main_loop_test.exe 
mem_profile_test.exe 
g_test1.exe 
g_test2.exe 
helloworld.exe 

