#include <stdio.h>    
#include <iostream>   
#include <stdlib.h>  
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "BaseCache.h"

#define ADDR_SIZE 64
#define DEBUG_1 1
#define DEBUG_2 1

using namespace std;

int main(int argc, char **argv) {
  ifstream inFile(argv[1]);
  string line;
  bool g_Debug = false;
  BaseCache mCacheL1;
  while(getline(inFile, line)) {
    istringstream iss(line);
    int val;
    string parm;
    if(!(iss>>parm>>val)) { break; }
    if(parm == "l1_cap:") {
      mCacheL1.setCacheSize(val*1024);
    } else if(parm == "l1_assoc:") {
      mCacheL1.setAssoc(val);
    } else if(parm == "l1_cline:") {
      mCacheL1.setBlockSize(val);
    } else if(parm == "l1_hlat:") {
      mCacheL1.setHitLatency(val);
    } else if(parm == "mem_lat:") {
      mCacheL1.setMemLatency(val);
    }
  }
  mCacheL1.init_Cache();
  ifstream inFile1(argv[2]);
  char *line_tr = NULL;
  while(getline(inFile1, line)) {
    int cnt=0, r_w=0;
    long cycle;
    long long addr;
    char *line_tr = &line[0];
    char *seg = NULL;
    if(strlen(line_tr)>1) {
      seg = strtok(line_tr, ",");
      while(seg != NULL) {
        string temp(seg);
        istringstream convert(temp);
        if(cnt == 0) cycle = strtol(seg, NULL, 16);
        else if(cnt == 1) r_w = atoi(seg);
        else if(cnt == 2) addr = strtol(seg, NULL, 16);
        seg = strtok(NULL, ",");
        cnt++;
      }
      mCacheL1.processActiveReloads(cycle);
      if(r_w == 1) {
        if(g_Debug) cout<<"Cyc:"<<hex<<cycle<<dec<<" Read Request for Addr:"<<hex<<addr<<dec<<endl; 
        mCacheL1.readAddr(addr, cycle);
      }
      else {
        if(g_Debug) cout<<"Cyc:"<<hex<<cycle<<dec<<" Write Request for Addr:"<<hex<<addr<<dec<<endl; 
        mCacheL1.writeAddr(addr, cycle);
      }
    }
  }
  if(g_Debug) cout<<" Results "<<endl<<" Hits: "<<mCacheL1.getHits()<<endl<<" Misses: "
                  <<mCacheL1.getMisses()<<endl<<" Latency: "<<mCacheL1.getTotalLatency()
                  <<endl<<" References: "<<mCacheL1.getReferences()<<endl;
  cout<<"L1 hit rate: "<<fixed<<setprecision(2)<<(float(mCacheL1.getHits())/(float(mCacheL1.getReferences())))<<endl
      <<"Total latency: "<<mCacheL1.getTotalLatency()<<endl
      <<"L1 references: "<<mCacheL1.getReferences()<<endl
      <<"AMAT: "<<fixed<<setprecision(2)<<((float(mCacheL1.getTotalLatency()))/(float(mCacheL1.getReferences())))<<endl;
  return 0;
}
