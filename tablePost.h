#ifndef TABLEPOST_H
#define TABLEPOST_H
#include "types.h"
class postAppend{
public:
    uint16	numGlyphs;//	Number of glyphs (this should be the same as numGlyphs in 'maxp' table).
    //uint16	glyphNameIndex[numGlyphs].	Array of indices into the string data. See below for details.
    //uint8	stringData[variable]	Storage for the string data.
    uint16 *glyphNameIndex;
    uint8 *stringData;
    uint16 fzStrLen;
    void read(QDataStream&dt,uint32 appendLen);
    void save(QDataStream&dt);
};

class POSTTable: public TableTTF{//必须
    Q_OBJECT
public:
    POSTTable(QObject *parent=nullptr);
    ~POSTTable();
    void read(QDataStream& dt, uint32 offset, uint32 postLen);
    void save(QDataStream& dt);
    void show();
    Version16Dot16	version;
    //0x00010000 for version 1.0
    //0x00020000 for version 2.0
    //0x00025000 for version 2.5 (deprecated)
    //0x00030000 for version 3.0
    Fixed	italicAngle;//	Italic angle in counter-clockwise degrees from the vertical. Zero for upright text, negative for text that leans to the right (forward).
    FWORD	underlinePosition;//	This is the suggested distance of the top of the underline from the baseline (negative values indicate below baseline).
    //The PostScript definition of this FontInfo dictionary key (the y coordinate of the center of the stroke) is not used for historical reasons. The value of the PostScript key may be calculated by subtracting half the underlineThickness from the value of this field.
    FWORD	underlineThickness;//	Suggested values for the underline thickness. In general, the underline thickness should match the thickness of the underscore character (U+005F LOW LINE), and should also match the strikeout thickness, which is specified in the OS/2 table.
    uint32	isFixedPitch;//	Set to 0 if the font is proportionally spaced, non-zero if the font is not proportionally spaced (i.e. monospaced).
    uint32	minMemType42;//	Minimum memory usage when an OpenType font is downloaded.
    uint32	maxMemType42;//	Maximum memory usage when an OpenType font is downloaded.
    uint32	minMemType1;//	Minimum memory usage when an OpenType font is downloaded as a Type 1 font.
    uint32	maxMemType1;//	Maximum memory usage when an OpenType font is downloaded as a Type 1 font.
    postAppend *v2append;
    void jisuan(bool isDengkuan, FWORD underlineThick, FWORD underlinePos);
};
#endif // TABLEPOST_H
