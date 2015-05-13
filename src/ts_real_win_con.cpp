/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * @author Lijing Wang OoOfreedom@gmail.com 
 */

// correct way to include ndn-cxx headers
// #include <ndn-cxx/contexts/producer-context.hpp>
#include "verificator.hpp"
#include <vector>
#include <cstdlib>
#include <time.h>
#include <thread>
#include <chrono>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

class Performance
{
public:
  Performance() : m_byteCounter(0) {
  }
  
  void onInterestLeaves(Consumer& c, Interest& interest) {
    std::cout << "Leaving: " << interest.toUri() << std::endl;
  }

  void onDataEnters(Consumer& c, const Data& data) {
  }

  void onContent(Consumer& c, const uint8_t* buffer, size_t bufferSize) {
    m_byteCounter += bufferSize;
//    std::cout << "GOT " << m_byteCounter << " BYTES" << std::endl;
  }
  
private:
  uint32_t m_byteCounter;
};

void test(int argc, char** argv) { 
  
  auto t1 = std::chrono::high_resolution_clock::now();
  int window_size = 8;

  std::string suffix = "1";
  if (argc > 1) {
    suffix = argv[1];
    if (argc > 2)
      window_size = atoi(argv[2]);
  }
  std::cout << "window_size " << window_size << std::endl;

  Verificator verificator;
  Performance performance;
  
  Name sampleName(PREFIX_NAME + suffix);
  
  Consumer c(sampleName, SDR);
  c.setContextOption(MUST_BE_FRESH_S, true);
  c.setContextOption(INTEREST_RETX, 1000);
  c.setContextOption(INTEREST_LIFETIME, 20000);
  c.setContextOption(MAX_WINDOW_SIZE, 1);
  c.setContextOption(MIN_WINDOW_SIZE, 1);
    
  c.setContextOption(DATA_ENTER_CNTX,
              (ConsumerDataCallback)bind(&Performance::onDataEnters, &performance, _1, _2));

  c.setContextOption(INTEREST_LEAVE_CNTX,
                (ConsumerInterestCallback)bind(&Performance::onInterestLeaves, &performance, _1, _2));

  c.setContextOption(DATA_TO_VERIFY,
              (ConsumerDataVerificationCallback)bind(&Verificator::onPacket, &verificator, _1, _2));
    
  c.setContextOption(CONTENT_RETRIEVED,
              (ConsumerContentCallback)bind(&Performance::onContent, &performance, _1, _2, _3));               
  
//  std::vector<bool> vec(20, false);  
  std::vector<int> vec_segments(20, 0);
  int ran = -1; 
  int i = 0;

  time::system_clock::TimePoint start;
  time::system_clock::TimePoint stop;
  ndn::time::steady_clock::TimePoint::clock::duration duration;

  // start counting time
  start = time::system_clock::now();

  while (i < 20 * 30) {
    while (1) { 
      auto t2 = std::chrono::high_resolution_clock::now();
      srand(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
      ran = rand() % 20;
      if (vec_segments[ran] < 30) {
        break;
      } 
    }
    int end = vec_segments[ran] + window_size < 30 ? vec_segments[ran] + window_size : 30;  


    for (uint64_t j = vec_segments[ran]; j < end; j++) {
      std::cout << "frame:" << ran << " segment: " << j << std::endl;
      i++;
//      name::Component::fromSegment(j); 
      c.consume(Name(std::to_string(ran)).append(name::Component::fromSegment(j)));
    } 
    vec_segments[ran] = end;
  }  
  // stop counting time
  stop = time::system_clock::now();
  duration = stop - start;

  std::cout << "DONE" << std::endl;
  std::cout << "*************************************************************" << std::endl;
  std::cout << "Final duration " << duration <<std::endl;
   
}

} // namespace ndn

int
main(int argc, char** argv)
{
  ndn::test(argc, argv);
//  std::vector<std::thread *> threads;
//  for (int i = 0; i < 5; i++) {
////    usleep(100);
//    if (i == 4) ndn::output = true;
//    threads.push_back(new std::thread(ndn::test, argc, argv));
//  }
//  for (int i = 0; i < 5; i++)
//    threads[i]->join();
  return 0;
}
