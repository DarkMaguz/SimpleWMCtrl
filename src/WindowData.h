/*
 ============================================================================
 Name        : WindowData.h
 Author      : Peter S. Balling
 Created on  : 30. nov. 2017
 Version     :
 Copyright   : MIT
 Description : Tool to do simple manipulation of windows under X11.
 ============================================================================
 */

#ifndef WINDOWDATA_H_
#define WINDOWDATA_H_

#include <unistd.h>
#include <X11/Xlib.h>

#include <string>

struct WindowData
{
	Window window;
	uint64_t pid;
	std::string title;
};

#endif /* WINDOWDATA_H_ */
