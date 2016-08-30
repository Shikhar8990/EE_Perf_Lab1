#include "BaseCache.h"
using namespace std;

BaseCache::BaseCache() {
  mAssoc=0;
  mBlock_size = 0;
  mCache_size = 0;
  mCache      = NULL;
  mSets       = 0;
  mTagBits    = 0;
  mBlockOffet = 0;
  mIndexBits  = 0;
  mIndexMask  = 0;
  mHitLat     = 0;
  mMemLat     = 0;
  mRead_Reqs  = 0;
  mWrite_Reqs = 0;
  mMisses     = 0;
  mHits       = 0;
  mLatency    = 0;
  mHits       = 0;
} 

void BaseCache::init_Cache() {
  int blocks = mCache_size/mBlock_size;
  int sets = blocks/mAssoc;
  mSets = sets;
  mBlockOffet = log2(mBlock_size);
  mIndexBits = log2(mSets);
  cout<<"Cache Parms "<<endl<<" Block Size" <<mBlock_size<<endl<<" Cache Size "<<mCache_size<<endl
      <<" Assoc "<<mAssoc<<endl<<" Number of Lines "<<blocks<<endl<<" Sets "<<sets<<endl
      <<" BLock offset "<<mBlockOffet<<endl<<" index bits "<<mIndexBits<<endl;
  for(int x=0; x<mIndexBits;x++) {
    mIndexMask = (mIndexMask<<1)|1;
  }
  cout<<" Index Mask "<<mIndexMask<<endl;
  mCache = (CacheLine **)malloc(sets*sizeof(CacheLine *));
  for(int set=0; set<sets; set++) {
    mCache[set] = (CacheLine *)malloc(mAssoc*sizeof(CacheLine));
  }
  //Initializing the LRU counter
  for(int set=0; set<mSets; set++) {
    for(int way=0; way<mAssoc; way++) {
      mCache[set][way].setLRU(way);
    }
  }
}

void BaseCache::readAddr(long long inAddr) { 
  int way=0, set=0;
  if(searchTag(inAddr, set, way)) {
    cout<<"Cache hit 0x"<<hex<<inAddr<<dec<<endl;
  } else {
    cout<<"Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    //allocateLine(inAddr, set, way);
    way = getWaytoAllocate(inAddr);
    set = getSet(inAddr);
    sendMemReq(inAddr, set, way);
  }
  updateLRU(set, way);
}

void BaseCache::writeAddr(long long inAddr) {
  int way=0, set=0;
  if(searchTag(inAddr, set, way)) { //hit
    cout<<"Cache hit 0x"<<hex<<inAddr<<dec<<endl;
  } else {
    cout<<"Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    //allocateLine(inAddr, set, way);
    way = getWaytoAllocate(inAddr); 
    set = getSet(inAddr);           
    sendMemReq(inAddr, set, way);   
  }
  updateLRU(set, way);
  mCache[set][way].setDirty();
}

bool BaseCache::searchTag(long long inAddr, int &inSet, int &inWay) {
  cout<<" Searching for Tag 0x"<<hex<<getTag(inAddr)<<dec<<endl;
  for(int set=0; set<mSets; set++) {
    for(int way=0; way<mAssoc; way++) {
      if((getTag(inAddr) == mCache[set][way].getTag()) && (mCache[set][way].isValid())) {
        inSet = set;
        inWay = way;
        return true;
      }
    }
  }
  return false;
}

void BaseCache::sendMemReq(long long inAddr, int inSet, int inWay) {
  reloadPkt pkt;
  pkt.tag = getTag(inAddr);
  pkt.set = inSet;
  pkt.way = inWay;
  pkt.cnt = mMemLat;
  pkt.valid = 1;
  mActiveRld.push_front(pkt);
  cout<<" Mem Req for Tag: 0x"<<hex<<pkt.tag<<dec<<endl;
}

void BaseCache::allocateLine(long long inTag, int inSet, int inWay) {
  mCache[inSet][inWay].setTag(inTag);
  mCache[inSet][inWay].setValid();
}

void BaseCache::processActiveReloads() {
  deque<reloadPkt>::iterator it = mActiveRld.begin();
  while(it != mActiveRld.end()) {
    it->cnt--;
    if(it->cnt == 0) {
      allocateLine(it->tag, it->set, it->way);
      cout<<" Cache Written Tag: 0x"<<hex<<it->tag<<dec<<endl;
      mActiveRld.pop_back();
    }
    it++;
  }
}

int BaseCache::getSet(long long inAddr) {
  inAddr = inAddr>>mBlockOffet;
  return (inAddr&mIndexMask); 
}
int BaseCache::getWaytoAllocate(long long inAddr) {
  int way = 0;
  int set = getSet(inAddr);
  int lru = mCache[set][0].getLRU();
  for(int y=1; y<mAssoc; y++) {
    if(mCache[set][y].getLRU() > lru) {
      lru = mCache[set][y].getLRU();
      way = y;
    }
  }
  return way; 
}

void BaseCache::updateLRU(int inSet, int inWay) {
  mCache[inSet][inWay].setLRU(0);
  for(int way=0; way<mAssoc; way++) {
    if(way != inWay) {
      mCache[inSet][way].incrementLRU();
    }
  }
}

long long BaseCache::getTag(long long inAddr) {
  return (inAddr>>(mBlockOffet+mIndexBits));
}
