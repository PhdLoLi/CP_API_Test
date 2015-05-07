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
    std::cout << "DATA IN Segment No. " << data.getName().get(-1).toSegment() << std::endl;
    std::cout << "FinalBlockId: " << data.getFinalBlockId() << std::endl;
    if (data.getName().get(-1).toSegment() == 0) {
      std::cout << "Record Start Time Here!" << std::endl;
      m_reassemblyStart = time::system_clock::now();
    }
  }

  void onContent(Consumer& c, const uint8_t* buffer, size_t bufferSize) {
    m_byteCounter += bufferSize;
    std::cout << "GOT " << m_byteCounter << " BYTES" << std::endl;
    if (m_byteCounter == CONTENT_LENGTH)
    {
    std::cout << "DONE" << std::endl;
    m_reassemblyStop = time::system_clock::now();
    }
  }

  ndn::time::steady_clock::TimePoint::clock::duration getReassemblyDuration() {
    return m_reassemblyStop - m_reassemblyStart;
  }
  
private:
  uint32_t m_byteCounter;
  time::system_clock::TimePoint m_reassemblyStart;
  time::system_clock::TimePoint m_reassemblyStop;
};

int main(int argc, char** argv) { 
  std::string suffix = "1";
  if (argc > 1) {
    suffix = argv[1];
  }

  Verificator verificator;
  Performance performance;
  
  Name sampleName(PREFIX_NAME + suffix);
  
  Consumer c(sampleName, RDR);
  c.setContextOption(MUST_BE_FRESH_S, true);
    
  c.setContextOption(DATA_ENTER_CNTX,
              (ConsumerDataCallback)bind(&Performance::onDataEnters, &performance, _1, _2));

  c.setContextOption(INTEREST_LEAVE_CNTX,
                (ConsumerInterestCallback)bind(&Performance::onInterestLeaves, &performance, _1, _2));

  c.setContextOption(DATA_TO_VERIFY,
              (ConsumerDataVerificationCallback)bind(&Verificator::onPacket, &verificator, _1, _2));
    
  c.setContextOption(CONTENT_RETRIEVED,
              (ConsumerContentCallback)bind(&Performance::onContent, &performance, _1, _2, _3));               
    
  c.consume(Name());
    
  std::cout << "**************************************************************" << std::endl;
  std::cout << "Reassembly duration " << performance.getReassemblyDuration() << std::endl;
  
  return 0;
}

} // namespace ndn

int
main(int argc, char** argv)
{
  return ndn::main(argc, argv);
}
