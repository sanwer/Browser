#include "stdafx.h"
#include "ClientAppRenderer.h"
#include "PerfTest.h"
#include <algorithm>
#include <string>
#include "include/wrapper/cef_stream_resource_handler.h"

namespace Browser
{
	namespace PerfTest
	{
		const char kGetPerfTests[] = "GetPerfTests";

		class V8Handler : public CefV8Handler
		{
		public:
			V8Handler() {}

			virtual bool Execute(const CefString& name,
				CefRefPtr<CefV8Value> object,
				const CefV8ValueList& arguments,
				CefRefPtr<CefV8Value>& retval,
				CefString& exception) OVERRIDE
			{
				if (name == kGetPerfTests) {
					retval = CefV8Value::CreateString("Performance Test");
				}
				return true;
			}
		private:
			IMPLEMENT_REFCOUNTING(V8Handler);
		};

		// Handle bindings in the render process.
		class RenderDelegate : public ClientAppRenderer::Delegate
		{
		public:
			RenderDelegate() {}

			virtual void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) OVERRIDE {
					CefRefPtr<CefV8Value> object = context->GetGlobal();

					CefRefPtr<CefV8Handler> handler = new V8Handler();

					// Bind functions.
					object->SetValue(kGetPerfTests,
						CefV8Value::CreateFunction(kGetPerfTests, handler),
						V8_PROPERTY_ATTRIBUTE_READONLY);
			}

		private:
			IMPLEMENT_REFCOUNTING(RenderDelegate);
		};

		void CreateDelegates(ClientAppRenderer::DelegateSet& delegates) {
			delegates.insert(new RenderDelegate);
		}
	}
}
