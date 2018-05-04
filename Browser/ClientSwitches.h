#ifndef __CLIENT_SWITCHES_H__
#define __CLIENT_SWITCHES_H__
#pragma once

namespace Browser {
	namespace Switches {
		extern const char kMultiThreadedMessageLoop[];
		extern const char kCachePath[];
		extern const char kUrl[];
		extern const char kOffScreenRenderingEnabled[];
		extern const char kOffScreenFrameRate[];
		extern const char kTransparentPaintingEnabled[];
		extern const char kShowUpdateRect[];
		extern const char kMouseCursorChangeDisabled[];
		extern const char kRequestContextPerBrowser[];
		extern const char kRequestContextSharedCache[];
		extern const char kBackgroundColor[];
		extern const char kEnableGPU[];
		extern const char kFilterURL[];
	}
}

#endif