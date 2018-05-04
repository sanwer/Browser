#ifndef __MAIN_CONTEXT_H__
#define __MAIN_CONTEXT_H__
#pragma once
#include <string>
#include "include/base/cef_ref_counted.h"
#include "include/base/cef_scoped_ptr.h"
#include "include/base/cef_thread_checker.h"
#include "include/internal/cef_types_wrappers.h"
#include "include/cef_app.h"
#include "include/cef_command_line.h"
#include "BrowserDlgManager.h"

namespace Browser
{
	class BrowserDlgManager;

	// Used to store global context in the browser process. The methods of this
	// class are thread-safe unless otherwise indicated.
	class MainContext
	{
	public:
		// Returns the singleton instance of this object.
		static MainContext* Get();
		MainContext(CefRefPtr<CefCommandLine> command_line, bool terminate_when_all_windows_closed);
  
 public:
		// Returns the full path to the console log file.
		std::string GetConsoleLogPath();

		// Returns the full path to |file_name|.
		std::string GetDownloadPath(const std::string& file_name);

		// Returns the app working directory including trailing path separator.
		std::string GetAppWorkingDirectory();

		// Returns the main application URL.
		std::wstring GetMainURL();

		// Returns the background color.
		virtual cef_color_t GetBackgroundColor();

		// Populate |settings| based on command-line arguments.
		virtual void PopulateSettings(CefSettings* settings);
		virtual void PopulateBrowserSettings(CefBrowserSettings* settings);

		// Returns the object used to create/manage RootWindow instances.
		virtual BrowserDlgManager* GetBrowserDlgManager();

		// Initialize CEF and associated main context state. This method must be
		// called on the same thread that created this object.
		bool Initialize(const CefMainArgs& args,
			const CefSettings& settings,
			CefRefPtr<CefApp> application,
			void* windows_sandbox_info);

		// Shut down CEF and associated context state. This method must be called on
		// the same thread that created this object.
		void Shutdown();

	private:
		// Allow deletion via scoped_ptr only.
		friend struct base::DefaultDeleter<MainContext>;
		virtual ~MainContext();

		// Returns true if the context is in a valid state (initialized and not yet
		// shut down).
		bool InValidState() const {
			return m_bInitialized && !m_bShutdown;
		}

		CefRefPtr<CefCommandLine> m_CommandLine;
		const bool m_bTerminateWhenAllWindowsClosed;

		// Track context state. Accessing these variables from multiple threads is
		// safe because only a single thread will exist at the time that they're set
		// (during context initialization and shutdown).
		bool m_bInitialized;
		bool m_bShutdown;

		std::wstring m_sMainUrl;
		cef_color_t m_BackgroundColor;

		scoped_ptr<BrowserDlgManager> m_BrowserDlgManager;

		// Used to verify that methods are called on the correct thread.
		base::ThreadChecker m_ThreadChecker;

		DISALLOW_COPY_AND_ASSIGN(MainContext);
	};
}

#endif
