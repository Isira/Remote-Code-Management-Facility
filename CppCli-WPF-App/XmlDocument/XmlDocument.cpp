//////////////////////////////////////////////////////////////////////////
// XmlDocument.cpp -   a container of XmlElement nodes   				//
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

#include <iostream>
#include <fstream>
#include <functional>
#include "XmlDocument.h"
#include "../XmlElement/XmlElementFactory.h"
using namespace XmlProcessing;

//----< construct XmlDocument instance >---------------------------

XmlProcessing::XmlDocument::XmlDocument(const std::string& src, sourceType srcType) : _src(src), _srcType(srcType)
{
}

//----< move constructor for XmlDocument >---------------------------

XmlDocument::XmlDocument(XmlDocument&& doc) :pDocElement_(doc.pDocElement_), found_(std::move(doc.found_))
{
	doc.pDocElement_ = nullptr;
}

//----< move assignement for XmlDocument >---------------------------

XmlDocument& XmlDocument::operator=(XmlDocument&& doc)
{
	if (this == &doc)
		return *this;
	pDocElement_ = doc.pDocElement_;
	doc.pDocElement_ = nullptr;
	found_ = std::move(doc.found_);
	return *this;
}

//----< get String representation of XmlDocument >---------------------------

std::string XmlDocument::toString() const
{
	std::string ast = "";
	if (pDocElement_!= nullptr)
		ast = pDocElement_->toString();
	return ast;
}

//----< search xmlDocument for a element with the tag element and push to the first element to _found vector >---------------------------

XmlDocument& XmlDocument::element(const std::string& tag)
{
	found_.clear();
	std::function<void(sPtr)> f =
		[&](sPtr pNode) { 
		if (found_.empty() && (tag == "" || pNode->tag() == tag))
		{ 
			found_.push_back(pNode); 
		}
	};

	DFS(pDocElement_,f);
	return *this;
}

//----< search xmlDocument for a element with the tag element and push all to _found vector >---------------------------

XmlDocument& XmlDocument::elements(const std::string& tag)
{
	found_.clear();
	std::function<void(sPtr)> f =
		[&](sPtr pNode) {
		if (tag == "" || pNode->tag() == tag)
		{
			found_.push_back(pNode);
		}
	};

	DFS(pDocElement_, f);
	return *this;
}

//---< search xmlDocument for a children  with the tag, in the _found vector  and push all to _found vector >---------------------------

XmlDocument& XmlDocument::children(const std::string& tag)
{
	if (found_.empty())
		found_.push_back(pDocElement_);

	sPtr pNode = found_[0];
	found_.clear();
	std::vector<sPtr> children = pNode->children();
	for (auto it = children.begin(); it != children.end(); it++)
	{
		if ((*it)->tag() == tag || tag == "" )
			found_.push_back((*it));
	}
	return *this;
}

//---< search xmlDocument for a discendants with the tag, in the _found vector  and push all to _found vector >---------------------------

XmlDocument& XmlDocument::descendents(const std::string& tag)
{
	if (found_.empty())
		found_.push_back(pDocElement_);

	std::function<void(sPtr)> f =
		[&](sPtr pNode) {
		std::vector<std::shared_ptr<AbstractXmlElement>> children = pNode->children();
		for (auto it = children.begin(); it != children.end(); it++)
		{
			if (tag == "" || (*it)->tag() == tag )
				found_.push_back((*it));
		}
	};

	sPtr ptr= std::shared_ptr<AbstractXmlElement>(found_[0]);
	found_.clear();
	DFS(ptr, f);

	return *this;
}

//---< search xmlDocument for elements with the matching attribute name, value pair, and push it in to the found vector>---------------------------
 
XmlDocument& XmlDocument::elementsWithAttribute(const std::string& attribute, const std::string& value)
{
	found_.clear();
	
	std::function<void(sPtr)> f =
		[&](sPtr pNode) {
		if (found_.empty())
		{
			std::vector<std::pair<std::string, std::string>> attbs = pNode->attribs();
			for (auto it: attbs)
			{
				if (it.first == attribute && it.second == value )
					found_.push_back(pNode);
			}	
		}
	};

	DFS(pDocElement_, f);
	if (found_.empty())
		found_.push_back(nullptr);
	return *this;
}
//---< get the contents of the found_ vector and clear the contents>---------------------------

std::vector<sPtr> XmlDocument::select()
{
	return std::move(found_);
}

std::vector<IAbstractXmlElement*> XmlDocument::selectNativePointer()
{
	std::vector<IAbstractXmlElement*> found;
	for (auto it : found_)
	{
		found.push_back(std::move(it.get()));
	}
	return std::move(found);
}
//---< save the XmlDocument in the file path provided>---------------------------

void XmlDocument::save(std::string filePath)
{
	std::ofstream out(filePath);
	
	out << *this;
	out.close();
}

//---< insertion operator for XmlDocument >---------------------------

std::ostream& operator<< (std::ostream& stream, const XmlDocument& doc)
{
	stream << doc.toString();
	return stream;
}

#ifdef TEST_XMLDOCUMENT
#include "../DocumentBuilder/DocumentBuilder.h"

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing XmlDocument class\n "
		<< std::string(23, '=') << std::endl;

	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	std::cout << "\n  Processing file " << argv[1];
	std::cout << "\n  " << std::string(16 + strlen(argv[1]), '-') ;

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
	{
		XmlDocument document(argv[1],XmlDocument::filename);
		// <title>Programming Microsoft .Net</title>
		DocumentBuilder builder;
		builder.build(document);
		document.element("").children("");
		std::vector<sPtr> results = document.select(); // size needs to be one and the 
		std::cout << "\n";
		for (auto i : results)
		{
			std::cout << "\nchild\n";
			std::cout << i->toString();
			std::cout << "\n";
		}
		


	}
		//document.elements("LectureNote").descendents("note");
		//std::vector<AbstractXmlElement*> results = document.select(); // size needs to be one and the 
		//if(!results.empty() )
		//{
		//	for (auto i : results)
		//	{
		//		 i->addAttrib("Company","myVal");
		//		 i->addChild(makeTaggedElement("This is the comment"));
		//		 std::cout << i->toString();
		//	}
		//	std::cout<<"\n";
		//}

	{
		XmlDocument document(argv[1],XmlDocument::filename);
		DocumentBuilder builder;
		builder.build(document);
		document.elements("LectureNote").descendents("note");
		std::vector<sPtr> results = document.select(); // size needs to be one and the 
		if (!results.empty())
		{
			for (auto i : results)
			{
				i->addAttrib("Company", "myVal");
				i->addChild(XmlElementFactory::makeTaggedElement("This is the comment"));
				//std::cout << document.toString();
			}
			std::cout << "\n";
		}
	}

	/*{
		XmlDocument document(xmlString,XmlProcessing::XmlDocument::sourceType::string);
		std::cout << document.toString();
	}*/

	{
		XmlDocument document(xmlString);
		DocumentBuilder builder;
		builder.build(document);
		document.elementsWithAttribute("course","CSE681");

		sPtr result = document.select()[0]; // size needs to be one and the 
		if (result == nullptr)
		{
			std::cout << "No results were returned";
		}
		else
		{
		     std::cout << result->toString();
		}
		std::cout << "\n";

	}

}
#endif



