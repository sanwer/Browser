#ifndef __MESSAGE_LOOP_H__
#define __MESSAGE_LOOP_H__
#pragma once
#include <memory>
#include <functional>
#include "include/cef_task.h"
#include <windows.h>
#include "include/base/cef_bind.h"
#include "include/base/cef_scoped_refptr.h"

namespace Browser {
	struct DeleteOnMainThread {
		template<typename T>
		static void Destruct(const T* x) {
			if (CefCurrentlyOn(TID_UI)) {
				delete x;
			} else {
				scoped_refptr<const T> ref_ptr(const_cast<T*>(x));
				CefPostTask(TID_UI, base::BindOnce(&DeleteOnMainThread::DeleteOnUIThread<T>, ref_ptr));
			}
		}

		template<typename T>
		static void DeleteOnUIThread(scoped_refptr<const T> x) {
			delete x.get();
		}
	};
}

#endif