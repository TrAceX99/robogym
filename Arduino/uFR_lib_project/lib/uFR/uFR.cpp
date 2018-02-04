#include "uFR.h"

uFR::uFR(uint8_t rx, uint8_t tx) : readerSerial(SoftwareSerial(rx, tx)) {}

void uFR::begin(unsigned long baud) {
	readerSerial.begin(baud);
}

void uFR::flushSerial() {
	while (readerSerial.available() > 0)
		readerSerial.read();
}

uint8_t uFR::checksum(uint8_t *packet, uint8_t size) {
	uint8_t result = packet[0];
	for (uint8_t i = 1; i < size; i++)
		result ^= packet[i];
	return result + 0x07;
}

void uFR::sendCMDPacket(uint8_t command, uint8_t EXTlength, uint8_t par0, uint8_t par1) {
	uint8_t packet[PACKET_LENGTH];
	packet[0] = CMD_HEADER;
	packet[1] = command;
	packet[2] = CMD_TRAILER;
	packet[3] = EXTlength;
	packet[4] = par0;
	packet[5] = par1;
	packet[6] = checksum(packet);

	readerSerial.write(packet, PACKET_LENGTH);
}

PacketType uFR::readPacket(uint8_t *response) {
	unsigned long time = millis();
	uint8_t incoming = 0;
	while(incoming != ACK_HEADER && incoming != ERR_HEADER && incoming != RESPONSE_HEADER) {
		if((unsigned long)(millis() - time) > TIMEOUT_MS) return PACKET_TIMEOUT;
		if (readerSerial.available() > 0) incoming = readerSerial.read();
	}
	while (readerSerial.available() < 6)
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) {
			// Clear buffer on timeout
			flushSerial();
			return PACKET_TIMEOUT;
		}

	response[0] = incoming;
	for (uint8_t i = 1; i < PACKET_LENGTH; i++)
		response[i] = readerSerial.read();

	return static_cast<PacketType>(incoming);
}

uint8_t uFR::validatePacket(uint8_t *packet, PacketType type) {
	if (checksum(packet) != packet[6]) return CHKSUM_ERROR;
	switch (type) {
		case PACKET_ERR:
			if (packet[2] == ERR_TRAILER) return packet[1];
			else return COMMUNICATION_ERROR;
		case PACKET_ACK:
			if (packet[0] != type) return COMMUNICATION_ERROR;
			if (packet[2] != ACK_TRAILER) return COMMUNICATION_ERROR;
			break;
		case PACKET_RSP:
			if (packet[0] != type) return COMMUNICATION_ERROR;
			if (packet[2] != RESPONSE_TRAILER) return COMMUNICATION_ERROR;
			break;
		default:
			return COMMUNICATION_ERROR;
	}
	return 0;
}

uint8_t uFR::setRedLED(bool state) {
	flushSerial();
	sendCMDPacket(RED_LIGHT_CONTROL, 0, state);
	uint8_t packet[PACKET_LENGTH];
	if(readPacket(packet) == PACKET_TIMEOUT) return COMMUNICATION_TIMEOUT;
	return validatePacket(packet, PACKET_RSP);
}