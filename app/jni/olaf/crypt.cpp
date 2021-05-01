#include "crypt.h"


const static std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}


std::string crypt::base64Encode(uint8_t const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	uint8_t char_array_3[3];
	uint8_t char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}

std::vector<uint8_t> crypt::base64Decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	uint8_t char_array_4[4];
	uint8_t char_array_3[3];
	 
	std::vector<uint8_t> retBytes;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				retBytes.push_back(char_array_3[i]);

			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			retBytes.push_back(char_array_3[j]);
	}

	return retBytes;
}

std::vector<uint8_t> crypt::decodeHexString(const std::string& string) {
	std::vector<uint8_t> bytes;

	if (string.empty()) {
		return bytes;
	}

	for (size_t i = 0; i < string.length(); i += 2) {
		const std::string hex = string.substr(i, 2);
		const uint8_t decimal = std::strtol(hex.c_str(), 0, 16);
		bytes.push_back(decimal);
	}

	return bytes;
}

std::string crypt::encodeHexString(const std::vector<uint8_t>& v) {
	std::string result;
	result.resize(v.size() * 2);
	const char letters[] = "0123456789abcdef";
	char* current_hex_char = &result[0];
	for (uint8_t b : v) {
		*current_hex_char++ = letters[b >> 4];
		*current_hex_char++ = letters[b & 0xf];
	}
	return result;
}

std::vector<uint8_t> crypt::calculateCRC16(std::vector<uint8_t> bytes)
{
	uint16_t   polynomial = 0x1021;
	uint16_t crc = 0x0000;


	for (uint8_t b : bytes) {
		for (int i = 0; i < 8; i++) {
			bool bit = ((b >> (7 - i) & 1) == 1);
			bool c15 = ((crc >> 15 & 1) == 1);
			crc <<= 1;
			if (c15 ^ bit)
				crc ^= polynomial;
		}
	}

	crc &= 0xFFFF;

	std::vector<uint8_t> crc16;
	crc16.push_back((crc >> 8));
	crc16.push_back((crc & 0x00ff));

	return crc16;
}