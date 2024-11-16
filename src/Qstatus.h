#ifndef STATUS_H
#define STATUS_H

/**
 * Status codes for Qengine
 *
 * 0x00 - 0x1F: Success codes
 * 0x20 - 0x5F: Warnings (non-fatal errors)
 * 0x60 - 0x9F: Non-fatal errors (which may be recoverable)
 * 0xA0 - 0xFF: Fatal errors (which typically stop execution)
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

const Qstatus ERROR(0x42, "reverted");
const Qstatus ANOTHER(0x69, "This is an error");
const Qstatus YET_ANOTHER(0x88, "This is an error");

#endif // STATUS_H
