/* FILE NAME: SD.h
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

#ifndef __SD_H__
#define __SD_H__

#include "utility/SdFat.h"

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)
#define FILE_WRITE_APPEND (FILE_WRITE | O_APPEND)

// "File" class member variable
typedef struct  {
  char _name[13];      // our name
  st_SdFile_v *_file;  // underlying file pointer
} st_File_v;

// "File" class member function
typedef struct {
  void     (*init)(st_File_v *v);
  int      (*available)(st_File_v *v);
  void     (*close)(st_File_v *v);
  void     (*flush)(st_File_v *v);
  int      (*peek)(st_File_v *v);
  uint32_t (*position)(st_File_v *v);
  size_t   (*print)(st_File_v *v, const uint8_t *buf);         //  low performance, not recommended
  size_t   (*println)(st_File_v *v, const uint8_t *buf);       //  low performance, not recommended
  boolean  (*seek)(st_File_v *v, uint32_t pos);
  uint32_t (*size)(st_File_v *v);
  int      (*read)(st_File_v *v, void *buf, uint16_t nbyte);
  size_t   (*write)(st_File_v *v, const uint8_t *buf, size_t size);
  boolean  (*isDirectory)(st_File_v *v);
  boolean  (*openNextFile)(st_File_v *v, st_File_v *ret, uint8_t mode);
  void     (*rewindDirectory)(st_File_v *v);
  char*    (*name)(st_File_v *v);
} st_File_f;

// "File" class global object
extern const st_File_f File;

// Prototype declareration
void     File_init(st_File_v *v);
int      File_available(st_File_v *v);
void     File_close(st_File_v *v);
void     File_flush(st_File_v *v);
int      File_peek(st_File_v *v);
uint32_t File_position(st_File_v *v);
size_t   File_print(st_File_v *v, const uint8_t *buf);
size_t   File_println(st_File_v *v, const uint8_t *buf);
boolean  File_seek(st_File_v *v, uint32_t pos);
uint32_t File_size(st_File_v *v);
int      File_read(st_File_v *v, uint8_t *buf, uint16_t nbyte);
size_t   File_write(st_File_v *v, const uint8_t *buf, size_t size);
boolean  File_isDirectory(st_File_v *v);
boolean  File_openNextFile(st_File_v *v, st_File_v *ret, uint8_t mode);
void     File_rewindDirectory(st_File_v *v);
char*    File_name(st_File_v *v);

// "SD" class member function
typedef struct {
  boolean (*begin)(uint8_t csPin);
  boolean (*exists)(const char *filepath);
  boolean (*mkdir)(const char *filepath);
  boolean (*remove)(const char *filepath);
  boolean (*open)(const char *filepath, uint8_t mode, st_File_v *v);
  boolean (*rmdir)(const char *filepath);
} st_SD_f;

// "SD" class global object
extern const st_SD_f SD;

// Prototype declareration
boolean File_File_SdFile(st_File_v *v, st_SdFile_v *f, const char *n);
boolean File_File(st_File_v *v);

boolean SD_getNextPathComponent(const char *path, unsigned int *p_offset, char *buffer);
boolean SD_walkPath(const char *filepath, st_SdFile_v *parentDir, \
                boolean (*callback)(st_SdFile_v *parentDir, const char *filePathComponent, \
                boolean isLastComponent));
boolean SD_callback_pathExists(st_SdFile_v *parentDir, const char *filePathComponent, \
                boolean isLastComponent);
boolean SD_callback_makeDirPath(st_SdFile_v *parentDir, const char *filePathComponent, \
                boolean isLastComponent);
boolean SD_callback_remove(st_SdFile_v *parentDir, const char *filePathComponent, \
                boolean isLastComponent);
boolean SD_callback_rmdir(st_SdFile_v *parentDir, const char *filePathComponent, \
                boolean isLastComponent);
st_SdFile_v SD_getParentDir(const char *filepath, int *index);

boolean SD_begin(uint8_t csPin);
boolean SD_exists(const char *filepath);
boolean SD_mkdir(const char *filepath);
boolean SD_rmdir(const char *filepath);
boolean SD_open(const char *filepath, uint8_t mode, st_File_v *v);
boolean SD_rmdir(const char *filepath);
boolean SD_remove(const char *filepath);

extern st_Sd2Card_v  card;
extern st_SdVolume_v volume;
extern st_SdFile_v   root;

#endif
