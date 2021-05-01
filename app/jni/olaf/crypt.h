#include <iostream>
#include <string>
#include <vector>


class crypt {

public:

    static std::vector<uint8_t> base64Decode(std::string const& encoded_string);

    static std::string  base64Encode(uint8_t const* bytes_to_encode, unsigned int in_len);

    static std::vector<uint8_t> decodeHexString(const std::string& string);

    static std::string encodeHexString(const std::vector<uint8_t>& v);

    static std::vector<uint8_t> calculateCRC16(std::vector<uint8_t> bytes);
};