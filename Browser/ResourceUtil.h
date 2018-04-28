#ifndef __RESOURCE_UTIL_H__
#define __RESOURCE_UTIL_H__
#pragma once
#include <string>
#include "include/cef_stream.h"
#include "include/wrapper/cef_resource_manager.h"

namespace Browser
{
	// Retrieve a resource as a string.
	bool LoadBinaryResource(const char* resource_name, std::string& resource_data);

	// Retrieve a resource as a steam reader.
	CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char* resource_name);

#ifdef OS_WIN
	// Create a new provider for loading binary resources.
	CefResourceManager::Provider* CreateBinaryResourceProvider(const std::string& url_path);
#endif
}

#endif
