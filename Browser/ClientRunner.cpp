#include <sstream>
#include <algorithm>
#include <string>
#include "include/base/cef_bind.h"
#include "include/cef_parser.h"
#include "include/cef_task.h"
#include "include/cef_trace.h"
#include "include/cef_web_plugin.h"
#include "include/wrapper/cef_stream_resource_handler.h"
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

		std::string GetDataURI(const std::string& data, const std::string& mime_type)
		{
			return "data:" + mime_type + ";base64," +
				CefURIEncode(CefBase64Encode(data.data(), data.size()), false).ToString();
		}

		std::string GetErrorString(cef_errorcode_t code)
		{
			// Case condition that returns |code| as a string.
			#define CASE(code) case code: return #code

			switch (code) {
				CASE(ERR_NONE);
				CASE(ERR_FAILED);
				CASE(ERR_ABORTED);
				CASE(ERR_INVALID_ARGUMENT);
				CASE(ERR_INVALID_HANDLE);
				CASE(ERR_FILE_NOT_FOUND);
				CASE(ERR_TIMED_OUT);
				CASE(ERR_FILE_TOO_BIG);
				CASE(ERR_UNEXPECTED);
				CASE(ERR_ACCESS_DENIED);
				CASE(ERR_NOT_IMPLEMENTED);
				CASE(ERR_CONNECTION_CLOSED);
				CASE(ERR_CONNECTION_RESET);
				CASE(ERR_CONNECTION_REFUSED);
				CASE(ERR_CONNECTION_ABORTED);
				CASE(ERR_CONNECTION_FAILED);
				CASE(ERR_NAME_NOT_RESOLVED);
				CASE(ERR_INTERNET_DISCONNECTED);
				CASE(ERR_SSL_PROTOCOL_ERROR);
				CASE(ERR_ADDRESS_INVALID);
				CASE(ERR_ADDRESS_UNREACHABLE);
				CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
				CASE(ERR_TUNNEL_CONNECTION_FAILED);
				CASE(ERR_NO_SSL_VERSIONS_ENABLED);
				CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
				CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
				CASE(ERR_CERT_COMMON_NAME_INVALID);
				CASE(ERR_CERT_DATE_INVALID);
				CASE(ERR_CERT_AUTHORITY_INVALID);
				CASE(ERR_CERT_CONTAINS_ERRORS);
				CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
				CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
				CASE(ERR_CERT_REVOKED);
				CASE(ERR_CERT_INVALID);
				CASE(ERR_CERT_END);
				CASE(ERR_INVALID_URL);
				CASE(ERR_DISALLOWED_URL_SCHEME);
				CASE(ERR_UNKNOWN_URL_SCHEME);
				CASE(ERR_TOO_MANY_REDIRECTS);
				CASE(ERR_UNSAFE_REDIRECT);
				CASE(ERR_UNSAFE_PORT);
				CASE(ERR_INVALID_RESPONSE);
				CASE(ERR_INVALID_CHUNKED_ENCODING);
				CASE(ERR_METHOD_NOT_SUPPORTED);
				CASE(ERR_UNEXPECTED_PROXY_AUTH);
				CASE(ERR_EMPTY_RESPONSE);
				CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
				CASE(ERR_CACHE_MISS);
				CASE(ERR_INSECURE_RESPONSE);
			default:
				return "UNKNOWN";
			}
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

		namespace Binding
		{
			const char kTestUrl[] = "http://tests/binding";
			const char kTestMessageName[] = "BindingTest";

			// Handle messages in the browser process.
			class BindingHandler : public CefMessageRouterBrowserSide::Handler {
			public:
				BindingHandler() {}

				// Called due to cefQuery execution in binding.html.
				virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
					CefRefPtr<CefFrame> frame,
					int64 query_id,
					const CefString& request,
					bool persistent,
					CefRefPtr<Callback> callback) OVERRIDE
				{
					// Only handle messages from the test URL.
					const std::string& url = frame->GetURL();
					if (url.find(kTestUrl) != 0)
						return false;

					const std::string& message_name = request;
					if (message_name.find(kTestMessageName) == 0) {
						// Reverse the string and return.
						std::string result = message_name.substr(sizeof(kTestMessageName));
						std::reverse(result.begin(), result.end());
						callback->Success(result);
						return true;
					}

					return false;
				}
			};

			void CreateMessageHandlers(MessageHandlerSet& handlers)
			{
				handlers.insert(new BindingHandler());
			}
		}

		void CreateMessageHandlers(MessageHandlerSet& handlers)
		{
			// Create the binding test handlers.
			Binding::CreateMessageHandlers(handlers);
		}
	}
}
