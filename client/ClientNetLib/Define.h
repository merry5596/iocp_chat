#pragma once
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <ws2tcpip.h>
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace ClientNetLib {

	const UINT16 BUFFER_SIZE = 256;

}