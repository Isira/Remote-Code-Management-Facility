// TestMockChannel.cpp

#include <iostream>
#include <string>
#include "..\MockChannel\MockChannel.h"

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr(5678);
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  //pSendr->postMessage(Message("Hello World"));
  //pSendr->postMessage(Message("CSE687 - Object Oriented Design"));
  ServiceMessage msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg.toString() << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg.toString() << "\"";
 // pSendr->postMessage(Message("stopping"));
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg.toString() << "\"";
  pMockChannel->stop();
  //pSendr->postMessage(Message("quit"));
  std::cin.get();
}