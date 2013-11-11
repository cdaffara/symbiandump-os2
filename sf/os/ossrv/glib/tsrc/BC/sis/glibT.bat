rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
rem
rem This library is free software; you can redistribute it and/or
rem modify it under the terms of the GNU Lesser General Public
rem License as published by the Free Software Foundation; either
rem version 2 of the License, or (at your option) any later version.
rem
rem This library is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
rem Lesser General Public License for more details.
rem
rem You should have received a copy of the GNU Lesser General Public
rem License along with this library; if not, write to the
rem Free Software Foundation, Inc., 59 Temple Place - Suite 330,
rem Boston, MA 02111-1307, USA.
rem
rem Description:      
rem

del c:\spd_logs\xml\*.xml

md e:\bctest
md e:\bctest\results
md e:\bctest\results\glib

accumulator.exe
array-test.exe
asyncqueue-test.exe
atomic-test.exe
closure-marshal.exe
closures.exe
completion-test.exe
convert-test.exe
date-test.exe
defaultiface.exe
dirname-test.exe
env-test.exe
extra_tests.exe
gio-test.exe
gvalue-test.exe
g_test1.exe
g_test2.exe
hash-test.exe
ifacecheck.exe
ifaceinherit.exe
ifaceinit.exe
ifaceproperties.exe
iochannel-test.exe
keyfile-test.exe
list-test.exe
mainloop-test.exe
markup-escape-test.exe
markup-test.exe
module-test.exe
node-test.exe
objects.exe
objects2.exe
option-test.exe
override.exe
paramspec-test.exe
patterntest.exe
properties.exe
properties2.exe
qsort-test.exe
queue-test.exe
rand-test.exe
references.exe
relation-test.exe
shell-test.exe
signals-multithread.exe
signals-singlethread.exe
slist-test.exe
strfunc-test.exe
string-test.exe
tasyncqueue.exe
testgdate.exe
testgdateparser.exe
testgobject.exe
thread-test.exe
threadpool-test.exe
timeloop-closure.exe
timeloop.exe
tmisc.exe
tnode.exe
toption.exe
tscanner.exe
tslist.exe
tthread.exe
ttrash.exe
ttree.exe
tunichar.exe
tutf8.exe
unichar_test.exe
unicode-collate.exe
unicode-encoding.exe
unicode-normalize.exe
uri-test.exe
utf8-pointer.exe
utf8-validate.exe

copy c:\spd_logs\xml\*.xml e:\bctest\results\glib\