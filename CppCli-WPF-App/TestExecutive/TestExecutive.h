#ifndef TESTEXECUTIVE_H
#define TESTEXECUTIVE_H
//////////////////////////////////////////////////////////////////////////////
// TestExecutive.h - Demonstrating requirements of Project #3				//
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
*	Demonstrating functional requirements of project #3
*
* Public Interface:
* =================
*	TestExecutive testExecutive(serverPort1, serverPort2, clientPort1, clientPort2);
*	testExecutive.Requirement3and4();
*	testExecutive.Requirement4();
*	testExecutive.Requirement7();
*	
* Required Files:
* ===============
*	TestExecutive.h	TestExecutive.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_TESTEXECUTIVE TestExecutive.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 09 April 15
* - first release
*/


//5. (3) Shall, for text search commands, accept one or more end - point addresses to specify machines to search, wild cards
//to specify file patterns to match, and regular expressions to specify text to find.
//6. (4) Shall, in response to a text search command, return a list of files that contain the text and the paths and machines
//on which they reside.
//7. (4) Shall, in response to a file search command, passing a path4 and file patterns, return a list of files with their paths
//found in the directory subtree rooted at the specified path.The client shall provide the option5
//to return an XML string
//showing the entire directory subtree containing matched files.
//8. (1) shall support concurrent processing of text searches, including the capability to specify, at the client, the number of
//threads that will participate in that server processing.
//9. (1) Shall enable the measurement of time required to carry out a processing request and also the end - to - end time to
//request a processing task and receive the corresponding reply.Please display the results in milliseconds.
//10. (1) shall use the high resolution timer provided by the std::chrono library.


#include "../Display/Display.h"

class TestExecutive
{	


public:
	void Requirement3();
	void Requirement4();
	void Requirement5();
	void Requirement6();
	void Requirement7();
	void Requirement8();
	void Requirement9();

	
};

#endif