/* Arduino SdFat Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SdFat_h
#define SdFat_h

#include "Sd2Card.h"
#include "FatStructs.h"

// flags for ls()
/** ls() flag to print modify date */
#define LS_DATE (1)
/** ls() flag to print file size */
#define LS_SIZE (2)
/** ls() flag for recursive list of subdirectories */
#define LS_R (4)
/** ls() flag to print file size */
#define LS_INDENT (4)

// use the gnu style oflag in open()
/** open() oflag for reading */
#define O_READ (0x01)
/** open() oflag - same as O_READ */
#define O_RDONLY O_READ
/** open() oflag for write */
#define O_WRITE (0x02)
/** open() oflag - same as O_WRITE */
#define O_WRONLY O_WRITE
/** open() oflag for reading and writing */
#define O_RDWR (O_READ | O_WRITE)
/** open() oflag mask for access modes */
#define O_ACCMODE (O_READ | O_WRITE)
/** The file offset shall be set to the end of the file prior to each write. */
#define O_APPEND (0x04)
/** synchronous writes - call sync() after each write */
#define O_SYNC (0x08)
/** create the file if nonexistent */
#define O_CREAT (0x10)
/** If O_CREAT and O_EXCL are set, open() shall fail if the file exists */
#define O_EXCL (0x20)
/** truncate the file to zero length */
#define O_TRUNC (0x40)

// flags for timestamp
/** set the file's last access date */
#define T_ACCESS (1)
/** set the file's creation date and time */
#define T_CREATE (2)
/** Set the file's write date and time */
#define T_WRITE (4)
// values for type_
/** This SdFile has not been opened. */
#define FAT_FILE_TYPE_CLOSED (0)
/** SdFile for a file */
#define FAT_FILE_TYPE_NORMAL (1)
/** SdFile for a FAT16 root directory */
#define FAT_FILE_TYPE_ROOT16 (2)
/** SdFile for a FAT32 root directory */
#define FAT_FILE_TYPE_ROOT32 (3)
/** SdFile for a subdirectory */
#define FAT_FILE_TYPE_SUBDIR (4)
/** Test value for directory type */
#define FAT_FILE_TYPE_MIN_DIR FAT_FILE_TYPE_ROOT16

// bits defined in flags_
// should be 0xF
#define F_OFLAG (O_ACCMODE | O_APPEND | O_SYNC)
// available bits
#define F_UNUSED (0x30)
// use unbuffered SD read
#define F_FILE_UNBUFFERED_READ (0x40)
// sync of directory entry required
#define F_FILE_DIR_DIRTY (0x80)

// make sure F_OFLAG is ok
#if ((F_UNUSED | F_FILE_UNBUFFERED_READ | F_FILE_DIR_DIRTY) & F_OFLAG)
#error flags_ bits conflict
#endif  // flags_ bits

// value for action argument in cacheRawBlock to indicate read from cache
#define CACHE_FOR_READ (0)
// value for action argument in cacheRawBlock to indicate cache dirty
#define CACHE_FOR_WRITE (1)

__packed union cache_t {
           /** Used to access cached file data blocks. */
  uint8_t  data[512];
           /** Used to access cached FAT16 entries. */
  uint16_t fat16[256];
           /** Used to access cached FAT32 entries. */
  uint32_t fat32[128];
           /** Used to access cached directory entries. */
  dir_t    dir[16];
           /** Used to access a cached MasterBoot Record. */
  mbr_t    mbr;
           /** Used to access to a cached FAT boot sector. */
  fbs_t    fbs;
};

typedef __packed struct {
  __packed union cache_t cacheBuffer_;        // 512 byte cache for device blocks
  uint32_t  cacheBlockNumber_;  // Logical number of block in the cache
  st_Sd2Card_v* sdCard_;            // Sd2Card object for cache
  uint8_t   cacheDirty_;         // cacheFlush() will write block if true
  uint32_t  cacheMirrorBlock_;  // block number for mirror FAT
  uint32_t  allocSearchStart_;   // start cluster for alloc search
  uint8_t   blocksPerCluster_;    // cluster size in blocks
  uint32_t  blocksPerFat_;       // FAT size in blocks
  uint32_t  clusterCount_;       // clusters in one FAT
  uint8_t   clusterSizeShift_;    // shift to convert cluster count to block count
  uint32_t  dataStartBlock_;     // first data block number
  uint8_t   fatCount_;            // number of FATs on volume
  uint32_t  fatStartBlock_;      // start block for first FAT
  uint8_t 	fatType_;             // volume type (12, 16, OR 32)
  uint16_t	rootDirEntryCount_;  // number of entries in FAT16 root dir
  uint32_t	rootDirStart_;       // root start block for FAT16, cluster for FAT32
} st_SdVolume_v;

typedef struct {
  int       write_error_;   // come from Print class
  uint8_t   flags_;         // See above for definition of flags_ bits
  uint8_t   type_;          // type of file see above for values
  uint32_t  curCluster_;    // cluster for current file position
  uint32_t  curPosition_;   // current file position in bytes from beginning
  uint32_t  dirBlock_;      // SD block that contains directory entry for file
  uint8_t   dirIndex_;      // index of entry in SdFile_dirBlock 0 <= dirIndex_ <= 0XF
  uint32_t  fileSize_;      // file size in bytes
  uint32_t  firstCluster_;  // first cluster of file
  st_SdVolume_v *vol_;           // volume where file is located
} st_SdFile_v;

void     SdFile_setWriteError(st_SdFile_v *v);
int      SdFile_getWriteError(st_SdFile_v *v);
void     SdFile_clearWriteError(st_SdFile_v *v);
uint8_t  SdFile_addCluster(st_SdFile_v *v);
uint8_t  SdFile_addDirCluster(st_SdFile_v *v);
dir_t*   SdFile_cacheDirEntry(st_SdFile_v *v, uint8_t action);
uint8_t  SdFile_make83Name(const char* str, uint8_t* name);
uint8_t  SdFile_close(st_SdFile_v *v);
uint32_t SdFile_curCluster(st_SdFile_v *v);
uint32_t SdFile_curPosition(st_SdFile_v *v);
void     SdFile_dateTime(uint16_t* date, uint16_t* time);
void     SdFile_dirName(const dir_t* dir, char* name);
void     SdFile_init(st_SdFile_v *v);
boolean  SdFile_isDir(st_SdFile_v *v);
boolean  SdFile_isFile(st_SdFile_v *v);
boolean  SdFile_isOpen(st_SdFile_v *v);
boolean  SdFile_isSubDir(st_SdFile_v *v);
boolean  SdFile_isRoot(st_SdFile_v *v);
void     SdFile_ls(st_SdFile_v *v, uint8_t flags, uint8_t indent);
uint8_t  SdFile_makeDir(st_SdFile_v *v, st_SdFile_v* dir, const char* dirName);
uint8_t  SdFile_openName(st_SdFile_v *v, st_SdFile_v* dirFile, const char* fileName, uint8_t oflag);
uint8_t  SdFile_openIndex(st_SdFile_v *v, st_SdFile_v *dirFile, uint16_t index, uint8_t oflag);
uint8_t  SdFile_openCachedEntry(st_SdFile_v *v, uint8_t dirIndex, uint8_t oflag);
boolean  SdFile_openRoot(st_SdFile_v *v, st_SdVolume_v *vol);
int16_t  SdFile_readNextByte(st_SdFile_v *v);
int16_t  SdFile_read(st_SdFile_v *v, uint8_t* buf, uint16_t nbyte);
int8_t   SdFile_readDir(st_SdFile_v *v, dir_t* dir);
dir_t*   SdFile_readDirCache(st_SdFile_v *v);
uint8_t  SdFile_remove(st_SdFile_v *v);
uint8_t  SdFile_removeFileName(st_SdFile_v* dirFile, const char* fileName);
uint8_t  SdFile_removeParam(st_SdFile_v *dirFile, const char* fileName);
void     SdFile_rewind(st_SdFile_v *v);
uint8_t  SdFile_rmDir(st_SdFile_v *v);
uint8_t  SdFile_rmRfStar(st_SdFile_v *v);
uint8_t  SdFile_seekCur(st_SdFile_v *v, int32_t pos);
uint8_t  SdFile_seekEnd(st_SdFile_v *v);
uint8_t  SdFile_seekSet(st_SdFile_v *v, uint32_t pos);
uint8_t  SdFile_sync(st_SdFile_v *v);
uint8_t  SdFile_timestamp(st_SdFile_v *v, uint8_t flag, uint16_t year, uint8_t month, uint8_t day,\
                          uint8_t hour, uint8_t minute, uint8_t second);
uint8_t  SdFile_truncate(st_SdFile_v *v, uint32_t length);
uint8_t  SdFile_unbufferedRead(st_SdFile_v *v);
size_t   SdFile_write(st_SdFile_v *v, const uint8_t* buf, uint16_t nbyte);

uint8_t  SdVolume_allocContiguous(st_SdVolume_v *v, uint32_t count, uint32_t *curCluster);
uint8_t  SdVolume_blockOfCluster(st_SdVolume_v *v, uint32_t position);
uint8_t  SdVolume_cacheFlush(st_SdVolume_v *v);
uint8_t  SdVolume_cacheRawBlock(st_SdVolume_v *v, uint32_t blockNumber, uint8_t action);
void     SdVolume_cacheSetDirty(st_SdVolume_v *v);
uint8_t  SdVolume_cacheZeroBlock(st_SdVolume_v *v, uint32_t blockNumber);
uint8_t  SdVolume_chainSize(st_SdVolume_v *v, uint32_t beginCluster, uint32_t* size);
uint32_t SdVolume_clusterStartBlock(st_SdVolume_v *v, uint32_t cluster);
uint8_t  SdVolume_fatGet(st_SdVolume_v *v, uint32_t cluster, uint32_t* value);
uint8_t  SdVolume_fatPut(st_SdVolume_v *v, uint32_t cluster, uint32_t value);
uint8_t  SdVolume_fatPutEOC(st_SdVolume_v *v, uint32_t cluster);
uint8_t  SdVolume_freeChain(st_SdVolume_v *v, uint32_t cluster);
boolean  SdVolume_init(st_SdVolume_v *v, st_Sd2Card_v* dev);
boolean  SdVolume_initPart(st_SdVolume_v *v, st_Sd2Card_v* dev, uint8_t part);
boolean  SdVolume_isEOC(st_SdVolume_v *v, uint32_t cluster);
uint8_t  SdVolume_readData(st_SdVolume_v *v, uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
uint8_t  SdVolume_writeBlock(st_SdVolume_v *v, uint32_t block, const uint8_t* dst);

#endif  // SdFat_h
