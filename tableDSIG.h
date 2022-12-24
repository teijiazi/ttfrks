#ifndef TABLEDSIG_H
#define TABLEDSIG_H
#include<types.h>
class SignatureRecord{
public:
    uint32 format;// Format of the signature
    uint32 length;// Length of signature in bytes
    Offset32 signatureBlockOffset;// Offset to the signature block from the beginning of the table
    void read(QDataStream&dt);
};
class SignatureBlockFormat1{
public:
    SignatureBlockFormat1();
    ~SignatureBlockFormat1();
    uint16 reserved1;// Reserved for future use; set to zero.
    uint16 reserved2;// Reserved for future use; set to zero.
    uint32 signatureLength;// Length (in bytes) of the PKCS#7 packet in the signature field.
    uint8 *pSsignature;// PKCS#7 packet len=signatureLength
};
class DSIGTable:public TableTTF{
    //https://docs.microsoft.com/en-us/typography/opentype/spec/dsig
    Q_OBJECT
public:
    DSIGTable(QObject *parent=nullptr);
    ~DSIGTable();
    void read(QDataStream& dt, uint32 offset, uint32 sigLen);
    void save(QDataStream& dt);
    void show();
    //dsig header
    uint32 version;// Version number of the DSIG table (0x00000001)
    uint16 numSignatures;// Number of signatures in the table
    uint16 flags;// permission flags Bit 0: cannot be resigned Bits 1-7: Reserved (Set to 0)
    SignatureRecord* pSignatureRecord;// Array of signature records  len=numSignatures
    SignatureBlockFormat1* sbf1;
    char * fzData;
    uint32 fzLen;
    void jisuan();
};
#endif // TABLEDSIG_H
