#ifndef DOCUMENTBUILDER_H
#define DOCUMENTBUILDER_H

//////////////////////////////////////////////////////////////////////////////////////
// DocumentBuilder.h -  Build an XmlDocument										//																			//
// ver 1.0																			//
// ---------------------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015												//
// All rights granted provided that this notice is retained							//
// ---------------------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013								//
// Platform:    Mac Book Pro, Core i5, Windows 8.1									//
// Application: Project #2 – XmlDocument,2015										//
// Author:      Isira Samarasekera, Syracuse University								//
//              issamara@syr.edu													//
//////////////////////////////////////////////////////////////////////////////////////

/*
* Module Operations:
* ==================
*  Build a internal parse tree for an XmlDocument
*
* Public Interface:
* =================
* DocumentBuilder builder;
* XmlDocument doc("LectureNote.xml",XmlDocument::filename);
* builder.build(doc);
*
* Required Files:
* ===============
* Display.h, Display.cpp
* XmlDocument.h , XmlDocument.cpp
*xmlElementParts.h  xmlElementParts.cpp
* AbstractXmlElement.h  AbstractXmlElement.cpp
*
* Build Command: 
* ==============
* cl /EHa /DTEST_DOCUMENTBUILDER DocumentBuilder.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 20 Mar 15
* - first release
*/

#include "../XmlDocument/XmlDocument.h"
#include "../XmlElementParts/xmlElementParts.h"
#include "../XmlElement/AbstractXmlElement.h"

#include <stack>

namespace XmlProcessing
{
	class DocumentBuilder
	{
	public:

		// Given a file or a string build XmlDocument for it
		void build(XmlDocument& document);
	private:
		std::stack<std::shared_ptr<XmlProcessing::AbstractXmlElement>> _elementStack;
		void parse(XmlParts part);
		bool isProcIns(XmlParts xmlPart);
		bool isDeclare(XmlParts xmlPart);
		bool isComment(XmlParts xmlPart);
		bool isElement(XmlParts xmlPart);
		bool isEndElement(XmlParts xmlPart);
		bool isTextElement(XmlParts xmlPart);

		std::shared_ptr<AbstractXmlElement> createTaggedElement(XmlParts xmlPart);
		std::shared_ptr<AbstractXmlElement> createXMLDeclaration(XmlParts xmlPart);
		std::shared_ptr<AbstractXmlElement> createProcessingInstructor(XmlParts xmlPart);
		std::shared_ptr<AbstractXmlElement> createComment(XmlParts xmlPart);

	};
}
#endif