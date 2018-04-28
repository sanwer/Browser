#ifndef __BROWSERAPP_H__
#define __BROWSERAPP_H__
#pragma once
#include <set>
#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_app.h"

namespace Browser
{
	class ClientApp : public CefApp
	{
	public:
		ClientApp(){}

	protected:
		std::vector<CefString> cookie_schemes;
		DISALLOW_COPY_AND_ASSIGN(ClientApp);
	};

	class ClientAppBrowser : public ClientApp , public CefBrowserProcessHandler
	{
	public:
		class Delegate : public virtual CefBase {
		public:
			virtual void OnBeforeCommandLineProcessing(
				CefRefPtr<ClientAppBrowser> app,
				CefRefPtr<CefCommandLine> command_line) {}

			virtual void OnContextInitialized(CefRefPtr<ClientAppBrowser> app) {}

			virtual void OnBeforeChildProcessLaunch(
				CefRefPtr<ClientAppBrowser> app,
				CefRefPtr<CefCommandLine> command_line) {}

			virtual void OnRenderProcessThreadCreated(
				CefRefPtr<ClientAppBrowser> app,
				CefRefPtr<CefListValue> extra_info) {}
		};
		typedef std::set<CefRefPtr<Delegate> > DelegateSet;

		ClientAppBrowser();

	private:
		// Creates all of the Delegate objects.
		static void CreateDelegates(DelegateSet& delegates);
		// CefApp methods.
		void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;
		void OnRegisterCustomSchemes( CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;
		CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE{return this;}

		// CefBrowserProcessHandler methods.
		void OnContextInitialized() OVERRIDE;
		void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;
		void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;

	private:
		DelegateSet m_delegates;

		IMPLEMENT_REFCOUNTING(ClientAppBrowser);
		DISALLOW_COPY_AND_ASSIGN(ClientAppBrowser);
	};

	class ClientAppRenderer : public ClientApp , public CefRenderProcessHandler
	{
	public:
		class Delegate : public virtual CefBase {
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

			virtual bool OnBeforeNavigation(CefRefPtr<ClientAppRenderer> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				cef_navigation_type_t navigation_type,
				bool is_redirect) {
					return false;
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
		bool OnBeforeNavigation(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			NavigationType navigation_type,
			bool is_redirect) OVERRIDE;
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
