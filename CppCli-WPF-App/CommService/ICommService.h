#ifndef ICOMMSERVICE_H
#define ICOMMSERVICE_H
//////////////////////////////////////////////////////////////////////////
// ICommService.h -  Interface for communication service				//
// ver 1.0																//
// ---------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015									//
// All rights granted provided that this notice is retained				//
// ---------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013                 //
// Platform:    Mac Book Pro, Core i5, Windows 8.1						//
// Application: Project #3 – Message Passing Communication,2015			//
// Author:      Isira Samarasekera, Syracuse University					//
//              issamara@syr.edu										//
//////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This modle provides an interface which a concrete service needs to implement.
*
* Public Interface:
* =================
* ICommService* someConcreteService;
* ServiceMessage msg;
* someConcreteService->postMessage(msg);
*
* Required Files:
* ===============
*	ICommService.h
*
* Maintenance History:
* ====================
* ver 1.0 : 09 April 15
* - first release
*/

#include "ServiceMessage.h"

class ICommService
{
public:
	virtual void postMessage(ServiceMessage& msg) = 0;
	virtual ~ICommService(){}
};


#endif