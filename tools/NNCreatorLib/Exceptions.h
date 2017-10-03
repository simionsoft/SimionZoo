#pragma once
#include <stdexcept>
#include <sstream>

class ProblemParserElementNotFound : public std::runtime_error
{
private:

	std::string m_elementName;
public:
	ProblemParserElementNotFound(std::string elementName)
		: std::runtime_error("Error parsing the XML file of the Problem. "
			"Error parsing the XML file of the Problem. "
			"Element \"" + elementName + "\" not found.")
	{
		m_elementName = elementName;
	}

	const std::string& getElementName() const { return m_elementName; }
};

class ProblemParserElementValueNotValid : public std::runtime_error
{
private:

	std::string m_elementName;
public:
	ProblemParserElementValueNotValid(std::string elementName)
		: std::runtime_error("Error parsing the XML file of the Problem. "
			"Error parsing the XML file of the Problem. "
			"Value of element \"" + elementName + "\" is not valid.")
	{
		m_elementName = elementName;
	}

	const std::string& getElementName() const { return m_elementName; }
};