/*
 ============================================================================
 Name        : WMXCtrl.cpp
 Author      : Peter S. Balling
 Created on  : 9. nov. 2017
 Version     :
 Copyright   : MIT
 Description : Tool to do simple manipulation of windows under X11.
 ============================================================================
 */

#include "WMXCtrl.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <iostream>

#define MAX_PROPERTY_VALUE_LEN 4096

WMXCtrl::WMXCtrl() :
 m_display(XOpenDisplay(NULL))
{
	if (!m_display)
		throw "Cannot open display.";
}

WMXCtrl::~WMXCtrl()
{
	XCloseDisplay(m_display);
}

std::list<WindowData> WMXCtrl::GetWindows()
{
	
	std::list<WindowData> res;
	
	for (auto client : GetClients())
	{
		WindowData winData = WindowData {client,
			*(uint64_t *)GetWindowProperty(client, XA_CARDINAL, "_NET_WM_PID").Property(),
			GetWindowTitle(client)};
		res.push_back(winData);
	}
	
	return res;
	
}

const WindowData WMXCtrl::GetWindowByPID(const uint64_t& pid)
{
	
	uint64_t windowPid;
	
	for (auto client : GetClients())
	{
		windowPid = *(uint64_t *)GetWindowProperty(client, XA_CARDINAL, "_NET_WM_PID").Property();
		
		if (pid != windowPid)
			continue;
		
		WindowData winData;
		winData.pid = pid;
		winData.window = client;
		winData.title = GetWindowTitle(client);
		
		return winData;
	}
	
	throw "Couldn't find any window with the pid: " + std::to_string(pid);
	
}

bool WMXCtrl::ActivateWindow(const Window& win, const bool& switchDesktop)
{
	
	if (switchDesktop)
		SwitchDesktop(win);
	
	try
	{
		ClientMSG(win, "_NET_ACTIVE_WINDOW", 0, 0, 0, 0, 0);
	}
	catch (const std::string& msg)
	{
		std::cout << "Unable to activate window: " << msg << std::endl;
		return false;
	}
	XMapRaised(m_display, win);
	
	return true;
	
}

WindowProperty WMXCtrl::GetWindowProperty(const Window& win, const Atom& xaPropType, const std::string& propName)
{
	
	Atom xaPropName, xaRetType;
	int32_t retFormat;
	uint64_t retNItems, bytesAfterRet;
	uint8_t *retPropName;
	
	xaPropName = XInternAtom(m_display, propName.c_str(), false);
	
	// MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
	// long_length = Specifies the length in 32-bit multiples of the data to be retrieved.
	if (XGetWindowProperty(m_display, win, xaPropName, 0, MAX_PROPERTY_VALUE_LEN / 4, false, xaPropType, &xaRetType, &retFormat, &retNItems, &bytesAfterRet, &retPropName) != Success)
		throw "Cannot get " + propName + " property.";
	
	if (xaRetType != xaPropType)
	{
		XFree(retPropName);
		throw "Invalid type of " + propName + " property.";
	}
	
	// Calculate size in bytes of property.
	uint64_t size = (retFormat / 8) * retNItems;
	// Debain patch for handling 64 bit architecture.
	if(retFormat==32)
		size *= sizeof(long)/4;
	
	WindowProperty winProp((char *)retPropName, size);
	XFree(retPropName);
	
	return winProp;
	
}

std::list<const Window> WMXCtrl::GetClients(const std::string& propName)
{
	
	std::list<const Window> windows;
	
	try
	{
		WindowProperty winProp = GetWindowProperty(DefaultRootWindow(m_display), XA_WINDOW, propName);
		Window *windowList = (Window *)winProp.Property();
		for (uint64_t i = 0; i < winProp.Size() / sizeof(Window); i++)
			windows.push_back(windowList[i]);
	}
	catch (const std::string& msg)
	{
		std::cout << msg << std::endl;
		if (propName != "_WIN_CLIENT_LIST")
			windows = GetClients("_WIN_CLIENT_LIST");
	}
	
	return windows;
	
}

const std::string WMXCtrl::GetWindowTitle(const Window& win, const std::string& propName)
{
	
	std::string title;
	Atom xaPropType = (propName == "WM_NAME") ? XA_STRING : XInternAtom(m_display, "UTF8_STRING", False);
	try
	{
		WindowProperty winProp = GetWindowProperty(win, xaPropType, propName);
		title = (char *)winProp.Property();
	}
	catch (const std::string& msg)
	{
		std::cout << msg << std::endl;
		if (propName != "_WIN_CLIENT_LIST")
			title = GetWindowTitle(win, "_NET_WM_NAME");
	}
	
	return title;
	
}

void WMXCtrl::ClientMSG(const Window& win, const std::string& msg, const uint64_t& data0, const uint64_t& data1, const uint64_t& data2, const uint64_t& data3, const uint64_t& data4)
{
	
	XEvent event;
	long mask = SubstructureRedirectMask | SubstructureNotifyMask;
	
	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.message_type = XInternAtom(m_display, msg.c_str(), False);
	event.xclient.window = win;
	event.xclient.format = 32;
	event.xclient.data.l[0] = data0;
	event.xclient.data.l[1] = data1;
	event.xclient.data.l[2] = data2;
	event.xclient.data.l[3] = data3;
	event.xclient.data.l[4] = data4;
	
	if (!XSendEvent(m_display, DefaultRootWindow(m_display), False, mask, &event))
		throw "Cannot send " + msg + " event.";
	
}

// Try to switch to the desktop of win.
bool WMXCtrl::SwitchDesktop(const Window& win)
{
	
	static const char *propertyNames[2] = { "_NET_WM_DESKTOP", "_WIN_WORKSPACE" };
	static uint8_t propIt = 0;
	
	try
	{
		unsigned long desktopID = *(unsigned long *)GetWindowProperty(win, XA_CARDINAL, propertyNames[propIt]).Property();
		
		propIt = 0;
		
		try
		{
			ClientMSG(DefaultRootWindow(m_display), "_NET_CURRENT_DESKTOP", desktopID, 0, 0, 0, 0);
		}
		catch (const std::string& msg)
		{
			std::cout << "Unable to switch desktop: " << msg << std::endl;
			return false;
		}
	}
	catch (const std::string& msg)
	{
		propIt++;
		if (propIt >= 2)
		{
			propIt = 0;
			std::cout << "Cannot find desktop ID of the window: " << msg << std::endl;
			return false;
		}
		else
		{
			std::cout << "Retrying to find desktop ID of the window: " << msg << std::endl;
			SwitchDesktop(win);
		}
	}
	
	return true;
	
}
