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
	// XOR size bytes
	for (uint8_t i = 1; i < size; i++)
		result ^= packet[i];
	return result + 0x07;
}

void uFR::sendPacketCMD(uint8_t command, uint8_t EXTlength, uint8_t par0, uint8_t par1) {
	uint8_t packet[PACKET_LENGTH] = {
		CMD_HEADER,
		command,
		CMD_TRAILER,
		EXTlength,
		par0,
		par1,
		checksum(packet)
	};
	readerSerial.write(packet, PACKET_LENGTH);
}

void uFR::sendPacketEXT(uint8_t *packet, uint8_t length) {
	readerSerial.write(packet, length);
	readerSerial.write(checksum(packet, length));
}

PacketType uFR::readPacket(uint8_t *response) {
	unsigned long time = millis();
	uint8_t incoming = 0;
	// Read bytes until header found
	while(incoming != ACK_HEADER && incoming != ERR_HEADER && incoming != RESPONSE_HEADER) {
		if((unsigned long)(millis() - time) > TIMEOUT_MS) return PACKET_TIMEOUT;
		if (readerSerial.available() > 0) incoming = readerSerial.read();
	}
	// Read remaining bytes (PACKET_LENGTH - 1)
	while (readerSerial.available() < 6)
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) {
			// Clear buffer on timeout
			flushSerial();
			return PACKET_TIMEOUT;
		}

	// Store bytes
	response[0] = incoming;
	for (uint8_t i = 1; i < PACKET_LENGTH; i++)
		response[i] = readerSerial.read();

	return static_cast<PacketType>(incoming);
}

uint8_t uFR::readPacketEXT(uint8_t *response, uint8_t length) {
	unsigned long time = millis();
	uint8_t i = 0;
	int b;
	// Read length bytes
	while (i < length) {
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) return COMMUNICATION_TIMEOUT_EXT;
		b = readerSerial.read();
		if(b != -1) {
			response[i] = b;
			i++;
		}
	}
	// Read and check checksum byte (length + 1)
	while (readerSerial.available() < 1)
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) return COMMUNICATION_TIMEOUT_EXT;
	if (readerSerial.read() != checksum(response, length)) return CHKSUM_ERROR_EXT;

	return 0;
}

uint8_t uFR::validatePacket(uint8_t *packet, PacketType type, uint8_t command) {
	if (checksum(packet) != packet[CHKSUM_BYTE]) return CHKSUM_ERROR_RESPONSE;
	if (packet[HEADER_BYTE] == ERR_HEADER) {
		if (packet[TRAILER_BYTE] == ERR_TRAILER) return packet[CMD_BYTE];
		return COMMUNICATION_ERROR;
	}
	if (packet[HEADER_BYTE] != type) return COMMUNICATION_ERROR;
	if (packet[CMD_BYTE] != command) return COMMUNICATION_ERROR;
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

uint8_t uFR::setRedLED(bool state) {
	flushSerial();
	sendPacketCMD(RED_LIGHT_CONTROL, 0, state);

	uint8_t packet[PACKET_LENGTH];
	if (readPacket(packet) == PACKET_TIMEOUT) return COMMUNICATION_TIMEOUT;
	return validatePacket(packet, PACKET_RSP, RED_LIGHT_CONTROL);
}

uint8_t uFR::getReaderType(uint8_t *readerType) {
	flushSerial();
	sendPacketCMD(GET_READER_TYPE);

	uint8_t packet[PACKET_LENGTH];
	if (readPacket(packet) == PACKET_TIMEOUT) return COMMUNICATION_TIMEOUT;

	uint8_t errorCode = validatePacket(packet, PACKET_RSP, GET_READER_TYPE);
	if (errorCode != 0) return errorCode;

	uint8_t lengthEXT = packet[EXT_LENGTH_BYTE] - 1;
	uint8_t packetEXT[lengthEXT];
	errorCode = readPacketEXT(packetEXT, lengthEXT);
	if (errorCode != 0) return errorCode;

	readerType = packetEXT;	
	return 0;
}

uint8_t uFR::getCardID(uint8_t *cardID, uint8_t *cardType) {
	flushSerial();
	sendPacketCMD(GET_CARD_ID);

	uint8_t packet[PACKET_LENGTH];
	if (readPacket(packet) == PACKET_TIMEOUT) return COMMUNICATION_TIMEOUT;

	uint8_t errorCode = validatePacket(packet, PACKET_RSP, GET_CARD_ID);
	if (errorCode != 0) return errorCode;
	
	uint8_t lengthEXT = packet[EXT_LENGTH_BYTE] - 1;
	uint8_t packetEXT[lengthEXT];
	errorCode = readPacketEXT(packetEXT, lengthEXT);
	if (errorCode != 0) return errorCode;

	cardID = packetEXT;
	*cardType = packet[PAR0_BYTE];
	return 0;
}