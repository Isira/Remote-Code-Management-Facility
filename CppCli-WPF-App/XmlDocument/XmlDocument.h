#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
///////////////////////////////////////////////////////////////////
// XmlDocument.h - a container of XmlElement nodes               //
// Ver 1.3                                                       //
// Application: Help for CSE687 Pr#2, Spring 2015                //
// Platform:    Dell XPS 2720, Win 8.1 Pro, Visual Studio 2013   //
// Author:      Isira Samarasekera, Syracuse University			 //
//              issamara@syr.edu								 //
// Source:      Jim Fawcett, CST 4-187, 443-3948                 //
//              jfawcett@twcny.rr.com                            //
///////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* The XML Document Model is essentially
* a program-friendly wrapper around an Abstract Syntax Tree (AST) used to
* contain the results of parsing XML markup.
*
* Abstract Syntax Trees, defined in this package, are unordered trees with 
* two types of nodes:
*   Terminal nodes     - nodes with no children
*   Non-Terminal nodes - nodes which may have a finite number of children
* They are often used to contain the results of parsing some language.
*
* The elements defined in the companion package, XmlElement, will be used in
* the AST defined in this package.  They are:
*   AbstractXmlElement - base for all the XML Element types
*   DocElement         - XML element with children designed to hold prologue, Xml root, and epilogue
*   TaggedElement      - XML element with tag, attributes, child elements
*   TextElement        - XML element with only text, no markup
*   CommentElement     - XML element with comment markup and text
*   ProcInstrElement   - XML element with markup and attributes but no children
*   XmlDeclarElement   - XML declaration element with attributes
*
* Required Files:
* ---------------
*   - XmlDocument.h, XmlDocument.cpp, 
*     XmlElement.h, XmlElement.cpp
*
* Build Process:
* --------------
*   devenv AST.sln /debug rebuild
*
* Maintenance History:
* --------------------
* ver 1.3 : 20 Mar 15
* ver 1.2 : 21 Feb 15
* - modified these comments
* ver 1.1 : 14 Feb 15
* - minor changes to comments, method arguments
* Ver 1.0 : 11 Feb 15
* - first release
*
*/

#include <memory>
#include <string>
#include "../XmlElement/AbstractXmlElement.h"

namespace XmlProcessing
{
  using sPtr = std::shared_ptr < AbstractXmlElement >;
  class XmlDocument : public IXmlDocument
  {
  public:
  

    enum sourceType { string, filename };

	XmlDocument(){}
    XmlDocument(const std::string& src, sourceType srcType=string);

    // queries return XmlDocument references so they can be chained, e.g., doc.element("foobar").descendents();
	XmlDocument(const XmlDocument& tag)= delete;
	XmlDocument& operator = (const XmlDocument& tag) = delete;
    XmlDocument& element(const std::string& tag);           // found_[0] contains first element (DFS order) with tag
    XmlDocument& elements(const std::string& tag);          // found_ contains all elements with tag
    XmlDocument& children(const std::string& tag = "");     // found_ contains sPtrs to children of prior found_[0] 
    XmlDocument& descendents(const std::string& tag = "");  // found_ contains sPtrs to descendents of prior found_[0]

	XmlDocument& elementsWithAttribute(const std::string& attribute, const std::string& value);  // found_ contains sPtrs to descendents of prior found_[0]

	std::vector<sPtr> select();                            // return reference to found_.  Use std::move(found_) to clear found_
	virtual std::vector<IAbstractXmlElement*> selectNativePointer();
	std::string toString() const;

	XmlDocument(XmlDocument&& doc);
	XmlDocument& operator=(XmlDocument&& doc);

	void save(std::string filePath);
	

	std::string src()
	{
		return _src;
	}
	sourceType srcType()
	{
		return _srcType;
	}
	void setDocElement(sPtr pDocElement)
	{
		pDocElement_ = pDocElement;
	}

	sPtr docElement()
	{
		return pDocElement_;
	}


  private:
    sPtr pDocElement_;         // AST that holds procInstr, comments, XML root, and more comments
	std::vector<sPtr> found_;  // query results
	std::string _src;
	sourceType _srcType;
  };

  template<typename CallObj>
  void DFS(sPtr pNode, CallObj& co)
  {
	  co(pNode);
	  for (auto pChild : pNode->children())
		  DFS(pChild, co);
	
  }

  class Factory : public XmlDocumentFactory
  {
	  IXmlDocument* makeXmlDocument(const std::string& content)
	  {
		  return new XmlDocument(content);
	  }
  };

}

std::ostream& operator<< (std::ostream& stream, const XmlProcessing::XmlDocument& doc);


#endif
