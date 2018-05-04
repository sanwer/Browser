#ifndef __BROWSERAPP_H__
#define __BROWSERAPP_H__
#pragma once
#include <vector>
#include "include/cef_app.h"

namespace Browser
{
	class ClientApp : public CefApp
	{
	public:
		ClientApp();

		enum ProcessType {
			BrowserProcess,
			RendererProcess,
			OtherProcess,
		};

		// Determine the process type based on command-line arguments.
		static ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);

	protected:
		std::vector<CefString> cookie_schemes;

	private:
		DISALLOW_COPY_AND_ASSIGN(ClientApp);
	};

	class ClientAppOther : public ClientApp {
	public:
		ClientAppOther(){}

	private:
		IMPLEMENT_REFCOUNTING(ClientAppOther);
		DISALLOW_COPY_AND_ASSIGN(ClientAppOther);
	};
}

#endif
