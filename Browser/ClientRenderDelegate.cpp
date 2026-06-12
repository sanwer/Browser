#include "stdafx.h"
#include "ClientRenderDelegate.h"
#include <sstream>
#include <string>
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"

namespace Browser
{
	namespace Renderer
	{
		class ClientRenderDelegate : public ClientAppRenderer::Delegate {
		public:
			ClientRenderDelegate() {
			}

			virtual void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app) override {
				CefMessageRouterConfig config;
				m_MessageRouter = CefMessageRouterRendererSide::Create(config);
			}

			virtual void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) override {
					m_MessageRouter->OnContextCreated(browser,  frame, context);
			}

			virtual void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) override {
					m_MessageRouter->OnContextReleased(browser,  frame, context);
			}

			virtual void OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefDOMNode> node) override {
			}

			virtual bool OnProcessMessageReceived(
				CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) override {
					return m_MessageRouter->OnProcessMessageReceived(
						browser, frame, source_process, message);
			}

		private:
			// Handles the renderer side of query routing.
			CefRefPtr<CefMessageRouterRendererSide> m_MessageRouter;

			IMPLEMENT_REFCOUNTING(ClientRenderDelegate);
		};

		void CreateDelegates(ClientAppRenderer::DelegateSet& delegates) {
			delegates.insert(new ClientRenderDelegate);
		}
	}
}