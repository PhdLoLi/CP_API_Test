/** 
 * Author: Lijing Wang
 * Date: 5/7/2015
 */
#pragma once
#include "key.hpp"

namespace ndn {
class Verificator
{
public:
  Verificator()
  {
    Name identity(IDENTITY_NAME);
    Name keyName = m_keyChain.getDefaultKeyNameForIdentity(identity);
    m_publicKey = m_keyChain.getPublicKey(keyName); 
  };
  
  bool
  onPacket(Consumer& c, const Data& data)
  {
    if (Validator::verifySignature(data, *m_publicKey))
    {
      std::cout << "VERIFIED " << data.getName() << std::endl;
      return true;
    }
    else
    {
      std::cout << "UNVERIFIED " << data.getName() << std::endl;
      return false;
    }
  }
  
private:
  KeyChain m_keyChain;
  shared_ptr<PublicKey> m_publicKey;
};
} // namespace ndn
