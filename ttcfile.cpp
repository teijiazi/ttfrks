#include "ttcfile.h"

void TTCHeader::getNameFromTTC(QDataStream &dt,u32vector& tableDirectoryOffsets)
{
    ttcTag="ttcf";
    dt>>majorVersion>>minorVersion>>numFonts;
    tableDirectoryOffsets.fill(0,numFonts);
    for(uint i=0;i<numFonts;i++){
        dt>>tableDirectoryOffsets[i];
    }
    if(majorVersion==2){
        dt>>dsigTag>>dsigLength>>dsigOffset;
    }
}
