//////////////////////////////////////////////////////////////////////////////
// DocumentBuilder.cpp -   Build an XmlDocument  with internal parse tree	//
// ver 1.0																	//
// -------------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015										//
// All rights granted provided that this notice is retained					//
// -------------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013						//
// Platform:    Mac Book Pro, Core i5, Windows 8.1							//
// Application: Project #2 – XmlDocument,2015								//
// Author:      Isira Samarasekera, Syracuse University						//
//              issamara@syr.edu											//									
//////////////////////////////////////////////////////////////////////////////

#include "DocumentBuilder.h"
#include "../XmlElementParts/Tokenizer.h"
#include "../XmlElement/XmlElementFactory.h"

using namespace XmlProcessing;

//----< build the AST and set XmlDocument internal parse tree  >-----------

void DocumentBuilder::build(XmlDocument& document)
{
	std::shared_ptr<AbstractXmlElement> pDocElement_ = XmlElementFactory::makeDocElement();
	_elementStack.push(pDocElement_);

	if (!document.src().empty())
	{
		try
		{
			Toker* toker;
			if (document.srcType() == XmlDocument::filename)
			{
				toker = new Toker(document.src());
			}
			else 
			{
				toker = new Toker(document.src(), false);
			}
			toker->setMode(Toker::xml);
			XmlParts parts(toker);
			while (parts.get())
				parse(parts);
		}
		catch (std::exception ex)
		{
			std::cout << "\n  " << ex.what() << "\n\n";
		}
	}

	document.setDocElement(pDocElement_);
}

//----< parse the xmlPart, create the appropriate xmlElement, do the operion with the stack >-----------

void DocumentBuilder::parse(XmlParts part)
{
	if (isEndElement(part))
	{
		std::shared_ptr<XmlProcessing::AbstractXmlElement> finishedObj = _elementStack.top();
		_elementStack.pop();
		_elementStack.top()->addChild(finishedObj);
		return;

	}
	else if (isProcIns(part))
	{
		std::shared_ptr<XmlProcessing::AbstractXmlElement> procIns = createProcessingInstructor(part);
		std::shared_ptr<XmlProcessing::AbstractXmlElement> elem = _elementStack.top();
		elem->addChild(procIns);
		return;
	}
	else if (isDeclare(part))
	{
		std::shared_ptr<XmlProcessing::AbstractXmlElement> declare = createXMLDeclaration(part);
		std::shared_ptr<XmlProcessing::AbstractXmlElement> elem = _elementStack.top();
		elem->addChild(declare);
		return;
	}
	else if (isComment(part))
	{
		std::shared_ptr<XmlProcessing::AbstractXmlElement> comment = createComment(part);
		std::shared_ptr<XmlProcessing::AbstractXmlElement> elem = _elementStack.top();
		elem->addChild(comment);
		return;
	}
	else if (isElement(part))
	{
		std::shared_ptr<XmlProcessing::AbstractXmlElement> tagElem = createTaggedElement(part);
		_elementStack.push(tagElem);
		return;
	}
	else if (isTextElement(part))
	{
		std::shared_ptr<XmlProcessing::AbstractXmlElement> textElem = XmlElementFactory::makeTextElement(part.show());
		std::shared_ptr<XmlProcessing::AbstractXmlElement> elem = _elementStack.top();
		elem->addChild(textElem);
		return;
	}
}

//----< Check the XmlPart is corresponding to a ProcessorInstruction  >-----------

bool DocumentBuilder::isProcIns(XmlParts xmlPart)
{
	return (xmlPart[0] == "<" &&xmlPart[1] == "?" && xmlPart[2] != "xml" &&xmlPart[xmlPart.length() - 2] == "?"
		&&xmlPart[xmlPart.length() - 1] == ">");
}

//----< Check the XmlPart is corresponding to an XML Declaration  >----------------

bool DocumentBuilder::isDeclare(XmlParts xmlPart)
{
	return (xmlPart[0] == "<" &&xmlPart[1] == "?" && xmlPart[2] == "xml" &&xmlPart[xmlPart.length() - 2] == "?"
		&&xmlPart[xmlPart.length() - 1] == ">");
}

//----< Check the XmlPart is corresponding to a comment  >----------------

bool DocumentBuilder::isComment(XmlParts xmlPart)
{
	return (xmlPart[0] == "<" &&xmlPart[1] == "!" &&xmlPart[2] == "--" &&xmlPart[xmlPart.length() - 2] == "--"
		&& xmlPart[xmlPart.length() - 1] == ">");
}

//----< Check the XmlPart is corresponding to a start of a tagged element  >----------------

bool DocumentBuilder::isElement(XmlParts xmlPart)
{
	return (xmlPart[0] == "<" &&xmlPart[1] != "!" &&xmlPart[1] != "?"&& xmlPart[1] != "/"&& xmlPart[xmlPart.length() - 1] == ">");
}

//----< Check the XmlPart is corresponding to an end of a tagged element  >----------------

bool DocumentBuilder::isEndElement(XmlParts xmlPart)
{
	return (xmlPart[0] == "<"  && xmlPart[1] == "/"&& xmlPart[xmlPart.length() - 1] == ">");
}


//----< Check the XmlPart is corresponding to a text element   >----------------

bool DocumentBuilder::isTextElement(XmlParts xmlPart)
{
	return (xmlPart[0] != "<" && xmlPart[xmlPart.length() - 1] != ">");
}

//----< Create Tagged Element from an XmlPart   >----------------------------------------------

std::shared_ptr<XmlProcessing::AbstractXmlElement> DocumentBuilder::createTaggedElement(XmlParts xmlPart)
{
	std::shared_ptr<XmlProcessing::AbstractXmlElement> ptr = nullptr;
	ptr = XmlElementFactory::makeTaggedElement(xmlPart[1]);
	for (int i = 2; i < xmlPart.length() - 1; i += 3)
	{
		std::string attribute = xmlPart[i];
		std::string tValue = xmlPart[i + 2];
		std::string value = tValue.substr(1, tValue.length() - 2);
		ptr->addAttrib(attribute, value);
	}
	return ptr;
}

//----< Create Xml Declaration from an XmlPart   >----------------------------------------------

std::shared_ptr<XmlProcessing::AbstractXmlElement> DocumentBuilder::createXMLDeclaration(XmlParts xmlPart)
{
	std::shared_ptr<XmlProcessing::AbstractXmlElement> ptr = nullptr;
	ptr = XmlElementFactory::makeXmlDeclarElement();
	for (int i = 3; i < xmlPart.length() - 2; i += 3)
	{
		std::string attribute = xmlPart[i];
		std::string tValue = xmlPart[i + 2];
		std::string value = tValue.substr(1, tValue.length() - 2);
		ptr->addAttrib(attribute, value);
	}
	return ptr;
}

//----< Create Processing Instruction from an XmlPart   >----------------------------------------------

std::shared_ptr<XmlProcessing::AbstractXmlElement> DocumentBuilder::createProcessingInstructor(XmlParts xmlPart)
{
	std::shared_ptr<XmlProcessing::AbstractXmlElement> ptr = nullptr;
	ptr = XmlElementFactory::makeProcInstrElement(xmlPart[2]);
	for (int i = 3; i < xmlPart.length() - 2; i += 3)
	{
		std::string attribute = xmlPart[i];
		std::string tValue = xmlPart[i + 2];
		std::string value = tValue.substr(1, tValue.length() - 2);
		ptr->addAttrib(attribute, value);
	}
	return ptr;
}

//----< Create Comment Element from an XmlPart   >----------------------------------------------

std::shared_ptr<AbstractXmlElement> DocumentBuilder::createComment(XmlParts xmlPart)
{
	std::shared_ptr<XmlProcessing::AbstractXmlElement> ptr = nullptr;

	std::string comment = "";
	for (int i = 3; i < xmlPart.length() - 3; i++)
	{
		comment += xmlPart[i] + " ";
	}
	comment += xmlPart[xmlPart.length() - 3];
	ptr = XmlElementFactory::makeCommentElement(comment);
	return ptr;
}



#ifdef TEST_DOCUMENTBUILDER
int main()
{
	DocumentBuilder builder;
	XmlDocument doc("LectureNote.xml",XmlDocument::filename);
	builder.build(doc);

	std::cout << doc.toString();
}
#endif