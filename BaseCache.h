#ifndef BaseCache_h
#define BaseCache_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <deque>
#include "CacheLine.h"

using namespace std;

class BaseCache {
  public:
    BaseCache();
    void setAssoc(int inAssoc)         { mAssoc = inAssoc; }
    void setCacheSize(int inCacheSize) { mCache_size = inCacheSize; }
    void setBlockSize(int inBlockSize) { mBlock_size = inBlockSize; }
    void setHitLatency(int inLat)      { mHitLat = inLat; }
    void setMemLatency(int inLat)      { mMemLat = inLat; }
    void init_Cache();
    void readAddr(long long inAddr);
    void writeAddr(long long inAddr);
    void updateLRU(int inSet, int inWay);
    void allocateLine(long long inTag, int inSet, int inWay);
    bool searchTag(long long inAddr, int &inSet, int &inWay);
    void sendMemReq(long long inAddr, int inSet, int inWay);
    void processActiveReloads();
    int  getWaytoAllocate(long long inAddr);
    int  getSet(long long inAddr);
    long long getTag(long long inAddr);
    int  getTotalReferences() { return (mRead_Reqs+mWrite_Reqs); }
    int  getMisses() { return mMisses; }
    int  getTotalLatency() { return mLatency; }
    int  getHits() { return mHits; }
    struct reloadPkt {
      long long tag;
      int set;
      int way;
      int cnt;
      int valid;
    };
  private:  
    CacheLine **mCache;
    int mAssoc;
    int mSets;
    int mBlock_size;
    int mCache_size;
    int mIndexBits;
    int mIndexMask;
    int mBlockOffet;
    int mTagBits;
    int mMemLat;
    int mHitLat;
    int mRead_Reqs;
    int mWrite_Reqs;
    int mMisses;
    int mHits;
    int mLatency;
    deque<reloadPkt> mActiveRld;
};

#endif
