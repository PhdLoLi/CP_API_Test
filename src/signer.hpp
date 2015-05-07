/** 
 * Author: Lijing Wang
 * Date: 5/7/2015
 */
#pragma once
#include "key.hpp"
namespace ndn {
class Signer
{
public:
  Signer()
  : m_counter(0)
  , m_identityName(IDENTITY_NAME)
  {
    m_keyChain.createIdentity(m_identityName);
  };
  
  void
  onPacket(Producer& p, Data& data)
  {
    m_counter++;
    m_keyChain.signByIdentity(data, m_identityName);
  }
  
private:
  KeyChain m_keyChain;
  int m_counter;
  Name m_identityName;
};
} // namespace ndn
