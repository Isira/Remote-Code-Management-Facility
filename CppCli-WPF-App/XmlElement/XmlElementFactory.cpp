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

#include "XmlElementFactory.h"
#include "XmlElement.h"

using namespace XmlProcessing;

//----< factory for doc elements >----------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeDocElement(std::shared_ptr<AbstractXmlElement> pRoot)
{
	std::shared_ptr<AbstractXmlElement> ptr(new DocElement(pRoot));
	return ptr;
}

//----< factory for tagged elements >----------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeTaggedElement(const std::string& tag)
{
	std::shared_ptr<AbstractXmlElement> ptr(new TaggedElement(tag));
	return ptr;
}
//----< factory for text elements >------------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeTextElement(const std::string& text)
{
	std::shared_ptr<AbstractXmlElement> ptr(new TextElement(text));
	return ptr;
}
//----< factory for comment elements >---------------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeCommentElement(const std::string& text)
{
	std::shared_ptr<AbstractXmlElement> ptr(new CommentElement(text));
	return ptr;
}
//----< factory for processing instruction elements >------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeProcInstrElement(const std::string& text)
{
	std::shared_ptr<AbstractXmlElement> ptr(new ProcInstrElement(text));
	return ptr;
}
//----< factory for XML Declaration elements >-------------------------------

std::shared_ptr<AbstractXmlElement> XmlElementFactory::makeXmlDeclarElement()
{
	std::shared_ptr<AbstractXmlElement> ptr(new XmlDeclarElement());
	ptr->addAttrib("version","1.0");
	ptr->addAttrib("encoding", "utf-8");
	return ptr;
}

AbstractXmlElement* XmlElementFactory::makeTaggedElementRowPointer(const std::string& tag)
{
	return new TaggedElement(tag);
}

#ifdef TEST_XMLELEMENTFACTORY
#include <iostream>
void main()
{
	std::shared_ptr<AbstractXmlElement> root=  XmlElementFactory::makeTaggedElement("Root");
	std::shared_ptr<AbstractXmlElement> xDec = XmlElementFactory::makeXmlDeclarElement();
	std::shared_ptr<AbstractXmlElement> com = XmlElementFactory::makeCommentElement("This is a comment");

	
	std::shared_ptr<AbstractXmlElement> doc = XmlElementFactory::makeDocElement(root);
	doc->addChild(com);
	doc->addChild(xDec);
	std::cout << doc->toString();
	std::cout <<"\n";
}
#endif