#ifndef TTCFILE_H
#define TTCFILE_H

#include <QObject>
#include "types.h"

class TTCHeader{
public:
    Tag	ttcTag;//	Font Collection ID string: 'ttcf' (used for fonts with CFF or CFF2 outlines as well as TrueType outlines)
    uint16	majorVersion;//	Major version of the TTC Header, = 1.
    uint16	minorVersion;//	Minor version of the TTC Header, = 0.
    uint32	numFonts;//	Number of fonts in TTC
    //Offset32	tableDirectoryOffsets[numFonts]	Array of offsets to the TableDirectory for each font from the beginning of the file
    //u32vector tableDirectoryOffsets;
    uint32	dsigTag;//	Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature)
    uint32	dsigLength;//	The length (in bytes) of the DSIG table (null if no signature)
    uint32	dsigOffset;//	The offset (in bytes) of the DSIG table from the beginning of the TTC file (null if no signature)
    void getNameFromTTC(QDataStream &dt, u32vector &tableDirectoryOffsets);
};

#endif // TTCFILE_H
