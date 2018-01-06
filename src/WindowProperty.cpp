/*
 ============================================================================
 Name        : WindowProperty.cpp
 Author      : Peter S. Balling
 Created on  : 11. nov. 2017
 Version     :
 Copyright   : MIT
 Description : Tool to do simple manipulation of windows under X11.
 ============================================================================
 */

#include "WindowProperty.h"

#include <algorithm>
#include <iostream>

WindowProperty::WindowProperty(const char *property, const uint64_t& size, bool nullTerminate) :
	m_property(new char[size+nullTerminate]),
	m_size(size)
{
	std::copy(property, property + m_size, (char *)m_property);
	if (nullTerminate)
		((char *)m_property)[m_size] = '\0';
}

WindowProperty::~WindowProperty()
{
	delete[] (char *)m_property;
}

const property_t& WindowProperty::Property( void ) const
{
	return m_property;
}

const uint64_t& WindowProperty::Size( void ) const
{
	return m_size;
}
