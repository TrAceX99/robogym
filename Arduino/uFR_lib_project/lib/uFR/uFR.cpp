#include "uFR.h"

uFR::uFR(uint8_t rx, uint8_t tx) : readerSerial(SoftwareSerial(rx, tx)) {}

uFR::uFR(uint8_t rx, uint8_t tx, uint8_t reset) : readerSerial(SoftwareSerial(rx, tx)) {
	pinMode(reset, OUTPUT);
	digitalWrite(reset, HIGH);
	resetPin = reset;
}

void uFR::begin(unsigned long baud) {
	if(resetPin != 0) {
		delay(10);
		digitalWrite(resetPin, LOW);
	}
	readerSerial.begin(baud);
}

void uFR::flushSerial() {
	while (readerSerial.available() > 0)
		readerSerial.read();
}

void uFR::sendPacketCMD(uint8_t command, uint8_t EXTlength, uint8_t par0, uint8_t par1) {
	uint8_t packet[PACKET_LENGTH] = {
		CMD_HEADER,
		command,
		CMD_TRAILER,
		EXTlength,
		par0,
		par1,
		Packet::checksum(packet)
	};
	readerSerial.write(packet, PACKET_LENGTH);
}

void uFR::sendPacketEXT(uint8_t *packet, uint8_t length) {
	readerSerial.write(packet, length);
	readerSerial.write(Packet::checksum(packet, length));
}

uint8_t uFR::setRedLED(bool state) {
	flushSerial();
	sendPacketCMD(RED_LIGHT_CONTROL, 0, state);

	CommonPacket packet(&readerSerial, PACKET_RSP, RED_LIGHT_CONTROL);
	return packet.getErrorCode();
}

uint8_t uFR::getReaderType(uint8_t *readerType) {
	flushSerial();
	sendPacketCMD(GET_READER_TYPE);

	CommonPacket packet(&readerSerial, PACKET_RSP, GET_READER_TYPE);
	if (packet.getErrorCode() != 0) return packet.getErrorCode();

	EXTPacket extpacket(&readerSerial, READER_TYPE_SIZE);
	if (extpacket.getErrorCode() != 0) return extpacket.getErrorCode();
	extpacket.copyData(readerType, 0, READER_TYPE_SIZE);

	return 0;
}

uint8_t uFR::getCardID(uint8_t *cardID, uint8_t *cardType) {
	flushSerial();
	sendPacketCMD(GET_CARD_ID);

	CommonPacket packet(&readerSerial, PACKET_RSP, GET_CARD_ID);
	if (packet.getErrorCode() != 0) return packet.getErrorCode();

	EXTPacket extpacket(&readerSerial, CARD_ID_SIZE);
	if (extpacket.getErrorCode() != 0) return extpacket.getErrorCode();
	extpacket.copyData(cardID, 0, CARD_ID_SIZE);

	*cardType = packet[PAR0_BYTE];
	return 0;
}