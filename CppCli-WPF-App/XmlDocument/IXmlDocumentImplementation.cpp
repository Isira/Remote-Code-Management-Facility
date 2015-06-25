
//////////////////////////////////////////////////////////////////////////////////
// IXmlDocumentImplementation.cpp - Implementation for object Factories			//
// -																			//
//																				//
// Authour: Isira Samarasekera													/
//	Version 1.0 04 April 2015													//
//////////////////////////////////////////////////////////////////////////////////
#define XML_EXPORT
#include "IXmlDocument.h"
#include "XmlDocument.h"
#include "../XmlElement/XmlElementFactory.h"
#include "../XmlElement/XmlElement.h"
#include "../DocumentBuilder/DocumentBuilder.h"

//----< make Xml Document from string >-----------

IXmlDocument* XmlDocumentFactory::makeXmlDocument(const std::string& content)
{
	XmlProcessing::XmlDocument* doc = new XmlProcessing::XmlDocument(content);
	XmlProcessing::DocumentBuilder builder;
	builder.build(*doc);
	return doc;
}

//----< create Doc Element Row pointer >-----------

IAbstractXmlElement* XmlFactory::makeDocElement(IAbstractXmlElement* pRoot)
{
	std::shared_ptr<XmlProcessing::AbstractXmlElement> ptr = XmlProcessing::XmlElementFactory::makeDocElement(XmlProcessing::AbstractXmlElement::sharedPointerFromRowPointer(pRoot));
	return std::move(XmlProcessing::XmlElementFactory::makeDocElement(ptr).get());
}

//----< create Text Element Row pointer >-----------

IAbstractXmlElement* XmlFactory::makeTextElement(const std::string& text)
{
	return std::move(XmlProcessing::XmlElementFactory::makeTextElement(text).get());
}

//----< create Tag Element Row pointer >-----------

IAbstractXmlElement* XmlFactory::makeTaggedElement(const std::string& tag)
{
	std::shared_ptr<XmlProcessing::AbstractXmlElement> ptr = XmlProcessing::XmlElementFactory::makeTaggedElement(tag);
	return std::move(ptr.get());
}

//----< create Comment Element Row pointer >-----------

IAbstractXmlElement* XmlFactory::makeCommentElement(const std::string& comment)
{
	return std::move(XmlProcessing::XmlElementFactory::makeCommentElement(comment).get());
}

//----< create XmlDeclaration Element Row pointer >-----------

IAbstractXmlElement* XmlFactory::makeXmlDeclarElement()
{
	return std::move(XmlProcessing::XmlElementFactory::makeXmlDeclarElement().get());
}

//----< create Proc Instruction Element raw pointer >-----------

IAbstractXmlElement* XmlFactory::makeProcInstrElement(const std::string& text)
{
	return std::move(XmlProcessing::XmlElementFactory::makeProcInstrElement(text).get());
}

//----< create Tag ELement raw pointer >-----------

IAbstractXmlElement* XmlFactory::makeTaggedElementRowPointer(const std::string& tag)
{
	return XmlProcessing::XmlElementFactory::makeTaggedElementRowPointer(tag);
}


#ifdef TEST_IXMLDOCUMENTIMPLEMENTATION
void main()
{
	std::string xmlString = "< ? xml version = \"1.0\" encoding = \"utf - 8\" ? >"
		"<!--XML testcase -->"
		"<LectureNote course = \"CSE681\">"
		"<title>XML Example #1 < / title >"
		"<reference>"
		"<title>Programming Microsoft.Net< / title>"
		"<author>Jeff Prosise <note Company = 'Wintellect'>< / note>< / author>"
		"<publisher>Microsoft Press< / publisher>"
		"<date>2002 < / date >"
		"<page>608 < / page >"
		"< / reference>"
		"<comment>Description of PCDATA< / comment>"
		"< / LectureNote>";
	XmlDocumentFactory factory;

	IXmlDocument*  document =factory.makeXmlDocument(xmlString);
	std::cout<< document->toString();
}
#endif