#include "stdafx.h"
#include "ClientApp.h"
#include "include/cef_command_line.h"

namespace Browser
{
	namespace {
		// These flags must match the Chromium values.
		const char kProcessType[] = "type";
		const char kRendererProcess[] = "renderer";
	}

	ClientApp::ClientApp() {
	}

	// static
	ClientApp::ProcessType ClientApp::GetProcessType(CefRefPtr<CefCommandLine> command_line)
	{
		if (!command_line->HasSwitch(kProcessType))
			return BrowserProcess;
		const std::string& process_type = command_line->GetSwitchValue(kProcessType);
		if (process_type == kRendererProcess)
			return RendererProcess;
		return OtherProcess;
	}
}
