/*
 ============================================================================
 Name        : SimpleWMCtrl.cpp
 Author      : Peter S. Balling
 Created on  : 9. nov. 2017
 Version     :
 Copyright   : MIT
 Description : Tool to do simple manipulation of windows under X11.
 ============================================================================
 */

#include "WMXCtrl.h"
#include "WindowProperty.h"
#include <iostream>
#include <string>
#include <list>
#include <map>

#include <json.hpp>
using json = nlohmann::json;

enum args
{
	cmd_0,
	cmd_list,
	cmd_list_json,
	cmd_activate_pid,
	cmd_activate_win,
	cmd_help
};

const char useage[] =
{
	"\nUsage: { swmctrl } [ OPTION ]\n" \
	"\tOptions\t\t"		"Long options\t\t"				"Descriptions\n" \
	"\t -l\t\t"				" --list\t\t\t"						" List windows.\n" \
	"\t -L\t\t"				" --list-json\t\t"				" List windows in JSON format.\n" \
	"\t -a PID\t\t"		" --activate-pid PID \t"	" Activate window.\n" \
	"\t -A WIN\t\t"		" --activate-win WIN \t"	" Activate window.\n" \
	"\t -h\t\t"				" --help\t\t\t"						" Your looking at it.\n"
};

void print_useage(void)
{
	std::cout << useage << std::endl;
}

void list_windows(bool useJSON)
{
	
	WMXCtrl *wmXCtrl = new WMXCtrl;
	
	std::list<WindowData> windows = wmXCtrl->GetWindows();
	
	if (useJSON)
	{
		
		json output;
		output["windows"] = json::array();
		
		for (auto window : windows)
		{
			json win;
			win["id"] = window.window;
			win["pid"] = window.pid;
			win["title"] = window.title;
			output["windows"].push_back(win);
		}
		
		std::cout << output << std::endl;
		
	}
	else
	{
		
		std::cout << "Window count: " << windows.size() << std::endl;
		
		std::cout << "window ID\tPID\ttitle" << std::endl;
		for (auto window : windows)
		{
			std::cout << window.window << "\t";
			std::cout << window.pid << "\t";
			std::cout << window.title << std::endl;
		}
		
	}
	
	delete wmXCtrl;
	
}

void activate_by_win(const uint64_t& win)
{
	
	WMXCtrl *wmXCtrl = new WMXCtrl;
	
	wmXCtrl->ActivateWindow(win, true);
	
	delete wmXCtrl;
	
}

void activate_by_pid(const uint64_t& pid)
{
	
	WMXCtrl *wmXCtrl = new WMXCtrl;
	WindowData winData;
	
	try
	{
		winData = wmXCtrl->GetWindowByPID(pid);
	}
	catch (const std::string& msg)
	{
		std::cout << "Failed to activate window: " << msg << std::endl;
	}
	
	if (winData.pid == pid)
		wmXCtrl->ActivateWindow(winData.window, true);
	
	delete wmXCtrl;
	
}

int main(int argc, char **argv)
{
	
	//list_windows(false);
	//activate_by_win(atoi(argv[1]));
	//activate_by_pid(atoi(argv[1]));
	
	std::map<std::string, args> argsMap;
	argsMap["-l"] = cmd_list;
	argsMap["--list"] = cmd_list;
	argsMap["-L"] = cmd_list_json;
	argsMap["--list-json"] = cmd_list_json;
	argsMap["-a"] = cmd_activate_pid;
	argsMap["--activate-pid"] = cmd_activate_pid;
	argsMap["-A"] = cmd_activate_win;
	argsMap["--activate-win"] = cmd_activate_win;
	argsMap["-h"] = cmd_help;
	argsMap["--help"] = cmd_help;
	
	if ( argc > 3 )
	{
		print_useage();
		return 0;
	}
	
	if ( argc != 1 )
	{
		
		switch ( argsMap[argv[1]] )
		{
			case cmd_list:
			{
				list_windows(false);
				break;
			}
			case cmd_list_json:
			{
				list_windows(true);
				break;
			}
			case cmd_activate_pid:
			{
				if ( argc < 3 )
				{
					print_useage();
					return 0;
				}
				
				activate_by_pid(atoi(argv[2]));
				
				break;
			}
			case cmd_activate_win:
			{
				if ( argc < 3 )
				{
					print_useage();
					return 0;
				}
				
				activate_by_win(atoi(argv[2]));
				
				break;
			}
			case cmd_help:
			default:
				print_useage();
				break;
		}
		
	}
	else
	{
		
		print_useage();
		
	}
	
	return 0;
	
}
