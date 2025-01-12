#pragma once

#include <cstdint>
#include <sstream>


/**
 * Status codes for Qengine
 *
 * 0x0000 - 0x0FFF: Success codes
 * 0x1000 - 0x4FFF: Warnings (non-fatal errors)
 * 0x5000 - 0x9FFF: Non-fatal errors (which may be recoverable)
 * 0xA000 - 0xFFFF: Fatal errors (which typically stop execution)
 */

struct Qstatus {
	uint16_t code;
	const char *msg;

	Qstatus(const uint16_t code, const char* msg) : code(code), msg(msg) {}

	friend std::ostream& operator<<(std::ostream& os, const Qstatus& status) {
		os << status.msg;
		return os;
	}
};

// Errors
#define UNKNOWN_ERROR Qstatus(0xFFFF, "Unknown error")
