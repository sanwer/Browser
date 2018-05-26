#ifndef __MESSAGE_LOOP_H__
#define __MESSAGE_LOOP_H__
#pragma once
#include "include/base/cef_bind.h"
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_task.h"
#include <windows.h>

namespace Browser {
	struct DeleteOnMainThread {
		template<typename T>
		static void Destruct(const T* x) {
			if (CefCurrentlyOn(TID_UI)) {
				delete x;
			} else {
				CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&DeleteOnMainThread::Destruct<T>, x)));
			}
		}
	};
}

#endif
