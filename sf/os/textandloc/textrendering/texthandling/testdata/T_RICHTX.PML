<G>
<P>A single phrase.
<P KeepTogether KeepWithNext StartNewPage WidowOrphan LeftMargin=40 RightMargin=40 Indent=40 
Alignment=Center LineSpacing=400 LineSpacingControl=Exactly SpaceBefore=40 SpaceAfter=40 BorderMargin=40>
<C Italic>1
<C Bold>2
<C Underline>3
<C Strikethrough>4
<C FontHeight=300>5
<C PrintPos=SuperScript>6
<C Color=45>7
<C Language=2>8
<C TypefaceName=Arial>9
<C TypefaceFlags=1>0

<P> This pml file tests the overloaded SenseChars() and SenseParaFormatL() methods. Checking that
the 'valuesChangedAcrossSelectionMask' is operating correctly.
<P> The first paragraph is 16 characters long including the paragraph delimiter and consists of a
 single phrase, so there should be no undetermined values.
<P> The second paragraph consists of <C Bold Underline>10<C !Bold !Underline> characters including the paragraph delimiter 
which has multiple phrases, between them altering every character format attribute.  (The same applies
 for paragraph formatting.
