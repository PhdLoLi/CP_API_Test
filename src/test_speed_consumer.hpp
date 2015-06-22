/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Regents of the University of California.  *
 * @author Lijing Wang <phdloli@ucla.edu>
 */

#ifndef TEST_SPEED_CONSUMER_HPP
#define TEST_SPEED_CONSUMER_HPP
#include <pthread.h>
#include <ndn-cxx/security/validator.hpp>
#include <ndn-cxx/contexts/consumer-context.hpp>
#include <ctime>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include "threadpool.hpp"

using namespace boost::threadpool;

namespace ndn{

#define IDENTITY_NAME "/Lijing/Wang"

  struct DataNode
  {
    uint64_t length;
    uint8_t *data;
  };

  class ConsumerCallback
  {
      
  public:
    ConsumerCallback() {
      data_ready = false;
      payload_v = 0;
      payload_a = 0;
      interest_s = 0;
      interest_r = 0;
      interest_retx = 0;
      interest_expr = 0;
      frame_cnt_v = 0;
      frame_cnt_a = 0;
      frame_rate_v = 7;
      frame_rate_a = 6;
    }

    ConsumerCallback(int rate_v, int rate_a) : 
      frame_rate_v(rate_v), frame_rate_a(rate_a),
      buffers_v(rate_v), buffers_a(rate_a)
    {
      data_ready = false;
      payload_v = 0;
      payload_a = 0;
      interest_s = 0;
      interest_r = 0;
      interest_retx = 0;
      interest_expr = 0;
      frame_cnt_v = 0;
      frame_cnt_a = 0;
    }
    
    void
    processTestPayload(Consumer& con, const uint8_t* buffer, size_t bufferSize) 
    {
//      boost::lock_guard<boost::mutex> lock(frame_cnt_v_m);
      frame_cnt_v_m.lock();

      Name suffix;
      con.getContextOption(SUFFIX, suffix);
      int frameNumber = std::stoi(suffix.get(0).toUri());
      printf("Video Frame Counter: %d suffix: %s frameNumber: %d\n", frame_cnt_v, suffix.toUri().c_str(), frameNumber);
      payload_v += bufferSize;
      frame_cnt_v++;

      frame_cnt_v_m.unlock();

      buffers_v[frameNumber % frame_rate_v] = make_shared<Buffer>(buffer, bufferSize);
    }

    void 
    processTestPayloadAudio(Consumer& con, const uint8_t* buffer, size_t bufferSize) 
    {
//      boost::lock_guard<boost::mutex> lock(frame_cnt_a_m);
      frame_cnt_a_m.lock();

      Name suffix;
      con.getContextOption(SUFFIX, suffix);
      int frameNumber = std::stoi(suffix.get(0).toUri());
      printf("Audio Frame Counter: %d suffix: %s frameNumber: %d\n", frame_cnt_a, suffix.toUri().c_str(), frameNumber);
      payload_a += bufferSize;
      frame_cnt_a++;

      frame_cnt_a_m.unlock();
      buffers_a[frameNumber % frame_rate_a] = make_shared<Buffer>(buffer, bufferSize);
    }

    void
    processData(Consumer& con, const Data& data) {
      boost::lock_guard<boost::mutex> lock(interest_r_m);
      interest_r++;
 //     printf("DATA IN CNTX Name: %s  FinalBlockId: %s\n", data.getName().toUri().c_str(), data.getFinalBlockId().toUri().c_str());

    }
    
    void
    processLeavingInterest(Consumer& con, Interest& interest) {
      boost::lock_guard<boost::mutex> lock(interest_s_m);
      interest_s ++;
//      printf("Leaving Interest Name: %s\n",interest.toUri().c_str());
  //    std::cout << "LEAVES " << interest.toUri() << std::endl;
    }
    
    void
    processList(Consumer& con, const uint8_t* buffer, size_t bufferSize) {
      std::cout << "Video List: " << std::endl;
      std::vector<std::string> strs;
      boost::split(strs, (char *&)buffer, boost::is_any_of("\n"));
  
      for (int i=0; i<strs.size()-1; i++)
      {
        list.push_back(strs[i]);
        std::cout << i << "  " << strs[i] << std::endl ;
      }
      boost::lock_guard<boost::mutex> lock(mut);
      data_ready=true;
      cond.notify_one();
    }


    void
    onRetx(Consumer& con, Interest& interest) {
      boost::lock_guard<boost::mutex> lock(interest_retx_m);
      interest_retx ++;
      printf("Retransmitted %s\n", interest.getName().toUri().c_str());
    }

    void
    onExpr(Consumer& con, Interest& interest) {
      boost::lock_guard<boost::mutex> lock(interest_expr_m);
      interest_expr ++;
      printf("Expired %s\n", interest.getName().toUri().c_str());
    }
    
    std::vector<std::string> list;
    boost::condition_variable cond;

    boost::mutex mut;
    boost::mutex frame_cnt_v_m;
    boost::mutex frame_cnt_a_m;
    boost::mutex interest_s_m;
    boost::mutex interest_r_m;
    boost::mutex interest_retx_m;
    boost::mutex interest_expr_m;

    bool data_ready;
    int frame_cnt_v;
    int frame_cnt_a;
    uint32_t payload_v;
    uint32_t payload_a;
    uint32_t interest_s;
    uint32_t interest_r;
    uint32_t interest_retx;
    uint32_t interest_expr;
    uint32_t frame_rate_v;
    uint32_t frame_rate_a;

    std::vector<BufferPtr> buffers_v;
    std::vector<BufferPtr> buffers_a;
  };

}
#endif
