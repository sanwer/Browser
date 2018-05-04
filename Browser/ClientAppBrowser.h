#ifndef __CLIENT_APP_BROWSER_H__
#define __CLIENT_APP_BROWSER_H__
#pragma once
#include <set>
#include "ClientApp.h"

namespace Browser
{
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
}

#endif
