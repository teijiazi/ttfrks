#include<tableDSIG.h>

void SignatureRecord::read(QDataStream&dt){
    dt>>format>>length>>signatureBlockOffset;
}

SignatureBlockFormat1::SignatureBlockFormat1():pSsignature(nullptr){}
SignatureBlockFormat1::~SignatureBlockFormat1(){
    if(pSsignature){
        delete [] pSsignature;
    }
}


DSIGTable::DSIGTable(QObject *parent):TableTTF(parent),
    version(1),numSignatures(0),flags(0),
    pSignatureRecord(nullptr),sbf1(nullptr),fzData(nullptr){}
DSIGTable::~DSIGTable(){
    if(pSignatureRecord){delete [] pSignatureRecord;}
    if(sbf1){
        delete[]sbf1;
    }
    if(fzData){
        delete[]fzData;
    }
}
void DSIGTable::read(QDataStream& dt, uint32 offset, uint32 sigLen){
    dt.device()->seek(offset);
    fzLen=sigLen;
    if(sigLen>0){
        fzData=new char[fzLen];
        dt.readRawData((char*)fzData,fzLen);
    }
    //
    dt.device()->seek(offset);
    dt>>version>>numSignatures>>flags;
    if(numSignatures>0){
        pSignatureRecord=new SignatureRecord[numSignatures];
        for(int i=0;i<numSignatures;i++){
            pSignatureRecord[i].read(dt);
        }
    }
};
void DSIGTable::save(QDataStream& dt){
    dt<<version<<numSignatures<<flags;
}

void DSIGTable::jisuan(){
    version=1;
    numSignatures=0;
    flags=0;
}

void DSIGTable::show()
{
    datashow(QString("version:%1\t\t\r\n"
                     "numSignatures:%2\t\t\r\n"
                     "flags:%3\t\r\n")
             .arg(version).arg(numSignatures).arg(flags));
    if(numSignatures>0){
        for(int i=0;i<numSignatures;i++){
            datashow(QString("format:%1\t\t\r\n"
                             "length:%2\t\t\r\n"
                             "signatureBlockOffset:%3\t\r\n")
                     .arg(pSignatureRecord[i].format)
                     .arg(pSignatureRecord[i].length).
                     arg(pSignatureRecord[i].signatureBlockOffset));
        }
    }
}
