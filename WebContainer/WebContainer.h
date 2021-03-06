#pragma once

#include "sRPC/sRPC.h"

namespace WebContainer
{
	int MODULE_API GetInstance(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr);

	int MODULE_API ReleaseInstance(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr);

	int SRPC_API OnSRPCError(void* instance, void* usr);
}