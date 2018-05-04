#ifndef __CLIENT_RENDER_DELEGATE_H__
#define __CLIENT_RENDER_DELEGATE_H__
#pragma once
#include "include/cef_base.h"
#include "ClientAppRenderer.h"

namespace Browser
{
	namespace Renderer
	{
		void CreateDelegates(ClientAppRenderer::DelegateSet& delegates);
	}
}

#endif
