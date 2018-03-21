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
	extPacket.copyDataReverse(readerType, 0, READER_TYPE_SIZE);
	return 0;
}

uint8_t uFR::getReaderSerial(uint8_t readerSerialNumber[READER_SERIAL_SIZE]) {
	flushSerial();
	sendPacketCMD(GET_READER_SERIAL);
	PROCESS_RSP(GET_READER_SERIAL);
	PROCESS_EXT(READER_SERIAL_SIZE);
	extPacket.copyDataReverse(readerSerialNumber, 0, READER_SERIAL_SIZE);
	return 0;
}

uint8_t uFR::setReaderKey(uint8_t key[READER_KEY_SIZE], uint8_t index) {
	flushSerial();
	sendPacketCMD(READER_KEY_WRITE, READER_KEY_SIZE + 1, index);
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
	sendPacketCMD(USER_DATA_WRITE, USER_DATA_SIZE + 1);
	PROCESS_ACK(USER_DATA_WRITE);
	sendPacketEXT(data, USER_DATA_SIZE);
	PROCESS_RSP(USER_DATA_WRITE);
	return 0;
}

uint8_t uFR::softReset() {
	flushSerial();
	sendPacketCMD(SELF_RESET);
	PROCESS_RSP(SELF_RESET);
	return 0;
}

uint8_t uFR::getCardIDSimple(uint8_t cardID[CARD_ID_SIZE], uint8_t *cardType) {
	flushSerial();
	sendPacketCMD(GET_CARD_ID);
	PROCESS_RSP(GET_CARD_ID);
	PROCESS_EXT(CARD_ID_SIZE);
	extPacket.copyDataReverse(cardID, 0, CARD_ID_SIZE);
	if (cardType) *cardType = rspPacket[PAR0_BYTE];
	return 0;
}

uint8_t uFR::getCardID(uint8_t cardID[CARD_ID_EX_SIZE], uint8_t *length, uint8_t *cardType) {
	flushSerial();
	sendPacketCMD(GET_CARD_ID_EX);
	PROCESS_RSP(GET_CARD_ID_EX);
	PROCESS_EXT(CARD_ID_EX_SIZE);
	//extPacket.copyDataReverse(cardID, 0, rspPacket[PAR1_BYTE]);
	extPacket.copyData(cardID, 0, rspPacket[PAR1_BYTE]);
	if (cardType) *cardType = rspPacket[PAR0_BYTE];
	if (length) *length = rspPacket[PAR1_BYTE];
	return 0;
}

uint8_t uFR::getCardTypeDLogic(uint8_t *cardType) {
	flushSerial();
	sendPacketCMD(GET_DLOGIC_CARD_TYPE);
	PROCESS_RSP(GET_DLOGIC_CARD_TYPE);
	*cardType = rspPacket[PAR0_BYTE];
	return 0;
}

// Needs testing -----------------------------------------------------------------------

uint8_t uFR::readBlock(uint8_t data[BLOCK_SIZE], uint8_t address, uint8_t keyIndex, bool authModeB) {
	flushSerial();
	sendPacketCMD(BLOCK_READ, MIN_CMD_EXT_SIZE + 1, authModeB ? RKA_AUTH1B : RKA_AUTH1A, keyIndex);
	PROCESS_ACK(BLOCK_READ);
	uint8_t packet[MIN_CMD_EXT_SIZE] = {address, 0, 0, 0};
	sendPacketEXT(packet, MIN_CMD_EXT_SIZE);
	PROCESS_RSP(BLOCK_READ);
	PROCESS_EXT(BLOCK_SIZE);
	extPacket.copyData(data, 0, BLOCK_SIZE);
	return 0;
}

uint8_t uFR::readBlockAKM1(uint8_t data[BLOCK_SIZE], uint8_t address, bool authModeB) {
	flushSerial();
	sendPacketCMD(BLOCK_READ, MIN_CMD_EXT_SIZE + 1, authModeB ? AKM1_AUTH1B : AKM1_AUTH1A);
	PROCESS_ACK(BLOCK_READ);
	uint8_t packet[MIN_CMD_EXT_SIZE] = {address, 0, 0, 0};
	sendPacketEXT(packet, MIN_CMD_EXT_SIZE);
	PROCESS_RSP(BLOCK_READ);
	PROCESS_EXT(BLOCK_SIZE);
	extPacket.copyData(data, 0, BLOCK_SIZE);
	return 0;
}

uint8_t uFR::readBlockPK(uint8_t data[BLOCK_SIZE], uint8_t address, uint8_t key[READER_KEY_SIZE], bool authModeB) {
	flushSerial();
	sendPacketCMD(BLOCK_READ, 11, authModeB ? PK_AUTH1B : PK_AUTH1A);
	PROCESS_ACK(BLOCK_READ);
	uint8_t packet[10] = {address, 0, 0, 0, key[0], key[1], key[2], key[3], key[4], key[5]};
	sendPacketEXT(packet, 10);
	PROCESS_RSP(BLOCK_READ);
	PROCESS_EXT(BLOCK_SIZE);
	extPacket.copyData(data, 0, BLOCK_SIZE);
	return 0;
}

uint8_t uFR::writeBlock(uint8_t data[BLOCK_SIZE], uint8_t address, uint8_t keyIndex, bool authModeB) {
	flushSerial();
	sendPacketCMD(BLOCK_WRITE, 21, authModeB ? RKA_AUTH1B : RKA_AUTH1A, keyIndex);
	PROCESS_ACK(BLOCK_WRITE);
	uint8_t packet[20] = {address, 0, 0, 0};
	Packet::append(packet, data, BLOCK_SIZE, 4);
	sendPacketEXT(packet, 20);
	PROCESS_RSP(BLOCK_WRITE);
	return 0;
}

uint8_t uFR::readLinear(uint8_t data[], uint16_t address, uint16_t length, uint8_t keyIndex, bool authModeB) {
	flushSerial();
	sendPacketCMD(LINEAR_READ, 5, authModeB ? RKA_AUTH1B : RKA_AUTH1A, keyIndex);
	PROCESS_ACK(LINEAR_READ);
	// low byte, high byte (little-endian)
	uint8_t packet[4] = {
		static_cast<uint8_t>(address & 0xFF),
		static_cast<uint8_t>(address >> 8),
		static_cast<uint8_t>(length & 0xFF),
		static_cast<uint8_t>(length >> 8)
	};
	sendPacketEXT(packet, 4);
	PROCESS_RSP(LINEAR_READ);
	PROCESS_EXT(length);
	extPacket.copyData(data, 0, length);
	return 0;
}

uint8_t uFR::writeLinear(uint8_t data[], uint16_t address, uint16_t length, uint8_t keyIndex, bool authModeB) {
	flushSerial();
	sendPacketCMD(LINEAR_WRITE, length + 5, authModeB ? RKA_AUTH1B : RKA_AUTH1A, keyIndex);
	PROCESS_ACK(LINEAR_WRITE);
	uint8_t packet[length + 4] = {
		static_cast<uint8_t>(address & 0xFF),
		static_cast<uint8_t>(address >> 8),
		static_cast<uint8_t>(length & 0xFF),
		static_cast<uint8_t>(length >> 8)
	};
	Packet::append(packet, data, length, 4);
	sendPacketEXT(packet, length + 4);
	PROCESS_RSP(LINEAR_WRITE);
	return 0;
}

// ========================================================================================

// Needs beautifying
const char * TypeDLogicToString(uint8_t type) {
	switch (type) {
		case 0x00: return "TAG_UNKNOWN"; break;
		case 0x01: return "DL_MIFARE_ULTRALIGHT"; break;
		case 0x02: return "DL_MIFARE_ULTRALIGHT_EV1_11"; break;
		case 0x03: return "DL_MIFARE_ULTRALIGHT_EV1_21"; break;
		case 0x04: return "DL_MIFARE_ULTRALIGHT_C"; break;
		case 0x05: return "DL_NTAG_203"; break;
		case 0x06: return "DL_NTAG_210"; break;
		case 0x07: return "DL_NTAG_212"; break;
		case 0x08: return "DL_NTAG_213"; break;
		case 0x09: return "DL_NTAG_215"; break;
		case 0x0A: return "DL_NTAG_216"; break;
		case 0x0B: return "DL_MIKRON_MIK640D"; break;
		case 0x0C: return "NFC_T2T_GENERIC"; break;
		case 0x20: return "DL_MIFARE_MINI"; break;
		case 0x21: return "DL_MIFARE_CLASSIC_1K"; break;
		case 0x22: return "DL_MIFARE_CLASSIC_4K"; break;
		case 0x23: return "DL_MIFARE_PLUS_S_2K"; break;
		case 0x24: return "DL_MIFARE_PLUS_S_4K"; break;
		case 0x25: return "DL_MIFARE_PLUS_X_2K"; break;
		case 0x26: return "DL_MIFARE_PLUS_X_4K"; break;
		case 0x27: return "DL_MIFARE_DESFIRE"; break;
		case 0x28: return "DL_MIFARE_DESFIRE_EV1_2K"; break;
		case 0x29: return "DL_MIFARE_DESFIRE_EV1_4K"; break;
		case 0x2A: return "DL_MIFARE_DESFIRE_EV1_8K"; break;
		case 0x2B: return "DL_MIFARE_DESFIRE_EV2_2K"; break;
		case 0x2C: return "DL_MIFARE_DESFIRE_EV2_4K"; break;
		case 0x2D: return "DL_MIFARE_DESFIRE_EV2_8K"; break;
		case 0x40: return "DL_GENERIC_ISO14443_4"; break;
		case 0x41: return "DL_GENERIC_ISO14443_TYPE_B"; break;
		case 0x80: return "DL_IMEI_UID"; break;
		default: return "TYPE_ERROR";
	}
}