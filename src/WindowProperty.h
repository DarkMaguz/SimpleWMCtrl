/*
 ============================================================================
 Name        : WindowProperty.h
 Author      : Peter S. Balling
 Created on  : 11. nov. 2017
 Version     :
 Copyright   : MIT
 Description : Tool to do simple manipulation of windows under X11.
 ============================================================================
 */

#ifndef WINDOWPROPERTY_H_
#define WINDOWPROPERTY_H_

#include <stdint.h>

typedef void * property_t;

class WindowProperty
{
	public:
		WindowProperty(const char *property, const uint64_t& size, bool nullTerminate = true);
		virtual ~WindowProperty();
		
		const property_t& Property(void) const;
		const uint64_t& Size(void) const;
		
	private:
		property_t m_property;
		const uint64_t m_size;
};

#endif /* WINDOWPROPERTY_H_ */
