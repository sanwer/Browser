#ifndef __MESSAGE_LOOP_H__
#define __MESSAGE_LOOP_H__
#pragma once
#include "include/base/cef_bind.h"
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_task.h"
#include <windows.h>

namespace Browser {
	class MessageLoop {
	public:
		MessageLoop();

		static MessageLoop* Get();
		virtual int Run();
		virtual void Quit();
		virtual void PostTask(CefRefPtr<CefTask> task);
		virtual bool RunsTasksOnCurrentThread() const;
		virtual void SetCurrentModelessDialog(HWND hWndDialog);
		void PostClosure(const base::Closure& closure);

	protected:
		friend struct base::DefaultDeleter<MessageLoop>;

		virtual ~MessageLoop();

	private:
		DISALLOW_COPY_AND_ASSIGN(MessageLoop);
	};

#define CURRENTLY_ON_MAIN_THREAD() \
	Browser::MessageLoop::Get()->RunsTasksOnCurrentThread()

#define REQUIRE_MAIN_THREAD() DCHECK(CURRENTLY_ON_MAIN_THREAD())

#define MAIN_POST_TASK(task) \
	Browser::MessageLoop::Get()->PostTask(task)

#define MAIN_POST_CLOSURE(closure) \
	Browser::MessageLoop::Get()->PostClosure(closure)


	struct DeleteOnMainThread {
		template<typename T>
		static void Destruct(const T* x) {
			if (CURRENTLY_ON_MAIN_THREAD()) {
				delete x;
			} else {
				Browser::MessageLoop::Get()->PostClosure(
					base::Bind(&DeleteOnMainThread::Destruct<T>, x));
			}
		}
	};
}

#endif
