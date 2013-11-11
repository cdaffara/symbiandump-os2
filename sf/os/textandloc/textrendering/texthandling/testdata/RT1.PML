<G>
<P>
This is the first piece of "PML" code used to test the Rich Text
component of the Editable Text hierarchy.
<P>
First off, each paragraph is specified with a different paragraph
format attribute.
<P LEFTMARGIN=1440>
The idea is that using this tool we can quickly and easily build up
large complex rich text documents with lots of complex formatting. 
This paragraph, for example, has its left margin set to 1".
<P RIGHTMARGIN=1440>
...whilst this paragraph has a right margin of 1" also.  However, we
are currently in the process of making each paragraph inherit
attributes from the preceeding one, so this paragraph may also have
the one inch left margin, though currently it may not.
<P INDENT=2880>
This next paragraph should be long enough to word wrap to enable the
viewer to observe the attribute set here.  This paragraph has a first
line indent set to 2".  The appearance should be that the first line
is indented further than the following lines, a sort of hanging
indent if you like.
<P LINESPACING=720>
Now we move onto line spacing.  The line spacing for this paragraph
has been set to one half inch.  (This is stores in its twips platinum
bar value of 720 twips.  One twip is a twentieth of a point, and
there are seventy-two points to the inch.  Therefore, 20*72=1440.
<P LINESPACINGCONTROL=EXACTLY LINESPACING=360>
This paragraph sets the line spacing control to 'exactly' 360 twips,
or one quarter of an inch. Here's some rubbish to fill a couple of
lines---->jfdjkf kfj kl jlsdkfjdlsfjwoifj iojf lkfj ldkfjdslkfj sl;kj
fklsjlfjk fjkj flkdfj slfk klf jlskjlkdj flkdjflskjf lk fjlkdfj
lskfslk lksd fjkls fjklsdj flsfjskdfldsj fksdj fklj j fls fs ks lfks
jlskd jflsdkfjlsdkfjklsdjfklsdj fsldfjlsfjslfj lsfjlsj flsdfj
lsfjlsdfjlsdjflsdjflsdfjlsdkfjsldkfjlsdjflsdkjflsdkjflksdjflsdkfjlsdfj
lsdsdlk l.  Hope this worked !!!!!
<P KEEPTOGETHER KEEPWITHNEXT>
This paragraph has the keep together flag set, and the keep with next
flag also set.
<P STARTNEWPAGE WIDOWORPHAN>
...whilst this paragraph is formatted to start on a new page and has
widow/orphan control enabled.
<P SPACEBEFORE=123>
And there you have it......
<P SPACEAFTER=3432>
...the last paragraph. THE END.

