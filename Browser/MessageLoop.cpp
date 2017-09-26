#include "MessageLoop.h"
#include "include/cef_app.h"
#include "include/cef_task.h"
#include "include/wrapper/cef_closure_task.h"

namespace Browser {
	namespace {
		MessageLoop* g_main_message_loop = NULL;
	}

	MessageLoop::MessageLoop() {
		DCHECK(!g_main_message_loop);
		g_main_message_loop = this;
	}

	MessageLoop::~MessageLoop() {
		g_main_message_loop = NULL;
	}

	// static
	MessageLoop* MessageLoop::Get() {
		DCHECK(g_main_message_loop);
		return g_main_message_loop;
	}

	void MessageLoop::PostClosure(const base::Closure& closure) {
		PostTask(CefCreateClosureTask(closure));
	}

	int MessageLoop::Run() {
		CefRunMessageLoop();
		return 0;
	}

	void MessageLoop::Quit() {
		CefQuitMessageLoop();
	}

	void MessageLoop::PostTask(CefRefPtr<CefTask> task) {
		CefPostTask(TID_UI, task);
	}

	bool MessageLoop::RunsTasksOnCurrentThread() const {
		return CefCurrentlyOn(TID_UI);
	}

	void MessageLoop::SetCurrentModelessDialog(HWND hWndDialog) {
	}

}
