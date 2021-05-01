#include <jni.h>
#include <sstream>
#include "crypt.h"

using namespace std;

struct Account {

    int8_t workchain;
    vector<uint8_t> account_id;

    Account(int8_t workchain, vector<uint8_t> account_id) {
        this->workchain = workchain;
        this->account_id = std::move(account_id);
    }

};

Account fromStr(const string &input);
Account decodeHex(const string& input);
Account decodeBase64(string input);

string toHex(const Account& account);
string toBase64(Account account, bool bounceable, bool test, bool as_url);

vector<string> split(const string &input, char delimiter);
void replaceAll(string &string, char forReplacement, char target);


Account fromStr(const string &input) {
    if (input.length() == 48)
        return decodeBase64(input);
    else
        return decodeHex(input);
}

Account decodeHex(const string& input) {
    vector<string> vec = split(input, ':');

    if (vec.size() != 2) {
        //return Err(format!("Malformed std hex address. No \":\" delimiter. \"{}\"", data));
    }

    if (vec[0].length() != 64) {
        //return Err(format!("Malformed std hex address. Invalid account ID length. \"{}\"", data));
    }
    int8_t t = stoul(vec[0], nullptr, 10);
    return Account(t, crypt::decodeHexString(vec[1]));
}

Account decodeBase64(string input) {
    replaceAll(input, '_', '/');
    replaceAll(input, '-', '+');


    std::vector<uint8_t> bytes = crypt::base64Decode(input);

    vector<uint8_t> data(bytes.begin(), bytes.end() - 2);

    vector<uint8_t> originalCrc(bytes.end() - 2, bytes.end());
    vector<uint8_t> calculatedCrc = crypt::calculateCRC16(data);

    if (originalCrc != calculatedCrc) {
        //return Err(format!("base64 address invalid CRC \"{}\"", data));
    }

    if ((bytes[0] & 0x3fu) != 0x11u) {
        //return Err(format!("base64 address invalid tag \"{}\"", data));
    }


    int8_t workchain;
    vector<uint8_t> rawWorkchain(bytes.begin() + 1, bytes.begin() + 3);
    vector<uint8_t> rawAccountId(bytes.begin() + 2, bytes.end() - 2);
    std::memcpy(&workchain, rawWorkchain.data(), sizeof(int8_t));

    return Account(workchain, rawAccountId);
}

string toHex(const Account& account) {
    stringstream fmt;
    fmt << +account.workchain << ":" << crypt::encodeHexString(account.account_id);
    return fmt.str();
}

string toBase64(Account account, bool bounceable, bool test, bool as_url) {

    uint8_t tag;
    if (bounceable)
        tag = 0x11;
    else
        tag = 0x51;

    if (test)
        tag |= 0x80;

    vector<uint8_t> bytes;
    bytes.push_back(tag);
    bytes.push_back(account.workchain);
    bytes.insert(bytes.end(), account.account_id.begin(), account.account_id.end());

    vector<uint8_t> crc = crypt::calculateCRC16(bytes);
    bytes.insert(bytes.end(), crc.begin(), crc.end());

    string base64 = crypt::base64Encode(bytes.data(), bytes.size());

    if (as_url) {
        replaceAll(base64, '/', '_');
        replaceAll(base64, '+', '-');
    }

    return base64;
}


vector<string> split(const string &input, char delimiter) {
    vector<string> result;
    stringstream ss(input);
    string item;

    while (getline(ss, item, delimiter))
        result.push_back(item);

    return result;
}

void replaceAll(string &string, char forReplacement, char target) {
    for (int i = 0; i < string.size(); i++) {
        if (string[i] == forReplacement) {
            string[i] = target;
        }
    }
}


extern "C"
JNIEXPORT jstring JNICALL
Java_ru_olaf_custom_AddressDecoder_convertToHex(JNIEnv *env, jclass clazz, jstring address) {
    Account currentAccount = fromStr(env->GetStringUTFChars(address, nullptr));
    return env->NewStringUTF(toHex(currentAccount).c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_ru_olaf_custom_AddressDecoder_convertToBase64(JNIEnv *env, jclass clazz, jstring address, jboolean bounceable, jboolean test, jboolean as_url) {
    Account currentAccount = fromStr(env->GetStringUTFChars(address, nullptr));
    return env->NewStringUTF(toBase64(currentAccount, bounceable, test, as_url).c_str());
}