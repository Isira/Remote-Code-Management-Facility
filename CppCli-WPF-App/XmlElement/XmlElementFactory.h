#ifndef XMLELEMENTFACTORY_H
#define XMLELEMENTFACTORY_H

//////////////////////////////////////////////////////////////////////////
// XmlElementFactory.h -contains Static factory methods for Xml Elements//
// ver 1.0																//
// ---------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015									//
// All rights granted provided that this notice is retained				//
// ---------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013                 //
// Platform:    Mac Book Pro, Core i5, Windows 8.1						//
// Application: Project #2 – XmlDocument,2015							//
// Author:      Isira Samarasekera, Syracuse University					//
//              issamara@syr.edu										//
// Source:      Jim Fawcett, CST 4-187, 443-3948						//
//              jfawcett@twcny.rr.com									//
//////////////////////////////////////////////////////////////////////////

/* Module Operations :
*=====================
*  This module provides with the static factory methods to create concrete xml elements
*
* Public Interface :
*=================
* std::shared_ptr<AbstractXmlElement> root=  XmlElementFactory::makeTaggedElement("Root");
* std::shared_ptr<AbstractXmlElement> xDec = XmlElementFactory::makeXmlDeclarElement();
* std::shared_ptr<AbstractXmlElement> com = XmlElementFactory::makeCommentElement("This is a comment");
* std::shared_ptr<AbstractXmlElement> doc = XmlElementFactory::makeDocElement(root);
* std::shared_ptr<AbstractXmlElement> text = XmlElementFactory:: makeTextElement("text");
* std::shared_ptr<AbstractXmlElement> proc = XmlElementFactory::  makeProcInstrElement("instruc");

*
* Required Files :
* ===============
* XmlElementFactory.h, XmlElementFactory.cpp
* XmlElement.h, XmlElement.cpp

* Build Process:
* --------------
* devenv XmlDocument.sln /debug rebuild
*
* Maintenance History :
*+====================
*ver 1.0 : 20 Mar 15
* -first release
*/

#include "AbstractXmlElement.h"

namespace XmlProcessing
{
	class XmlElementFactory 
	{

	public:
		 static std::shared_ptr<AbstractXmlElement> makeDocElement(std::shared_ptr<AbstractXmlElement> pRoot = nullptr);
		 static std::shared_ptr<AbstractXmlElement> makeTextElement(const std::string& text);
		 static std::shared_ptr<AbstractXmlElement> makeTaggedElement(const std::string& tag);
		 static std::shared_ptr<AbstractXmlElement> makeCommentElement(const std::string& comment);
		 static std::shared_ptr<AbstractXmlElement> makeXmlDeclarElement();
		 static std::shared_ptr<AbstractXmlElement> makeProcInstrElement(const std::string& text);
		 static AbstractXmlElement* makeTaggedElementRowPointer(const std::string& tag);
	};

}

#endif