#include "uFR.h"

SoftwareSerial* uFR::Packet::serial;

uFR::CommonPacket::CommonPacket(PacketType type, uint8_t command) {
	data = new uint8_t[PACKET_LENGTH];
	errorCode = read(data);
	if (errorCode == 0) errorCode = validate(data, type, command);
}
uFR::CommonPacket::~CommonPacket() {
	delete[] data;
}

uFR::EXTPacket::EXTPacket(uint8_t length) {
	data = new uint8_t[length];
	errorCode = read(data, length);
}
uFR::EXTPacket::~EXTPacket() {
	delete[] data;
}

uint8_t uFR::Packet::checksum(uint8_t *packet, uint8_t size) {
	uint8_t result = packet[0];
	// XOR size bytes
	for (uint8_t i = 1; i < size; i++)
		result ^= packet[i];
	return result + 0x07;
}

uint8_t uFR::CommonPacket::validate(uint8_t packet[PACKET_LENGTH], PacketType type, uint8_t command) {
	if (checksum(packet) != packet[CHKSUM_BYTE]) return CHKSUM_ERROR_RESPONSE;
	if (packet[HEADER_BYTE] == ERR_HEADER) {
		if (packet[TRAILER_BYTE] == ERR_TRAILER) return packet[CMD_BYTE];
		return COMMUNICATION_ERROR;
	}
	if (packet[HEADER_BYTE] != type || packet[CMD_BYTE] != command) return COMMUNICATION_ERROR;
	switch (type) {
		case PACKET_ACK:
			if (packet[TRAILER_BYTE] != ACK_TRAILER) return COMMUNICATION_ERROR;
			break;
		case PACKET_RSP:
			if (packet[TRAILER_BYTE] != RESPONSE_TRAILER) return COMMUNICATION_ERROR;
			break;
		default:
			return COMMUNICATION_ERROR;
	}
	return 0;
}

uint8_t uFR::CommonPacket::read(uint8_t response[PACKET_LENGTH]) {
	unsigned long time = millis();
	uint8_t incoming = 0;
	// Read bytes until header found
	while(incoming != ACK_HEADER && incoming != ERR_HEADER && incoming != RESPONSE_HEADER) {
		if((unsigned long)(millis() - time) > TIMEOUT_MS) return COMMUNICATION_TIMEOUT;
		if (serial->available() > 0) incoming = serial->read();
	}
	// Read remaining bytes (PACKET_LENGTH - 1)
	while (serial->available() < 6)
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) return COMMUNICATION_TIMEOUT;

	// Store bytes
	response[0] = incoming;
	for (uint8_t i = 1; i < PACKET_LENGTH; i++)
		response[i] = serial->read();

	return 0;
}

uint8_t uFR::EXTPacket::read(uint8_t *response, uint8_t length) {
	unsigned long time = millis();
	uint8_t i = 0;
	int b;
	// Read length bytes
	while (i < length) {
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) return COMMUNICATION_TIMEOUT_EXT;
		b = serial->read();
		if(b != -1) {
			response[i] = b;
			i++;
		}
	}
	// Read and check checksum byte (length + 1)
	while (serial->available() < 1)
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) return COMMUNICATION_TIMEOUT_EXT;
	if (serial->read() != checksum(response, length)) return CHKSUM_ERROR_EXT;

	return 0;
}

void uFR::Packet::copyData(uint8_t *array, uint16_t start, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		array[i + start] = data[i];
}

void uFR::Packet::copyDataReverse(uint8_t *array, uint16_t start, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		array[i + start] = data[length - i - 1];
}