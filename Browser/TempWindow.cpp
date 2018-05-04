#include "stdafx.h"
#include "TempWindow.h"
#include <windows.h>
#include "include/base/cef_logging.h"

namespace Browser
{
	namespace {
		const wchar_t kWndClass[] = L"Client_TempWindow";

		// Create the temp window.
		HWND CreateTempWindow() {
			HINSTANCE hInstance = ::GetModuleHandle(NULL);

			WNDCLASSEX wc = {0};
			wc.cbSize = sizeof(wc);
			wc.lpfnWndProc = DefWindowProc;
			wc.hInstance = hInstance;
			wc.lpszClassName = kWndClass;
			RegisterClassEx(&wc);

			// Create a 1x1 pixel hidden window.
			return CreateWindow(kWndClass, 0,
				WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
				0, 0, 1, 1,
				NULL, NULL, hInstance, NULL);
		}

		TempWindow* g_temp_window = NULL;
	}

	TempWindow::TempWindow() : m_hWnd(NULL) {
		DCHECK(!g_temp_window);
		g_temp_window = this;
		m_hWnd = CreateTempWindow();
		CHECK(m_hWnd);
	}

	TempWindow::~TempWindow() {
		g_temp_window = NULL;
		DCHECK(m_hWnd);
		DestroyWindow(m_hWnd);
	}

	// static
	CefWindowHandle TempWindow::GetWindowHandle() {
		DCHECK(g_temp_window);
		return g_temp_window->m_hWnd;
	}
}
