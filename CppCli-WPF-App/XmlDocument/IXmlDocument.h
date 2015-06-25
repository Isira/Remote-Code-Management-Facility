#ifndef IXMLDOCUMENT_H
#define IXMLDOCUMENT_H
//////////////////////////////////////////////////////////////////////////////////
// IXmlDocument.h - Interface and Object Factories to build dlls				//
// -																			//
//																				//
// Authour: Isira Samarasekera													/
//	Version 1.0 04 April 2015													//
//////////////////////////////////////////////////////////////////////////////////
#ifdef XML_EXPORT
#define XML_EXPORT_API __declspec(dllexport)
#else
#define XML_EXPORT_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

struct IAbstractXmlElement
{
	virtual bool addChildNativePtr(IAbstractXmlElement* pChild)= 0;
	virtual bool removeChildNativePtr(IAbstractXmlElement* pChild) = 0;
	virtual bool addAttrib(const std::string& name, const std::string& value) = 0;
	virtual bool removeAttrib(const std::string& name) = 0;
	virtual std::string value() = 0;
	virtual std::string toString() = 0;
	virtual std::vector<std::pair<std::string, std::string>> attribs() = 0;
	virtual std::string tag() = 0;
	virtual std::string attribValue(const std::string& key) = 0;
	virtual std::vector<IAbstractXmlElement*> childreNativePtrs() = 0;
};


struct IXmlDocument
{

	virtual IXmlDocument& element(const std::string& tag) = 0;          
	virtual IXmlDocument& elements(const std::string& tag) = 0;         
	virtual IXmlDocument& children(const std::string& tag = "") = 0;    
	virtual IXmlDocument& descendents(const std::string& tag = "")= 0;  
	virtual IXmlDocument& elementsWithAttribute(const std::string& attribute, const std::string& value) = 0;  
	virtual std::vector<IAbstractXmlElement*> selectNativePointer() = 0;   
	virtual std::string toString() const= 0;
};

extern "C" {
	struct XmlDocumentFactory
	{
		XML_EXPORT_API IXmlDocument* makeXmlDocument(const std::string& content);
	};

	struct XmlFactory
	{
		XML_EXPORT_API IAbstractXmlElement* makeDocElement(IAbstractXmlElement* pRoot = nullptr);
		XML_EXPORT_API IAbstractXmlElement* makeTextElement(const std::string& text);
		XML_EXPORT_API IAbstractXmlElement* makeTaggedElement(const std::string& tag);
		XML_EXPORT_API IAbstractXmlElement* makeCommentElement(const std::string& comment);
		XML_EXPORT_API IAbstractXmlElement* makeXmlDeclarElement();
		XML_EXPORT_API IAbstractXmlElement* makeProcInstrElement(const std::string& text);
		XML_EXPORT_API IAbstractXmlElement* makeTaggedElementRowPointer(const std::string& tag);
		
	};
}
#endif