/*
 ============================================================================
 Name        : WMXCtrl.h
 Author      : Peter S. Balling
 Created on  : 9. nov. 2017
 Version     :
 Copyright   : MIT
 Description : Tool to do simple manipulation of windows under X11.
 ============================================================================
 */

#ifndef WMXCTRL_H_
#define WMXCTRL_H_

#include <unistd.h>
#include <X11/Xlib.h>

#include <list>
#include <string>

#include "WindowData.h"
#include "WindowProperty.h"

class WMXCtrl
{
	public:
		WMXCtrl();
		virtual ~WMXCtrl();
		
		std::list<WindowData> GetWindows(void);
		const WindowData GetWindowByPID(const uint64_t& pid);
		bool ActivateWindow(const Window& window, const bool& switchDesktop);
		
	private:
		WindowProperty GetWindowProperty(const Window& win, const Atom& xa_prop_type, const std::string& prop_name);
		Window *GetClientsList(unsigned long& size);
		std::list<const Window> GetClients(const std::string& prop_name = "_NET_CLIENT_LIST");
		const std::string GetWindowTitle(const Window& win, const std::string& prop_name = "_NET_WM_NAME");
		void ClientMSG(const Window& win, const std::string& msg, const uint64_t& data0, const uint64_t& data1, const uint64_t& data2, const uint64_t& data3, const uint64_t& data4);
		bool SwitchDesktop(const Window& win);
		
		Display *m_display;
};

#endif /* WMXCTRL_H_ */
