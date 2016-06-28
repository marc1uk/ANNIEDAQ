#ifndef CARDDATA_H
#define CARDDATA_H

#include <stdint.h>
#include <iostream>
#include <stdlib.h>

#include "zmq.hpp"


struct CardData{

  int FirmwareVersion;
  uint64_t LastSync;
  int SequenceID;
  int StartTimeSec;
  int StartTimeNSec;
  uint64_t StartCount;
  int triggerNumber;
  std::vector<uint64_t> triggerCounts;
  std::vector<uint32_t> Rates;
  int CardID;
  int DownsampleMode;
  int channels; // eg number of connected PMTs
  int buffersize;
  int eventsize;
  int fullbuffsize; // buffersize * num channels
  std::vector<uint16_t> Data; //1D array of card readout size fullbuffsize


  //the form and structure of this data is probably something you want to change this was just my guess baised on what we talked about.
  //so feel free to make this what ever you making the card output to be
  
  ~CardData();  

  void  Send(zmq::socket_t *socket);
  void Receive(zmq::socket_t *socket);
  
 
  
};


#endif
