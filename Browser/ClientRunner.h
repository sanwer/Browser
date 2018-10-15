#ifndef __CLIENTRUNNER_H__
#define __CLIENTRUNNER_H__
#pragma once
#include <set>
#include <string>
#include "include/cef_browser.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"

namespace Browser
{
	namespace ClientRunner
	{
		// Returns a data: URI with the specified contents.
		std::string GetDataURI(const std::string& data,
			const std::string& mime_type);

		// Returns the string representation of the specified error code.
		std::string GetErrorString(cef_errorcode_t code);

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
