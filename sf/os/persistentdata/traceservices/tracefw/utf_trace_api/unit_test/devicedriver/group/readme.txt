It's not possible to compile these files from here.

How to:
Create a new folder in \\master\cedar\generic\base\e32\drivers\
Copy all *.cpp, *.mm and bld.inf files to this folder

Copy the *.def files to 
\\master\cedar\generic\base\e32\drivers\bwins and
\\master\cedar\generic\base\e32\drivers\eabi respectively

compile the device driver from the newly created folder as follows:
bldmake bldfiles
abld export
abld build
abld freeze
abld library
abld target