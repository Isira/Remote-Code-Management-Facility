#ifndef DISPLAY_H
#define DISPLAY_H
//////////////////////////////////////////////////////////////////////////////
// ServiceMessage.h -  Display results on the standard output	//
// ver 1.0																	//
// -------------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015										//
// All rights granted provided that this notice is retained					//
// -------------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013						//
// Platform:    Mac Book Pro, Core i5, Windows 8.1							//
// Application: Project #3 – Message Passing Communication,2015				//
// Author:      Isira Samarasekera, Syracuse University						//
//              issamara@syr.edu											//
//////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
*	Display results on the standard output
*
* Public Interface:
* =================
*	Display display;
*	display.showString("Hello World");
*	display.RequirementHeader("123");
*	std::vector<std::string> header = { "srcIp:localhost", "srcPort:1234", "dstIp:localhost", "3333" };
*	ServiceMessage msg = ServiceMessage::interpret(header);
*	Display::setShowMessage(true);
*	display.showMessageReceived(msg);
*	display.showMessageSent(msg);
*	Display::setShowMessage(false);
*
* Required Files:
* ===============
*	Display.h	Display.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_DISPLAY Display.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 12 April 15
* - first release
*/

#include <string>
#include <iostream>
#include <sstream>
#include <mutex>

class ServiceMessage;
class Display
{
private:
	static bool _showMessage;
	static void showMessage(ServiceMessage& msg);
	
	static bool showState_;

	
public:
	Display(bool showState = false);
	void static show(const std::string& msg, bool always = false);
	void RequirementHeader(const std::string& num);
	void showString(const std::string& str);
	static void showMessageReceived(ServiceMessage&  msg);
	static void showMessageSent(ServiceMessage&  msg);
	static void setShowMessage(bool value); 
	static void title(const std::string& msg, char underline = '-');
	static void putLine(const std::string& msg = "");
	static std::mutex mtx;

}; 


template<typename T>
std::string toString(T t)
{
	std::ostringstream out;
	out << t;
	return out.str();
}



extern const bool always;


#endif