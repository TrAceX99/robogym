#include "uFR.h"

#define PROCESS_EXT(length) \
EXTPacket extPacket(length); \
if (extPacket.getErrorCode() != 0) return extPacket.getErrorCode()

#define PROCESS_ACK(command) \
CommonPacket ackPacket(PACKET_ACK, command); \
if (ackPacket.getErrorCode() != 0) return ackPacket.getErrorCode()

#define PROCESS_RSP(command) \
CommonPacket rspPacket(PACKET_RSP, command); \
if (rspPacket.getErrorCode() != 0) return rspPacket.getErrorCode()


uFR::uFR(uint8_t rx, uint8_t tx) : readerSerial(SoftwareSerial(rx, tx)) {
	setPacketSerial();
}

uFR::uFR(uint8_t rx, uint8_t tx, uint8_t reset) : readerSerial(SoftwareSerial(rx, tx)) {
	pinMode(reset, OUTPUT);
	digitalWrite(reset, HIGH);
	resetPin = reset;
	setPacketSerial();
}

void uFR::setPacketSerial() {
	Packet::serial = &readerSerial;
}

void uFR::begin(unsigned long baud) {
	if(resetPin != 0) {
		delay(10);
		digitalWrite(resetPin, LOW);
	}
	readerSerial.begin(baud);
}

void uFR::hardReset() {
	if (resetPin != 0) {
		digitalWrite(resetPin, HIGH);
		delay(10);
		digitalWrite(resetPin, LOW);
	}
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
		static_cast<uint8_t>(EXTlength + 1), // EXT checksum byte
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

// ========================================================================================

uint8_t uFR::setRedLED(bool state) {
	flushSerial();
	sendPacketCMD(RED_LIGHT_CONTROL, 0, state);
	PROCESS_RSP(RED_LIGHT_CONTROL);
	return 0;
}

uint8_t uFR::getReaderType(uint8_t readerType[READER_TYPE_SIZE]) {
	flushSerial();
	sendPacketCMD(GET_READER_TYPE);
	PROCESS_RSP(GET_READER_TYPE);
	PROCESS_EXT(READER_TYPE_SIZE);
	extPacket.copyData(readerType, 0, READER_TYPE_SIZE);
	return 0;
}

uint8_t uFR::getReaderSerial(uint8_t readerSerialNumber[READER_SERIAL_SIZE]) {
	flushSerial();
	sendPacketCMD(GET_READER_SERIAL);
	PROCESS_RSP(GET_READER_SERIAL);
	PROCESS_EXT(READER_SERIAL_SIZE);
	extPacket.copyData(readerSerialNumber, 0, READER_SERIAL_SIZE);
	return 0;
}

uint8_t uFR::setReaderKey(uint8_t key[READER_KEY_SIZE], uint8_t index) {
	flushSerial();
	sendPacketCMD(READER_KEY_WRITE, READER_KEY_SIZE, index);
	PROCESS_ACK(READER_KEY_WRITE);
	sendPacketEXT(key, READER_KEY_SIZE);
	PROCESS_RSP(READER_KEY_WRITE);
	return 0;
}

uint8_t uFR::getUserData(uint8_t data[USER_DATA_SIZE]) {
	flushSerial();
	sendPacketCMD(USER_DATA_READ);
	PROCESS_RSP(USER_DATA_READ);
	PROCESS_EXT(USER_DATA_SIZE);
	extPacket.copyData(data, 0, USER_DATA_SIZE);
	return 0;
}

uint8_t uFR::setUserData(uint8_t data[USER_DATA_SIZE]) {
	flushSerial();
	sendPacketCMD(USER_DATA_WRITE, USER_DATA_SIZE);
	PROCESS_ACK(USER_DATA_WRITE);
	sendPacketEXT(data, USER_DATA_SIZE);
	PROCESS_RSP(USER_DATA_WRITE);
	return 0;
}

uint8_t uFR::getCardID(uint8_t cardID[CARD_ID_SIZE], uint8_t *cardType) {
	flushSerial();
	sendPacketCMD(GET_CARD_ID);
	PROCESS_RSP(GET_CARD_ID);
	PROCESS_EXT(CARD_ID_SIZE);
	extPacket.copyData(cardID, 0, CARD_ID_SIZE);
	*cardType = rspPacket[PAR0_BYTE];
	return 0;
}