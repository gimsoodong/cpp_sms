#ifndef SMS_H_
#define SMS_H_

#include <string>

class SMS
{
public:
    SMS(const std::string& message_utf8,
            const std::string& cell_phone_number,
            const std::string& smsc_number);
    virtual ~SMS();

    SMS(const SMS& make_message);

    static std::string clear();
    static std::string cmgf();

    std::string cmgs_len() const;
    std::string cmgs_msg() const;

    std::string pdu_str() const;
    std::size_t pdu_len() const;
    std::string cell_phone_number() const;
    std::string message() const;

private:
    void stringToPDU_(std::string utf8String,
            std::string phoneNumber,
            std::string smscNumber,
            std::size_t& length,
            std::string& msg);
    std::string semiOctetToString_(std::string inp);
    std::string intToHex_(int i);

private:
    std::string pdu_str_;
    std::size_t pdu_len_;

private:
    std::string cell_phone_number_;
    std::string message_;

public:
    static constexpr const char str_control_cr = 0x0D;
    static constexpr const char str_control_sub = 0x1A;
    static constexpr const char* str_cmgf = "AT+CMGF";
    static constexpr const char* str_cmgs = "AT+CMGS";
};

#endif // SMS_H_
