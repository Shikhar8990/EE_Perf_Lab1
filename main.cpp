#include <stdio.h>    
#include <iostream>   
#include <stdlib.h>  
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include "BaseCache.h"

#define ADDR_SIZE 64
#define DEBUG_1 1
#define DEBUG_2 1

using namespace std;

int main() {
  ifstream inFile("sample_config.cfg");
  string line;
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
  ifstream inFile1("sample_trace.trc");
  char *line_tr = NULL;
  while(getline(inFile1, line)) {
    int cnt=0;
    int cycle=0, r_w=0;
    long long addr;
    char *line_tr = &line[0];
    char *seg = NULL;
    seg = strtok(line_tr, ",");
    while(seg != NULL) {
      string temp(seg);
      istringstream convert(temp);
      if(cnt == 0) convert>>cycle;
      else if(cnt == 1) r_w = atoi(seg);
      else if(cnt == 2) addr = strtol(seg, NULL, 16);
      seg = strtok(NULL, ",");
      cnt++;
    }
    if(r_w == 1) {
      cout<<"Read Request for Addr:"<<hex<<addr<<dec<<endl; 
      mCacheL1.readAddr(addr);
    }
    else {
      cout<<"Write Request for Addr:"<<hex<<addr<<dec<<endl; 
      mCacheL1.writeAddr(addr);
    }
  }
  return 0;
}
