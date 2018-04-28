#ifndef __RESPONSE_FILTER_H__
#define __RESPONSE_FILTER_H__
#pragma once
#include <string>
#include "include/cef_stream.h"
#include "include/wrapper/cef_resource_manager.h"

namespace Browser
{
	namespace Filter
	{
		// Create a resource response filter. Called from test_runner.cc.
		CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefResponse> response);
	}
}

#endif
