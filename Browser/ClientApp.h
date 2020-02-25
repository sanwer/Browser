#ifndef __CLIENT_APP_H__
#define __CLIENT_APP_H__
#pragma once
#include <vector>
#include "include/cef_version.h"
#include "include/cef_app.h"

namespace Browser
{
	class ClientApp : public CefApp
	{
	public:
		ClientApp(){};

#if CHROME_VERSION_BUILD >= 2924
		class Delegate : public virtual CefBaseRefCounted
#else
		class Delegate : public virtual CefBase
#endif
		{};

		static bool IsBrowser(CefRefPtr<CefCommandLine> command_line){
			return !command_line->HasSwitch("type");
		}

		static bool IsRenderer(CefRefPtr<CefCommandLine> command_line) {
			const std::string& process_type = command_line->GetSwitchValue("type");
			return process_type == "renderer";
		}

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
