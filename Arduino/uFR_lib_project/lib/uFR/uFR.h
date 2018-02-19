/// Digital Logic uFR NFC card reader library for Arduino
///
/// Based on IS21 DLogic serial communication protocol
///
/// Version: 1.0.0
/// 2018 Marko Djordjevic


#include <Arduino.h>
#include <SoftwareSerial.h>

#define TIMEOUT_MS 100 // Debugging

// Communication constants
#define MAX_PACKET_LENGTH 64
#define HEADER_BYTE       0
#define CMD_BYTE          1
#define TRAILER_BYTE      2
#define EXT_LENGTH_BYTE   3
#define PAR0_BYTE         4
#define PAR1_BYTE         5
#define CHKSUM_BYTE       6
#define PACKET_LENGTH     7
#define CMD_HEADER        0x55
#define ACK_HEADER        0xAC
#define RESPONSE_HEADER   0xDE
#define ERR_HEADER        0xEC
#define CMD_TRAILER       0xAA
#define ACK_TRAILER       0xCA
#define RESPONSE_TRAILER  0xED
#define ERR_TRAILER       0xCE

// CMD codes
#define GET_READER_TYPE             0x10
#define GET_READER_SERIAL           0x11
#define READER_KEY_WRITE            0x12
#define GET_CARD_ID                 0x13
#define LINEAR_READ                 0x14
#define LINEAR_WRITE                0x15
#define BLOCK_READ                  0x16
#define BLOCK_WRITE                 0x17
#define BLOCK_IN_SECTOR_READ        0x18
#define BLOCK_IN_SECTOR_WRITE       0x19
#define SECTOR_TRAILER_WRITE        0X1A
#define USER_DATA_READ              0x1B
#define USER_DATA_WRITE             0x1C
#define VALUE_BLOCK_READ            0x1D
#define VALUE_BLOCK_WRITE           0x1E
#define VALUE_BLOCK_IN_SECTOR_READ  0x1F
#define VALUE_BLOCK_IN_SECTOR_WRITE 0x20
#define VALUE_BLOCK_INC             0x21
#define VALUE_BLOCK_DEC             0x22
#define VALUE_BLOCK_IN_SECTOR_INC   0x23
#define VALUE_BLOCK_IN_SECTOR_DEC   0x24
#define LINEAR_FORMAT_CARD          0x25
#define GET_CARD_ID_EX              0x2C
#define SECTOR_TRAILER_WRITE_UNSAFE 0x2F
#define SELF_RESET                  0x30
#define GET_DLOGIC_CARD_TYPE        0x3C
#define SET_CARD_ID_SEND_CONF       0x3D
#define GET_CARD_ID_SEND_CONF       0x3E
#define SET_UART_SPEED              0x70
#define RED_LIGHT_CONTROL           0x71

// ERR codes
#define OK                                      0x00
#define COMMUNICATION_ERROR                     0x01
#define COMMUNICATION_TIMEOUT                   0x50
#define COMMUNICATION_TIMEOUT_EXT               0x51
#define CHKSUM_ERROR                            0x02
#define CHKSUM_ERROR_RESPONSE                   0x52
#define CHKSUM_ERROR_EXT                        0x53
#define READING_ERROR                           0x03
#define WRITING_ERROR                           0x04
#define BUFFER_OVERFLOW                         0x05
#define MAX_ADDRESS_EXCEEDED                    0x06
#define MAX_KEY_INDEX_EXCEEDED                  0x07
#define NO_CARD                                 0x08
#define COMMAND_NOT_SUPPORTED                   0x09
#define FORBIDEN_DIRECT_WRITE_IN_SECTOR_TRAILER 0x0A
#define ADDRESSED_BLOCK_IS_NOT_SECTOR_TRAILER   0x0B
#define WRONG_ADDRESS_MODE                      0x0C
#define WRONG_ACCESS_BITS_VALUES                0x0D
#define AUTH_ERROR                              0x0E
#define PARAMETERS_ERROR                        0x0F
#define WRITE_VERIFICATION_ERROR                0x70
#define BUFFER_SIZE_EXCEEDED                    0x71
#define VALUE_BLOCK_INVALID                     0x72
#define VALUE_BLOCK_ADDR_INVALID                0x73
#define VALUE_BLOCK_MANIPULATION_ERROR          0x74

// MIFARE CLASSIC type id's:
#define MIFARE_CLASSIC_1k   0x08
#define MF1ICS50            0x08
#define SLE66R35            0x88 // Infineon = Mifare Classic 1k
#define MIFARE_CLASSIC_4k   0x18
#define MF1ICS70            0x18
#define MIFARE_CLASSIC_MINI 0x09
#define MF1ICS20            0x09

// DLOGIC CARD TYPE
#define TAG_UNKNOWN                 0
#define DL_MIFARE_ULTRALIGHT        0x01
#define DL_MIFARE_ULTRALIGHT_EV1_11 0x02
#define DL_MIFARE_ULTRALIGHT_EV1_21 0x03
#define DL_MIFARE_ULTRALIGHT_C      0x04
#define DL_NTAG_203                 0x05
#define DL_NTAG_210                 0x06
#define DL_NTAG_212                 0x07
#define DL_NTAG_213                 0x08
#define DL_NTAG_215                 0x09
#define DL_NTAG_216                 0x0A
#define DL_MIKRON_MIK640D           0x0B
#define NFC_T2T_GENERIC             0x0C
#define DL_MIFARE_MINI              0x20
#define DL_MIFARE_CLASSIC_1K        0x21
#define DL_MIFARE_CLASSIC_4K        0x22
#define DL_MIFARE_PLUS_S_2K         0x23
#define DL_MIFARE_PLUS_S_4K         0x24
#define DL_MIFARE_PLUS_X_2K         0x25
#define DL_MIFARE_PLUS_X_4K         0x26
#define DL_MIFARE_DESFIRE           0x27
#define DL_MIFARE_DESFIRE_EV1_2K    0x28
#define DL_MIFARE_DESFIRE_EV1_4K    0x29
#define DL_MIFARE_DESFIRE_EV1_8K    0x2A
#define DL_MIFARE_DESFIRE_EV2_2K    0x2B
#define DL_MIFARE_DESFIRE_EV2_4K    0x2C
#define DL_MIFARE_DESFIRE_EV2_8K    0x2D
//#define DL_UNKNOWN_ISO_14443_4      0x40
#define DL_GENERIC_ISO14443_4       0x40
#define DL_GENERIC_ISO14443_TYPE_B  0x41
#define DL_IMEI_UID                 0x80

// Function return sizes in bytes
#define READER_TYPE_SIZE   4
#define READER_SERIAL_SIZE 4
#define READER_KEY_SIZE    6
#define USER_DATA_SIZE     16
#define CARD_ID_SIZE       4
#define CARD_ID_EX_SIZE    10

enum PacketType {
	PACKET_ACK = ACK_HEADER,
	PACKET_ERR = ERR_HEADER,
	PACKET_RSP = RESPONSE_HEADER
};

class uFR {
	public:
		uFR(uint8_t rx, uint8_t tx);
		uFR(uint8_t rx, uint8_t tx, uint8_t reset);

		void begin(unsigned long baud = 115200); // Resets the reader if reset pin is used; make sure to add delay!
		inline void end() { readerSerial.end(); }
		// Resets through reset pin (if declared)
		void hardReset(); // Make sure to add delay!

		// All following functions return error codes after execution
		// If 0 is returned, the function has executed normally

		// Controls the reader's red LED. Green LED stops flashing while red LED is on
		uint8_t setRedLED(bool state);

		uint8_t getReaderType(uint8_t readerType[READER_TYPE_SIZE]);

		uint8_t getReaderSerial(uint8_t readerSerialNumber[READER_SERIAL_SIZE]);

		// Writes MIFARE key into reader EEPROM, at index location (0-31)
		uint8_t setReaderKey(uint8_t key[READER_KEY_SIZE], uint8_t index);

		// User data are 16 bytes form internal EEPROM
		uint8_t getUserData(uint8_t data[USER_DATA_SIZE]);

		uint8_t setUserData(uint8_t data[USER_DATA_SIZE]);

		// Sends reset command (add 2s delay!)
		uint8_t softReset();
		
		// Gets card UID that is present in reader's RF field. Obsolete
		uint8_t getCardIDSimple(uint8_t cardID[CARD_ID_SIZE], uint8_t *cardType = nullptr);

		// Length - UID size in bytes (4, 7 or 10)
		uint8_t getCardID(uint8_t cardID[CARD_ID_EX_SIZE], uint8_t *length = nullptr, uint8_t *cardType = nullptr);

		// Card type per DLogic enumeration
		uint8_t getCardTypeDLogic(uint8_t *cardType);

		// -------------------------------------------------------------

		static const char * TypeDLogicToString(uint8_t type);
		
	private:
		SoftwareSerial readerSerial;
		uint8_t resetPin = 0;
		void flushSerial(); // Flush serial input buffer

		void sendPacketCMD(uint8_t command, uint8_t EXTlength = 0, uint8_t par0 = 0, uint8_t par1 = 0);
		void sendPacketEXT(uint8_t *packet, uint8_t length);

		void setPacketSerial (); // Sets static protected packet serial pointer

		class Packet {
			public:
				static uint8_t checksum(uint8_t *packet, uint8_t size = PACKET_LENGTH - 1);
				inline uint8_t getErrorCode() { return errorCode; }
				inline uint8_t getLength() { return length; }
				void copyData(uint8_t *array, uint16_t start, uint16_t length);
				void copyDataReverse(uint8_t *array, uint16_t start, uint16_t length);
				inline uint8_t operator[] (uint8_t i) { return data[i]; }
				friend void uFR::setPacketSerial ();
			protected:
				static SoftwareSerial *serial;
				uint8_t errorCode = 0;
				uint8_t length = PACKET_LENGTH;
				uint8_t *data;
		};
		class CommonPacket : public Packet {
			// Returns error code
			uint8_t read(uint8_t response[PACKET_LENGTH]);
			uint8_t validate(uint8_t packet[PACKET_LENGTH], PacketType type, uint8_t command);
			public:
				CommonPacket(PacketType type, uint8_t command);
				~CommonPacket();
		};
		class EXTPacket : public Packet {
			// Returns error code, reads AND validates
			uint8_t read(uint8_t *response, uint8_t length);
			public:
				EXTPacket(uint8_t length);
				~EXTPacket();
		};
};
