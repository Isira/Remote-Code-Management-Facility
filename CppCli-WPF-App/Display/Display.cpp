//////////////////////////////////////////////////////////////////////////
// Display.cpp -  Display results on the standard output				//
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
#define IN_DLL
#include "Display.h"
#include "../MockChannel/MockChannel.h"
#include <iostream>

bool Display::_showMessage = false;
std::mutex Display::mtx;
bool Display::showState_ = false;
const bool always = false;
//---< display the requirement header  >---------------------------

void Display::RequirementHeader(const std::string& num)
{
	std::lock_guard<std::mutex> lock(Display::mtx);
	std::cout << "\nDemonstrating Requirement " << num << ": \n";
	std::cout << "============================ \n\n";
	std::cout.flush();
}

//---< show string in the standard output  >---------------------------

void Display::showString(const std::string& str)
{
	std::lock_guard<std::mutex> lock(Display::mtx);
	std::cout << str << std::endl;
	std::cout.flush();
}

//---< Display Message  >---------------------------

void Display::showMessage(ServiceMessage& msg)
{
	std::cout << msg;
}

//---< display message received  >---------------------------

void Display::showMessageReceived(ServiceMessage& msg)
{
	if (_showMessage)
	{
		std::lock_guard<std::mutex> lock(Display::mtx);
		std::cout << "\nMessage Received\n";
		showMessage(msg);
		std::cout.flush();
	}
}

//---< display message sent  >---------------------------

void Display::showMessageSent(ServiceMessage&  msg)
{
	if (_showMessage)
	{
		std::lock_guard<std::mutex> lock(Display::mtx);
		std::cout << "\nMessage Sent\n";
		showMessage(msg);
		std::cout.flush();
	}
}

//---< set boolean to show message  >---------------------------
void Display::setShowMessage(bool value)
{
	_showMessage = value;
}



//std::unique_lock<std::mutex> iolock(mtx, std::defer_lock);

void Display::title(const std::string& msg, char underline)
{
	std::lock_guard<std::mutex> lock(mtx);
	std::cout << "\n  " << msg;
	std::cout << "\n " << std::string(msg.size() + 2, underline);
	std::cout.flush();
}

void Display::putLine(const std::string& msg)
{
	std::lock_guard<std::mutex> lock(mtx);
	std::cout << "\n" + msg;
	std::cout.flush();
}

Display::Display(bool showState)
{
	showState_ = showState;
	if (showState)
		Display::putLine("  Verbose mode turned on");
}

void Display::show(const std::string& msg, bool always)
{

	if (always == true || showState_ == true)
	{
		std::string temp = std::string("\n  ") + msg;
		{
			std::lock_guard<std::mutex> lock(Display::mtx);
			std::cout << temp;
			std::cout.flush();
		}
	}
}


#ifdef TEST_DISPLAY
void main()
{
	Display display;
	display.showString("Hello World");
	display.RequirementHeader("123");
	std::vector<std::string> header = { "srcIp:localhost", "srcPort:1234", "dstIp:localhost", "3333" };
	MessageFactory factory;
	ServiceMessage msg = factory.interpret(header);
	Display::setShowMessage(true);
	display.showMessageReceived(msg);
	display.showMessageSent(msg);
	Display::setShowMessage(false);
}
#endif