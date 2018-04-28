#include <sstream>
#include "include/base/cef_bind.h"
#include "include/cef_parser.h"
#include "include/cef_task.h"
#include "include/cef_trace.h"
#include "include/cef_web_plugin.h"
#include "ClientApp.h"
#include "ResourceUtil.h"
#include "ClientRunner.h"

namespace Browser
{
	namespace ClientRunner
	{
		const char kTestOrigin[] = "http://tests/";

		// Replace all instances of |from| with |to| in |str|.
		std::string StringReplace(const std::string& str, const std::string& from, const std::string& to)
		{
			std::string result = str;
			std::string::size_type pos = 0;
			std::string::size_type from_len = from.length();
			std::string::size_type to_len = to.length();
			do {
				pos = result.find(from, pos);
				if (pos != std::string::npos) {
					result.replace(pos, from_len, to);
					pos += to_len;
				}
			} while (pos != std::string::npos);
			return result;
		}

		// Add a file extension to |url| if none is currently specified.
		std::string RequestUrlFilter(const std::string& url) {
			if (url.find(kTestOrigin) != 0U) {
				// Don't filter anything outside of the test origin.
				return url;
			}

			// Identify where the query or fragment component, if any, begins.
			size_t suffix_pos = url.find('?');
			if (suffix_pos == std::string::npos)
				suffix_pos = url.find('#');

			std::string url_base, url_suffix;
			if (suffix_pos == std::string::npos) {
				url_base = url;
			} else {
				url_base = url.substr(0, suffix_pos);
				url_suffix = url.substr(suffix_pos);
			}

			// Identify the last path component.
			size_t path_pos = url_base.rfind('/');
			if (path_pos == std::string::npos)
				return url;

			const std::string& path_component = url_base.substr(path_pos);

			// Identify if a file extension is currently specified.
			size_t ext_pos = path_component.rfind(".");
			if (ext_pos != std::string::npos)
				return url;

			// Rebuild the URL with a file extension.
			return url_base + ".html" + url_suffix;
		}

		void SetupResourceManager(CefRefPtr<CefResourceManager> resource_manager) {
			if (!CefCurrentlyOn(TID_IO)) {
				// Execute on the browser IO thread.
				CefPostTask(TID_IO, base::Bind(SetupResourceManager, resource_manager));
				return;
			}

			const std::string& test_origin = kTestOrigin;

			// Add the URL filter.
			resource_manager->SetUrlFilter(base::Bind(RequestUrlFilter));

			// Add provider for resource dumps.
			//resource_manager->AddProvider(new RequestDumpResourceProvider(test_origin + "request.html"),0, std::string());

			// Add provider for bundled resource files.
			// Read resources from the binary.
			resource_manager->AddProvider(CreateBinaryResourceProvider(test_origin), 100, std::string());
		}

		void Alert(CefRefPtr<CefBrowser> browser, const std::string& message)
		{
			// Escape special characters in the message.
			std::string msg = StringReplace(message, "\\", "\\\\");
			msg = StringReplace(msg, "'", "\\'");

			// Execute a JavaScript alert().
			CefRefPtr<CefFrame> frame = browser->GetMainFrame();
			frame->ExecuteJavaScript("alert('" + msg + "');", frame->GetURL(), 0);
		}

		void CreateMessageHandlers(MessageHandlerSet& handlers)
		{
		}
	}

	// static
	void ClientAppBrowser::CreateDelegates(DelegateSet& delegates) {
	}

	// static
	void ClientAppRenderer::CreateDelegates(DelegateSet& delegates) {
		Renderer::CreateDelegates(delegates);
	}
}
