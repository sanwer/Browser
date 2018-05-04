#ifndef __TEMP_WINDOW_H__
#define __TEMP_WINDOW_H__
#pragma once
#include "include/cef_base.h"

namespace Browser
{
	// Represents a singleton hidden window that acts at temporary parent for
	// popup browsers.
	class TempWindow
	{
	public:
		// Returns the singleton window handle.
		static CefWindowHandle GetWindowHandle();

	private:
		// A single instance will be created/owned by BrowserDlgManager.
		friend class BrowserDlgManager;

		TempWindow();
		~TempWindow();

		CefWindowHandle m_hWnd;

		DISALLOW_COPY_AND_ASSIGN(TempWindow);
	};
}

#endif
