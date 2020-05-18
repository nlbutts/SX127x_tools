#pragma once

#include <stdint.h>
#include <vector>

// Forward declaration
class SX1276;

/**
 * @brief Data transfer class
 * @details The purpose of this class is to take some payload and
 * transfer via some radio device. Currently it adds a checksum and size
 * field on the payload. But in the future it could use ECC codes and
 * interleaving to improve tolerance to bad packets.
 *
 */
class DataTransfer
{
public:
    /**
     * @brief Constructor
     *
     * @param radio reference to a SX1276 device, this should be an interface class
     * @param timeout timeout in milliseconds
     * @param debug true to enable debug false otherwise
     */
    DataTransfer(SX1276 &radio, int timeout, bool debug);
    virtual ~DataTransfer();

    /**
     * @brief Send data
     * @details It takes a payload, does magic and transfers it
     *
     * @param payload payload to transfer
     * @return true if successful, false otherwise
     */
    bool send(std::vector<uint8_t> &payload);
    /**
     * @brief Receives data
     * @details Receives the magic, decodes it and returns it
     *
     * @param payload reference to a vector
     * @return true if successful, false if it timed out and CRC didn't match
     */
    bool receive(std::vector<uint8_t> &payload);

private:
    void print_vec(uint8_t * buf, int num);

private:
    static constexpr int PayloadSize = 240;
    static constexpr int SeqSize = 2;
    static constexpr int TxPayloadSize = SeqSize + PayloadSize;
    static constexpr int DataOverhead = 6;

    SX1276     *_radio;
    int         _timeout;
    bool        _debug;
};