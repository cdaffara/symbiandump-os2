<G>
<P Alignment=center KeepTogether KeepWithNext>
<C Bold Underline FontHeight=400>Overview
<P Alignment=left SpaceAfter=250 !KeepWithNext>
<C FontHeight=200 !Bold !Underline>The EditableText class hierarchy provides
 instantiable classes for editing and storing text.  The choice of which 
 class to use depends on your requirements of text editing.
<P SpaceBefore=250 Alignment=Left Indent=1440 KeepWithNext>
<C Bold StrikeThrough FontHeight=350>Usage
<P Indent=0>
<C !Bold !Strikethrough FontHeight=200>
Two instantiable editable text classes are supplied:
<P Indent=700 SpaceBefore=240 SpaceAfter=240>
GlobalText
<P>
RichText
<P Indent=0>
and each case, there is a choice of physical implementation:
<P Indent=700>Flat storage
<P>Segemented storage.
<P Indent=0>Flat storage is most efficient when there is little text to be stored, or in the case where little or no editing will take place on the stored text.
<P>Segmented storage is most efficient where large quantities of text are to be stored, or where the text has a large dynamic content.
<P>The rich text class supports:
<P Indent=700>paragraph styles
<P>character styles
<P>explicit paragraph & character formatting
<P>embedded pictures etc... (See the spec).
<P Indent=0>In contrast, global text applies a<C Bold Italic> single <C !Bold !Italic>paragraph/character style to the entire contents of the document.
<P>Global text is best used for edit boxes, whereas rich text applications include the word processor and agenda entries, to name but two.
<P Alignment=right SpaceBefore=1440>
<C Bold Underline FontHeight=450>CEditableText Methods<C !Bold !Underline>
<P Alignment=justified SpaceBefore=240 SpaceAfter=300>
<C Bold>ConstructL                                                                                                Phase2 construction<C !Bold>
<P Alignment=left KeepWithNext KeepTogether>
<C Bold>void ConstructL(TDocumentStorage aStorage);<C !Bold>
<P !KeepWithNext !KeepTogether>
All calls to a document constructor must be followed by an explicit call to the<C Underline> ConstructL <C !Underline>method.  This method completes the 2 phase object construction favoured by Epoc.  The parameter<C Italic Underline> aStorage <C !Ita
lic !Underline>defines which implementation to use.  The type<C Underline> TDocumentStorage <C !Underline>is defined in<C Underline> EDITTEXT.H <C !Underline>as follows:
<P Alignment=center>
<C Bold>enum TDocumentStorage {EFlat,ESegmented};<C !Bold>
<P Alignment=justified>
This method may leave.  A complete example of document construction is provided below:
<P Indent=1460>
<C Bold>CEditableText* document=new(ELeave) CGlobalText;
<P>
document->ConstructL(ESegmented);  // Leaves if construction fails.<C !Bold>
<P Indent=0 Alignment=justified>
A mandatory part of the ConstructL method inserts an end-of-document character into the document.  This character is not included in the DocumentLength method.
