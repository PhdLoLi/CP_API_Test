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

#include "signer.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

class Performance
{
public:
  Performance() 
    : m_duration_sum(0), m_nohit_times(0) {
  }
  
  void onNewSegment(Producer& p, Data& data) {
//    std::cout << "on New Segment " << std::endl;
    if (data.getName().get(-1).toSegment() == 0) {       
//      std::cout << "record start time " << std::endl;
      m_segmentationStart = time::system_clock::now();
    }
  }
  
  void onSegmentFinalized(Producer& p, Data& data) {
//    std::cout << "Segment No. " << data.getName().get(-1).toSegment() << std::endl;
    m_segmentationStop = time::system_clock::now();
  }

  ndn::time::steady_clock::TimePoint::clock::duration getSegmentationDuration() {
    return m_segmentationStop - m_segmentationStart;
  }
   
  void onInterest(Producer& p, const Interest& interest) {
    std::cout << "Entering " << interest.toUri() << std::endl;
  }

  void onCache(Producer& p, const Interest& interest) {
    std::cout << "**************************************************************" << std::endl;
    std::cout << "Cache Miss - Start " << interest.toUri() << std::endl;
    uint8_t* content = new uint8_t[CONTENT_LENGTH];
    std::string frame = interest.getName().get(-2).toUri();
    std::cout << "frame number " << frame << std::endl;
    p.produce(Name(frame), content, CONTENT_LENGTH);
    m_duration_sum += getSegmentationDuration(); 
    m_nohit_times++;
    std::cout << "Cache Miss - Finish Times " << m_nohit_times << std::endl; 
    std::cout << "This Time Segmentation duration " << getSegmentationDuration() <<std::endl;
    std::cout << "The whole segmentation duration " << m_duration_sum <<std::endl;
    std::cout << "**************************************************************" << std::endl;
  }  

private:
  time::system_clock::TimePoint m_segmentationStart;
  time::system_clock::TimePoint m_segmentationStop;
  ndn::time::steady_clock::TimePoint::clock::duration m_duration_sum;
  int m_nohit_times;
  std::string m_suffix;
};

int main(int argc, char** argv) {
  int buf_size = 1;
  std::string suffix = "1";
  if (argc > 2) {
    buf_size = atoi(argv[1]);
    suffix = argv[2];
  }

  Signer signer;
  Performance performance;
   
  Name sampleName(PREFIX_NAME + suffix);
 
  Producer p(sampleName);
//  p.setContextOption(FAST_SIGNING, true);
  p.setContextOption(SND_BUF_SIZE, buf_size);
//  p.setContextOption(DATA_FRESHNESS, 30);
  
  p.setContextOption(NEW_DATA_SEGMENT,
                (ProducerDataCallback)bind(&Performance::onNewSegment, &performance, _1, _2));
  
  p.setContextOption(DATA_TO_SECURE,
                (ProducerDataCallback)bind(&Signer::onPacket, &signer, _1, _2));
  
  p.setContextOption(DATA_LEAVE_CNTX,
                (ProducerDataCallback)bind(&Performance::onSegmentFinalized, &performance, _1, _2));
  
  p.setContextOption(INTEREST_ENTER_CNTX,
                (ProducerInterestCallback)bind(&Performance::onInterest, &performance, _1, _2));
  
  p.setContextOption(CACHE_MISS,
                (ProducerInterestCallback)bind(&Performance::onCache, &performance, _1, _2));
  p.attach();
    
    
  sleep(500);
  
  return 0;
}

} // namespace ndn

int main(int argc, char** argv) {
  return ndn::main(argc, argv);
}
