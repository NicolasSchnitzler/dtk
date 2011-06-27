/* dtkZipUtils.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Tue Jun 21 12:19:09 2011 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 21 15:19:09 2011 (+0200)
 *           By: Julien Wintz
 *     Update #: 116
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkZip.h"
#include "dtkZipFile.h"
#include "dtkZipUtils.h"

#include "zlib.h"

// /////////////////////////////////////////////////////////////////
// JlCompress
// /////////////////////////////////////////////////////////////////

#include <QtDebug>

bool JlCompress::compressFile(dtkZip* zip, QString fileName, QString fileDest)
{
    if (!zip) return false;
    if (zip->getMode()!=dtkZip::mdCreate &&
        zip->getMode()!=dtkZip::mdAppend &&
        zip->getMode()!=dtkZip::mdAdd) return false;

    QFile inFile;
    inFile.setFileName(fileName);
    if(!inFile.open(QIODevice::ReadOnly)) return false;

    dtkZipFile outFile(zip);
    if(!outFile.open(QIODevice::WriteOnly, dtkZipNewInfo(fileDest, inFile.fileName()))) return false;

    char c;
    while(inFile.getChar(&c)&&outFile.putChar(c));
    if(outFile.getZipError()!=UNZ_OK) return false;

    outFile.close();
    if (outFile.getZipError()!=UNZ_OK) return false;
    inFile.close();

    return true;
}

bool JlCompress::compressSubDir(dtkZip* zip, QString dir, QString origDir, bool recursive)
{
    if (!zip) return false;
    if (zip->getMode()!=dtkZip::mdCreate &&
        zip->getMode()!=dtkZip::mdAppend &&
        zip->getMode()!=dtkZip::mdAdd) return false;

    QDir directory(dir);
    if (!directory.exists()) return false;

    if (recursive) {
        QFileInfoList files = directory.entryInfoList(QDir::AllDirs|QDir::NoDotAndDotDot);
        foreach (QFileInfo file, files) {
            if(!compressSubDir(zip,file.absoluteFilePath(),origDir,recursive)) return false;
        }
    }

    QFileInfoList files = directory.entryInfoList(QDir::Files);
    foreach (QFileInfo file, files) {
        if(!file.isFile()||file.absoluteFilePath()==zip->getZipName()) continue;

        QString filename = file.absoluteFilePath().remove(QDir(origDir).absolutePath());

        if (!compressFile(zip,file.absoluteFilePath(),filename)) return false;
    }

    return true;
}

bool JlCompress::extractFile(dtkZip* zip, QString fileName, QString fileDest)
{
    if (!zip) return false;
    if (zip->getMode()!=dtkZip::mdUnzip) return false;

    zip->setCurrentFile(fileName);
    dtkZipFile inFile(zip);
    if(!inFile.open(QIODevice::ReadOnly) || inFile.getZipError()!=UNZ_OK) return false;

    QDir().mkpath(QFileInfo(fileDest).absolutePath());

    QFile outFile;
    outFile.setFileName(fileDest);
    if(!outFile.open(QIODevice::WriteOnly)) return false;

    char c;
    while(inFile.getChar(&c)) outFile.putChar(c);
    if (inFile.getZipError()!=UNZ_OK) {
        removeFile(QStringList(fileDest));
        return false;
    }

    inFile.close();
    if (inFile.getZipError()!=UNZ_OK) {
        removeFile(QStringList(fileDest));
        return false;
    }
    outFile.close();

    return true;
}

bool JlCompress::removeFile(QStringList listFile)
{
    bool ret = true;

    for (int i=0; i<listFile.count(); i++) {
        ret = ret && QFile::remove(listFile.at(i));
    }

    return ret;
}

bool JlCompress::compressFile(QString fileCompressed, QString file)
{
    dtkZip* zip  = new dtkZip(QFileInfo(fileCompressed).absoluteFilePath());
    QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
    if(!zip->open(dtkZip::mdCreate)) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    if (!compressFile(zip,file,QFileInfo(file).fileName())) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    zip->close();
    if(zip->getZipError()!=0) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }
    delete zip;

    return true;
}

bool JlCompress::compressFiles(QString fileCompressed, QStringList files)
{
    dtkZip* zip  = new dtkZip(QFileInfo(fileCompressed).absoluteFilePath());

    QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
    if(!zip->open(dtkZip::mdCreate)) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    QFileInfo info;
    foreach (QString file, files) {
        info.setFile(file);
        if (!info.exists() || !compressFile(zip,file,info.fileName())) {
            delete zip;
            QFile::remove(fileCompressed);
            return false;
        }
    }

    zip->close();

    if(zip->getZipError()!=0) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    delete zip;

    return true;
}

bool JlCompress::compressDir(QString fileCompressed, QString dir, bool recursive)
{
    dtkZip* zip  = new dtkZip(QFileInfo(fileCompressed).absoluteFilePath());

    QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
    if(!zip->open(dtkZip::mdCreate)) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    if (!compressSubDir(zip,dir,dir,recursive)<0) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    zip->close();

    if(zip->getZipError()!=0) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    delete zip;

    return true;
}

QString JlCompress::extractFile(QString fileCompressed, QString fileName, QString fileDest)
{
    dtkZip* zip  = new dtkZip(QFileInfo(fileCompressed).absoluteFilePath());

    if(!zip->open(dtkZip::mdUnzip)) {
        delete zip;
        return QString();
    }

    if (fileDest.isEmpty()) fileDest = fileName;
    if (!extractFile(zip,fileName,fileDest)) {
        delete zip;
        return QString();
    }

    zip->close();
    if(zip->getZipError()!=0) {
        removeFile(QStringList(fileDest));
        return QString();
    }

    delete zip;

    return QFileInfo(fileDest).absoluteFilePath();
}

QStringList JlCompress::extractFiles(QString fileCompressed, QStringList files, QString dir)
{
    dtkZip* zip  = new dtkZip(QFileInfo(fileCompressed).absoluteFilePath());

    if(!zip->open(dtkZip::mdUnzip)) {
        delete zip;
        return QStringList();
    }

    for (int i=0; i<files.count(); i++) {
        if (!extractFile(zip, files.at(i), QDir(dir).absoluteFilePath(files.at(i)))) {
            delete zip;
            removeFile(files);
            return QStringList();
        }
        files[i] = QDir(dir).absoluteFilePath(files.at(i));
    }

    zip->close();

    if(zip->getZipError()!=0) {
        delete zip;
        removeFile(files);
        return QStringList();
    }

    delete zip;

    return files;
}

QStringList JlCompress::extractDir(QString fileCompressed, QString dir)
{
    dtkZip* zip = new dtkZip(QFileInfo(fileCompressed).absoluteFilePath());

    if(!zip->open(dtkZip::mdUnzip)) {
        delete zip;
        return QStringList();
    }

    QStringList lst = getFileList(fileCompressed);

    for (int i=0; i<lst.count(); i++) {
        if (!extractFile(zip, lst.at(i), QDir(dir).absolutePath() + lst.at(i))) {
            delete zip;
            removeFile(lst);
            return QStringList();
        }
        lst[i] = QDir(dir).absolutePath() + lst.at(i);
    }

    zip->close();
    if(zip->getZipError()!=0) {
        delete zip;
        removeFile(lst);
        return QStringList();
    }

    delete zip;

    return lst;
}

QStringList JlCompress::getFileList(QString fileCompressed)
{
    dtkZip* zip = new dtkZip(QFileInfo(fileCompressed).absoluteFilePath());

    if(!zip->open(dtkZip::mdUnzip)) {
        delete zip;
        return QStringList();
    }

    QStringList lst;
    dtkZipFileInfo info;
    for(bool more=zip->goToFirstFile(); more; more=zip->goToNextFile()) {
        if(!zip->getCurrentFileInfo(&info)) {
            delete zip;
            return QStringList();
        }
        lst << info.name;
    }

    zip->close();
    if(zip->getZipError()!=0) {
        delete zip;
        return QStringList();
    }

    delete zip;

    return lst;
}

// /////////////////////////////////////////////////////////////////
// QIOApi
// /////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib.h"

#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

#ifndef SEEK_END
#define SEEK_END    2
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#endif

voidpf ZCALLBACK qiodevice_open_file_func (
    voidpf opaque,
    voidpf file,
    int mode)
{
    QIODevice *iodevice = reinterpret_cast<QIODevice*>(file);
    if(iodevice->isSequential())
        return NULL;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER)==ZLIB_FILEFUNC_MODE_READ)
        iodevice->open(QIODevice::ReadOnly);
    else
        if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
            iodevice->open(QIODevice::ReadWrite);
        else
            if (mode & ZLIB_FILEFUNC_MODE_CREATE)
                iodevice->open(QIODevice::WriteOnly);

    if(iodevice->isOpen())
        return iodevice;
    else
        return NULL;
}


uLong ZCALLBACK qiodevice_read_file_func (
    voidpf opaque,
    voidpf stream,
    void* buf,
    uLong size)
{
    uLong ret;
    ret = (uLong)((QIODevice*)stream)->read((char*)buf,size);
    return ret;
}


uLong ZCALLBACK qiodevice_write_file_func (
    voidpf opaque,
    voidpf stream,
    const void* buf,
    uLong size)
{
    uLong ret;
    ret = (uLong)((QIODevice*)stream)->write((char*)buf,size);
    return ret;
}

uLong ZCALLBACK qiodevice_tell_file_func (
    voidpf opaque,
    voidpf stream)
{
    uLong ret;
    ret = ((QIODevice*)stream)->pos();
    return ret;
}

int ZCALLBACK qiodevice_seek_file_func (
    voidpf opaque,
    voidpf stream,
    uLong offset,
    int origin)
{
    uLong qiodevice_seek_result=0;
    int ret;
    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR :
        qiodevice_seek_result = ((QIODevice*)stream)->pos() + offset;
        break;
    case ZLIB_FILEFUNC_SEEK_END :
        qiodevice_seek_result = ((QIODevice*)stream)->size() - offset;
        break;
    case ZLIB_FILEFUNC_SEEK_SET :
        qiodevice_seek_result = offset;
        break;
    default: return -1;
    }
    ret = !((QIODevice*)stream)->seek(qiodevice_seek_result);
    return ret;
}

int ZCALLBACK qiodevice_close_file_func (
    voidpf opaque,
    voidpf stream)
{
    ((QIODevice*)stream)->close();
    return 0;
}

int ZCALLBACK qiodevice_error_file_func (
    voidpf opaque,
    voidpf stream)
{
    return !((QIODevice*)stream)->errorString().isEmpty();
}

void fill_qiodevice_filefunc (
    zlib_filefunc_def* pzlib_filefunc_def)
{
    pzlib_filefunc_def->zopen_file = qiodevice_open_file_func;
    pzlib_filefunc_def->zread_file = qiodevice_read_file_func;
    pzlib_filefunc_def->zwrite_file = qiodevice_write_file_func;
    pzlib_filefunc_def->ztell_file = qiodevice_tell_file_func;
    pzlib_filefunc_def->zseek_file = qiodevice_seek_file_func;
    pzlib_filefunc_def->zclose_file = qiodevice_close_file_func;
    pzlib_filefunc_def->zerror_file = qiodevice_error_file_func;
    pzlib_filefunc_def->opaque = NULL;
}

// /////////////////////////////////////////////////////////////////
// quaadler32
// /////////////////////////////////////////////////////////////////

#include "zlib.h"

dtkAdler32::dtkAdler32()
{
    reset();
}

quint32 dtkAdler32::calculate(const QByteArray &data)
{
    return adler32( adler32(0L, Z_NULL, 0), (const Bytef*)data.data(), data.size() );
}

void dtkAdler32::reset()
{
    checksum = adler32(0L, Z_NULL, 0);
}

void dtkAdler32::update(const QByteArray &buf)
{
    checksum = adler32( checksum, (const Bytef*)buf.data(), buf.size() );
}

quint32 dtkAdler32::value()
{
    return checksum;
}

// /////////////////////////////////////////////////////////////////
// quacrc32
// /////////////////////////////////////////////////////////////////

#include "zlib.h"

dtkCrc32::dtkCrc32()
{
    reset();
}

quint32 dtkCrc32::calculate(const QByteArray &data)
{
    return crc32( crc32(0L, Z_NULL, 0), (const Bytef*)data.data(), data.size() );
}

void dtkCrc32::reset()
{
    checksum = crc32(0L, Z_NULL, 0);
}

void dtkCrc32::update(const QByteArray &buf)
{
    checksum = crc32( checksum, (const Bytef*)buf.data(), buf.size() );
}

quint32 dtkCrc32::value()
{
    return checksum;
}

// /////////////////////////////////////////////////////////////////
// zip
// /////////////////////////////////////////////////////////////////

/* zip.c -- IO on .zip files using zlib
   Version 1.01e, February 12th, 2005

   27 Dec 2004 Rolf Kalbermatter
   Modification to zipOpen2 to support globalComment retrieval.

   Copyright (C) 1998-2005 Gilles Vollant

   Read zip.h for more info

   Modified by Sergey A. Tachenov to integrate with Qt.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "zlib.h"

#ifdef STDC
#  include <stddef.h>
#  include <string.h>
#  include <stdlib.h>
#endif
#ifdef NO_ERRNO_H
extern int errno;
#else
#   include <errno.h>
#endif

#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

#ifndef VERSIONMADEBY
# define VERSIONMADEBY   (0x031e) /* best for standard pkware crypt */
#endif

#ifndef Z_BUFSIZE
#define Z_BUFSIZE (16384)
#endif

#ifndef Z_MAXFILENAMEINZIP
#define Z_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
# define ALLOC(size) (malloc(size))
#endif
#ifndef TRYFREE
# define TRYFREE(p) {if (p) free(p);}
#endif

/*
  #define SIZECENTRALDIRITEM (0x2e)
  #define SIZEZIPLOCALHEADER (0x1e)
*/

/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */

#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

#ifndef SEEK_END
#define SEEK_END    2
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#endif

#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
#endif
const char zip_copyright[] =
    " zip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";


#define SIZEDATA_INDATABLOCK (4096-(4*4))

#define LOCALHEADERMAGIC    (0x04034b50)
#define DESCRIPTORHEADERMAGIC    (0x08074b50)
#define CENTRALHEADERMAGIC  (0x02014b50)
#define ENDHEADERMAGIC      (0x06054b50)

#define FLAG_LOCALHEADER_OFFSET (0x06)
#define CRC_LOCALHEADER_OFFSET  (0x0e)

#define SIZECENTRALHEADER (0x2e) /* 46 */

typedef struct linkedlist_datablock_internal_s
{
    struct linkedlist_datablock_internal_s* next_datablock;
    uLong  avail_in_this_block;
    uLong  filled_in_this_block;
    uLong  unused; /* for future use and alignement */
    unsigned char data[SIZEDATA_INDATABLOCK];
} linkedlist_datablock_internal;

typedef struct linkedlist_data_s
{
    linkedlist_datablock_internal* first_block;
    linkedlist_datablock_internal* last_block;
} linkedlist_data;


typedef struct
{
    z_stream stream;            /* zLib stream structure for inflate */
    int  stream_initialised;    /* 1 is stream is initialised */
    uInt pos_in_buffered_data;  /* last written byte in buffered_data */

    uLong pos_local_header;     /* offset of the local header of the file
                                   currenty writing */
    char* central_header;       /* central header data for the current file */
    uLong size_centralheader;   /* size of the central header for cur file */
    uLong flag;                 /* flag of the file currently writing */

    int  method;                /* compression method of file currenty wr.*/
    int  raw;                   /* 1 for directly writing raw data */
    Byte buffered_data[Z_BUFSIZE];/* buffer contain compressed data to be writ*/
    uLong dosDate;
    uLong crc32;
    int  encrypt;
#ifndef NOCRYPT
    unsigned long keys[3];     /* keys defining the pseudo-random sequence */
    const unsigned long* pcrc_32_tab;
    int crypt_header_size;
#endif
} curfile_info;

typedef struct
{
    zlib_filefunc_def z_filefunc;
    voidpf filestream;        /* io structore of the zipfile */
    linkedlist_data central_dir;/* datablock with central dir in construction*/
    int  in_opened_file_inzip;  /* 1 if a file in the zip is currently writ.*/
    curfile_info ci;            /* info on the file curretly writing */

    uLong begin_pos;            /* position of the beginning of the zipfile */
    uLong add_position_when_writting_offset;
    uLong number_entry;
#ifndef NO_ADDFILEINEXISTINGZIP
    char *globalcomment;
#endif
} zip_internal;

local linkedlist_datablock_internal* allocate_new_datablock()
{
    linkedlist_datablock_internal* ldi;
    ldi = (linkedlist_datablock_internal*)
        ALLOC(sizeof(linkedlist_datablock_internal));
    if (ldi!=NULL)
    {
        ldi->next_datablock = NULL ;
        ldi->filled_in_this_block = 0 ;
        ldi->avail_in_this_block = SIZEDATA_INDATABLOCK ;
    }
    return ldi;
}

local void free_datablock(linkedlist_datablock_internal* ldi)
{
    while (ldi!=NULL)
    {
        linkedlist_datablock_internal* ldinext = ldi->next_datablock;
        TRYFREE(ldi);
        ldi = ldinext;
    }
}

local void init_linkedlist(linkedlist_data* ll)
{
    ll->first_block = ll->last_block = NULL;
}

#if 0 // unused
local void free_linkedlist(linkedlist_data* ll)
{
    free_datablock(ll->first_block);
    ll->first_block = ll->last_block = NULL;
}
#endif

local int add_data_in_datablock(
    linkedlist_data* ll,
    const void* buf,
    uLong len)
{
    linkedlist_datablock_internal* ldi;
    const unsigned char* from_copy;

    if (ll==NULL)
        return ZIP_INTERNALERROR;

    if (ll->last_block == NULL)
    {
        ll->first_block = ll->last_block = allocate_new_datablock();
        if (ll->first_block == NULL)
            return ZIP_INTERNALERROR;
    }

    ldi = ll->last_block;
    from_copy = (unsigned char*)buf;

    while (len>0)
    {
        uInt copy_this;
        uInt i;
        unsigned char* to_copy;

        if (ldi->avail_in_this_block==0)
        {
            ldi->next_datablock = allocate_new_datablock();
            if (ldi->next_datablock == NULL)
                return ZIP_INTERNALERROR;
            ldi = ldi->next_datablock ;
            ll->last_block = ldi;
        }

        if (ldi->avail_in_this_block < len)
            copy_this = (uInt)ldi->avail_in_this_block;
        else
            copy_this = (uInt)len;

        to_copy = &(ldi->data[ldi->filled_in_this_block]);

        for (i=0;i<copy_this;i++)
            *(to_copy+i)=*(from_copy+i);

        ldi->filled_in_this_block += copy_this;
        ldi->avail_in_this_block -= copy_this;
        from_copy += copy_this ;
        len -= copy_this;
    }
    return ZIP_OK;
}



/****************************************************************************/

#ifndef NO_ADDFILEINEXISTINGZIP
/* ===========================================================================
   Inputs a long in LSB order to the given file
   nbByte == 1, 2 or 4 (byte, short or long)
*/

local int ziplocal_putValue OF((const zlib_filefunc_def* pzlib_filefunc_def,
                                voidpf filestream, uLong x, int nbByte));
local int ziplocal_putValue (
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream,
    uLong x,
    int nbByte)
{
    unsigned char buf[4];
    int n;
    for (n = 0; n < nbByte; n++)
    {
        buf[n] = (unsigned char)(x & 0xff);
        x >>= 8;
    }
    if (x != 0)
    {     /* data overflow - hack for ZIP64 (X Roche) */
        for (n = 0; n < nbByte; n++)
        {
            buf[n] = 0xff;
        }
    }

    if (ZWRITE(*pzlib_filefunc_def,filestream,buf,nbByte)!=(uLong)nbByte)
        return ZIP_ERRNO;
    else
        return ZIP_OK;
}

local void ziplocal_putValue_inmemory OF((void* dest, uLong x, int nbByte));
local void ziplocal_putValue_inmemory (
    void* dest,
    uLong x,
    int nbByte)
{
    unsigned char* buf=(unsigned char*)dest;
    int n;
    for (n = 0; n < nbByte; n++) {
        buf[n] = (unsigned char)(x & 0xff);
        x >>= 8;
    }

    if (x != 0)
    {     /* data overflow - hack for ZIP64 */
        for (n = 0; n < nbByte; n++)
        {
            buf[n] = 0xff;
        }
    }
}

/****************************************************************************/


local uLong ziplocal_TmzDateToDosDate(
    const tm_zip* ptm,
    uLong dosDate)
{
    uLong year = (uLong)ptm->tm_year;
    if (year>1980)
        year-=1980;
    else if (year>80)
        year-=80;
    return
        (uLong) (((ptm->tm_mday) + (32 * (ptm->tm_mon+1)) + (512 * year)) << 16) |
        ((ptm->tm_sec/2) + (32* ptm->tm_min) + (2048 * (uLong)ptm->tm_hour));
}


/****************************************************************************/

local int ziplocal_getByte OF((
                                  const zlib_filefunc_def* pzlib_filefunc_def,
                                  voidpf filestream,
                                  int *pi));

local int ziplocal_getByte(
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream,
    int *pi)
{
    unsigned char c;
    int err = (int)ZREAD(*pzlib_filefunc_def,filestream,&c,1);
    if (err==1)
    {
        *pi = (int)c;
        return ZIP_OK;
    }
    else
    {
        if (ZERROR(*pzlib_filefunc_def,filestream))
            return ZIP_ERRNO;
        else
            return ZIP_EOF;
    }
}


/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets
*/
local int ziplocal_getShort OF((
                                   const zlib_filefunc_def* pzlib_filefunc_def,
                                   voidpf filestream,
                                   uLong *pX));

local int ziplocal_getShort (
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream,
    uLong *pX)
{
    uLong x ;
    int i;
    int err;

    err = ziplocal_getByte(pzlib_filefunc_def,filestream,&i);
    x = (uLong)i;

    if (err==ZIP_OK)
        err = ziplocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<8;

    if (err==ZIP_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

local int ziplocal_getLong OF((
                                  const zlib_filefunc_def* pzlib_filefunc_def,
                                  voidpf filestream,
                                  uLong *pX));

local int ziplocal_getLong (
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream,
    uLong *pX)
{
    uLong x ;
    int i;
    int err;

    err = ziplocal_getByte(pzlib_filefunc_def,filestream,&i);
    x = (uLong)i;

    if (err==ZIP_OK)
        err = ziplocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<8;

    if (err==ZIP_OK)
        err = ziplocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<16;

    if (err==ZIP_OK)
        err = ziplocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<24;

    if (err==ZIP_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif
/*
  Locate the Central directory of a zipfile (at the end, just before
  the global comment)
*/
local uLong ziplocal_SearchCentralDir OF((
                                             const zlib_filefunc_def* pzlib_filefunc_def,
                                             voidpf filestream));

local uLong ziplocal_SearchCentralDir(
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream)
{
    unsigned char* buf;
    uLong uSizeFile;
    uLong uBackRead;
    uLong uMaxBack=0xffff; /* maximum size of global comment */
    uLong uPosFound=0;

    if (ZSEEK(*pzlib_filefunc_def,filestream,0,ZLIB_FILEFUNC_SEEK_END) != 0)
        return 0;


    uSizeFile = ZTELL(*pzlib_filefunc_def,filestream);

    if (uMaxBack>uSizeFile)
        uMaxBack = uSizeFile;

    buf = (unsigned char*)ALLOC(BUFREADCOMMENT+4);
    if (buf==NULL)
        return 0;

    uBackRead = 4;
    while (uBackRead<uMaxBack)
    {
        uLong uReadSize,uReadPos ;
        int i;
        if (uBackRead+BUFREADCOMMENT>uMaxBack)
            uBackRead = uMaxBack;
        else
            uBackRead+=BUFREADCOMMENT;
        uReadPos = uSizeFile-uBackRead ;

        uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ?
            (BUFREADCOMMENT+4) : (uSizeFile-uReadPos);
        if (ZSEEK(*pzlib_filefunc_def,filestream,uReadPos,ZLIB_FILEFUNC_SEEK_SET)!=0)
            break;

        if (ZREAD(*pzlib_filefunc_def,filestream,buf,uReadSize)!=uReadSize)
            break;

        for (i=(int)uReadSize-3; (i--)>0;)
            if (((*(buf+i))==0x50) && ((*(buf+i+1))==0x4b) &&
                ((*(buf+i+2))==0x05) && ((*(buf+i+3))==0x06))
            {
                uPosFound = uReadPos+i;
                break;
            }

        if (uPosFound!=0)
            break;
    }
    TRYFREE(buf);
    return uPosFound;
}
#endif /* !NO_ADDFILEINEXISTINGZIP*/

/************************************************************/
extern zipFile ZEXPORT zipOpen2 (
    voidpf file,
    int append,
    zipcharpc* globalcomment,
    zlib_filefunc_def* pzlib_filefunc_def)
{
    zip_internal ziinit;
    zip_internal* zi;
    int err=ZIP_OK;


    if (pzlib_filefunc_def==NULL)
        fill_qiodevice_filefunc(&ziinit.z_filefunc);
    else
        ziinit.z_filefunc = *pzlib_filefunc_def;

    ziinit.filestream = (*(ziinit.z_filefunc.zopen_file))
        (ziinit.z_filefunc.opaque,
         file,
         (append == APPEND_STATUS_CREATE) ?
         (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_WRITE | ZLIB_FILEFUNC_MODE_CREATE) :
         (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_WRITE | ZLIB_FILEFUNC_MODE_EXISTING));

    if (ziinit.filestream == NULL)
        return NULL;
    ziinit.begin_pos = ZTELL(ziinit.z_filefunc,ziinit.filestream);
    ziinit.in_opened_file_inzip = 0;
    ziinit.ci.stream_initialised = 0;
    ziinit.number_entry = 0;
    ziinit.add_position_when_writting_offset = 0;
    init_linkedlist(&(ziinit.central_dir));


    zi = (zip_internal*)ALLOC(sizeof(zip_internal));
    if (zi==NULL)
    {
        ZCLOSE(ziinit.z_filefunc,ziinit.filestream);
        return NULL;
    }

    /* now we add file in a zipfile */
#    ifndef NO_ADDFILEINEXISTINGZIP
    ziinit.globalcomment = NULL;
    if (append == APPEND_STATUS_ADDINZIP)
    {
        uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/

        uLong size_central_dir;     /* size of the central directory  */
        uLong offset_central_dir;   /* offset of start of central directory */
        uLong central_pos,uL;

        uLong number_disk;          /* number of the current dist, used for
                                       spaning ZIP, unsupported, always 0*/
        uLong number_disk_with_CD;  /* number the the disk with central dir, used
                                       for spaning ZIP, unsupported, always 0*/
        uLong number_entry;
        uLong number_entry_CD;      /* total number of entries in
                                       the central dir
                                       (same than number_entry on nospan) */
        uLong size_comment;

        central_pos = ziplocal_SearchCentralDir(&ziinit.z_filefunc,ziinit.filestream);
        if (central_pos==0)
            err=ZIP_ERRNO;

        if (ZSEEK(ziinit.z_filefunc, ziinit.filestream,
                  central_pos,ZLIB_FILEFUNC_SEEK_SET)!=0)
            err=ZIP_ERRNO;

        /* the signature, already checked */
        if (ziplocal_getLong(&ziinit.z_filefunc, ziinit.filestream,&uL)!=ZIP_OK)
            err=ZIP_ERRNO;

        /* number of this disk */
        if (ziplocal_getShort(&ziinit.z_filefunc, ziinit.filestream,&number_disk)!=ZIP_OK)
            err=ZIP_ERRNO;

        /* number of the disk with the start of the central directory */
        if (ziplocal_getShort(&ziinit.z_filefunc, ziinit.filestream,&number_disk_with_CD)!=ZIP_OK)
            err=ZIP_ERRNO;

        /* total number of entries in the central dir on this disk */
        if (ziplocal_getShort(&ziinit.z_filefunc, ziinit.filestream,&number_entry)!=ZIP_OK)
            err=ZIP_ERRNO;

        /* total number of entries in the central dir */
        if (ziplocal_getShort(&ziinit.z_filefunc, ziinit.filestream,&number_entry_CD)!=ZIP_OK)
            err=ZIP_ERRNO;

        if ((number_entry_CD!=number_entry) ||
            (number_disk_with_CD!=0) ||
            (number_disk!=0))
            err=ZIP_BADZIPFILE;

        /* size of the central directory */
        if (ziplocal_getLong(&ziinit.z_filefunc, ziinit.filestream,&size_central_dir)!=ZIP_OK)
            err=ZIP_ERRNO;

        /* offset of start of central directory with respect to the
           starting disk number */
        if (ziplocal_getLong(&ziinit.z_filefunc, ziinit.filestream,&offset_central_dir)!=ZIP_OK)
            err=ZIP_ERRNO;

        /* zipfile global comment length */
        if (ziplocal_getShort(&ziinit.z_filefunc, ziinit.filestream,&size_comment)!=ZIP_OK)
            err=ZIP_ERRNO;

        if ((central_pos<offset_central_dir+size_central_dir) &&
            (err==ZIP_OK))
            err=ZIP_BADZIPFILE;

        if (err!=ZIP_OK)
        {
            ZCLOSE(ziinit.z_filefunc, ziinit.filestream);
            return NULL;
        }

        if (size_comment>0)
        {
            ziinit.globalcomment = (char *)ALLOC(size_comment+1);
            if (ziinit.globalcomment)
            {
                size_comment = ZREAD(ziinit.z_filefunc, ziinit.filestream,ziinit.globalcomment,size_comment);
                ziinit.globalcomment[size_comment]=0;
            }
        }

        byte_before_the_zipfile = central_pos -
            (offset_central_dir+size_central_dir);
        ziinit.add_position_when_writting_offset = byte_before_the_zipfile;

        {
            uLong size_central_dir_to_read = size_central_dir;
            size_t buf_size = SIZEDATA_INDATABLOCK;
            void* buf_read = (void*)ALLOC(buf_size);
            if (ZSEEK(ziinit.z_filefunc, ziinit.filestream,
                      offset_central_dir + byte_before_the_zipfile,
                      ZLIB_FILEFUNC_SEEK_SET) != 0)
                err=ZIP_ERRNO;

            while ((size_central_dir_to_read>0) && (err==ZIP_OK))
            {
                uLong read_this = SIZEDATA_INDATABLOCK;
                if (read_this > size_central_dir_to_read)
                    read_this = size_central_dir_to_read;
                if (ZREAD(ziinit.z_filefunc, ziinit.filestream,buf_read,read_this) != read_this)
                    err=ZIP_ERRNO;

                if (err==ZIP_OK)
                    err = add_data_in_datablock(&ziinit.central_dir,buf_read,
                                                (uLong)read_this);
                size_central_dir_to_read-=read_this;
            }
            TRYFREE(buf_read);
        }
        ziinit.begin_pos = byte_before_the_zipfile;
        ziinit.number_entry = number_entry_CD;

        if (ZSEEK(ziinit.z_filefunc, ziinit.filestream,
                  offset_central_dir+byte_before_the_zipfile,ZLIB_FILEFUNC_SEEK_SET)!=0)
            err=ZIP_ERRNO;
    }

    if (globalcomment)
    {
        *globalcomment = ziinit.globalcomment;
    }
#    endif /* !NO_ADDFILEINEXISTINGZIP*/

    if (err != ZIP_OK)
    {
#    ifndef NO_ADDFILEINEXISTINGZIP
        TRYFREE(ziinit.globalcomment);
#    endif /* !NO_ADDFILEINEXISTINGZIP*/
        TRYFREE(zi);
        return NULL;
    }
    else
    {
        *zi = ziinit;
        return (zipFile)zi;
    }
}

extern zipFile ZEXPORT zipOpen (
    voidpf file,
    int append)
{
    return zipOpen2(file,append,NULL,NULL);
}

#define RAND_HEAD_LEN 12

extern int ZEXPORT zipOpenNewFileInZip3 (
    zipFile file,
    const char* filename,
    const zip_fileinfo* zipfi,
    const void* extrafield_local,
    uInt size_extrafield_local,
    const void* extrafield_global,
    uInt size_extrafield_global,
    const char* comment,
    int method,
    int level,
    int raw,
    int windowBits,
    int memLevel,
    int strategy,
    const char* password,
    uLong crcForCrypting)
{
    zip_internal* zi;
    uInt size_filename;
    uInt size_comment;
    uInt i;
    int err = ZIP_OK;

#    ifdef NOCRYPT
    if (password != NULL)
        return ZIP_PARAMERROR;
#    endif

    if (file == NULL)
        return ZIP_PARAMERROR;
    if ((method!=0) && (method!=Z_DEFLATED))
        return ZIP_PARAMERROR;

    zi = (zip_internal*)file;

    if (zi->in_opened_file_inzip == 1)
    {
        err = zipCloseFileInZip (file);
        if (err != ZIP_OK)
            return err;
    }


    if (filename==NULL)
        filename="-";

    if (comment==NULL)
        size_comment = 0;
    else
        size_comment = (uInt)strlen(comment);

    size_filename = (uInt)strlen(filename);

    if (zipfi == NULL)
        zi->ci.dosDate = 0;
    else
    {
        if (zipfi->dosDate != 0)
            zi->ci.dosDate = zipfi->dosDate;
        else zi->ci.dosDate = ziplocal_TmzDateToDosDate(&zipfi->tmz_date,zipfi->dosDate);
    }

    zi->ci.flag = 0;
    if ((level==8) || (level==9))
        zi->ci.flag |= 2;
    if ((level==2))
        zi->ci.flag |= 4;
    if ((level==1))
        zi->ci.flag |= 6;
    if (password != NULL)
    {
        zi->ci.flag |= 1;
    }
    zi->ci.flag |= 8;
    zi->ci.crc32 = 0;
    zi->ci.method = method;
    zi->ci.encrypt = 0;
    zi->ci.stream_initialised = 0;
    zi->ci.pos_in_buffered_data = 0;
    zi->ci.raw = raw;
    zi->ci.pos_local_header = ZTELL(zi->z_filefunc,zi->filestream) ;
    zi->ci.size_centralheader = SIZECENTRALHEADER + size_filename +
        size_extrafield_global + size_comment;
    zi->ci.central_header = (char*)ALLOC((uInt)zi->ci.size_centralheader);

    ziplocal_putValue_inmemory(zi->ci.central_header,(uLong)CENTRALHEADERMAGIC,4);
    /* version info */
    ziplocal_putValue_inmemory(zi->ci.central_header+4,(uLong)VERSIONMADEBY,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+6,(uLong)20,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+8,(uLong)zi->ci.flag,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+10,(uLong)zi->ci.method,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+12,(uLong)zi->ci.dosDate,4);
    ziplocal_putValue_inmemory(zi->ci.central_header+16,(uLong)0,4); /*crc*/
    ziplocal_putValue_inmemory(zi->ci.central_header+20,(uLong)0,4); /*compr size*/
    ziplocal_putValue_inmemory(zi->ci.central_header+24,(uLong)0,4); /*uncompr size*/
    ziplocal_putValue_inmemory(zi->ci.central_header+28,(uLong)size_filename,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+30,(uLong)size_extrafield_global,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+32,(uLong)size_comment,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+34,(uLong)0,2); /*disk nm start*/

    if (zipfi==NULL)
        ziplocal_putValue_inmemory(zi->ci.central_header+36,(uLong)0,2);
    else
        ziplocal_putValue_inmemory(zi->ci.central_header+36,(uLong)zipfi->internal_fa,2);

    if (zipfi==NULL)
        ziplocal_putValue_inmemory(zi->ci.central_header+38,(uLong)0,4);
    else
        ziplocal_putValue_inmemory(zi->ci.central_header+38,(uLong)zipfi->external_fa,4);

    ziplocal_putValue_inmemory(zi->ci.central_header+42,(uLong)zi->ci.pos_local_header- zi->add_position_when_writting_offset,4);

    for (i=0;i<size_filename;i++)
        *(zi->ci.central_header+SIZECENTRALHEADER+i) = *(filename+i);

    for (i=0;i<size_extrafield_global;i++)
        *(zi->ci.central_header+SIZECENTRALHEADER+size_filename+i) =
            *(((const char*)extrafield_global)+i);

    for (i=0;i<size_comment;i++)
        *(zi->ci.central_header+SIZECENTRALHEADER+size_filename+
          size_extrafield_global+i) = *(comment+i);
    if (zi->ci.central_header == NULL)
        return ZIP_INTERNALERROR;

    /* write the local header */
    err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)LOCALHEADERMAGIC,4);

    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)20,2);/* version needed to extract */
    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)zi->ci.flag,2);

    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)zi->ci.method,2);

    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)zi->ci.dosDate,4);

    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)0,4); /* crc 32, unknown */
    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)0,4); /* compressed size, unknown */
    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)0,4); /* uncompressed size, unknown */

    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)size_filename,2);

    if (err==ZIP_OK)
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)size_extrafield_local,2);

    if ((err==ZIP_OK) && (size_filename>0))
        if (ZWRITE(zi->z_filefunc,zi->filestream,filename,size_filename)!=size_filename)
            err = ZIP_ERRNO;

    if ((err==ZIP_OK) && (size_extrafield_local>0))
        if (ZWRITE(zi->z_filefunc,zi->filestream,extrafield_local,size_extrafield_local)
            !=size_extrafield_local)
            err = ZIP_ERRNO;

    zi->ci.stream.avail_in = (uInt)0;
    zi->ci.stream.avail_out = (uInt)Z_BUFSIZE;
    zi->ci.stream.next_out = zi->ci.buffered_data;
    zi->ci.stream.total_in = 0;
    zi->ci.stream.total_out = 0;

    if ((err==ZIP_OK) && (zi->ci.method == Z_DEFLATED) && (!zi->ci.raw))
    {
        zi->ci.stream.zalloc = (alloc_func)0;
        zi->ci.stream.zfree = (free_func)0;
        zi->ci.stream.opaque = (voidpf)0;

        if (windowBits>0)
            windowBits = -windowBits;

        err = deflateInit2(&zi->ci.stream, level,
                           Z_DEFLATED, windowBits, memLevel, strategy);

        if (err==Z_OK)
            zi->ci.stream_initialised = 1;
    }
#    ifndef NOCRYPT
    zi->ci.crypt_header_size = 0;
    if ((err==Z_OK) && (password != NULL))
    {
        unsigned char bufHead[RAND_HEAD_LEN];
        unsigned int sizeHead;
        zi->ci.encrypt = 1;
        zi->ci.pcrc_32_tab = get_crc_table();
        /*init_keys(password,zi->ci.keys,zi->ci.pcrc_32_tab);*/

        crcForCrypting = (uLong)zi->ci.dosDate << 16; // ATTANTION! Without this row, you don't unpack your password protected archive in other app.

        sizeHead = crypthead(password,bufHead,RAND_HEAD_LEN,zi->ci.keys,zi->ci.pcrc_32_tab,crcForCrypting);

        zi->ci.crypt_header_size = sizeHead;

        if (ZWRITE(zi->z_filefunc,zi->filestream,bufHead,sizeHead) != sizeHead)
            err = ZIP_ERRNO;
    }
#    endif

    if (err==Z_OK)
        zi->in_opened_file_inzip = 1;
    return err;
}

extern int ZEXPORT zipOpenNewFileInZip2(
    zipFile file,
    const char* filename,
    const zip_fileinfo* zipfi,
    const void* extrafield_local,
    uInt size_extrafield_local,
    const void* extrafield_global,
    uInt size_extrafield_global,
    const char* comment,
    int method,
    int level,
    int raw)
{
    return zipOpenNewFileInZip3 (file, filename, zipfi,
                                 extrafield_local, size_extrafield_local,
                                 extrafield_global, size_extrafield_global,
                                 comment, method, level, raw,
                                 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                 NULL, 0);
}

extern int ZEXPORT zipOpenNewFileInZip (
    zipFile file,
    const char* filename,
    const zip_fileinfo* zipfi,
    const void* extrafield_local,
    uInt size_extrafield_local,
    const void* extrafield_global,
    uInt size_extrafield_global,
    const char* comment,
    int method,
    int level)
{
    return zipOpenNewFileInZip2 (file, filename, zipfi,
                                 extrafield_local, size_extrafield_local,
                                 extrafield_global, size_extrafield_global,
                                 comment, method, level, 0);
}

local int zipFlushWriteBuffer(
    zip_internal* zi)
{
    int err=ZIP_OK;

    if (zi->ci.encrypt != 0)
    {
#ifndef NOCRYPT
        uInt i;
        int t;
        for (i=0;i<zi->ci.pos_in_buffered_data;i++)
            zi->ci.buffered_data[i] = zencode(zi->ci.keys, zi->ci.pcrc_32_tab,
                                              zi->ci.buffered_data[i],t);
#endif
    }
    if (ZWRITE(zi->z_filefunc,zi->filestream,zi->ci.buffered_data,zi->ci.pos_in_buffered_data)
        !=zi->ci.pos_in_buffered_data)
        err = ZIP_ERRNO;
    zi->ci.pos_in_buffered_data = 0;
    return err;
}

extern int ZEXPORT zipWriteInFileInZip (
    zipFile file,
    const void* buf,
    unsigned len)
{
    zip_internal* zi;
    int err=ZIP_OK;

    if (file == NULL)
        return ZIP_PARAMERROR;
    zi = (zip_internal*)file;

    if (zi->in_opened_file_inzip == 0)
        return ZIP_PARAMERROR;

    zi->ci.stream.next_in = (Bytef*)buf;
    zi->ci.stream.avail_in = len;
    zi->ci.crc32 = crc32(zi->ci.crc32,(const Bytef *)buf,len);

    while ((err==ZIP_OK) && (zi->ci.stream.avail_in>0))
    {
        if (zi->ci.stream.avail_out == 0)
        {
            if (zipFlushWriteBuffer(zi) == ZIP_ERRNO)
                err = ZIP_ERRNO;
            zi->ci.stream.avail_out = (uInt)Z_BUFSIZE;
            zi->ci.stream.next_out = zi->ci.buffered_data;
        }


        if(err != ZIP_OK)
            break;

        if ((zi->ci.method == Z_DEFLATED) && (!zi->ci.raw))
        {
            uLong uTotalOutBefore = zi->ci.stream.total_out;
            err=deflate(&zi->ci.stream,  Z_NO_FLUSH);
            zi->ci.pos_in_buffered_data += (uInt)(zi->ci.stream.total_out - uTotalOutBefore) ;

        }
        else
        {
            uInt copy_this,i;
            if (zi->ci.stream.avail_in < zi->ci.stream.avail_out)
                copy_this = zi->ci.stream.avail_in;
            else
                copy_this = zi->ci.stream.avail_out;
            for (i=0;i<copy_this;i++)
                *(((char*)zi->ci.stream.next_out)+i) =
                    *(((const char*)zi->ci.stream.next_in)+i);
            {
                zi->ci.stream.avail_in -= copy_this;
                zi->ci.stream.avail_out-= copy_this;
                zi->ci.stream.next_in+= copy_this;
                zi->ci.stream.next_out+= copy_this;
                zi->ci.stream.total_in+= copy_this;
                zi->ci.stream.total_out+= copy_this;
                zi->ci.pos_in_buffered_data += copy_this;
            }
        }
    }

    return err;
}

extern int ZEXPORT zipCloseFileInZipRaw (
    zipFile file,
    uLong uncompressed_size,
    uLong crc32)
{
    zip_internal* zi;
    uLong compressed_size;
    int err=ZIP_OK;

    if (file == NULL)
        return ZIP_PARAMERROR;
    zi = (zip_internal*)file;

    if (zi->in_opened_file_inzip == 0)
        return ZIP_PARAMERROR;
    zi->ci.stream.avail_in = 0;

    if ((zi->ci.method == Z_DEFLATED) && (!zi->ci.raw))
        while (err==ZIP_OK)
        {
            uLong uTotalOutBefore;
            if (zi->ci.stream.avail_out == 0)
            {
                if (zipFlushWriteBuffer(zi) == ZIP_ERRNO)
                    err = ZIP_ERRNO;
                zi->ci.stream.avail_out = (uInt)Z_BUFSIZE;
                zi->ci.stream.next_out = zi->ci.buffered_data;
            }
            uTotalOutBefore = zi->ci.stream.total_out;
            err=deflate(&zi->ci.stream,  Z_FINISH);
            zi->ci.pos_in_buffered_data += (uInt)(zi->ci.stream.total_out - uTotalOutBefore) ;
        }

    if (err==Z_STREAM_END)
        err=ZIP_OK; /* this is normal */

    if ((zi->ci.pos_in_buffered_data>0) && (err==ZIP_OK))
        if (zipFlushWriteBuffer(zi)==ZIP_ERRNO)
            err = ZIP_ERRNO;

    if ((zi->ci.method == Z_DEFLATED) && (!zi->ci.raw))
    {
        err=deflateEnd(&zi->ci.stream);
        zi->ci.stream_initialised = 0;
    }

    if (!zi->ci.raw)
    {
        crc32 = (uLong)zi->ci.crc32;
        uncompressed_size = (uLong)zi->ci.stream.total_in;
    }
    compressed_size = (uLong)zi->ci.stream.total_out;
#    ifndef NOCRYPT
    compressed_size += zi->ci.crypt_header_size;
#    endif

    ziplocal_putValue_inmemory(zi->ci.central_header+16,crc32,4); /*crc*/
    ziplocal_putValue_inmemory(zi->ci.central_header+20,
                               compressed_size,4); /*compr size*/
    if (zi->ci.stream.data_type == Z_ASCII)
        ziplocal_putValue_inmemory(zi->ci.central_header+36,(uLong)Z_ASCII,2);
    ziplocal_putValue_inmemory(zi->ci.central_header+24,
                               uncompressed_size,4); /*uncompr size*/

    if (err==ZIP_OK)
        err = add_data_in_datablock(&zi->central_dir,zi->ci.central_header,
                                    (uLong)zi->ci.size_centralheader);
    free(zi->ci.central_header);

    if (err==ZIP_OK)
    {
        uLong cur_pos_inzip = ZTELL(zi->z_filefunc,zi->filestream);
        if (ZSEEK(zi->z_filefunc,zi->filestream,
                  zi->ci.pos_local_header + 14,ZLIB_FILEFUNC_SEEK_SET)!=0)
            err = ZIP_ERRNO;

        if (err==ZIP_OK)
            err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,crc32,4); /* crc 32, unknown */

        if (err==ZIP_OK) /* compressed size, unknown */
            err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,compressed_size,4);

        if (err==ZIP_OK) /* uncompressed size, unknown */
            err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,uncompressed_size,4);

        if (ZSEEK(zi->z_filefunc,zi->filestream,
                  cur_pos_inzip,ZLIB_FILEFUNC_SEEK_SET)!=0)
            err = ZIP_ERRNO;

        /* Write local Descriptor after file data */
        if (err==ZIP_OK)
            err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)DESCRIPTORHEADERMAGIC,4);

        if (err==ZIP_OK)
            err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,crc32,4); /* crc 32, unknown */

        if (err==ZIP_OK) /* compressed size, unknown */
            err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,compressed_size,4);

        if (err==ZIP_OK) /* uncompressed size, unknown */
            err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,uncompressed_size,4);


    }

    zi->number_entry ++;
    zi->in_opened_file_inzip = 0;

    return err;
}

extern int ZEXPORT zipCloseFileInZip (
    zipFile file)
{
    return zipCloseFileInZipRaw (file,0,0);
}

extern int ZEXPORT zipClose (
    zipFile file,
    const char* global_comment)
{
    zip_internal* zi;
    int err = 0;
    uLong size_centraldir = 0;
    uLong centraldir_pos_inzip;
    uInt size_global_comment;
    if (file == NULL)
        return ZIP_PARAMERROR;
    zi = (zip_internal*)file;

    if (zi->in_opened_file_inzip == 1)
    {
        err = zipCloseFileInZip (file);
    }

#ifndef NO_ADDFILEINEXISTINGZIP
    if (global_comment==NULL)
        global_comment = zi->globalcomment;
#endif
    if (global_comment==NULL)
        size_global_comment = 0;
    else
        size_global_comment = (uInt)strlen(global_comment);

    centraldir_pos_inzip = ZTELL(zi->z_filefunc,zi->filestream);
    if (err==ZIP_OK)
    {
        linkedlist_datablock_internal* ldi = zi->central_dir.first_block ;
        while (ldi!=NULL)
        {
            if ((err==ZIP_OK) && (ldi->filled_in_this_block>0))
                if (ZWRITE(zi->z_filefunc,zi->filestream,
                           ldi->data,ldi->filled_in_this_block)
                    !=ldi->filled_in_this_block )
                    err = ZIP_ERRNO;

            size_centraldir += ldi->filled_in_this_block;
            ldi = ldi->next_datablock;
        }
    }
    free_datablock(zi->central_dir.first_block);

    if (err==ZIP_OK) /* Magic End */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)ENDHEADERMAGIC,4);

    if (err==ZIP_OK) /* number of this disk */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)0,2);

    if (err==ZIP_OK) /* number of the disk with the start of the central directory */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)0,2);

    if (err==ZIP_OK) /* total number of entries in the central dir on this disk */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)zi->number_entry,2);

    if (err==ZIP_OK) /* total number of entries in the central dir */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)zi->number_entry,2);

    if (err==ZIP_OK) /* size of the central directory */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)size_centraldir,4);

    if (err==ZIP_OK) /* offset of start of central directory with respect to the
                        starting disk number */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,
                                (uLong)(centraldir_pos_inzip - zi->add_position_when_writting_offset),4);

    if (err==ZIP_OK) /* zipfile comment length */
        err = ziplocal_putValue(&zi->z_filefunc,zi->filestream,(uLong)size_global_comment,2);

    if ((err==ZIP_OK) && (size_global_comment>0))
        if (ZWRITE(zi->z_filefunc,zi->filestream,
                   global_comment,size_global_comment) != size_global_comment)
            err = ZIP_ERRNO;

    if (ZCLOSE(zi->z_filefunc,zi->filestream) != 0)
        if (err == ZIP_OK)
            err = ZIP_ERRNO;

#ifndef NO_ADDFILEINEXISTINGZIP
    TRYFREE(zi->globalcomment);
#endif
    TRYFREE(zi);

    return err;
}

// /////////////////////////////////////////////////////////////////
// unzip
// /////////////////////////////////////////////////////////////////

/* unzip.c -- IO for uncompress .zip files using zlib
   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant

   Read unzip.h for more info

   Modified by Sergey A. Tachenov to integrate with Qt.
*/

/* Decryption code comes from crypt.c by Info-ZIP but has been greatly reduced in terms of
   compatibility with older software. The following is from the original crypt.c. Code
   woven in by Terry Thorsen 1/2003.
*/
/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
  crypt.c (full version) by Info-ZIP.      Last revised:  [see crypt.h]

  The encryption/decryption parts of this source code (as opposed to the
  non-echoing password parts) were originally written in Europe.  The
  whole source package can be freely distributed, including from the USA.
  (Prior to January 2000, re-export from the US was a violation of US law.)
*/

/*
  This encryption code is a direct transcription of the algorithm from
  Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
  file (appnote.txt) is distributed with the PKZIP program (even in the
  version without encryption capabilities).
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef STDC
#  include <stddef.h>
#  include <string.h>
#  include <stdlib.h>
#endif
#ifdef NO_ERRNO_H
extern int errno;
#else
#   include <errno.h>
#endif

#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

#ifndef CASESENSITIVITYDEFAULT_NO
#  if !defined(unix) && !defined(CASESENSITIVITYDEFAULT_YES)
#    define CASESENSITIVITYDEFAULT_NO
#  endif
#endif

#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif

#ifndef UNZ_MAXFILENAMEINZIP
#define UNZ_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
# define ALLOC(size) (malloc(size))
#endif
#ifndef TRYFREE
# define TRYFREE(p) {if (p) free(p);}
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

const char unz_copyright[] =
    " unzip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";

/* unz_file_info_interntal contain internal info about a file in zipfile*/
typedef struct unz_file_info_internal_s
{
    uLong offset_curfile;/* relative offset of local header 4 bytes */
} unz_file_info_internal;


/* file_in_zip_read_info_s contain internal information about a file in zipfile,
   when reading and decompress it */
typedef struct
{
    char  *read_buffer;         /* internal buffer for compressed data */
    z_stream stream;            /* zLib stream structure for inflate */

    uLong pos_in_zipfile;       /* position in byte on the zipfile, for fseek*/
    uLong stream_initialised;   /* flag set if stream structure is initialised*/

    uLong offset_local_extrafield;/* offset of the local extra field */
    uInt  size_local_extrafield;/* size of the local extra field */
    uLong pos_local_extrafield;   /* position in the local extra field in read*/

    uLong crc32;                /* crc32 of all data uncompressed */
    uLong crc32_wait;           /* crc32 we must obtain after decompress all */
    uLong rest_read_compressed; /* number of byte to be decompressed */
    uLong rest_read_uncompressed;/*number of byte to be obtained after decomp*/
    zlib_filefunc_def z_filefunc;
    voidpf filestream;        /* io structore of the zipfile */
    uLong compression_method;   /* compression method (0==store) */
    uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
    int   raw;
} file_in_zip_read_info_s;


/* unz_s contain internal information about the zipfile
 */
typedef struct
{
    zlib_filefunc_def z_filefunc;
    voidpf filestream;        /* io structore of the zipfile */
    unz_global_info gi;       /* public global information */
    uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
    uLong num_file;             /* number of the current file in the zipfile*/
    uLong pos_in_central_dir;   /* pos of the current file in the central dir*/
    uLong current_file_ok;      /* flag about the usability of the current file*/
    uLong central_pos;          /* position of the beginning of the central dir*/

    uLong size_central_dir;     /* size of the central directory  */
    uLong offset_central_dir;   /* offset of start of central directory with
                                   respect to the starting disk number */

    unz_file_info cur_file_info; /* public info about the current file in zip*/
    unz_file_info_internal cur_file_info_internal; /* private info about it*/
    file_in_zip_read_info_s* pfile_in_zip_read; /* structure about the current
                                                   file if we are decompressing it */
    int encrypted;
#    ifndef NOUNCRYPT
    unsigned long keys[3];     /* keys defining the pseudo-random sequence */
    const unsigned long* pcrc_32_tab;
#    endif
} unz_s;

/* ===========================================================================
   Read a byte from a gz_stream; update next_in and avail_in. Return EOF
   for end of file.
   IN assertion: the stream s has been sucessfully opened for reading.
*/


local int unzlocal_getByte OF((
                                  const zlib_filefunc_def* pzlib_filefunc_def,
                                  voidpf filestream,
                                  int *pi));

local int unzlocal_getByte(
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream,
    int *pi)
{
    unsigned char c;
    int err = (int)ZREAD(*pzlib_filefunc_def,filestream,&c,1);
    if (err==1)
    {
        *pi = (int)c;
        return UNZ_OK;
    }
    else
    {
        if (ZERROR(*pzlib_filefunc_def,filestream))
            return UNZ_ERRNO;
        else
            return UNZ_EOF;
    }
}


/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets
*/
local int unzlocal_getShort OF((
                                   const zlib_filefunc_def* pzlib_filefunc_def,
                                   voidpf filestream,
                                   uLong *pX));

local int unzlocal_getShort (
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream,
    uLong *pX)
{
    uLong x ;
    int i;
    int err;

    err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x = (uLong)i;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

local int unzlocal_getLong OF((
                                  const zlib_filefunc_def* pzlib_filefunc_def,
                                  voidpf filestream,
                                  uLong *pX));

local int unzlocal_getLong (
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream,
    uLong *pX)
{
    uLong x ;
    int i;
    int err;

    err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x = (uLong)i;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<16;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<24;

    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}


/* My own strcmpi / strcasecmp */
local int strcmpcasenosensitive_internal (
    const char* fileName1,
    const char* fileName2)
{
    for (;;)
    {
        char c1=*(fileName1++);
        char c2=*(fileName2++);
        if ((c1>='a') && (c1<='z'))
            c1 -= 0x20;
        if ((c2>='a') && (c2<='z'))
            c2 -= 0x20;
        if (c1=='\0')
            return ((c2=='\0') ? 0 : -1);
        if (c2=='\0')
            return 1;
        if (c1<c2)
            return -1;
        if (c1>c2)
            return 1;
    }
}


#ifdef  CASESENSITIVITYDEFAULT_NO
#define CASESENSITIVITYDEFAULTVALUE 2
#else
#define CASESENSITIVITYDEFAULTVALUE 1
#endif

#ifndef STRCMPCASENOSENTIVEFUNCTION
#define STRCMPCASENOSENTIVEFUNCTION strcmpcasenosensitive_internal
#endif

/*
  Compare two filename (fileName1,fileName2).
  If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
  If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi
  or strcasecmp)
  If iCaseSenisivity = 0, case sensitivity is defaut of your operating system
  (like 1 on Unix, 2 on Windows)

*/
extern int ZEXPORT unzStringFileNameCompare (
    const char* fileName1,
    const char* fileName2,
    int iCaseSensitivity)
{
    if (iCaseSensitivity==0)
        iCaseSensitivity=CASESENSITIVITYDEFAULTVALUE;

    if (iCaseSensitivity==1)
        return strcmp(fileName1,fileName2);

    return STRCMPCASENOSENTIVEFUNCTION(fileName1,fileName2);
}

#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif

/*
  Locate the Central directory of a zipfile (at the end, just before
  the global comment)
*/
local uLong unzlocal_SearchCentralDir OF((
                                             const zlib_filefunc_def* pzlib_filefunc_def,
                                             voidpf filestream));

local uLong unzlocal_SearchCentralDir(
    const zlib_filefunc_def* pzlib_filefunc_def,
    voidpf filestream)
{
    unsigned char* buf;
    uLong uSizeFile;
    uLong uBackRead;
    uLong uMaxBack=0xffff; /* maximum size of global comment */
    uLong uPosFound=0;

    if (ZSEEK(*pzlib_filefunc_def,filestream,0,ZLIB_FILEFUNC_SEEK_END) != 0)
        return 0;


    uSizeFile = ZTELL(*pzlib_filefunc_def,filestream);

    if (uMaxBack>uSizeFile)
        uMaxBack = uSizeFile;

    buf = (unsigned char*)ALLOC(BUFREADCOMMENT+4);
    if (buf==NULL)
        return 0;

    uBackRead = 4;
    while (uBackRead<uMaxBack)
    {
        uLong uReadSize,uReadPos ;
        int i;
        if (uBackRead+BUFREADCOMMENT>uMaxBack)
            uBackRead = uMaxBack;
        else
            uBackRead+=BUFREADCOMMENT;
        uReadPos = uSizeFile-uBackRead ;

        uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ?
            (BUFREADCOMMENT+4) : (uSizeFile-uReadPos);
        if (ZSEEK(*pzlib_filefunc_def,filestream,uReadPos,ZLIB_FILEFUNC_SEEK_SET)!=0)
            break;

        if (ZREAD(*pzlib_filefunc_def,filestream,buf,uReadSize)!=uReadSize)
            break;

        for (i=(int)uReadSize-3; (i--)>0;)
            if (((*(buf+i))==0x50) && ((*(buf+i+1))==0x4b) &&
                ((*(buf+i+2))==0x05) && ((*(buf+i+3))==0x06))
            {
                uPosFound = uReadPos+i;
                break;
            }

        if (uPosFound!=0)
            break;
    }
    TRYFREE(buf);
    return uPosFound;
}

/*
  Open a Zip file. path contain the full pathname (by example,
  on a Windows NT computer "c:\\test\\zlib114.zip" or on an Unix computer
  "zlib/zlib114.zip".
  If the zipfile cannot be opened (file doesn't exist or in not valid), the
  return value is NULL.
  Else, the return value is a unzFile Handle, usable with other function
  of this unzip package.
*/
extern unzFile ZEXPORT unzOpen2 (
    voidpf file,
    zlib_filefunc_def* pzlib_filefunc_def)
{
    unz_s us;
    unz_s *s;
    uLong central_pos,uL;

    uLong number_disk;          /* number of the current dist, used for
                                   spaning ZIP, unsupported, always 0*/
    uLong number_disk_with_CD;  /* number the the disk with central dir, used
                                   for spaning ZIP, unsupported, always 0*/
    uLong number_entry_CD;      /* total number of entries in
                                   the central dir
                                   (same than number_entry on nospan) */

    int err=UNZ_OK;

    if (unz_copyright[0]!=' ')
        return NULL;

    if (pzlib_filefunc_def==NULL)
        fill_qiodevice_filefunc(&us.z_filefunc);
    else
        us.z_filefunc = *pzlib_filefunc_def;

    us.filestream= (*(us.z_filefunc.zopen_file))(us.z_filefunc.opaque,
                                                 file,
                                                 ZLIB_FILEFUNC_MODE_READ |
                                                 ZLIB_FILEFUNC_MODE_EXISTING);
    if (us.filestream==NULL)
        return NULL;

    central_pos = unzlocal_SearchCentralDir(&us.z_filefunc,us.filestream);
    if (central_pos==0)
        err=UNZ_ERRNO;

    if (ZSEEK(us.z_filefunc, us.filestream,
              central_pos,ZLIB_FILEFUNC_SEEK_SET)!=0)
        err=UNZ_ERRNO;

    /* the signature, already checked */
    if (unzlocal_getLong(&us.z_filefunc, us.filestream,&uL)!=UNZ_OK)
        err=UNZ_ERRNO;

    /* number of this disk */
    if (unzlocal_getShort(&us.z_filefunc, us.filestream,&number_disk)!=UNZ_OK)
        err=UNZ_ERRNO;

    /* number of the disk with the start of the central directory */
    if (unzlocal_getShort(&us.z_filefunc, us.filestream,&number_disk_with_CD)!=UNZ_OK)
        err=UNZ_ERRNO;

    /* total number of entries in the central dir on this disk */
    if (unzlocal_getShort(&us.z_filefunc, us.filestream,&us.gi.number_entry)!=UNZ_OK)
        err=UNZ_ERRNO;

    /* total number of entries in the central dir */
    if (unzlocal_getShort(&us.z_filefunc, us.filestream,&number_entry_CD)!=UNZ_OK)
        err=UNZ_ERRNO;

    if ((number_entry_CD!=us.gi.number_entry) ||
        (number_disk_with_CD!=0) ||
        (number_disk!=0))
        err=UNZ_BADZIPFILE;

    /* size of the central directory */
    if (unzlocal_getLong(&us.z_filefunc, us.filestream,&us.size_central_dir)!=UNZ_OK)
        err=UNZ_ERRNO;

    /* offset of start of central directory with respect to the
       starting disk number */
    if (unzlocal_getLong(&us.z_filefunc, us.filestream,&us.offset_central_dir)!=UNZ_OK)
        err=UNZ_ERRNO;

    /* zipfile comment length */
    if (unzlocal_getShort(&us.z_filefunc, us.filestream,&us.gi.size_comment)!=UNZ_OK)
        err=UNZ_ERRNO;

    if ((central_pos<us.offset_central_dir+us.size_central_dir) &&
        (err==UNZ_OK))
        err=UNZ_BADZIPFILE;

    if (err!=UNZ_OK)
    {
        ZCLOSE(us.z_filefunc, us.filestream);
        return NULL;
    }

    us.byte_before_the_zipfile = central_pos -
        (us.offset_central_dir+us.size_central_dir);
    us.central_pos = central_pos;
    us.pfile_in_zip_read = NULL;
    us.encrypted = 0;


    s=(unz_s*)ALLOC(sizeof(unz_s));
    *s=us;
    unzGoToFirstFile((unzFile)s);
    return (unzFile)s;
}


extern unzFile ZEXPORT unzOpen (
    voidpf file)
{
    return unzOpen2(file, NULL);
}

/*
  Close a ZipFile opened with unzipOpen.
  If there is files inside the .Zip opened with unzipOpenCurrentFile (see later),
  these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
  return UNZ_OK if there is no problem. */
extern int ZEXPORT unzClose (
    unzFile file)
{
    unz_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;

    if (s->pfile_in_zip_read!=NULL)
        unzCloseCurrentFile(file);

    ZCLOSE(s->z_filefunc, s->filestream);
    TRYFREE(s);
    return UNZ_OK;
}


/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem. */
extern int ZEXPORT unzGetGlobalInfo (
    unzFile file,
    unz_global_info *pglobal_info)
{
    unz_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    *pglobal_info=s->gi;
    return UNZ_OK;
}


/*
  Translate date/time from Dos format to tm_unz (readable more easilty)
*/
local void unzlocal_DosDateToTmuDate (
    uLong ulDosDate,
    tm_unz* ptm)
{
    uLong uDate;
    uDate = (uLong)(ulDosDate>>16);
    ptm->tm_mday = (uInt)(uDate&0x1f) ;
    ptm->tm_mon =  (uInt)((((uDate)&0x1E0)/0x20)-1) ;
    ptm->tm_year = (uInt)(((uDate&0x0FE00)/0x0200)+1980) ;

    ptm->tm_hour = (uInt) ((ulDosDate &0xF800)/0x800);
    ptm->tm_min =  (uInt) ((ulDosDate&0x7E0)/0x20) ;
    ptm->tm_sec =  (uInt) (2*(ulDosDate&0x1f)) ;
}

/*
  Get Info about the current file in the zipfile, with internal only info
*/
local int unzlocal_GetCurrentFileInfoInternal OF((unzFile file,
                                                  unz_file_info *pfile_info,
                                                  unz_file_info_internal
                                                  *pfile_info_internal,
                                                  char *szFileName,
                                                  uLong fileNameBufferSize,
                                                  void *extraField,
                                                  uLong extraFieldBufferSize,
                                                  char *szComment,
                                                  uLong commentBufferSize));

local int unzlocal_GetCurrentFileInfoInternal (
    unzFile file,
    unz_file_info *pfile_info,
    unz_file_info_internal *pfile_info_internal,
    char *szFileName,
    uLong fileNameBufferSize,
    void *extraField,
    uLong extraFieldBufferSize,
    char *szComment,
    uLong commentBufferSize)
{
    unz_s* s;
    unz_file_info file_info;
    unz_file_info_internal file_info_internal;
    int err=UNZ_OK;
    uLong uMagic;
    uLong uSeek=0;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (ZSEEK(s->z_filefunc, s->filestream,
              s->pos_in_central_dir+s->byte_before_the_zipfile,
              ZLIB_FILEFUNC_SEEK_SET)!=0)
        err=UNZ_ERRNO;


    /* we check the magic */
    if (err==UNZ_OK) {
        if (unzlocal_getLong(&s->z_filefunc, s->filestream,&uMagic) != UNZ_OK)
            err=UNZ_ERRNO;
        else if (uMagic!=0x02014b50)
            err=UNZ_BADZIPFILE;
    }

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.version) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.version_needed) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.flag) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.compression_method) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.dosDate) != UNZ_OK)
        err=UNZ_ERRNO;

    unzlocal_DosDateToTmuDate(file_info.dosDate,&file_info.tmu_date);

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.crc) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.compressed_size) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.uncompressed_size) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.size_filename) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.size_file_extra) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.size_file_comment) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.disk_num_start) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.internal_fa) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.external_fa) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info_internal.offset_curfile) != UNZ_OK)
        err=UNZ_ERRNO;

    uSeek+=file_info.size_filename;
    if ((err==UNZ_OK) && (szFileName!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_filename<fileNameBufferSize)
        {
            *(szFileName+file_info.size_filename)='\0';
            uSizeRead = file_info.size_filename;
        }
        else
            uSizeRead = fileNameBufferSize;

        if ((file_info.size_filename>0) && (fileNameBufferSize>0))
            if (ZREAD(s->z_filefunc, s->filestream,szFileName,uSizeRead)!=uSizeRead)
                err=UNZ_ERRNO;
        uSeek -= uSizeRead;
    }


    if ((err==UNZ_OK) && (extraField!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_file_extra<extraFieldBufferSize)
            uSizeRead = file_info.size_file_extra;
        else
            uSizeRead = extraFieldBufferSize;

        if (uSeek!=0) {
            if (ZSEEK(s->z_filefunc, s->filestream,uSeek,ZLIB_FILEFUNC_SEEK_CUR)==0)
                uSeek=0;
            else
                err=UNZ_ERRNO;
        }
        if ((file_info.size_file_extra>0) && (extraFieldBufferSize>0))
            if (ZREAD(s->z_filefunc, s->filestream,extraField,uSizeRead)!=uSizeRead)
                err=UNZ_ERRNO;
        uSeek += file_info.size_file_extra - uSizeRead;
    }
    else
        uSeek+=file_info.size_file_extra;


    if ((err==UNZ_OK) && (szComment!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_file_comment<commentBufferSize)
        {
            *(szComment+file_info.size_file_comment)='\0';
            uSizeRead = file_info.size_file_comment;
        }
        else
            uSizeRead = commentBufferSize;

        if (uSeek!=0) {
            if (ZSEEK(s->z_filefunc, s->filestream,uSeek,ZLIB_FILEFUNC_SEEK_CUR)==0)
                uSeek=0;
            else
                err=UNZ_ERRNO;
        }
        if ((file_info.size_file_comment>0) && (commentBufferSize>0))
            if (ZREAD(s->z_filefunc, s->filestream,szComment,uSizeRead)!=uSizeRead)
                err=UNZ_ERRNO;
        uSeek+=file_info.size_file_comment - uSizeRead;
    }
    else
        uSeek+=file_info.size_file_comment;

    if ((err==UNZ_OK) && (pfile_info!=NULL))
        *pfile_info=file_info;

    if ((err==UNZ_OK) && (pfile_info_internal!=NULL))
        *pfile_info_internal=file_info_internal;

    return err;
}



/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem.
*/
extern int ZEXPORT unzGetCurrentFileInfo (
    unzFile file,
    unz_file_info *pfile_info,
    char *szFileName,
    uLong fileNameBufferSize,
    void *extraField,
    uLong extraFieldBufferSize,
    char *szComment,
    uLong commentBufferSize)
{
    return unzlocal_GetCurrentFileInfoInternal(file,pfile_info,NULL,
                                               szFileName,fileNameBufferSize,
                                               extraField,extraFieldBufferSize,
                                               szComment,commentBufferSize);
}

/*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/
extern int ZEXPORT unzGoToFirstFile (
    unzFile file)
{
    int err=UNZ_OK;
    unz_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    s->pos_in_central_dir=s->offset_central_dir;
    s->num_file=0;
    err=unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                            &s->cur_file_info_internal,
                                            NULL,0,NULL,0,NULL,0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

/*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/
extern int ZEXPORT unzGoToNextFile (
    unzFile file)
{
    unz_s* s;
    int err;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;
    if (s->gi.number_entry != 0xffff)    /* 2^16 files overflow hack */
        if (s->num_file+1==s->gi.number_entry)
            return UNZ_END_OF_LIST_OF_FILE;

    s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename +
        s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment ;
    s->num_file++;
    err = unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                              &s->cur_file_info_internal,
                                              NULL,0,NULL,0,NULL,0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}


/*
  Try locate the file szFileName in the zipfile.
  For the iCaseSensitivity signification, see unzipStringFileNameCompare

  return value :
  UNZ_OK if the file is found. It becomes the current file.
  UNZ_END_OF_LIST_OF_FILE if the file is not found
*/
extern int ZEXPORT unzLocateFile (
    unzFile file,
    const char *szFileName,
    int iCaseSensitivity)
{
    unz_s* s;
    int err;

    /* We remember the 'current' position in the file so that we can jump
     * back there if we fail.
     */
    unz_file_info cur_file_infoSaved;
    unz_file_info_internal cur_file_info_internalSaved;
    uLong num_fileSaved;
    uLong pos_in_central_dirSaved;


    if (file==NULL)
        return UNZ_PARAMERROR;

    if (strlen(szFileName)>=UNZ_MAXFILENAMEINZIP)
        return UNZ_PARAMERROR;

    s=(unz_s*)file;
    if (!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;

    /* Save the current state */
    num_fileSaved = s->num_file;
    pos_in_central_dirSaved = s->pos_in_central_dir;
    cur_file_infoSaved = s->cur_file_info;
    cur_file_info_internalSaved = s->cur_file_info_internal;

    err = unzGoToFirstFile(file);

    while (err == UNZ_OK)
    {
        char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];
        err = unzGetCurrentFileInfo(file,NULL,
                                    szCurrentFileName,sizeof(szCurrentFileName)-1,
                                    NULL,0,NULL,0);
        if (err == UNZ_OK)
        {
            if (unzStringFileNameCompare(szCurrentFileName,
                                         szFileName,iCaseSensitivity)==0)
                return UNZ_OK;
            err = unzGoToNextFile(file);
        }
    }

    /* We failed, so restore the state of the 'current file' to where we
     * were.
     */
    s->num_file = num_fileSaved ;
    s->pos_in_central_dir = pos_in_central_dirSaved ;
    s->cur_file_info = cur_file_infoSaved;
    s->cur_file_info_internal = cur_file_info_internalSaved;
    return err;
}


/*
///////////////////////////////////////////
// Contributed by Ryan Haksi (mailto://cryogen@infoserve.net)
// I need random access
//
// Further optimization could be realized by adding an ability
// to cache the directory in memory. The goal being a single
// comprehensive file read to put the file I need in a memory.
*/

/*
  typedef struct unz_file_pos_s
  {
  uLong pos_in_zip_directory;   // offset in file
  uLong num_of_file;            // # of file
  } unz_file_pos;
*/

extern int ZEXPORT unzGetFilePos(
    unzFile file,
    unz_file_pos* file_pos)
{
    unz_s* s;

    if (file==NULL || file_pos==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;

    file_pos->pos_in_zip_directory  = s->pos_in_central_dir;
    file_pos->num_of_file           = s->num_file;

    return UNZ_OK;
}

extern int ZEXPORT unzGoToFilePos(
    unzFile file,
    unz_file_pos* file_pos)
{
    unz_s* s;
    int err;

    if (file==NULL || file_pos==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;

    /* jump to the right spot */
    s->pos_in_central_dir = file_pos->pos_in_zip_directory;
    s->num_file           = file_pos->num_of_file;

    /* set the current file */
    err = unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                              &s->cur_file_info_internal,
                                              NULL,0,NULL,0,NULL,0);
    /* return results */
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

/*
// Unzip Helper Functions - should be here?
///////////////////////////////////////////
*/

/*
  Read the local header of the current zipfile
  Check the coherency of the local header and info in the end of central
  directory about this file
  store in *piSizeVar the size of extra info in local header
  (filename and size of extra field data)
*/
local int unzlocal_CheckCurrentFileCoherencyHeader (
    unz_s* s,
    uInt* piSizeVar,
    uLong *poffset_local_extrafield,
    uInt  *psize_local_extrafield)
{
    uLong uMagic,uData,uFlags;
    uLong size_filename;
    uLong size_extra_field;
    int err=UNZ_OK;

    *piSizeVar = 0;
    *poffset_local_extrafield = 0;
    *psize_local_extrafield = 0;

    if (ZSEEK(s->z_filefunc, s->filestream,s->cur_file_info_internal.offset_curfile +
              s->byte_before_the_zipfile,ZLIB_FILEFUNC_SEEK_SET)!=0)
        return UNZ_ERRNO;


    if (err==UNZ_OK) {
        if (unzlocal_getLong(&s->z_filefunc, s->filestream,&uMagic) != UNZ_OK)
            err=UNZ_ERRNO;
        else if (uMagic!=0x04034b50)
            err=UNZ_BADZIPFILE;
    }

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&uData) != UNZ_OK)
        err=UNZ_ERRNO;
/*
  else if ((err==UNZ_OK) && (uData!=s->cur_file_info.wVersion))
  err=UNZ_BADZIPFILE;
*/
    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&uFlags) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&uData) != UNZ_OK)
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compression_method))
        err=UNZ_BADZIPFILE;

    if ((err==UNZ_OK) && (s->cur_file_info.compression_method!=0) &&
        (s->cur_file_info.compression_method!=Z_DEFLATED))
        err=UNZ_BADZIPFILE;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&uData) != UNZ_OK) /* date/time */
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&uData) != UNZ_OK) /* crc */
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.crc) &&
             ((uFlags & 8)==0))
        err=UNZ_BADZIPFILE;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&uData) != UNZ_OK) /* size compr */
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compressed_size) &&
             ((uFlags & 8)==0))
        err=UNZ_BADZIPFILE;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&uData) != UNZ_OK) /* size uncompr */
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.uncompressed_size) &&
             ((uFlags & 8)==0))
        err=UNZ_BADZIPFILE;


    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&size_filename) != UNZ_OK)
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (size_filename!=s->cur_file_info.size_filename))
        err=UNZ_BADZIPFILE;

    *piSizeVar += (uInt)size_filename;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&size_extra_field) != UNZ_OK)
        err=UNZ_ERRNO;
    *poffset_local_extrafield= s->cur_file_info_internal.offset_curfile +
        SIZEZIPLOCALHEADER + size_filename;
    *psize_local_extrafield = (uInt)size_extra_field;

    *piSizeVar += (uInt)size_extra_field;

    return err;
}

/*
  Open for reading data the current file in the zipfile.
  If there is no error and the file is opened, the return value is UNZ_OK.
*/
extern int ZEXPORT unzOpenCurrentFile3 (
    unzFile file,
    int* method,
    int* level,
    int raw,
    const char* password)
{
    int err=UNZ_OK;
    uInt iSizeVar;
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    uLong offset_local_extrafield;  /* offset of the local extra field */
    uInt  size_local_extrafield;    /* size of the local extra field */
#    ifndef NOUNCRYPT
    char source[12];
#    else
    if (password != NULL)
        return UNZ_PARAMERROR;
#    endif

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (!s->current_file_ok)
        return UNZ_PARAMERROR;

    if (s->pfile_in_zip_read != NULL)
        unzCloseCurrentFile(file);

    if (unzlocal_CheckCurrentFileCoherencyHeader(s,&iSizeVar,
                                                 &offset_local_extrafield,&size_local_extrafield)!=UNZ_OK)
        return UNZ_BADZIPFILE;

    pfile_in_zip_read_info = (file_in_zip_read_info_s*)
        ALLOC(sizeof(file_in_zip_read_info_s));
    if (pfile_in_zip_read_info==NULL)
        return UNZ_INTERNALERROR;

    pfile_in_zip_read_info->read_buffer=(char*)ALLOC(UNZ_BUFSIZE);
    pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
    pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
    pfile_in_zip_read_info->pos_local_extrafield=0;
    pfile_in_zip_read_info->raw=raw;

    if (pfile_in_zip_read_info->read_buffer==NULL)
    {
        TRYFREE(pfile_in_zip_read_info);
        return UNZ_INTERNALERROR;
    }

    pfile_in_zip_read_info->stream_initialised=0;

    if (method!=NULL)
        *method = (int)s->cur_file_info.compression_method;

    if (level!=NULL)
    {
        *level = 6;
        switch (s->cur_file_info.flag & 0x06)
        {
        case 6 : *level = 1; break;
        case 4 : *level = 2; break;
        case 2 : *level = 9; break;
        }
    }

    if ((s->cur_file_info.compression_method!=0) &&
        (s->cur_file_info.compression_method!=Z_DEFLATED))
        err=UNZ_BADZIPFILE;

    pfile_in_zip_read_info->crc32_wait=s->cur_file_info.crc;
    pfile_in_zip_read_info->crc32=0;
    pfile_in_zip_read_info->compression_method =
        s->cur_file_info.compression_method;
    pfile_in_zip_read_info->filestream=s->filestream;
    pfile_in_zip_read_info->z_filefunc=s->z_filefunc;
    pfile_in_zip_read_info->byte_before_the_zipfile=s->byte_before_the_zipfile;

    pfile_in_zip_read_info->stream.total_out = 0;

    if ((s->cur_file_info.compression_method==Z_DEFLATED) &&
        (!raw))
    {
        pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
        pfile_in_zip_read_info->stream.zfree = (free_func)0;
        pfile_in_zip_read_info->stream.opaque = (voidpf)0;
        pfile_in_zip_read_info->stream.next_in = (Bytef *)0;
        pfile_in_zip_read_info->stream.avail_in = 0;

        err=inflateInit2(&pfile_in_zip_read_info->stream, -MAX_WBITS);
        if (err == Z_OK)
            pfile_in_zip_read_info->stream_initialised=1;
        else
        {
            TRYFREE(pfile_in_zip_read_info);
            return err;
        }
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END.
         * In unzip, i don't wait absolutely Z_STREAM_END because I known the
         * size of both compressed and uncompressed data
         */
    }
    pfile_in_zip_read_info->rest_read_compressed =
        s->cur_file_info.compressed_size ;
    pfile_in_zip_read_info->rest_read_uncompressed =
        s->cur_file_info.uncompressed_size ;


    pfile_in_zip_read_info->pos_in_zipfile =
        s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
        iSizeVar;

    pfile_in_zip_read_info->stream.avail_in = (uInt)0;

    s->pfile_in_zip_read = pfile_in_zip_read_info;

#    ifndef NOUNCRYPT
    if (password != NULL)
    {
        int i;
        s->pcrc_32_tab = get_crc_table();
        init_keys(password,s->keys,s->pcrc_32_tab);
        if (ZSEEK(s->z_filefunc, s->filestream,
                  s->pfile_in_zip_read->pos_in_zipfile +
                  s->pfile_in_zip_read->byte_before_the_zipfile,
                  SEEK_SET)!=0)
            return UNZ_INTERNALERROR;
        if(ZREAD(s->z_filefunc, s->filestream,source, 12)<12)
            return UNZ_INTERNALERROR;

        for (i = 0; i<12; i++)
            zdecode(s->keys,s->pcrc_32_tab,source[i]);

        s->pfile_in_zip_read->pos_in_zipfile+=12;
        s->encrypted=1;
    }
#    endif


    return UNZ_OK;
}

extern int ZEXPORT unzOpenCurrentFile (
    unzFile file)
{
    return unzOpenCurrentFile3(file, NULL, NULL, 0, NULL);
}

extern int ZEXPORT unzOpenCurrentFilePassword (
    unzFile file,
    const char* password)
{
    return unzOpenCurrentFile3(file, NULL, NULL, 0, password);
}

extern int ZEXPORT unzOpenCurrentFile2 (
    unzFile file,
    int* method,
    int* level,
    int raw)
{
    return unzOpenCurrentFile3(file, method, level, raw, NULL);
}

/*
  Read bytes from the current file.
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
  return <0 with error code if there is an error
  (UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/
extern int ZEXPORT unzReadCurrentFile  (
    unzFile file,
    voidp buf,
    unsigned len)
{
    int err=UNZ_OK;
    uInt iRead = 0;
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;


    if ((pfile_in_zip_read_info->read_buffer == NULL))
        return UNZ_END_OF_LIST_OF_FILE;
    if (len==0)
        return 0;

    pfile_in_zip_read_info->stream.next_out = (Bytef*)buf;

    pfile_in_zip_read_info->stream.avail_out = (uInt)len;

    if ((len>pfile_in_zip_read_info->rest_read_uncompressed) &&
        (!(pfile_in_zip_read_info->raw)))
        pfile_in_zip_read_info->stream.avail_out =
            (uInt)pfile_in_zip_read_info->rest_read_uncompressed;

    if ((len>pfile_in_zip_read_info->rest_read_compressed+
         pfile_in_zip_read_info->stream.avail_in) &&
        (pfile_in_zip_read_info->raw))
        pfile_in_zip_read_info->stream.avail_out =
            (uInt)pfile_in_zip_read_info->rest_read_compressed+
            pfile_in_zip_read_info->stream.avail_in;

    while (pfile_in_zip_read_info->stream.avail_out>0)
    {
        if ((pfile_in_zip_read_info->stream.avail_in==0) &&
            (pfile_in_zip_read_info->rest_read_compressed>0))
        {
            uInt uReadThis = UNZ_BUFSIZE;
            if (pfile_in_zip_read_info->rest_read_compressed<uReadThis)
                uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
            if (uReadThis == 0)
                return UNZ_EOF;
            if (ZSEEK(pfile_in_zip_read_info->z_filefunc,
                      pfile_in_zip_read_info->filestream,
                      pfile_in_zip_read_info->pos_in_zipfile +
                      pfile_in_zip_read_info->byte_before_the_zipfile,
                      ZLIB_FILEFUNC_SEEK_SET)!=0)
                return UNZ_ERRNO;
            if (ZREAD(pfile_in_zip_read_info->z_filefunc,
                      pfile_in_zip_read_info->filestream,
                      pfile_in_zip_read_info->read_buffer,
                      uReadThis)!=uReadThis)
                return UNZ_ERRNO;


#            ifndef NOUNCRYPT
            if(s->encrypted)
            {
                uInt i;
                for(i=0;i<uReadThis;i++)
                    pfile_in_zip_read_info->read_buffer[i] =
                        zdecode(s->keys,s->pcrc_32_tab,
                                pfile_in_zip_read_info->read_buffer[i]);
            }
#            endif


            pfile_in_zip_read_info->pos_in_zipfile += uReadThis;

            pfile_in_zip_read_info->rest_read_compressed-=uReadThis;

            pfile_in_zip_read_info->stream.next_in =
                (Bytef*)pfile_in_zip_read_info->read_buffer;
            pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
        }

        if ((pfile_in_zip_read_info->compression_method==0) || (pfile_in_zip_read_info->raw))
        {
            uInt uDoCopy,i ;

            if ((pfile_in_zip_read_info->stream.avail_in == 0) &&
                (pfile_in_zip_read_info->rest_read_compressed == 0))
                return (iRead==0) ? UNZ_EOF : iRead;

            if (pfile_in_zip_read_info->stream.avail_out <
                pfile_in_zip_read_info->stream.avail_in)
                uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
            else
                uDoCopy = pfile_in_zip_read_info->stream.avail_in ;

            for (i=0;i<uDoCopy;i++)
                *(pfile_in_zip_read_info->stream.next_out+i) =
                    *(pfile_in_zip_read_info->stream.next_in+i);

            pfile_in_zip_read_info->crc32 = crc32(pfile_in_zip_read_info->crc32,
                                                  pfile_in_zip_read_info->stream.next_out,
                                                  uDoCopy);
            pfile_in_zip_read_info->rest_read_uncompressed-=uDoCopy;
            pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
            pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
            pfile_in_zip_read_info->stream.next_out += uDoCopy;
            pfile_in_zip_read_info->stream.next_in += uDoCopy;
            pfile_in_zip_read_info->stream.total_out += uDoCopy;
            iRead += uDoCopy;
        }
        else
        {
            uLong uTotalOutBefore,uTotalOutAfter;
            const Bytef *bufBefore;
            uLong uOutThis;
            int flush=Z_SYNC_FLUSH;

            uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
            bufBefore = pfile_in_zip_read_info->stream.next_out;

            /*
              if ((pfile_in_zip_read_info->rest_read_uncompressed ==
              pfile_in_zip_read_info->stream.avail_out) &&
              (pfile_in_zip_read_info->rest_read_compressed == 0))
              flush = Z_FINISH;
            */
            err=inflate(&pfile_in_zip_read_info->stream,flush);

            if ((err>=0) && (pfile_in_zip_read_info->stream.msg!=NULL))
                err = Z_DATA_ERROR;

            uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
            uOutThis = uTotalOutAfter-uTotalOutBefore;

            pfile_in_zip_read_info->crc32 =
                crc32(pfile_in_zip_read_info->crc32,bufBefore,
                      (uInt)(uOutThis));

            pfile_in_zip_read_info->rest_read_uncompressed -=
                uOutThis;

            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

            if (err==Z_STREAM_END)
                return (iRead==0) ? UNZ_EOF : iRead;
            if (err!=Z_OK)
                break;
        }
    }

    if (err==Z_OK)
        return iRead;
    return err;
}


/*
  Give the current position in uncompressed data
*/
extern z_off_t ZEXPORT unztell (
    unzFile file)
{
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;

    return (z_off_t)pfile_in_zip_read_info->stream.total_out;
}


/*
  return 1 if the end of file was reached, 0 elsewhere
*/
extern int ZEXPORT unzeof (
    unzFile file)
{
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;

    if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
        return 1;
    else
        return 0;
}



/*
  Read extra field from the current file (opened by unzOpenCurrentFile)
  This is the local-header version of the extra field (sometimes, there is
  more info in the local-header version than in the central-header)

  if buf==NULL, it return the size of the local extra field that can be read

  if buf!=NULL, len is the size of the buffer, the extra header is copied in
  buf.
  the return value is the number of bytes copied in buf, or (if <0)
  the error code
*/
extern int ZEXPORT unzGetLocalExtrafield (
    unzFile file,
    voidp buf,
    unsigned len)
{
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    uInt read_now;
    uLong size_to_read;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;

    size_to_read = (pfile_in_zip_read_info->size_local_extrafield -
                    pfile_in_zip_read_info->pos_local_extrafield);

    if (buf==NULL)
        return (int)size_to_read;

    if (len>size_to_read)
        read_now = (uInt)size_to_read;
    else
        read_now = (uInt)len ;

    if (read_now==0)
        return 0;

    if (ZSEEK(pfile_in_zip_read_info->z_filefunc,
              pfile_in_zip_read_info->filestream,
              pfile_in_zip_read_info->offset_local_extrafield +
              pfile_in_zip_read_info->pos_local_extrafield,
              ZLIB_FILEFUNC_SEEK_SET)!=0)
        return UNZ_ERRNO;

    if (ZREAD(pfile_in_zip_read_info->z_filefunc,
              pfile_in_zip_read_info->filestream,
              buf,read_now)!=read_now)
        return UNZ_ERRNO;

    return (int)read_now;
}

/*
  Close the file in zip opened with unzipOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/
extern int ZEXPORT unzCloseCurrentFile (
    unzFile file)
{
    int err=UNZ_OK;

    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;


    if ((pfile_in_zip_read_info->rest_read_uncompressed == 0) &&
        (!pfile_in_zip_read_info->raw))
    {
        if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
            err=UNZ_CRCERROR;
    }


    TRYFREE(pfile_in_zip_read_info->read_buffer);
    pfile_in_zip_read_info->read_buffer = NULL;
    if (pfile_in_zip_read_info->stream_initialised)
        inflateEnd(&pfile_in_zip_read_info->stream);

    pfile_in_zip_read_info->stream_initialised = 0;
    TRYFREE(pfile_in_zip_read_info);

    s->pfile_in_zip_read=NULL;

    return err;
}


/*
  Get the global comment string of the ZipFile, in the szComment buffer.
  uSizeBuf is the size of the szComment buffer.
  return the number of byte copied or an error code <0
*/
extern int ZEXPORT unzGetGlobalComment (
    unzFile file,
    char *szComment,
    uLong uSizeBuf)
{
    unz_s* s;
    uLong uReadThis ;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;

    uReadThis = uSizeBuf;
    if (uReadThis>s->gi.size_comment)
        uReadThis = s->gi.size_comment;

    if (ZSEEK(s->z_filefunc,s->filestream,s->central_pos+22,ZLIB_FILEFUNC_SEEK_SET)!=0)
        return UNZ_ERRNO;

    if (uReadThis>0)
    {
        *szComment='\0';
        if (ZREAD(s->z_filefunc,s->filestream,szComment,uReadThis)!=uReadThis)
            return UNZ_ERRNO;
    }

    if ((szComment != NULL) && (uSizeBuf > s->gi.size_comment))
        *(szComment+s->gi.size_comment)='\0';
    return (int)uReadThis;
}

extern uLong ZEXPORT unzGetOffset (
    unzFile file)
{
    unz_s* s;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (!s->current_file_ok)
        return 0;
    if (s->gi.number_entry != 0 && s->gi.number_entry != 0xffff)
        if (s->num_file==s->gi.number_entry)
            return 0;
    return s->pos_in_central_dir;
}

extern int ZEXPORT unzSetOffset (
    unzFile file,
    uLong pos)
{
    unz_s* s;
    int err;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;

    s->pos_in_central_dir = pos;
    s->num_file = s->gi.number_entry;      /* hack */
    err = unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                              &s->cur_file_info_internal,
                                              NULL,0,NULL,0,NULL,0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}
