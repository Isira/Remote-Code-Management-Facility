//////////////////////////////////////////////////////////////////////////
// XmlElement.cpp -  define XML Element types  							//
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

#include "XmlElement.h"
#include <iostream>


using namespace XmlProcessing;

std::shared_ptr<AbstractXmlElement> AbstractXmlElement::sharedPointerFromRowPointer(IAbstractXmlElement* ptr)
{
	std::shared_ptr<AbstractXmlElement> child;
	// don't add TextElements
	TextElement* e1 = dynamic_cast<TextElement*>(ptr);
	if (e1 != 0)
	{
		child = std::make_shared<TextElement>(ptr->tag());
		return child;
	} 
	//	

	DocElement* e2 = dynamic_cast<DocElement*>(ptr);
	if (e2 != 0)
	{
		child = std::make_shared<DocElement>(std::move(*e2));
		return child;
	} 
	//	

	// add XmlDeclaration
	XmlDeclarElement* e3 = dynamic_cast<XmlDeclarElement*>(ptr);
	if (e3 != 0)
	{
		child = std::make_shared<XmlDeclarElement>();
		return child;
	}

	// add ProcInstrElements and CommentElements 
	TaggedElement* e4 = dynamic_cast<TaggedElement*>(ptr);
	if (e4 == nullptr) // is not a TaggedElement
	{
		child = std::make_shared<TaggedElement>(e4->tag());
		for (auto it:e4->children())
		{
			child->addChild(it);
		}
		for (auto it : e4->attribs())
		{
			child->addAttrib(it.first,it.second);
		}
		return child;
	}

	CommentElement* e5 = dynamic_cast<CommentElement*>(ptr);
	if (e5 == nullptr) // is not a TaggedElement
	{
		child = std::make_shared<CommentElement>(std::move(*e5));
		return child;
	}

	ProcInstrElement* e6 = dynamic_cast<ProcInstrElement*>(ptr);
	if (e6 == nullptr) // is not a TaggedElement
	{
		child = std::make_shared<ProcInstrElement>(ptr->value());
		for (auto it : e4->attribs())
		{
			child->addAttrib(it.first, it.second);
		}
		return child;
	}

	return child;

}
//----< doc element constructor and destructor >-----------------------------

DocElement::DocElement(std::shared_ptr<AbstractXmlElement> pRoot) 
{
  if (pRoot != nullptr)
    children_.push_back(pRoot);
}

//----< doc element move constructor >-----------------------------

DocElement::DocElement(DocElement&& doc) :children_(std::move(doc.children_))
{

}

//----< doc element move assingmenet operator >-----------------------------

DocElement& DocElement::operator=(DocElement&& doc)
{
	if (this == &doc)
		return *this;
	children_ = std::move(doc.children_);
	return *this;
}

//----< private helper for DocElement::addChild(...) >-----------------------

bool DocElement::hasXmlRoot()
{
  for (auto pElement : children_)
  {
    if (dynamic_cast<TaggedElement*>(pElement.get()) != nullptr)
      return true;
  }
  return false;
}

//----< private helper for DocElement::addChild(...) >-----------------------

bool DocElement::hasXmlDecl()
{
	for (auto pElement : children_)
	{
		if (dynamic_cast<XmlDeclarElement*>(pElement.get()) != nullptr)
			return true;
	}
	return false;
}

bool DocElement::addChildNativePtr(IAbstractXmlElement* pChild)
{
	// don't add TextElements
	TextElement* pTxEl = dynamic_cast<TextElement*>(pChild);
	if (pTxEl != 0)  // don't add text elements to DocElement
		return false;

	// don't add another DocElement
	DocElement* pDcEl = dynamic_cast<DocElement*>(pChild);
	if (pDcEl != 0)  // don't add text elements to DocElement
		return false;

	// add XmlDeclaration
	XmlDeclarElement* pXcEl = dynamic_cast<XmlDeclarElement*>(pChild);
	if (pXcEl != 0)
	{
		if (!hasXmlDecl()) // add only one declaration always in the begining
		{
			AbstractXmlElement* pChild_ = dynamic_cast<AbstractXmlElement*>(pChild);
			children_.insert(children_.begin(), std::shared_ptr<AbstractXmlElement>(pChild_));
			return true;
		}
		return false;
	}

	// add ProcInstrElements and CommentElements 
	TaggedElement* te = dynamic_cast<TaggedElement*>(pChild);
	if (te == nullptr) // is not a TaggedElement
	{
		AbstractXmlElement* pChild_ = dynamic_cast<AbstractXmlElement*>(pChild);
		children_.push_back(std::shared_ptr<AbstractXmlElement>(pChild_));
		return true;
	}

	// add only one TaggedElement
	if (!hasXmlRoot())
	{
		AbstractXmlElement* pChild_ = dynamic_cast<AbstractXmlElement*>(pChild);
		children_.push_back(std::shared_ptr<AbstractXmlElement>(pChild_));
		return true;
	}
	return false;

}

bool DocElement::removeChildNativePtr(IAbstractXmlElement* pChild)
{
	std::shared_ptr<AbstractXmlElement> child = sharedPointerFromRowPointer(pChild);
	return removeChild(child);
}
//----< add only one child to doc element using pointer to child >-----------

bool DocElement::addChild(std::shared_ptr<AbstractXmlElement> pChild)
{
  // don't add TextElements
  TextElement* pTxEl = dynamic_cast<TextElement*>(pChild.get());
  if (pTxEl != 0)  // don't add text elements to DocElement
    return false;

  // don't add another DocElement
  DocElement* pDcEl = dynamic_cast<DocElement*>(pChild.get());
  if (pDcEl != 0)  // don't add text elements to DocElement
    return false;

  // add XmlDeclaration
  XmlDeclarElement* pXcEl = dynamic_cast<XmlDeclarElement*>(pChild.get());
  if (pXcEl != 0)
  {
	  if (!hasXmlDecl()) // add only one declaration always in the begining
	  {
		  children_.insert(children_.begin(),pChild);
		  return true;
	  }
	  return false;
  }  

  // add ProcInstrElements and CommentElements 
  TaggedElement* te = dynamic_cast<TaggedElement*>(pChild.get());
  if (te == nullptr) // is not a TaggedElement
  {
	  DocElement* pDcEl = dynamic_cast<DocElement*>(pChild.get());
    children_.push_back(pChild);
    return true;
  }

  // add only one TaggedElement
  if (!hasXmlRoot())
  {
    children_.push_back(pChild);
    return true;
  }
  return false;
}
//----< remove child from doc element using pointer to child >---------------

bool DocElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild)
{
  auto iter = std::find(begin(children_), end(children_), pChild);
  if (iter != end(children_))
  {
    children_.erase(iter);
    return true;
  }
  return false;
}
//----< return value = tag from doc element >-----------------------------

std::string DocElement::value() { return std::string(""); }

//----< generate xml string for tagged element >-----------------------------

std::string DocElement::toString()
{
  std::string rtn;
  for (auto elem : children_)
    rtn += elem->toString();
  return rtn;
}

std::vector<IAbstractXmlElement*> DocElement::childreNativePtrs()
{
	std::vector<IAbstractXmlElement*> _childrenPtrs;
	for (auto it : children_)
		_childrenPtrs.push_back(it.get());
	return _childrenPtrs;
}
//----< add child to tagged element using pointer to child >-----------------

bool TaggedElement::addChildNativePtr(IAbstractXmlElement* pChild)
{
	XmlDeclarElement* pTxEl = dynamic_cast<XmlDeclarElement*>(pChild);
	if (pTxEl != 0)
		return false;
	AbstractXmlElement* pChild_ = dynamic_cast<AbstractXmlElement*>(pChild);
	children_.push_back(std::shared_ptr<AbstractXmlElement>(pChild_));
	return true;

}

bool TaggedElement::removeChildNativePtr(IAbstractXmlElement* pChild)
{
	std::shared_ptr<AbstractXmlElement> child = sharedPointerFromRowPointer(pChild);
	return addChild(child);

}

bool TaggedElement::addChild(std::shared_ptr<AbstractXmlElement> pChild)
{
	// don't add XmlDeclarElement
 XmlDeclarElement* pTxEl = dynamic_cast<XmlDeclarElement*>(pChild.get());
 if (pTxEl != 0)  
	return false;
  children_.push_back(pChild);
  return true;
}
//----< remove child from tagged element using pointer to child >------------

bool TaggedElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild)
{
  auto iter = std::find(begin(children_), end(children_), pChild);
  if (iter != end(children_))
  {
    children_.erase(iter);
    return true;
  }
  return false;
}
//----< remove child from tagged element using child's value >---------------

bool TaggedElement::removeChild(const std::string& value)
{
  for (auto iter = begin(children_); iter != end(children_); ++iter)
  {
    if ((*iter)->value() == value)
    {
      children_.erase(iter);
      return true;
    }
  }
  return false;
}
//----< add attribute to tagged element >------------------------------------

bool TaggedElement::addAttrib(const std::string& name, const std::string& value)
{
  removeAttrib(name);
  std::pair<std::string, std::string> atPair(name, value);
  attribs_.push_back(atPair);
  return true;
}
//----< remove attribute from tagged element >-------------------------------

bool TaggedElement::removeAttrib(const std::string& name)
{
  for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
  {
    if (iter->first == name)
    {
      attribs_.erase(iter);
      return true;
    }
  }
  return false;
}
//----< return value = tag from tagged element >-----------------------------

std::string TaggedElement::value() { return tag_; }

//----< generate xml string for tagged element >-----------------------------

std::string TaggedElement::toString()
{
  std::string spacer(tabSize*count++, ' ');
  std::string xml = "\n" + spacer + "<" + tag_;
  for (auto at : attribs_)
  {
    xml += " ";
    xml += at.first;
    xml += "=\"";
    xml += at.second;
    xml += "\"";
  }
  xml += ">";
  for (auto pChild : children_)
    xml += pChild->toString();
  xml += "\n" + spacer + "</" + tag_ + ">";
  --count;
  return xml;
}
//----< generate xml string for text element >-------------------------------

std::string TextElement::toString()
{
  std::string spacer(tabSize *count++, ' ');
  std::string xml = "\n" + spacer + text_;
  --count;
  return xml;
}

std::vector<IAbstractXmlElement*> TaggedElement::childreNativePtrs()
{
	std::vector<IAbstractXmlElement*> _childrenPtrs;
	for (auto it : children_)
		_childrenPtrs.push_back(it.get());
	return _childrenPtrs;
}
//----< generate xml string for Priocessing Instruction element >-------------------------------

std::string ProcInstrElement::toString()
{
	std::string spacer(tabSize * count++, ' ');
	std::string xml = "\n" + spacer + "<?" + type_ ;
	for (auto at : attribs_)
	{
		xml += " ";
		xml += at.first;
		xml += "=\"";
		xml += at.second;
		xml += "\"";
	}
	xml += "?>";
	--count;
	return xml;
}

//----< add attribute to ProcInstElement >-----------------------------------

bool ProcInstrElement::addAttrib(const std::string& name, const std::string& value)
{
  removeAttrib(name);
  std::pair<std::string, std::string> atPair(name, value);
  attribs_.push_back(atPair);
  return true;
}
//----< remove attribute from ProcInstElement >------------------------------

bool ProcInstrElement::removeAttrib(const std::string& name)
{
  for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
  {
    if (iter->first == name)
    {
      attribs_.erase(iter);
      return true;
    }
  }
  return false;
}
//----< generate xml string for text element >-------------------------------

std::string XmlDeclarElement::toString()
{
  std::string xml = "<?xml";
  for (auto at : attribs_)
  {
    xml += " ";
    xml += at.first;
    xml += "=\"";
    xml += at.second;
    xml += "\"";
  }
  xml += "?>";
  return xml;
}
//----< add attribute to ProcInstElement >-----------------------------------

bool XmlDeclarElement::addAttrib(const std::string& name, const std::string& value)
{
	removeAttrib(name);
  std::pair<std::string, std::string> atPair(name, value);
  attribs_.push_back(atPair);
  return true;
}
//----< remove attribute from ProcInstElement >------------------------------

bool XmlDeclarElement::removeAttrib(const std::string& name)
{
  for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
  {
    if (iter->first == name)
    {
      attribs_.erase(iter);
      return true;
    }
  }
  return false;
}



std::string CommentElement::toString()
{
	std::string spacer(tabSize *count++, ' ');
	std::string xml = "\n" + spacer + "<!--"+commentText_+"-->";
	--count;
	return xml;
}

CommentElement::CommentElement(CommentElement&& other) :commentText_(std::move(other.commentText_))
{ 
	std::cout << "move Construct\n"; 
}
CommentElement& CommentElement::operator=(CommentElement&& other) {
	commentText_ = std::move(other.commentText_);
	std::cout << "move assigned\n";
	return *this;
}
//----< helper function displays titles >------------------------------------

void XmlProcessing::title(const std::string& title, char underlineChar)
{
  std::cout << "\n  " << title;
  std::cout << "\n " << std::string(title.size() + 2, underlineChar);
}

//----< test stub >----------------------------------------------------------

#ifdef TEST_XMLELEMENT

int main()
{
  title("Testing XmlElement Package", '=');
  std::cout << "\n";

  using sPtr = std::shared_ptr < AbstractXmlElement > ;

  sPtr root(new TaggedElement("root"));
  sPtr txt(new TextElement("this is a test"));
  root->addChild(txt);

  sPtr child(new TaggedElement("child"));
  sPtr txtChild(new TextElement("this is another test"));
  child->addChild(txtChild);
  child->addAttrib("first", "test");
  root->addChild(child);

  sPtr docEl(new DocElement(root));
  std::cout << "  " << docEl->toString();
  std::cout << "\n\n";
}

#endif
