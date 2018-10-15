#ifndef __CLIENT_APP_RENDERER_H__
#define __CLIENT_APP_RENDERER_H__
#pragma once
#include <set>
#include "ClientApp.h"

namespace Browser
{
	class ClientAppRenderer : public ClientApp , public CefRenderProcessHandler
	{
	public:
		class Delegate : public ClientApp::Delegate
		{
		public:
			virtual void OnRenderThreadCreated(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefListValue> extra_info) {}

			virtual void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app) {}

			virtual void OnBrowserCreated(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser) {}

			virtual void OnBrowserDestroyed(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser) {}

			virtual CefRefPtr<CefLoadHandler> GetLoadHandler(
				CefRefPtr<ClientAppRenderer> app) {
					return NULL;
			}

			virtual void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) {}

			virtual void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) {}

			virtual void OnUncaughtException(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context,
				CefRefPtr<CefV8Exception> exception,
				CefRefPtr<CefV8StackTrace> stackTrace) {}

			virtual void OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefDOMNode> node) {}

			virtual bool OnProcessMessageReceived(
				CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) {
					return false;
			}
		};
		typedef std::set<CefRefPtr<Delegate> > DelegateSet;

		ClientAppRenderer();

	private:
		// Creates all of the Delegate objects.
		static void CreateDelegates(DelegateSet& delegates);

		// CefApp methods.
		CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE{return this;}

		// CefRenderProcessHandler methods.
		void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
		void OnWebKitInitialized() OVERRIDE;
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
		void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
		void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
		void OnUncaughtException(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context,
			CefRefPtr<CefV8Exception> exception,
			CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
		void OnFocusedNodeChanged(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefDOMNode> node) OVERRIDE;
		bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;

	private:
		DelegateSet m_delegates;

		IMPLEMENT_REFCOUNTING(ClientAppRenderer);
		DISALLOW_COPY_AND_ASSIGN(ClientAppRenderer);
	};

	namespace Renderer
	{
		void CreateDelegates(ClientAppRenderer::DelegateSet& delegates);
	}
}

#endif
