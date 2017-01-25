/* FILE NAME: SdFile.c
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This Library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "SdFat.h"
#include "rtc.h"
#include <string.h>

//------------------------------------------------------------------------------
void SdFile_setWriteError(st_SdFile_v *v) {
	v->write_error_ = 1;
}
//------------------------------------------------------------------------------
int SdFile_getWriteError(st_SdFile_v *v) {
	return v->write_error_;
}
//------------------------------------------------------------------------------
void SdFile_clearWriteError(st_SdFile_v *v) {
	v->write_error_ = 0;
}
//------------------------------------------------------------------------------
// add a cluster to a file
uint8_t SdFile_addCluster(st_SdFile_v *v) {
  if (!SdVolume_allocContiguous(v->vol_, (uint32_t)1, &v->curCluster_)) return false;

  // if first cluster of file link to directory entry
  if (v->firstCluster_ == 0) {
    v->firstCluster_ = v->curCluster_;
    v->flags_ |= F_FILE_DIR_DIRTY;
  }
  return true;
}
//------------------------------------------------------------------------------
// Add a cluster to a directory file and zero the cluster.
// return with first block of cluster in the cache
uint8_t SdFile_addDirCluster(st_SdFile_v *v) {
  uint32_t block, i;

  if (!SdFile_addCluster(v)) return false;

  // zero data in cluster insure first cluster is in cache
  block = SdVolume_clusterStartBlock(v->vol_, v->curCluster_);
  for (i = v->vol_->blocksPerCluster_; i != 0; i--) {
    if (!SdVolume_cacheZeroBlock(v->vol_, block + i - 1)) return false;
  }
  // Increase directory file size by cluster size
  v->fileSize_ += 512UL << v->vol_->clusterSizeShift_;
  return true;
}
//------------------------------------------------------------------------------
// cache a file's directory entry
// return pointer to cached entry or null for failure
dir_t* SdFile_cacheDirEntry(st_SdFile_v *v, uint8_t action) {
  if (!SdVolume_cacheRawBlock(v->vol_, v->dirBlock_, action)) return NULL;
  return v->vol_->cacheBuffer_.dir + v->dirIndex_;
}
//------------------------------------------------------------------------------
/**
 *  Close a file and force cached data and directory information
 *  to be written to the storage device.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include no file is open or an I/O error.
 */
uint8_t SdFile_close(st_SdFile_v *v) {
  if (!SdFile_sync(v)) return false;
  v->type_ = FAT_FILE_TYPE_CLOSED;
  return true;
}
//------------------------------------------------------------------------------
/** \return The current position for a file or directory. */
uint32_t SdFile_curPosition(st_SdFile_v *v) {return v->curPosition_;}
//------------------------------------------------------------------------------
void SdFile_dateTime(uint16_t* date, uint16_t* time) {
	uint16_t year, month, day, hour, minute, second;
	year   = (uint16_t) RTC.getYear() + 20;
	month  = (uint16_t) RTC.getMonth();
	day    = (uint16_t) RTC.getDay();
    hour   = (uint16_t) RTC.getHours();
    minute = (uint16_t) RTC.getMinutes();
    second = (uint16_t) RTC.getSeconds();

	*date  = (year << 9) + (month << 5) + day;
	*time  = (hour << 11) + (minute << 5) + second;
}
//------------------------------------------------------------------------------
/**
 * Format the name field of \a dir into the 13 byte array
 * \a name in standard 8.3 short name format.
 *
 * \param[in] dir The directory structure containing the name.
 * \param[out] name A 13 byte char array for the formatted name.
 */
void SdFile_dirName(const dir_t* dir, char* name) {
  uint8_t i;
  uint8_t j = 0;
  for (i = 0; i < 11; i++) {
    if (dir->name[i] == ' ') continue;
    if (i == 8) name[j++] = '.';
    name[j++] = dir->name[i];
  }
  name[j] = 0;
}
//------------------------------------------------------------------------------
/* constructer for SdFile Class */
void SdFile_init(st_SdFile_v *v) {
  v->type_ = FAT_FILE_TYPE_CLOSED;
}
//------------------------------------------------------------------------------
/** \return True if this is a SdFile for a directory else false. */
boolean SdFile_isDir(st_SdFile_v *v) {return v->type_ >= FAT_FILE_TYPE_MIN_DIR;}
//------------------------------------------------------------------------------
/** \return True if this is a SdFile for a file else false. */
boolean SdFile_isFile(st_SdFile_v *v) {return v->type_ == FAT_FILE_TYPE_NORMAL;}
//------------------------------------------------------------------------------
/** \return True if this is a SdFile for an open file/directory else false. */
boolean SdFile_isOpen(st_SdFile_v *v) {return v->type_ != FAT_FILE_TYPE_CLOSED;}
//------------------------------------------------------------------------------
/** \return True if this is a SdFile for a subdirectory else false. */
boolean SdFile_isSubDir(st_SdFile_v *v) {return v->type_ == FAT_FILE_TYPE_SUBDIR;}
//------------------------------------------------------------------------------
/** \return True if this is a SdFile for the root directory. */
boolean SdFile_isRoot(st_SdFile_v *v) {
    return (boolean)(v->type_ == FAT_FILE_TYPE_ROOT16 || v->type_ == FAT_FILE_TYPE_ROOT32);
}
//------------------------------------------------------------------------------
// format directory name field from a 8.3 name string
uint8_t SdFile_make83Name(const char* str, uint8_t* name) {
  uint8_t c, b;
  uint8_t n = 7;  // max index for part before dot
  uint8_t i = 0;
  uint8_t valid[] = "|<>^+=?/[];,*\"\\";
  uint8_t *p;

  // blank fill name and extension
  while (i < 11) name[i++] = ' ';
  i = 0;
  while ((c = *str++) != '\0') {
    if (c == '.') {
      if (n == 10) return false;  // only one dot allowed
      n = 10;  // max index for full 8.3 name
      i = 8;   // place for extension
    } else {
      // illegal FAT characters
      p = valid;
      while ((b = *p++)) if (b == c) return false;
      // check size and only allow ASCII printable characters
      if (i > n || c < 0x21 || c > 0x7E)return false;
      // only upper case allowed in 8.3 names - convert lower to upper
      name[i++] = (uint8_t)(c < 'a' || c > 'z' ?  c : c + ('A' - 'a'));
    }
  }
  // must have a file name, extension is optional
  return name[0] != ' ';
}
//------------------------------------------------------------------------------
/** Make a new directory.
 *
 * \param[in] dir An open SdFat instance for the directory that will containing
 * the new directory.
 *
 * \param[in] dirName A valid 8.3 DOS name for the new directory.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include this SdFile is already open, \a dir is not a
 * directory, \a dirName is invalid or already exists in \a dir.
 */
uint8_t SdFile_makeDir(st_SdFile_v *v, st_SdFile_v* dir, const char* dirName) {
  dir_t d;
  dir_t* p;
  uint8_t i;
  uint32_t block;

  // create a normal file
  if (!SdFile_openName(v, dir, dirName, O_CREAT | O_EXCL | O_RDWR)) return false;

  // convert SdFile to directory
  v->flags_ = O_READ;
  v->type_ = FAT_FILE_TYPE_SUBDIR;

  // allocate and zero first cluster
  if (!SdFile_addDirCluster(v))return false;

  // force entry to SD
  if (!SdFile_sync(v)) return false;

  // cache entry - should already be in cache due to sync() call
  p = SdFile_cacheDirEntry(v, CACHE_FOR_WRITE);
  if (!p) return false;

  // change directory entry  attribute
  p->attributes = DIR_ATT_DIRECTORY;

  // make entry for '.'
  memcpy(&d, p, sizeof(d));
  for (i = 1; i < 11; i++) d.name[i] = ' ';
  d.name[0] = '.';

  // cache block for '.'  and '..'
  block = SdVolume_clusterStartBlock(v->vol_, v->firstCluster_);
  if (!SdVolume_cacheRawBlock(v->vol_, block, CACHE_FOR_WRITE)) return false;

  // copy '.' to block
  memcpy(&v->vol_->cacheBuffer_.dir[0], &d, sizeof(d));

  // make entry for '..'
  d.name[1] = '.';
  if (SdFile_isRoot(dir)) {
    d.firstClusterLow = 0;
    d.firstClusterHigh = 0;
  } else {
    d.firstClusterLow = (uint16_t)(dir->firstCluster_ & 0xFFFF);
    d.firstClusterHigh = (uint16_t)(dir->firstCluster_ >> 16);
  }
  // copy '..' to block
  memcpy(&v->vol_->cacheBuffer_.dir[1], &d, sizeof(d));

  // set position after '..'
  v->curPosition_ = 2 * sizeof(d);

  // write first block
  return SdVolume_cacheFlush(v->vol_);
}
//------------------------------------------------------------------------------
/**
 * Open a file or directory by name.
 *
 * \param[in] dirFile An open SdFat instance for the directory containing the
 * file to be opened.
 *
 * \param[in] fileName A valid 8.3 DOS name for a file to be opened.
 *
 * \param[in] oflag Values for \a oflag are constructed by a bitwise-inclusive
 * OR of flags from the following list
 *
 * O_READ - Open for reading.
 *
 * O_RDONLY - Same as O_READ.
 *
 * O_WRITE - Open for writing.
 *
 * O_WRONLY - Same as O_WRITE.
 *
 * O_RDWR - Open for reading and writing.
 *
 * O_APPEND - If set, the file offset shall be set to the end of the
 * file prior to each write.
 *
 * O_CREAT - If the file exists, this flag has no effect except as noted
 * under O_EXCL below. Otherwise, the file shall be created
 *
 * O_EXCL - If O_CREAT and O_EXCL are set, open() shall fail if the file exists.
 *
 * O_SYNC - Call sync() after each write.  This flag should not be used with
 * write(uint8_t), write_P(PGM_P), writeln_P(PGM_P), or the Arduino Print class.
 * These functions do character at a time writes so sync() will be called
 * after each byte.
 *
 * O_TRUNC - If the file exists and is a regular file, and the file is
 * successfully opened and is not read only, its length shall be truncated to 0.
 *
 * \note Directory files must be opened read only.  Write and truncation is
 * not allowed for directory files.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include this SdFile is already open, \a difFile is not
 * a directory, \a fileName is invalid, the file does not exist
 * or can't be opened in the access mode specified by oflag.
 */
uint8_t SdFile_openName(st_SdFile_v *v, st_SdFile_v* dirFile, const char* fileName, uint8_t oflag) {
  uint8_t dname[11];
  dir_t* p;
  uint8_t emptyFound = false;
  uint8_t index;

  // error if already open
  if (SdFile_isOpen(v))return false;

  if (!SdFile_make83Name(fileName, dname)) return false;
  v->vol_ = dirFile->vol_;
  SdFile_rewind(dirFile);

  // search for file
  while (dirFile->curPosition_ < dirFile->fileSize_) {
    index = (uint8_t)(0xF & (dirFile->curPosition_ >> 5));
    p = SdFile_readDirCache(dirFile);
    if (p == NULL) return false;

    if (p->name[0] == DIR_NAME_FREE || p->name[0] == DIR_NAME_DELETED) {
      // remember first empty slot
      if (!emptyFound) {
        emptyFound = true;
        v->dirIndex_ = index;
        v->dirBlock_ = v->vol_->cacheBlockNumber_;
      }
      // done if no entries follow
      if (p->name[0] == DIR_NAME_FREE) break;
    } else if (!memcmp(dname, p->name, 11)) {
      // don't open existing file if O_CREAT and O_EXCL
      if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return false;

      // open found file
      return SdFile_openCachedEntry(v, (uint8_t)(0xF & index), oflag);
    }
  }
  // only create file if O_CREAT and O_WRITE
  if ((oflag & (O_CREAT | O_WRITE)) != (O_CREAT | O_WRITE)) return false;

  // cache found slot or add cluster if end of file
  if (emptyFound) {
    p = SdFile_cacheDirEntry(v, CACHE_FOR_WRITE);
    if (!p) return false;
  } else {
    if (dirFile->type_ == FAT_FILE_TYPE_ROOT16) return false;

    // add and zero cluster for dirFile - first cluster is in cache for write
    if (!SdFile_addDirCluster(dirFile)) return false;

    // use first entry in cluster
    v->dirIndex_ = 0;
    p = v->vol_->cacheBuffer_.dir;
  }
  // initialize as empty file
  memset(p, 0, sizeof(dir_t));
  memcpy(p->name, dname, 11);

  // set timestamps
  SdFile_dateTime(&p->creationDate, &p->creationTime);
  p->lastAccessDate = p->creationDate;
  p->lastWriteDate = p->creationDate;
  p->lastWriteTime = p->creationTime;

  // force write of entry to SD
  if (!SdVolume_cacheFlush(v->vol_)) return false;

  // open entry in cache
  return SdFile_openCachedEntry(v, v->dirIndex_, oflag);
}
//------------------------------------------------------------------------------
/**
 * Open a file by index.
 *
 * \param[in] dirFile An open SdFat instance for the directory.
 *
 * \param[in] index The \a index of the directory entry for the file to be
 * opened.  The value for \a index is (directory file position)/32.
 *
 * \param[in] oflag Values for \a oflag are constructed by a bitwise-inclusive
 * OR of flags O_READ, O_WRITE, O_TRUNC, and O_SYNC.
 *
 * See open() by fileName for definition of flags and return values.
 *
 */
uint8_t SdFile_openIndex(st_SdFile_v *v, st_SdFile_v *dirFile, uint16_t index, uint8_t oflag) {
  dir_t* p;

  // error if already open
  if (SdFile_isOpen(v))return false;

  // don't open existing file if O_CREAT and O_EXCL - user call error
  if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return false;

  v->vol_ = dirFile->vol_;

  // seek to location of entry
  if (!SdFile_seekSet(dirFile, (uint32_t)(32 * index))) return false;

  // read entry into cache
  p = SdFile_readDirCache(dirFile);
  if (p == NULL) return false;

  // error if empty slot or '.' or '..'
  if (p->name[0] == DIR_NAME_FREE ||
      p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') {
    return false;
  }
  // open cached entry
  return SdFile_openCachedEntry(v, (uint8_t)(index & 0xF), oflag);
}
//------------------------------------------------------------------------------
// open a cached directory entry. Assumes v->vol_ is initializes
uint8_t SdFile_openCachedEntry(st_SdFile_v *v, uint8_t dirIndex, uint8_t oflag) {
  // location of entry in cache
  dir_t* p = v->vol_->cacheBuffer_.dir + dirIndex;  // W4028(1) warning is no problem after comfirmation.

  // write or truncate is an error for a directory or read-only file
  if (p->attributes & (DIR_ATT_READ_ONLY | DIR_ATT_DIRECTORY)) {
    if (oflag & (O_WRITE | O_TRUNC)) return false;
  }
  // remember location of directory entry on SD
  v->dirIndex_ = dirIndex;
  v->dirBlock_ = v->vol_->cacheBlockNumber_;

  // copy first cluster number for directory fields
  v->firstCluster_ = (uint32_t)p->firstClusterHigh << 16;
  v->firstCluster_ |= p->firstClusterLow;

  // make sure it is a normal file or subdirectory
  if (DIR_IS_FILE(p)) {
    v->fileSize_ = p->fileSize;
    v->type_ = FAT_FILE_TYPE_NORMAL;
  } else if (DIR_IS_SUBDIR(p)) {
    if (!SdVolume_chainSize(v->vol_, v->firstCluster_, &v->fileSize_)) return false;
    v->type_ = FAT_FILE_TYPE_SUBDIR;
  } else {
    return false;
  }
  // save open flags for read/write
  v->flags_ = (uint8_t)(oflag & (O_ACCMODE | O_SYNC | O_APPEND));

  // set to start of file
  v->curCluster_ = 0;
  v->curPosition_ = 0;

  // truncate file to zero length if requested
  if (oflag & O_TRUNC) return SdFile_truncate(v, (uint32_t)0);
  return true;
}
//------------------------------------------------------------------------------
/**
 * Open a volume's root directory.
 *
 * \param[in] vol The FAT volume containing the root directory to be opened.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the FAT volume has not been initialized
 * or it a FAT12 volume.
 */
boolean SdFile_openRoot(st_SdFile_v *v, st_SdVolume_v *vol) {
  // error if file is already open
  if (SdFile_isOpen(v)) return false;

  if (vol->fatType_ == 16) {
    v->type_ = FAT_FILE_TYPE_ROOT16;
    v->firstCluster_ = 0;
    v->fileSize_ = 32 * vol->rootDirEntryCount_;
  } else if (vol->fatType_ == 32) {
    v->type_ = FAT_FILE_TYPE_ROOT32;
    v->firstCluster_ = vol->rootDirStart_;
    if (!SdVolume_chainSize(vol, v->firstCluster_, &v->fileSize_)) return false;
  } else {
    // volume is not initialized or FAT12
    return false;
  }
  v->vol_ = vol;
  // read only
  v->flags_ = O_READ;

  // set to start of file
  v->curCluster_ = 0;
  v->curPosition_ = 0;

  // root has no directory entry
  v->dirBlock_ = 0;
  v->dirIndex_ = 0;
  return true;
}
//------------------------------------------------------------------------------
/**
 * Read the next byte from a file.
 *
 * \return For success read returns the next byte in the file as an int.
 * If an error occurs or end of file is reached -1 is returned.
 */
int16_t SdFile_readNextByte(st_SdFile_v *v) {
  uint8_t b;
  return SdFile_read(v, &b, 1) == 1 ? b : -1;
}
//------------------------------------------------------------------------------
/**
 * Read data from a file starting at the current position.
 *
 * \param[out] buf Pointer to the location that will receive the data.
 *
 * \param[in] nbyte Maximum number of bytes to read.
 *
 * \return For success read() returns the number of bytes read.
 * A value less than \a nbyte, including zero, will be returned
 * if end of file is reached.
 * If an error occurs, read() returns -1.  Possible errors include
 * read() called before a file has been opened, corrupt file system
 * or an I/O error occurred.
 */
int16_t SdFile_read(st_SdFile_v *v, uint8_t* buf, uint16_t nbyte) {
  uint8_t* dst = buf;
  uint8_t *src, *end;
  uint16_t toRead, offset, n;
  uint32_t block;  // raw device block number
  uint8_t blockOfCluster;

  // error if not open or write only
  if (!SdFile_isOpen(v) || !(v->flags_ & O_READ)) return -1;

  // max bytes left in file
  if (nbyte > (v->fileSize_ - v->curPosition_)) nbyte = (uint16_t)(v->fileSize_ - v->curPosition_);

  // amount left to read
  toRead = nbyte;
  while (toRead > 0) {
    offset = (uint16_t)(v->curPosition_ & 0x1FF);  // offset in block
    if (v->type_ == FAT_FILE_TYPE_ROOT16) {
      block = v->vol_->rootDirStart_ + (v->curPosition_ >> 9);
    } else {
      blockOfCluster = SdVolume_blockOfCluster(v->vol_, v->curPosition_);
      if (offset == 0 && blockOfCluster == 0) {
        // start of new cluster
        if (v->curPosition_ == 0) {
          // use first cluster in file
          v->curCluster_ = v->firstCluster_;
        } else {
          // get next cluster from FAT
          if (!SdVolume_fatGet(v->vol_, v->curCluster_, &v->curCluster_)) return -1;
        }
      }
      block = SdVolume_clusterStartBlock(v->vol_, v->curCluster_) + blockOfCluster;
    }
    n = toRead;

    // amount to be read from current block
    if (n > (512 - offset)) n = 512 - offset;

    // no buffering needed if n == 512 or user requests no buffering
    if ((SdFile_unbufferedRead(v) || n == 512) &&
      block != v->vol_->cacheBlockNumber_) {
      if (!SdVolume_readData(v->vol_, block, offset, n, dst)) return -1;
      dst += n;
    } else {
      // read block to cache and copy data to caller
      if (!SdVolume_cacheRawBlock(v->vol_, block, CACHE_FOR_READ)) return -1;
      src = v->vol_->cacheBuffer_.data + offset;
      end = src + n;
      while (src != end) *dst++ = *src++;
    }
    v->curPosition_ += n;
    toRead -= n;
  }
  return nbyte;
}
//------------------------------------------------------------------------------
/**
 * Read the next directory entry from a directory file.
 *
 * \param[out] dir The dir_t struct that will receive the data.
 *
 * \return For success readDir() returns the number of bytes read.
 * A value of zero will be returned if end of file is reached.
 * If an error occurs, readDir() returns -1.  Possible errors include
 * readDir() called before a directory has been opened, this is not
 * a directory file or an I/O error occurred.
 */
int8_t SdFile_readDir(st_SdFile_v *v, dir_t* dir) {
  int8_t n;
  // if not a directory file or miss-positioned return an error
  if (!SdFile_isDir(v) || (0x1F & v->curPosition_)) return -1;

  while ((n = (int8_t)SdFile_read(v, (uint8_t*)dir, sizeof(dir_t))) == sizeof(dir_t)) {
    // last entry if DIR_NAME_FREE
    if (dir->name[0] == DIR_NAME_FREE) break;
    // skip empty entries and entry for .  and ..
    if (dir->name[0] == DIR_NAME_DELETED || dir->name[0] == '.') continue;
    // return if normal file or subdirectory
    if (DIR_IS_FILE_OR_SUBDIR(dir)) return n;
  }
  // error, end of file, or past last entry
  return (int8_t)(n < 0 ? -1 : 0);
}
//------------------------------------------------------------------------------
// Read next directory entry into the cache
// Assumes file is correctly positioned
dir_t* SdFile_readDirCache(st_SdFile_v *v) {
  uint8_t i;

  // error if not directory
  if (!SdFile_isDir(v)) return NULL;

  // index of entry in cache
  i = (uint8_t)((v->curPosition_ >> 5) & 0xF);

  // use read to locate and cache block
  if (SdFile_readNextByte(v) < 0) return NULL;

  // advance to next entry
  v->curPosition_ += 31;

  // return pointer to entry
  return (v->vol_->cacheBuffer_.dir + i);
}
//------------------------------------------------------------------------------
/**
 * Remove a file.
 *
 * The directory entry and all data for the file are deleted.
 *
 * \note This function should not be used to delete the 8.3 version of a
 * file that has a long name. For example if a file has the long name
 * "New Text Document.txt" you should not delete the 8.3 name "NEWTEX~1.TXT".
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the file read-only, is a directory,
 * or an I/O error occurred.
 */
uint8_t SdFile_remove(st_SdFile_v *v) {
  dir_t* d;

  // free any clusters - will fail if read-only or directory
  if (!SdFile_truncate(v, (uint32_t)0)) return false;

  // cache directory entry
  d = SdFile_cacheDirEntry(v, CACHE_FOR_WRITE);
  if (!d) return false;

  // mark entry deleted
  d->name[0] = DIR_NAME_DELETED;

  // set this SdFile closed
  v->type_ = FAT_FILE_TYPE_CLOSED;

  // write entry to SD
  return SdVolume_cacheFlush(v->vol_);
}
//------------------------------------------------------------------------------
/**
 * Remove a file.
 *
 * The directory entry and all data for the file are deleted.
 *
 * \param[in] dirFile The directory that contains the file.
 * \param[in] fileName The name of the file to be removed.
 *
 * \note This function should not be used to delete the 8.3 version of a
 * file that has a long name. For example if a file has the long name
 * "New Text Document.txt" you should not delete the 8.3 name "NEWTEX~1.TXT".
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the file is a directory, is read only,
 * \a dirFile is not a directory, \a fileName is not found
 * or an I/O error occurred.
 */
uint8_t SdFile_removeFileName(st_SdFile_v* dirFile, const char* fileName) {
  st_SdFile_v file;

  SdFile_init(&file);          // constructor, do not forget!

  if (!SdFile_openName(&file, dirFile, fileName, O_WRITE)) return false;
  return SdFile_remove(&file);
}
//------------------------------------------------------------------------------
/** Set the file's current position to zero. */
void SdFile_rewind(st_SdFile_v *v) {
    v->curPosition_ = 0;
	v->curCluster_ = 0;
}
//------------------------------------------------------------------------------
/** Remove a directory file.
 *
 * The directory file will be removed only if it is empty and is not the
 * root directory.  rmDir() follows DOS and Windows and ignores the
 * read-only attribute for the directory.
 *
 * \note This function should not be used to delete the 8.3 version of a
 * directory that has a long name. For example if a directory has the
 * long name "New folder" you should not delete the 8.3 name "NEWFOL~1".
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include the file is not a directory, is the root
 * directory, is not empty, or an I/O error occurred.
 */
uint8_t SdFile_rmDir(st_SdFile_v *v) {
  // must be open subdirectory
  if (!SdFile_isSubDir(v)) return false;

  SdFile_rewind(v);

  // make sure directory is empty
  while (v->curPosition_ < v->fileSize_) {
    dir_t* p = SdFile_readDirCache(v);
    if (p == NULL) return false;
    // done if past last used entry
    if (p->name[0] == DIR_NAME_FREE) break;
    // skip empty slot or '.' or '..'
    if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;
    // error not empty
    if (DIR_IS_FILE_OR_SUBDIR(p)) return false;
  }
  // convert empty directory to normal file for remove
  v->type_ = FAT_FILE_TYPE_NORMAL;
  v->flags_ |= O_WRITE;
  return SdFile_remove(v);
}
//------------------------------------------------------------------------------
/** Set the files position to current position + \a pos. See seekSet(). */
uint8_t SdFile_seekCur(st_SdFile_v *v, int32_t pos) {
  return SdFile_seekSet(v, v->curPosition_ + pos);
}
//------------------------------------------------------------------------------
/**
 *  Set the files current position to end of file.  Useful to position
 *  a file for append. See seekSet().
 */
uint8_t SdFile_seekEnd(st_SdFile_v *v) {return SdFile_seekSet(v, v->fileSize_);}
//------------------------------------------------------------------------------
/**
 * Sets a file's position.
 *
 * \param[in] pos The new position in bytes from the beginning of the file.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t SdFile_seekSet(st_SdFile_v *v, uint32_t pos) {
  uint32_t nCur, nNew;
  // error if file not open or seek past end of file
  if (!SdFile_isOpen(v) || pos > v->fileSize_) return false;

  if (v->type_ == FAT_FILE_TYPE_ROOT16) {
    v->curPosition_ = pos;
    return true;
  }
  if (pos == 0) {
    // set position to start of file
    v->curCluster_ = 0;
    v->curPosition_ = 0;
    return true;
  }
  // calculate cluster index for cur and new position
  nCur = (v->curPosition_ - 1) >> (v->vol_->clusterSizeShift_ + 9);
  nNew = (pos - 1) >> (v->vol_->clusterSizeShift_ + 9);

  if (nNew < nCur || v->curPosition_ == 0) {
    // must follow chain from first cluster
    v->curCluster_ = v->firstCluster_;
  } else {
    // advance from curPosition
    nNew -= nCur;
  }
  while (nNew--) {
    if (!SdVolume_fatGet(v->vol_, v->curCluster_, &v->curCluster_)) return false;
  }
  v->curPosition_ = pos;
  return true;
}
//------------------------------------------------------------------------------
/**
 * The sync() call causes all modified data and directory fields
 * to be written to the storage device.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include a call to sync() before a file has been
 * opened or an I/O error.
 */
uint8_t SdFile_sync(st_SdFile_v *v) {
  dir_t* d;
  // only allow open files and directories
  if (!SdFile_isOpen(v)) return false;

  if (v->flags_ & F_FILE_DIR_DIRTY) {
    d = SdFile_cacheDirEntry(v, CACHE_FOR_WRITE);
    if (!d) return false;

    // do not set filesize for dir files
    if (!SdFile_isDir(v)) d->fileSize = v->fileSize_;

    // update first cluster fields
    d->firstClusterLow = (uint16_t)(v->firstCluster_ & 0xFFFF);
    d->firstClusterHigh = (uint16_t)(v->firstCluster_ >> 16);

    // set modify time if user supplied a callback date/time function
    SdFile_dateTime(&d->lastWriteDate, &d->lastWriteTime);
    d->lastAccessDate = d->lastWriteDate;
    // clear directory dirty
    v->flags_ &= (uint8_t)(~F_FILE_DIR_DIRTY);
  }
  return SdVolume_cacheFlush(v->vol_);
}

//------------------------------------------------------------------------------
/**
 * Truncate a file to a specified length.  The current file position
 * will be maintained if it is less than or equal to \a length otherwise
 * it will be set to end of file.
 *
 * \param[in] length The desired length for the file.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include file is read only, file is a directory,
 * \a length is greater than the current file size or an I/O error occurs.
 */
uint8_t SdFile_truncate(st_SdFile_v *v, uint32_t length) {
  uint32_t newPos;

  // error if not a normal file or read-only
  if (!SdFile_isFile(v) || !(v->flags_ & O_WRITE)) return false;

  // error if length is greater than current size
  if (length > v->fileSize_) return false;

  // fileSize and length are zero - nothing to do
  if (v->fileSize_ == 0) return true;

  // remember position for seek after truncation
  newPos = v->curPosition_ > length ? length : v->curPosition_;

  // position to last cluster in truncated file
  if (!SdFile_seekSet(v, length)) return false;

  if (length == 0) {
    // free all clusters
    if (!SdVolume_freeChain(v->vol_, v->firstCluster_)) return false;
    v->firstCluster_ = 0;
  } else {
    uint32_t toFree;
    if (!SdVolume_fatGet(v->vol_, v->curCluster_, &toFree)) return false;

    if (!SdVolume_isEOC(v->vol_, toFree)) {
      // free extra clusters
      if (!SdVolume_freeChain(v->vol_, toFree)) return false;

      // current cluster is end of chain
      if (!SdVolume_fatPutEOC(v->vol_, v->curCluster_)) return false;
    }
  }
  v->fileSize_ = length;

  // need to update directory entry
  v->flags_ |= F_FILE_DIR_DIRTY;

  if (!SdFile_sync(v)) return false;

  // set file to correct position
  return SdFile_seekSet(v, newPos);
}
//------------------------------------------------------------------------------
  /** \return Unbuffered read flag. */
uint8_t SdFile_unbufferedRead(st_SdFile_v *v) {
    return (uint8_t)(v->flags_ & F_FILE_UNBUFFERED_READ);
  }
//------------------------------------------------------------------------------
/**
 * Write data to an open file.
 *
 * \note Data is moved to the cache but may not be written to the
 * storage device until sync() is called.
 *
 * \param[in] buf Pointer to the location of the data to be written.
 *
 * \param[in] nbyte Number of bytes to write.
 *
 * \return For success write() returns the number of bytes written, always
 * \a nbyte.  If an error occurs, write() returns 0.  Possible errors
 * include write() is called before a file has been opened, write is called
 * for a read-only file, device is full, a corrupt file system or an I/O error.
 *
 */
size_t SdFile_write(st_SdFile_v *v, const uint8_t* buf, uint16_t nbyte) {
  // convert void* to uint8_t*  -  must be before goto statements
  const uint8_t *src = buf;
  uint8_t blockOfCluster;
  uint16_t blockOffset, n;
  uint32_t next, block;
  uint8_t *dst, *end;

  // number of bytes left to write  -  must be before goto statements
  uint16_t nToWrite = nbyte;

  // error if not a normal file or is read-only
  if (!SdFile_isFile(v) || !(v->flags_ & O_WRITE)) goto writeErrorReturn;

  // seek to end of file if append flag
  if ((v->flags_ & O_APPEND) && v->curPosition_ != v->fileSize_) {
    if (!SdFile_seekEnd(v)) goto writeErrorReturn;
  }

  while (nToWrite > 0) {
    blockOfCluster = SdVolume_blockOfCluster(v->vol_, v->curPosition_);
    blockOffset = (uint16_t)(v->curPosition_ & 0x1FF);
    if (blockOfCluster == 0 && blockOffset == 0) {
      // start of new cluster
      if (v->curCluster_ == 0) {
        if (v->firstCluster_ == 0) {
          // allocate first cluster of file
          if (!SdFile_addCluster(v)) goto writeErrorReturn;
        } else {
          v->curCluster_ = v->firstCluster_;
        }
      } else {
        if (!SdVolume_fatGet(v->vol_, v->curCluster_, &next)) return false;
        if (SdVolume_isEOC(v->vol_, next)) {
          // add cluster if at end of chain
          if (!SdFile_addCluster(v)) goto writeErrorReturn;
        } else {
          v->curCluster_ = next;
        }
      }
    }
    // max space in block
    n = 512 - blockOffset;

    // lesser of space and amount to write
    if (n > nToWrite) n = nToWrite;

    // block for data write
    block = SdVolume_clusterStartBlock(v->vol_, v->curCluster_) + blockOfCluster;
    if (n == 512) {
      // full block - don't need to use cache
      // invalidate cache if block is in cache
      if (v->vol_->cacheBlockNumber_ == block) {
        v->vol_->cacheBlockNumber_ = 0xFFFFFFFF;
      }
      if (!SdVolume_writeBlock(v->vol_, block, src)) goto writeErrorReturn;
      src += 512;
    } else {
      if (blockOffset == 0 && v->curPosition_ >= v->fileSize_) {
        // start of new block don't need to read into cache
        if (!SdVolume_cacheFlush(v->vol_)) goto writeErrorReturn;
        v->vol_->cacheBlockNumber_ = block;
        SdVolume_cacheSetDirty(v->vol_);
      } else {
        // rewrite part of block
        if (!SdVolume_cacheRawBlock(v->vol_, block, CACHE_FOR_WRITE)) {
          goto writeErrorReturn;
        }
      }
      dst = v->vol_->cacheBuffer_.data + blockOffset;
      end = dst + n;
      while (dst != end) *dst++ = *src++;
    }
    nToWrite -= n;
    v->curPosition_ += n;
  }
  if (v->curPosition_ > v->fileSize_) {
    // update fileSize and insure sync will update dir entry
    v->fileSize_ = v->curPosition_;
    v->flags_ |= F_FILE_DIR_DIRTY;
  } else if (nbyte) {
    // insure sync will update modified date and time
    v->flags_ |= F_FILE_DIR_DIRTY;
  }

  if (v->flags_ & O_SYNC) {
    if (!SdFile_sync(v)) goto writeErrorReturn;
  }
  return nbyte;

 writeErrorReturn:
  // return for write error
  //writeError = true;
  SdFile_setWriteError(v);
  return 0;
}

