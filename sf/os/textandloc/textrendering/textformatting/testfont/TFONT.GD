#include "tfont13.gdf"
#include "tfont18.gdf"
#include "tfont41.gdf"


Typeface FormTestFont
    Name "FormTestFont" Proportional
    FontBitmaps
        tfont13
        tfont18
        tfont41
        tfont13 WidthFactor 2 HeightFactor 2
        tfont18 WidthFactor 2 HeightFactor 2
        tfont41 WidthFactor 2 HeightFactor 2
    EndFontBitmaps
EndTypeface

FontStoreFile
    CollectionUid 268435510
    Typefaces
        FormTestFont
    EndTypefaces
EndFontStoreFile
