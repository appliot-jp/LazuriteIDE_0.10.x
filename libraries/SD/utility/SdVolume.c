/* FILE NAME: SdVolume.h
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

//------------------------------------------------------------------------------
// find a contiguous group of clusters
uint8_t SdVolume_allocContiguous(st_SdVolume_v *v, uint32_t count, uint32_t *curCluster) {
  // start of group
  uint32_t bgnCluster, endCluster, fatEnd, n, f;

  // flag to save place to start next search
  uint8_t setStart;

  // set search start cluster
  if (*curCluster) {
    // try to make file contiguous
    bgnCluster = *curCluster + 1;

    // don't save new start location
    setStart = false;
  } else {
    // start at likely place for free cluster
    bgnCluster = v->allocSearchStart_;

    // save next search start if one cluster
    setStart = 1 == count;
  }
  // end of group
  endCluster = bgnCluster;

  // last cluster of FAT
  fatEnd = v->clusterCount_ + 1;

  // search the FAT for free clusters
  for (n = 0;; n++, endCluster++) {
    // can't find space checked all clusters
    if (n >= v->clusterCount_) return false;

    // past end - start from beginning of FAT
    if (endCluster > fatEnd) {
      bgnCluster = endCluster = 2;
    }
    if (!SdVolume_fatGet(v, endCluster, &f)) return false;

    if (f != 0) {
      // cluster in use try next cluster as bgnCluster
      bgnCluster = endCluster + 1;
    } else if ((endCluster - bgnCluster + 1) == count) {
      // done - found space
      break;
    }
  }
  // mark end of chain
  if (!SdVolume_fatPutEOC(v, endCluster)) return false;

  // link clusters
  while (endCluster > bgnCluster) {
    if (!SdVolume_fatPut(v, endCluster - 1, endCluster)) return false;
    endCluster--;
  }
  if (*curCluster != 0) {
    // connect chains
    if (!SdVolume_fatPut(v, *curCluster, bgnCluster)) return false;
  }
  // return first cluster number to caller
  *curCluster = bgnCluster;

  // remember possible next free cluster
  if (setStart) v->allocSearchStart_ = bgnCluster + 1;

  return true;
}
//------------------------------------------------------------------------------
uint8_t SdVolume_blockOfCluster(st_SdVolume_v *v, uint32_t position) {
  return (uint8_t)((position >> 9) & (v->blocksPerCluster_ - 1));
}
//------------------------------------------------------------------------------
uint8_t SdVolume_cacheFlush(st_SdVolume_v *v) {
  if (v->cacheDirty_) {
    if (!SdVolume_writeBlock(v, v->cacheBlockNumber_, v->cacheBuffer_.data)) {
      return false;
    }
    // mirror FAT tables
    if (v->cacheMirrorBlock_) {
      if (!SdVolume_writeBlock(v, v->cacheMirrorBlock_, v->cacheBuffer_.data)) {
        return false;
      }
      v->cacheMirrorBlock_ = 0;
    }
    v->cacheDirty_ = 0;
  }
  return true;
}
//------------------------------------------------------------------------------
uint8_t SdVolume_cacheRawBlock(st_SdVolume_v *v, uint32_t blockNumber, uint8_t action) {
  if (v->cacheBlockNumber_ != blockNumber) {
    if (!SdVolume_cacheFlush(v)) return false;
    if (!Sd2Card_readBlock(v->sdCard_, blockNumber, v->cacheBuffer_.data)) return false;
    v->cacheBlockNumber_ = blockNumber;
  }
  v->cacheDirty_ |= action;
  return true;
}
//------------------------------------------------------------------------------
void SdVolume_cacheSetDirty(st_SdVolume_v *v) {v->cacheDirty_ |= CACHE_FOR_WRITE;}
//------------------------------------------------------------------------------
// cache a zero block for blockNumber
uint8_t SdVolume_cacheZeroBlock(st_SdVolume_v *v, uint32_t blockNumber) {
  uint16_t i;

  if (!SdVolume_cacheFlush(v)) return false;

  // loop take less flash than memset(cacheBuffer_.data, 0, 512);
  for (i = 0; i < 512; i++) {
    v->cacheBuffer_.data[i] = 0;
  }
  v->cacheBlockNumber_ = blockNumber;
  SdVolume_cacheSetDirty(v);
  return true;
}
//------------------------------------------------------------------------------
// return the size in bytes of a cluster chain
uint8_t SdVolume_chainSize(st_SdVolume_v *v, uint32_t cluster, uint32_t* size) {
  uint32_t s = 0;
  do {
    if (!SdVolume_fatGet(v, cluster, &cluster)) return false;
    s += 512UL << v->clusterSizeShift_;
  } while (!SdVolume_isEOC(v, cluster));
  *size = s;
  return true;
}
//------------------------------------------------------------------------------
uint32_t SdVolume_clusterStartBlock(st_SdVolume_v *v, uint32_t cluster) {
  return v->dataStartBlock_ + ((cluster - 2) << v->clusterSizeShift_);
}
//------------------------------------------------------------------------------
// Fetch a FAT entry
uint8_t SdVolume_fatGet(st_SdVolume_v *v, uint32_t cluster, uint32_t* value) {
  uint32_t lba;

  if (cluster > (v->clusterCount_ + 1)) return false;
  lba = v->fatStartBlock_;
  lba += v->fatType_ == 16 ? cluster >> 8 : cluster >> 7;
  if (lba != v->cacheBlockNumber_) {
    if (!SdVolume_cacheRawBlock(v, lba, CACHE_FOR_READ)) return false;
  }
  if (v->fatType_ == 16) {
    *value = v->cacheBuffer_.fat16[cluster & 0XFF];
  } else {
    *value = v->cacheBuffer_.fat32[cluster & 0X7F] & FAT32MASK;
  }
  return true;
}
//------------------------------------------------------------------------------
// Store a FAT entry
uint8_t SdVolume_fatPut(st_SdVolume_v *v, uint32_t cluster, uint32_t value) {
  uint32_t lba;

  // error if reserved cluster
  if (cluster < 2) return false;

  // error if not in FAT
  if (cluster > (v->clusterCount_ + 1)) return false;

  // calculate block address for entry
  lba = v->fatStartBlock_;
  lba += v->fatType_ == 16 ? cluster >> 8 : cluster >> 7;

  if (lba != v->cacheBlockNumber_) {
    if (!SdVolume_cacheRawBlock(v, lba, CACHE_FOR_READ)) return false;
  }
  // store entry
  if (v->fatType_ == (uint8_t)16) {
    v->cacheBuffer_.fat16[cluster & 0XFF] = (uint16_t)value;
  } else {
    v->cacheBuffer_.fat32[cluster & 0X7F] = value;
  }
  SdVolume_cacheSetDirty(v);

  // mirror second FAT
  if (v->fatCount_ > 1) v->cacheMirrorBlock_ = lba + v->blocksPerFat_;
  return true;
}
//----------------------------------------------------------------------------
uint8_t SdVolume_fatPutEOC(st_SdVolume_v *v, uint32_t cluster) {
  return SdVolume_fatPut(v, cluster, 0x0FFFFFFF);
}
//------------------------------------------------------------------------------
// free a cluster chain
uint8_t SdVolume_freeChain(st_SdVolume_v *v, uint32_t cluster) {
  // clear free cluster location
  v->allocSearchStart_ = 2;

  do {
    uint32_t next;
    if (!SdVolume_fatGet(v, cluster, &next)) return false;

    // free cluster
    if (!SdVolume_fatPut(v, cluster, (uint32_t)0)) return false;

    cluster = next;
  } while (!SdVolume_isEOC(v, cluster));

  return true;
}
//------------------------------------------------------------------------------
/**
 * Initialize a FAT volume.  Try partition one first then try super
 * floppy format.
 *
 * \param[in] dev The Sd2Card where the volume is located.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.  Reasons for
 * failure include not finding a valid partition, not finding a valid
 * FAT file system or an I/O error.
 */
boolean SdVolume_init(st_SdVolume_v *v, st_Sd2Card_v* dev) {
  // raw block cache
  // init cacheBlockNumber_to invalid SD block number
  v->cacheBlockNumber_ = 0xFFFFFFFF;
  v->cacheDirty_ = 0;  // cacheFlush() will write block if true
  v->cacheMirrorBlock_ = 0;  // mirror  block for second FAT
  v->allocSearchStart_ = 2;
  v->fatType_ = 0;
  return (boolean)(SdVolume_initPart(v, dev, 1) ? true : SdVolume_initPart(v, dev, 0));
}
//------------------------------------------------------------------------------
/**
 * Initialize a FAT volume.
 *
 * \param[in] dev The SD card where the volume is located.
 *
 * \param[in] part The partition to be used.  Legal values for \a part are
 * 1-4 to use the corresponding partition on a device formatted with
 * a MBR, Master Boot Record, or zero if the device is formatted as
 * a super floppy with the FAT boot sector in block zero.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.  Reasons for
 * failure include not finding a valid partition, not finding a valid
 * FAT file system in the specified partition or an I/O error.
 */
boolean SdVolume_initPart(st_SdVolume_v *v, st_Sd2Card_v* dev, uint8_t part) {
  uint32_t volumeStartBlock = 0;
  part_t* p;
  bpb_t* bpb;
  uint32_t totalBlocks;

  v->sdCard_ = dev;
  // if part == 0 assume super floppy with FAT boot sector in block zero
  // if part > 0 assume mbr volume with partition table
  if (part) {
    if (part > 4)return false;
    if (!SdVolume_cacheRawBlock(v, volumeStartBlock, CACHE_FOR_READ)) return false;
    p = &v->cacheBuffer_.mbr.part[part-1];    // W4028(1) warning is no problem after comfirmation.
    if ((p->boot & 0X7F) !=0  ||
      p->totalSectors < 100 ||
      p->firstSector == 0) {
      // not a valid partition
      return false;
    }
    volumeStartBlock = p->firstSector;
  }
  if (!SdVolume_cacheRawBlock(v, volumeStartBlock, CACHE_FOR_READ)) return false;
  bpb = &v->cacheBuffer_.fbs.bpb;
  if (bpb->bytesPerSector != 512 ||
    bpb->fatCount == 0 ||
    bpb->reservedSectorCount == 0 ||
    bpb->sectorsPerCluster == 0) {
       // not valid FAT volume
      return false;
  }
  v->fatCount_ = bpb->fatCount;
  v->blocksPerCluster_ = bpb->sectorsPerCluster;

  // determine shift that is same as multiply by blocksPerCluster_
  v->clusterSizeShift_ = 0;
  while (v->blocksPerCluster_ != (1 << v->clusterSizeShift_)) {
    // error if not power of 2
    if (v->clusterSizeShift_++ > 7) return false;
  }
  v->blocksPerFat_ = bpb->sectorsPerFat16 ?
                    bpb->sectorsPerFat16 : bpb->sectorsPerFat32;

  v->fatStartBlock_ = volumeStartBlock + bpb->reservedSectorCount;

  // count for FAT16 zero for FAT32
  v->rootDirEntryCount_ = bpb->rootDirEntryCount;

  // directory start for FAT16 dataStart for FAT32
  v->rootDirStart_ = v->fatStartBlock_ + bpb->fatCount * v->blocksPerFat_;

  // data start for FAT16 and FAT32
  v->dataStartBlock_ = v->rootDirStart_ + ((32 * bpb->rootDirEntryCount + 511)/512);

  // total blocks for FAT16 or FAT32
  totalBlocks = bpb->totalSectors16 ?
                           bpb->totalSectors16 : bpb->totalSectors32;
  // total data blocks
  v->clusterCount_ = totalBlocks - (v->dataStartBlock_ - volumeStartBlock);

  // divide by cluster size to get cluster count
  v->clusterCount_ >>= v->clusterSizeShift_;

  // FAT type is determined by cluster count
  if (v->clusterCount_ < 4085) {
    v->fatType_ = 12;
  } else if (v->clusterCount_ < 65525) {
    v->fatType_ = 16;
  } else {
    v->rootDirStart_ = bpb->fat32RootCluster;
    v->fatType_ = 32;
  }
  return true;
}
//------------------------------------------------------------------------------
boolean SdVolume_isEOC(st_SdVolume_v *v, uint32_t cluster) {
    return  (boolean)(cluster >= (v->fatType_ == 16 ? FAT16EOC_MIN : FAT32EOC_MIN));
}
//------------------------------------------------------------------------------
uint8_t SdVolume_readData(st_SdVolume_v *v, uint32_t block, uint16_t offset,
    uint16_t count, uint8_t* dst) {
      return Sd2Card_readData(v->sdCard_, block, offset, count, dst);
  }
//------------------------------------------------------------------------------
uint8_t SdVolume_writeBlock(st_SdVolume_v *v, uint32_t block, const uint8_t* dst) {
    return Sd2Card_writeBlock(v->sdCard_, block, dst);
}
