#include "SMS.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/locale.hpp>
#include <boost/exception/to_string.hpp>
#include <boost/algorithm/string.hpp>

SMS::SMS(const std::string& message_utf8,
        const std::string& cell_phone_number,
        const std::string& smsc_number)
        : pdu_str_(""),
          pdu_len_(0),
          cell_phone_number_(cell_phone_number),
          message_(message_utf8)
{
    SMS::stringToPDU_(message_utf8,
                            cell_phone_number,
                            smsc_number,
                            this->pdu_len_,
                            this->pdu_str_);

    boost::to_upper(this->pdu_str_);
}

SMS::~SMS()
{
}

SMS::SMS(const SMS& sms)
{
    this->pdu_str_ = sms.pdu_str_;
    this->pdu_len_ = sms.pdu_len_;

    this->cell_phone_number_ = sms.cell_phone_number_;
    this->message_ = sms.message_;
}

std::string SMS::clear()
{
    return std::string() + SMS::str_control_sub;
}

std::string SMS::cmgf()
{
    return std::string(SMS::str_cmgf) + "=0" + SMS::str_control_cr;
}

std::string SMS::cmgs_len() const
{
    return std::string(SMS::str_cmgs) + "=" + boost::to_string(this->pdu_len_) + SMS::str_control_cr;
}

std::string SMS::cmgs_msg() const
{
    return this->pdu_str_ + SMS::str_control_sub;
}

std::string SMS::pdu_str() const
{
    return this->pdu_str_;
}

std::size_t SMS::pdu_len() const
{
    return this->pdu_len_;
}

std::string SMS::cell_phone_number() const
{
    return this->cell_phone_number_;
}

std::string SMS::message() const
{
    return this->message_;
}

// private

void SMS::stringToPDU_(std::string message_utf8,
        std::string phoneNumber,
        std::string smsc_number,
        std::size_t& length,
        std::string& msg)
{
    message_utf8 = boost::locale::conv::from_utf(message_utf8,
                                                 "utf16be");
    std::string SMSC_NUMBER_FORMAT = "92";

    if(smsc_number[0] == '+')
    {
        SMSC_NUMBER_FORMAT = "91";
        smsc_number = smsc_number.substr(1);
    }
    else if(smsc_number[0] != '0')
    {
        SMSC_NUMBER_FORMAT = "91";
    }

    if(smsc_number.size() % 2 != 0)
    {
        smsc_number += "F";
    }

    std::string SMSC = this->semiOctetToString_(smsc_number);
    int SMSC_INFO_LENGTH = ((SMSC_NUMBER_FORMAT + SMSC).size()) / 2;
    int SMSC_LENGTH = SMSC_INFO_LENGTH;

    std::string firstOctet = "1100";

    std::string REIVER_NUMBER_FORMAT = "92";
    if(phoneNumber[0] == '+')
    {
        REIVER_NUMBER_FORMAT = "91";
        phoneNumber = phoneNumber.substr(1);
    }
    else if(phoneNumber[0] != '0')
    {
        REIVER_NUMBER_FORMAT = "91";
    }

    std::string REIVER_NUMBER_LENGTH = this->intToHex_(phoneNumber.size());

    if(phoneNumber.size() % 2 != 0)
    {
        phoneNumber += "F";
    }

    std::string REIVER_NUMBER = this->semiOctetToString_(phoneNumber);
    std::string PROTO_ID = "00";
    std::string DATA_ENCODING = "08";
    std::string VALID_PERIOD = "AA";
    std::string userDataSize = this->intToHex_(message_utf8.size());

    std::string output = "";
    for(std::size_t i = 0; i < message_utf8.size(); i++)
    {
        output += this->intToHex_((message_utf8[i] & 0xFF));
    }

    std::ostringstream header;
    header << std::setfill('0')
           << std::setw(2)
           << SMSC_INFO_LENGTH
           << SMSC_NUMBER_FORMAT
           << SMSC
           << firstOctet
           << REIVER_NUMBER_LENGTH
           << REIVER_NUMBER_FORMAT
           << REIVER_NUMBER
           << PROTO_ID
           << DATA_ENCODING
           << VALID_PERIOD
           << userDataSize
           << output;

    msg = header.str();
    length = (msg.size() / 2 - SMSC_LENGTH - 1);
}

std::string SMS::semiOctetToString_(std::string inp)
{
    std::string out = "";
    for(std::size_t i = 0; i < inp.size(); i = i + 2)
    {
        std::string temp = inp.substr(i,
                                      i + 2);
        out = out + temp[1] + temp[0];
    }

    return out;
}

std::string SMS::intToHex_(int i)
{
    std::ostringstream os;
    os << std::setfill('0')
       << std::setw(2)
       << std::hex
       << i;
    return os.str();
}

