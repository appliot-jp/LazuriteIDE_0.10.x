/* FILE NAME: SD.c
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

#include <stdlib.h>
#include "utility/Sd2Card.c"
#include "utility/SdFile.c"
#include "utility/SdVolume.c"
#include "SD.h"
#include "SPI.h"

// Used by `SD_getNextPathComponent`
#define MAX_COMPONENT_LEN 12 // What is max length?
#define PATH_COMPONENT_BUFFER_LEN MAX_COMPONENT_LEN+1

st_Sd2Card_v  card;
st_SdVolume_v volume;
st_SdFile_v   root;

boolean File_File_SdFile(st_File_v *v, st_SdFile_v *f, const char *n) {
  v->_file = malloc(sizeof(st_SdFile_v));
  if (v->_file) {
    memcpy(v->_file, f, sizeof(st_SdFile_v));
    
    strncpy(v->_name, n, 12);
    v->_name[12] = 0;
  }
  return SdFile_isOpen(v->_file);
}

boolean File_File(st_File_v *v) {
  File_init(v);
  return false;
}

// constructor to be called directly
void File_init(st_File_v *v) {
  v->_file    = 0;
  v->_name[0] = 0;
}

// a directory is a special type of file
boolean File_isDirectory(st_File_v *v) {
    bool b = false;

    if(v->_file)
    {
        b = SdFile_isDir(v->_file);
    }
    return b;
}

// buffered write for more efficient, high speed writing
size_t File_write(st_File_v *v, const uint8_t *buf, size_t size) {
  size_t t;

  if (!v->_file) {
    return 0;   // write error
  }
  SdFile_clearWriteError(v->_file);
  t = SdFile_write(v->_file, buf, size);
  if (SdFile_getWriteError(v->_file)) {
    return 0;  // write error
  }
  return t;
}

int File_peek(st_File_v *v) {
  int c;

  if (! v->_file) return 0;

  c = SdFile_readNextByte(v->_file);
  if (c != -1) SdFile_seekCur(v->_file, (int32_t)-1);
  return c;
}

// not recommended for permanent use, less efficient and performance
size_t File_print(st_File_v *v, const uint8_t *buf) {
  size_t t = 0;

  while (*buf != NULL) {
    File_write(v, buf, 1);
    buf++;
    t++;
  }
  return t;
}

// not recommended for permanent use, less efficient and performance
size_t File_println(st_File_v *v, const uint8_t *buf) {
  size_t t;
  uint8_t ln[2] = {'\r','\n'};

  t = File_print(v, buf);
  t += File_write(v, ln, sizeof(ln));
  return t;
}

// buffered read for more efficient, high speed reading
int File_read(st_File_v *v, uint8_t *buf, uint16_t nbyte) {
  if (v->_file) 
    return SdFile_read(v->_file, buf, nbyte);
  return 0;
}

int File_available(st_File_v *v) {
  uint32_t n;

  if (! v->_file) return 0;

  n = File_size(v) - File_position(v);

  return (int)(n > 0x7FFF ? 0x7FFF : n);
}

void File_flush(st_File_v *v) {
  if (v->_file)
    SdFile_sync(v->_file);
}

boolean File_seek(st_File_v *v, uint32_t pos) {
  if (! v->_file) return false;

  return SdFile_seekSet(v->_file, pos);
}

uint32_t File_position(st_File_v *v) {
  if (! v->_file) return -1;
  return SdFile_curPosition(v->_file);
}

uint32_t File_size(st_File_v *v) {
  if (! v->_file) return 0;
  return v->_file->fileSize_;
}

void File_close(st_File_v *v) {
  if (v->_file) {
    SdFile_close(v->_file);
    free(v->_file);
    v->_file = 0;

    /* for debugging file open/close leaks
    nfilecount--;
    Serial.print("Deleted ");
    Serial.println(nfilecount, DEC);
    */
  }
}

// allows you to recurse into a directory
boolean File_openNextFile(st_File_v *v, st_File_v *ret, uint8_t mode) {
  dir_t p;
  st_SdFile_v f;
  char name[13];

  SdFile_init(&f);          // constructor, do not forget!
  //Serial.print("\t\treading dir...");
  while (SdFile_readDir(v->_file, &p) > 0) {

    // done if past last used entry
    if (p.name[0] == DIR_NAME_FREE) {
      //Serial.println("end");
      return File_File(ret);
    }

    // skip deleted entry and entries for . and  ..
    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') {
      //Serial.println("dots");
      continue;
    }

    // only list subdirectories and files
    if (!DIR_IS_FILE_OR_SUBDIR(&p)) {
      //Serial.println("notafile");
      continue;
    }

    // print file name with possible blank fill
    SdFile_dirName(&p, name);
    //Serial.print("try to open file ");
    //Serial.println(name);

    if (SdFile_openName(&f, v->_file, name, mode)) {
      //Serial.println("OK!");
      return File_File_SdFile(ret, &f, name);
    } else {
      //Serial.println("ugh");
      return File_File(ret);
    }
  }

  //Serial.println("nothing");
  return File_File(ret);
}

void File_rewindDirectory(st_File_v *v) {  
  if (File_isDirectory(v))
    SdFile_rewind(v->_file);
}

// returns a pointer to the file name
char * File_name(st_File_v *v) {
  return v->_name;
}

boolean SD_begin(uint8_t csPin) {
/*

Performs the initialisation required by the sdfatlib library.

Return true if initialization succeeds, false otherwise.

*/
  if (Sd2Card_init(&card, SPI_CLOCK_DIV4, csPin)) {
    if (SdVolume_init(&volume, &card)) {
      if (SdFile_openRoot(&root, &volume)) {
        return true;
      }
    }
  }
  return false;
}

// this little helper is used to traverse paths
st_SdFile_v SD_getParentDir(const char *filepath, int *index) {
  // get parent directory
  st_SdFile_v d1, d2, d3;
  st_SdFile_v *parent, *subdir, *t;
  const char *origpath;
  uint8_t idx;
  char subdirname[13];
 
  SdFile_init(&d1);          // constructor, do not forget!
  SdFile_init(&d2);          // constructor, do not forget!
  SdFile_init(&d3);          // constructor, do not forget!
  d1 = root;                 // start with the mostparent, root!
  // we'll use the pointers to swap between the two objects
  parent = &d1;
  subdir = &d2;

  origpath = filepath;

  while (strchr(filepath, '/')) {

    // get rid of leading /'s
    if (filepath[0] == '/') {
      filepath++;
      continue;
    }
    
    if (! strchr(filepath, '/')) {
      // it was in the root directory, so leave now
      break;
    }

    // extract just the name of the next subdirectory
    idx = (uint8_t)(strchr(filepath, '/') - filepath);
    if (idx > 12)
      idx = 12;    // dont let them specify long names
    strncpy(subdirname, filepath, idx);
    subdirname[idx] = 0;

    // close the subdir (we reuse them) if open
    SdFile_close(subdir);
    if (! SdFile_openName(subdir, parent, subdirname, O_READ)) {
      // failed to open one of the subdirectories
      SdFile_init(&d3);
      return d3;
    }
    // move forward to the next subdirectory
    filepath += idx;

    // we reuse the objects, close it.
    SdFile_close(parent);

    // swap the pointers
    t = parent;
    parent = subdir;
    subdir = t;
  }

  *index = (int)(filepath - origpath);
  // parent is now the parent diretory of the file!
  return *parent;
}


boolean SD_open(const char *filepath, uint8_t mode, st_File_v *v) {
/*

Open the supplied file path (full path) for reading or writing.

If `write` is true, default action (when `append` is true) is to
append data to the end of the file.

If `append` is false then the file will be truncated first.

If the file does not exist and it is opened for writing the file
will be created.

An attempt to open a file for reading that does not exist is an
error.

*/

  int pathidx;
  // Open the file itself
  st_SdFile_v file, parentdir;

  SdFile_init(&file);       // constructor, do not forget!
  SdFile_init(&parentdir);  // constructor, do not forget!

  // do the interative search
  parentdir = SD_getParentDir(filepath, &pathidx);
  // no more subdirs!

  filepath += pathidx;

  if (! filepath[0]) {
    // it was the directory itself!
    return File_File_SdFile(v, &parentdir, "/");
  }

  // failed to open a subdir!
  if (!SdFile_isOpen(&parentdir))
    return File_File(v);

  // there is a special case for the Root directory since its a static dir
  if (SdFile_isRoot(&parentdir)) {
    if ( ! SdFile_openName(&file, &root, filepath, mode)) {
      // failed to open the file :(
      return File_File(v);
    }
    // dont close the root!
  } else {
    if ( ! SdFile_openName(&file, &parentdir, filepath, mode)) {
      return File_File(v);
    }
    // close the parent
    SdFile_close(&parentdir);
  }

  if (mode & (O_APPEND | O_WRITE)) 
    SdFile_seekSet(&file, v->_file->fileSize_);
  return File_File_SdFile(v, &file, filepath);
}

boolean SD_exists(const char *filepath) {
/*

Returns true if the supplied file path exists.

*/
  return SD_walkPath(filepath, &root, SD_callback_pathExists);
}

boolean SD_mkdir(const char *filepath) {
/*

Makes a single directory or a heirarchy of directories.

A rough equivalent to `mkdir -p`.

*/
  return SD_walkPath(filepath, &root, SD_callback_makeDirPath);
}

boolean SD_rmdir(const char *filepath) {
/*

Remove a single directory or a heirarchy of directories.

A rough equivalent to `rm -rf`.

*/
  return SD_walkPath(filepath, &root, SD_callback_rmdir);
}

boolean SD_remove(const char *filepath) {
  return SD_walkPath(filepath, &root, SD_callback_remove);
}


boolean SD_getNextPathComponent(const char *path, unsigned int *p_offset, char *buffer) {
  /*

    Parse individual path components from a path.

      e.g. after repeated calls '/foo/bar/baz' will be split
           into 'foo', 'bar', 'baz'.

    This is similar to `strtok()` but copies the component into the
    supplied buffer rather than modifying the original string.


    `buffer` needs to be PATH_COMPONENT_BUFFER_LEN in size.

    `p_offset` needs to point to an integer of the offset at
    which the previous path component finished.

    Returns `true` if more components remain.

    Returns `false` if this is the last component.
      (This means path ended with 'foo' or 'foo/'.)

   */

  // TODO: Have buffer local to this function, so we know it's the
  //       correct length?

  int bufferOffset = 0;

  int offset = *p_offset;

  // Skip root or other separator
  if (path[offset] == '/') {
    offset++;
  }
  
  // Copy the next next path segment
  while (bufferOffset < MAX_COMPONENT_LEN
     && (path[offset] != '/')
     && (path[offset] != '\0')) {
    buffer[bufferOffset++] = path[offset++];
  }

  buffer[bufferOffset] = '\0';

  // Skip trailing separator so we can determine if this
  // is the last component in the path or not.
  if (path[offset] == '/') {
    offset++;
  }

  *p_offset = offset;

  return (path[offset] != '\0');
}


boolean SD_walkPath(const char *filepath, st_SdFile_v *parentDir,
        boolean (*callback)(st_SdFile_v *parentDir,
                     const char *filePathComponent,
                     boolean isLastComponent)) {
/*
 
 When given a file path (and parent directory--normally root),
 this function traverses the directories in the path and at each
 level calls the supplied callback function while also providing
 the supplied object for context if required.

   e.g. given the path '/foo/bar/baz'
        the callback would be called at the equivalent of
    '/foo', '/foo/bar' and '/foo/bar/baz'.

 The implementation swaps between two different directory/file
 handles as it traverses the directories and does not use recursion
 in an attempt to use memory efficiently.

 If a callback wishes to stop the directory traversal it should
 return false--in this case the function will stop the traversal,
 tidy up and return false.

 If a directory path doesn't exist at some point this function will
 also return false and not subsequently call the callback.

 If a directory path specified is complete, valid and the callback
 did not indicate the traversal should be interrupted then this
 function will return true.

*/

  st_SdFile_v subfile1, subfile2;
  st_SdFile_v *p_parent, *p_child, *p_tmp_sdfile;
  char buffer[PATH_COMPONENT_BUFFER_LEN]; 
  unsigned int offset = 0;
  boolean moreComponents, shouldContinue, exists;

  SdFile_init(&subfile1);      // constructor, do not forget!
  SdFile_init(&subfile2);      // constructor, do not forget!

  p_child = &subfile1;
  p_parent = parentDir;

  while (true) {

    moreComponents = SD_getNextPathComponent(filepath, &offset, buffer);
    shouldContinue = callback(p_parent, buffer, !moreComponents);

    if (!shouldContinue) {
      // TODO: Don't repeat this code?
      // If it's one we've created then we
      // don't need the parent handle anymore.
      if (p_parent != parentDir) {
        SdFile_close(p_parent);
      }
      return false;
    }
    
    if (!moreComponents) {
      break;
    }
    
    exists = SdFile_openName(p_child, p_parent, buffer, O_RDONLY);

    // If it's one we've created then we
    // don't need the parent handle anymore.
    if (p_parent != parentDir) {
      SdFile_close(p_parent);
    }
    
    // Handle case when it doesn't exist and we can't continue...
    if (exists) {
      // We alternate between two file handles as we go down
      // the path.
      if (p_parent == parentDir) {
        p_parent = &subfile2;
      }

      p_tmp_sdfile = p_parent;
      p_parent = p_child;
      p_child = p_tmp_sdfile;
    } else {
      return false;
    }
  }
  
  if (p_parent != parentDir) {
    SdFile_close(p_parent); // TODO: Return/ handle different?
  }

  return true;
}

/*

The callbacks used to implement various functionality follow.

Each callback is supplied with a parent directory handle,
character string with the name of the current file path component,
a flag indicating if this component is the last in the path and
a pointer to an arbitrary object used for context.

*/

boolean SD_callback_pathExists(st_SdFile_v *parentDir, const char *filePathComponent, 
                boolean isLastComponent) {
/*

Callback used to determine if a file/directory exists in parent
directory.

Returns true if file path exists.

*/
  st_SdFile_v child;
  boolean exists;

  SdFile_init(&child);          // constructor, do not forget!

  exists = SdFile_openName(&child, parentDir, filePathComponent, O_RDONLY);
  
  if (exists) {
     SdFile_close(&child); 
  }
  
  return exists;
}

boolean SD_callback_makeDirPath(st_SdFile_v *parentDir, const char *filePathComponent, 
                 boolean isLastComponent) {
/*

Callback used to create a directory in the parent directory if
it does not already exist.

Returns true if a directory was created or it already existed.

*/
  boolean result = false;
  st_SdFile_v child;
  
  SdFile_init(&child);          // constructor, do not forget!

  result = SD_callback_pathExists(parentDir, filePathComponent, isLastComponent);
  if (!result) {
    result = SdFile_makeDir(&child, parentDir, filePathComponent);
  } 
  
  return result;
}

boolean SD_callback_remove(st_SdFile_v *parentDir, const char *filePathComponent, 
            boolean isLastComponent) {
  if (isLastComponent) {
    return SdFile_removeFileName(parentDir, filePathComponent);
  }
  return true;
}

boolean SD_callback_rmdir(st_SdFile_v *parentDir, const char *filePathComponent, 
            boolean isLastComponent) {
  if (isLastComponent) {
    st_SdFile_v f;
    if (!SdFile_openName(&f, parentDir, filePathComponent, O_READ)) return false;
    return SdFile_rmDir(&f);
  }
  return true;
}

const st_File_f File = {
  File_init,
  File_available,
  File_close,
  File_flush,
  File_peek,
  File_position,
  File_print,
  File_println,
  File_seek,
  File_size,
  File_read,
  File_write,
  File_isDirectory,
  File_openNextFile,
  File_rewindDirectory,
  File_name
};

const st_SD_f SD = {
  SD_begin,
  SD_exists,
  SD_mkdir,
  SD_remove,
  SD_open,
  SD_rmdir
};

