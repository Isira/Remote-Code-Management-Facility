//////////////////////////////////////////////////////////////////////////////////////
// DocumentBuilder.cpp -   Implementation for  AbstractXmlElement abstract class	//
// ver 1.0																			//
// --------------------------------------------------------------------------------	//
// copyright © Isira Samarasekera, 2015												//
// All rights granted provided that this notice is retained							//
// --------------------------------------------------------------------------------	//
// Language:    Visual C++, Visual Studio Ultimate 2013								//
// Platform:    Mac Book Pro, Core i5, Windows 8.1									//
// Application: Project #2 – XmlDocument,2015										//
// Author:      Isira Samarasekera, Syracuse University								//
//              issamara@syr.edu													//									
//////////////////////////////////////////////////////////////////////////////////////

#include "AbstractXmlElement.h"

using namespace XmlProcessing;

size_t AbstractXmlElement::count = 0;
size_t AbstractXmlElement::tabSize = 2;


bool AbstractXmlElement::addChildNativePtr(IAbstractXmlElement* pChild)
{
	return false;
}
bool AbstractXmlElement::removeChildNativePtr(IAbstractXmlElement* pChild)
{
	return false;
}
//---< default add AbstractXmlElement child >---------------------------

bool AbstractXmlElement::addChild(std::shared_ptr<AbstractXmlElement> pChild) 
{ 
	return false; 
}

//---< default remove AbstractXmlElement child >---------------------------

bool AbstractXmlElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild) 
{ 
	return false; 
}

//---< default add attribute  >---------------------------

bool AbstractXmlElement::addAttrib(const std::string& name, const std::string& value) 
{
	return false; 
}

//---< default remove  attribute  >---------------------------

bool AbstractXmlElement::removeAttrib(const std::string& name) 
{ 
	return false; 
}

//---< destructor for abstractXmlElemet  >---------------------------

AbstractXmlElement::~AbstractXmlElement() 
{

}

//---< default get attribute method  >---------------------------

std::vector<std::pair<std::string, std::string>> AbstractXmlElement::attribs()
{
	return  std::vector<std::pair<std::string, std::string>>();
}

//---< default get children method  >---------------------------

std::vector<std::shared_ptr<AbstractXmlElement>> AbstractXmlElement::children()
{
	return std::vector<std::shared_ptr<AbstractXmlElement>>();
}

std::vector<IAbstractXmlElement*> AbstractXmlElement::childreNativePtrs()
{
	return std::vector<IAbstractXmlElement*> ();
}

//---< default get tag method  >---------------------------

std::string AbstractXmlElement::tag()
{ 
	return ""; 
}


std::string AbstractXmlElement::attribValue(const std::string& key)
{
	std::string val;
	return val;
}

// This doesnt have a test stub because its a abstract class