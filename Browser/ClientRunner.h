#ifndef __CLIENTRUNNER_H__
#define __CLIENTRUNNER_H__
#pragma once
#include <set>
#include <string>
#include "include/cef_base.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"

namespace Browser
{
	namespace ClientRunner
	{
		// Set up the resource manager for tests.
		void SetupResourceManager(CefRefPtr<CefResourceManager> resource_manager);

		// Show a JS alert message.
		void Alert(CefRefPtr<CefBrowser> browser, const std::string& message);

		// Create all CefMessageRouterBrowserSide::Handler objects. They will be
		// deleted when the ClientHandler is destroyed.
		typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
		void CreateMessageHandlers(MessageHandlerSet& handlers);
	}
}

#endif
