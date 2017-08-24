#ifndef __BROWSERAPP_H__
#define __BROWSERAPP_H__
#pragma once
#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_app.h"

namespace Browser
{
	class CBrowserApp
		: public CefApp
		, public CefBrowserProcessHandler
		, public CefRenderProcessHandler
	{
	public:
		CBrowserApp(){}

	private:
		// CefApp methods.
		virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;
		virtual void OnRegisterCustomSchemes( CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;
		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE{return this;}
		virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE{return this;}

		// CefBrowserProcessHandler methods.
		virtual void OnContextInitialized() OVERRIDE;
		virtual void OnBeforeChildProcessLaunch(
			CefRefPtr<CefCommandLine> command_line) OVERRIDE;
		virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;

		// CefRenderProcessHandler methods.
		virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
		virtual void OnWebKitInitialized() OVERRIDE;
		virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
		virtual bool OnBeforeNavigation(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			NavigationType navigation_type,
			bool is_redirect) OVERRIDE;
		virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
		virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
		virtual void OnUncaughtException(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context,
			CefRefPtr<CefV8Exception> exception,
			CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
		virtual void OnFocusedNodeChanged(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefDOMNode> node) OVERRIDE;
		virtual bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;

	private:
		std::vector<CefString> cookieable_schemes_;

		IMPLEMENT_REFCOUNTING(CBrowserApp);
	};
}

#endif
