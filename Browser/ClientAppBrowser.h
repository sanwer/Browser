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
		class Delegate : public ClientApp::Delegate
		{
		public:
			virtual void OnBeforeCommandLineProcessing(
				CefRefPtr<ClientAppBrowser> app,
				CefRefPtr<CefCommandLine> command_line) {}

			virtual void OnContextInitialized(CefRefPtr<ClientAppBrowser> app) {}

			virtual void OnBeforeChildProcessLaunch(
				CefRefPtr<ClientAppBrowser> app,
				CefRefPtr<CefCommandLine> command_line) {}
		};
		typedef std::set<CefRefPtr<Delegate> > DelegateSet;

		ClientAppBrowser();

	private:
		// Creates all of the Delegate objects.
		static void CreateDelegates(DelegateSet& delegates);

		// CefApp methods.
		void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;
		void OnRegisterCustomSchemes(
#if CHROME_VERSION_BUILD >= 2924
			CefRawPtr<CefSchemeRegistrar> registrar
#else
			CefRefPtr<CefSchemeRegistrar> registrar
#endif
			) override;
		CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override{return this;}

		// CefBrowserProcessHandler methods.
		void OnContextInitialized() override;
		void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

	private:
		DelegateSet m_delegates;

		IMPLEMENT_REFCOUNTING(ClientAppBrowser);
		DISALLOW_COPY_AND_ASSIGN(ClientAppBrowser);
	};
}

#endif