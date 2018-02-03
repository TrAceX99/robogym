#include "uFR.h"

uFR::uFR(uint8_t rx, uint8_t tx) : readerSerial(SoftwareSerial(rx, tx)) {}

void uFR::begin(unsigned long baud) {
	readerSerial.begin(baud);
}

uint8_t uFR::checksum(uint8_t *packet, uint8_t size) {
	uint8_t l = sizeof(packet);
	uint8_t result = packet[0];
	if(l > 1)
		// XOR all bytes
		for (uint8_t i = 1; i < l; i++)
			result ^= packet[i];
	return result + 0x07;
}

void uFR::sendCMDPacket(uint8_t command, uint8_t EXTlength, uint8_t par0, uint8_t par1) {
	uint8_t packet[7];
	packet[0] = CMD_HEADER;
	packet[1] = command;
	packet[2] = CMD_TRAILER;
	packet[3] = EXTlength;
	packet[4] = par0;
	packet[5] = par1;
	packet[6] = checksum(packet);
	
	readerSerial.write(packet, 7);
}

PACKET_TYPE uFR::readPacket(uint8_t *response) {
	unsigned long time = millis();
	uint8_t incoming = 0;

	while(incoming != ACK_HEADER || incoming != RESPONSE_HEADER || incoming != ERR_HEADER) {
		if((unsigned long)(millis() - time) > TIMEOUT_MS) return PACKET_TIMEOUT;
		if (readerSerial.available() > 0) incoming = readerSerial.read();
	}

	while (readerSerial.available() >= 6)
		if ((unsigned long)(millis() - time) > TIMEOUT_MS) return PACKET_TIMEOUT;
	
	response[0] = incoming;
	for (uint8_t i = 1; i < PACKET_LENGTH; i++)
		response[i] = readerSerial.read();

	if (incoming == ACK_HEADER) return PACKET_ACK;
	if (incoming == RESPONSE_HEADER) return PACKET_RSP;
	if (incoming == ERR_HEADER) return PACKET_ERR;
}

uint8_t uFR::setRedLED(bool state) {
	sendCMDPacket(RED_LIGHT_CONTROL, 0, 1);
	// stuff
	return 0;
}