/// Digital Logic uFR NFC card reader library for Arduino
///
/// Based on IS21 DLogic serial communication protocol
///
/// Version: 1.0.0
/// Copyright 2018 Marko Djordjevic


#include <Arduino.h>
#include <SoftwareSerial.h>

#define TIMEOUT_MS 10

// Communication constants
#define MAX_PACKET_LENGTH	64
#define HEADER_BYTE			0
#define CMD_BYTE			1
#define TRAILER_BYTE		2
#define EXT_LENGTH_BYTE		3
#define PAR0_BYTE			4
#define PAR1_BYTE			5
#define CHKSUM_BYTE			6
#define PACKET_LENGTH		7
#define CMD_HEADER			0x55
#define ACK_HEADER			0xAC
#define RESPONSE_HEADER		0xDE
#define ERR_HEADER			0xEC
#define CMD_TRAILER			0xAA
#define ACK_TRAILER			0xCA
#define RESPONSE_TRAILER	0xED
#define ERR_TRAILER			0xCE

// CMD codes
#define GET_READER_TYPE				0x10
#define GET_READER_SERIAL			0x11
#define READER_KEY_WRITE			0x12
#define GET_CARD_ID					0x13
#define LINEAR_READ					0x14
#define LINEAR_WRITE				0x15
#define BLOCK_READ					0x16
#define BLOCK_WRITE					0x17
#define BLOCK_IN_SECTOR_READ		0x18
#define BLOCK_IN_SECTOR_WRITE		0x19
#define SECTOR_TRAILER_WRITE		0X1A
#define USER_DATA_READ				0x1B
#define USER_DATA_WRITE				0x1C
#define VALUE_BLOCK_READ			0x1D
#define VALUE_BLOCK_WRITE			0x1E
#define VALUE_BLOCK_IN_SECTOR_READ	0x1F
#define VALUE_BLOCK_IN_SECTOR_WRITE	0x20
#define VALUE_BLOCK_INC				0x21
#define VALUE_BLOCK_DEC				0x22
#define VALUE_BLOCK_IN_SECTOR_INC	0x23
#define VALUE_BLOCK_IN_SECTOR_DEC	0x24
#define LINEAR_FORMAT_CARD			0x25
#define GET_CARD_ID_EX				0x2C
#define SECTOR_TRAILER_WRITE_UNSAFE	0x2F
#define SELF_RESET					0x30
#define GET_DLOGIC_CARD_TYPE		0x3C
#define SET_CARD_ID_SEND_CONF		0x3D
#define GET_CARD_ID_SEND_CONF		0x3E
#define SET_UART_SPEED				0x70
#define RED_LIGHT_CONTROL			0x71

// ERR codes
#define OK										0x00
#define COMMUNICATION_ERROR						0x01
#define COMMUNICATION_TIMEOUT                   0x50
#define COMMUNICATION_TIMEOUT_EXT				0x51
#define CHKSUM_ERROR							0x02
#define CHKSUM_ERROR_RESPONSE					0x52
#define CHKSUM_ERROR_EXT						0x53
#define READING_ERROR							0x03
#define WRITING_ERROR							0x04
#define BUFFER_OVERFLOW							0x05
#define MAX_ADDRESS_EXCEEDED					0x06
#define MAX_KEY_INDEX_EXCEEDED					0x07
#define NO_CARD									0x08
#define COMMAND_NOT_SUPPORTED					0x09
#define FORBIDEN_DIRECT_WRITE_IN_SECTOR_TRAILER	0x0A
#define ADDRESSED_BLOCK_IS_NOT_SECTOR_TRAILER	0x0B
#define WRONG_ADDRESS_MODE						0x0C
#define WRONG_ACCESS_BITS_VALUES				0x0D
#define AUTH_ERROR								0x0E
#define PARAMETERS_ERROR						0x0F
#define WRITE_VERIFICATION_ERROR				0x70
#define BUFFER_SIZE_EXCEEDED					0x71
#define VALUE_BLOCK_INVALID						0x72
#define VALUE_BLOCK_ADDR_INVALID				0x73
#define VALUE_BLOCK_MANIPULATION_ERROR			0x74

// MIFARE CLASSIC type id's:
#define MIFARE_CLASSIC_1k               0x08
#define MF1ICS50                        0x08
#define SLE66R35                        0x88 // Infineon = Mifare Classic 1k
#define MIFARE_CLASSIC_4k               0x18
#define MF1ICS70                        0x18
#define MIFARE_CLASSIC_MINI             0x09
#define MF1ICS20                        0x09

//DLOGIC CARD TYPE
#define TAG_UNKNOWN						0
#define DL_MIFARE_ULTRALIGHT			0x01
#define DL_MIFARE_ULTRALIGHT_EV1_11		0x02
#define DL_MIFARE_ULTRALIGHT_EV1_21		0x03
#define DL_MIFARE_ULTRALIGHT_C			0x04
#define DL_NTAG_203						0x05
#define DL_NTAG_210						0x06
#define DL_NTAG_212						0x07
#define DL_NTAG_213						0x08
#define DL_NTAG_215						0x09
#define DL_NTAG_216						0x0A
#define DL_MIKRON_MIK640D				0x0B
#define NFC_T2T_GENERIC					0x0C

#define DL_MIFARE_MINI					0x20
#define	DL_MIFARE_CLASSIC_1K			0x21
#define DL_MIFARE_CLASSIC_4K			0x22
#define DL_MIFARE_PLUS_S_2K				0x23
#define DL_MIFARE_PLUS_S_4K				0x24
#define DL_MIFARE_PLUS_X_2K				0x25
#define DL_MIFARE_PLUS_X_4K				0x26
#define DL_MIFARE_DESFIRE				0x27
#define DL_MIFARE_DESFIRE_EV1_2K		0x28
#define DL_MIFARE_DESFIRE_EV1_4K		0x29
#define DL_MIFARE_DESFIRE_EV1_8K		0x2A
#define DL_MIFARE_DESFIRE_EV2_2K		0x2B
#define DL_MIFARE_DESFIRE_EV2_4K		0x2C
#define DL_MIFARE_DESFIRE_EV2_8K		0x2D

#define DL_UNKNOWN_ISO_14443_4			0x40
#define DL_GENERIC_ISO14443_4			0x40
#define DL_GENERIC_ISO14443_TYPE_B		0x41
#define DL_IMEI_UID						0x80

enum PacketType {
	PACKET_ACK = ACK_HEADER,
	PACKET_ERR = ERR_HEADER,
	PACKET_RSP = RESPONSE_HEADER,
	PACKET_TIMEOUT = COMMUNICATION_TIMEOUT
};

class uFR {
	public:
		uFR(uint8_t rx, uint8_t tx);

		void begin(unsigned long baud = 115200);
		inline void end() { readerSerial.end(); }

		uint8_t setRedLED(bool state);
		uint8_t getReaderType(uint8_t *readerType); // 4-byte array
		uint8_t getCardID(uint8_t *cardID, uint8_t *cardType); // 4-byte array
	private:
		SoftwareSerial readerSerial;
		void flushSerial(); // Flush serial input buffer

		void sendPacketCMD(uint8_t command, uint8_t EXTlength = 0, uint8_t par0 = 0, uint8_t par1 = 0);
		void sendPacketEXT(uint8_t *packet, uint8_t length);

		PacketType readPacket(uint8_t *response);
		uint8_t readPacketEXT(uint8_t *response, uint8_t length);

		uint8_t validatePacket(uint8_t *packet, PacketType type, uint8_t command);
		uint8_t checksum(uint8_t *packet, uint8_t size = PACKET_LENGTH - 1);
};