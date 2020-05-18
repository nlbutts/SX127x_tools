/*
 * Author: Nick Butts <nlbutts@ieee.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdexcept>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/**
 * MRAA SPI Modes
 */
typedef enum {
    SPI_MODE0 = 0, /**< CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge,
                      output data (change) on falling edge */
    SPI_MODE1 = 1, /**< CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge,
                      output data (change) on rising edge */
    SPI_MODE2 = 2, /**< CPOL = 1, CPHA = 0, Clock idle low, data is clocked in on falling edge,
                      output data (change) on rising edge */
    SPI_MODE3 = 3, /**< CPOL = 1, CPHA = 1, Clock idle low, data is clocked in on rising, edge
                      output data (change) on falling edge */
} Spi_Mode;


/**
* @brief API to Serial Peripheral Interface
*
* This file defines the SPI interface for libmraa
*
* @snippet spi.cpp Interesting
*/
class Spi
{
  public:
    /**
     * Initialise SPI object using the board mapping to set muxes
     *
     * @param bus to use, as listed in the platform definition, normally 0
     */
    Spi(int bus)
    {
        char str[100];
        snprintf(str, 100, "/dev/spidev%d.0", bus);
        _fd = open(str, O_RDWR);
        printf("Opened spi bus %s received handle %d\n", str, _fd);
    }

    /**
     * Initialise SPI object using 'raw' mode. Mraa will go and grab the spidev device lablled /dev/spidev[bus].[cs]
     *
     * @param bus to use
     * @param cs to use
     */
    Spi(int bus, int cs)
    {
        char str[100];
        snprintf(str, 100, "/dev/spidev%d.%d", bus, cs);
        _fd = open(str, O_RDWR);
    }

    /**
     * Closes spi bus
     */
    ~Spi()
    {
        close(_fd);
    }

    /**
     * Set the SPI device mode. see spidev0-3
     *
     * @param mode the mode. See Linux spidev doc
     * @return int of operation
     */
    int mode(Spi_Mode mode)
    {
        _mode = mode;
        return 0;
    }

    /**
     * Set the SPI device operating clock frequency
     *
     * @param hz the frequency to set in hz
     * @return int of operation
     */
    int frequency(int hz)
    {
        _freq = hz;
        return 0;
    }

    /**
     * Write single byte to the SPI device
     *
     * @param data the byte to send
     * @return data received on the miso line or -1 in case of error
     */
    int writeByte(uint8_t data)
    {
        return 0;
    }

    /**
     * Write single byte to the SPI device
     *
     * @param data the byte to send
     * @return data received on the miso line or -1 in case of error
     */
    int writeWord(uint16_t data)
    {
        return 0;
    }

    /**
     * Write buffer of bytes to SPI device The pointer return has to be
     * free'd by the caller. It will return a NULL pointer in cases of
     * error
     *
     * @param txBuf buffer to send
     * @param length size of buffer to send
     * @return uint8_t* data received on the miso line. Same length as passed in
     */
    uint8_t* write(const uint8_t* txBuf, int length)
    {
        return 0;
    }

#ifndef SWIG
    /**
     * Write buffer of bytes to SPI device The pointer return has to be
     * free'd by the caller. It will return a NULL pointer in cases of
     * error
     *
     * @param txBuf buffer to send
     * @param length size of buffer (in bytes) to send
     * @return uint8_t* data received on the miso line. Same length as passed in
     */
    uint16_t* writeWord(const uint16_t* txBuf, int length)
    {
        return 0;
    }
#endif

#ifndef SWIG
    /**
     * Transfer data to and from SPI device Receive pointer may be null if
     * return data is not needed.
     *
     * @param txBuf buffer to send
     * @param rxBuf buffer to optionally receive data from spi device
     * @param length size of buffer to send
     * @return int of operation
     */
    int transfer(const uint8_t* txBuf, uint8_t* rxBuf, int length)
    {
        struct spi_ioc_transfer xfer = {0};
        xfer.tx_buf = (int)txBuf;
        xfer.rx_buf = (int)rxBuf;
        xfer.len = length;
        xfer.speed_hz = _freq;
        xfer.bits_per_word = 8;
        int status = ioctl(_fd, SPI_IOC_MESSAGE(1), &xfer);
        status = std::min(0, status);
        return status;
    }

    /**
     * Transfer data to and from SPI device Receive pointer may be null if
     * return data is not needed.
     *
     * @param txBuf buffer to send
     * @param rxBuf buffer to optionally receive data from spi device
     * @param length size of buffer to send
     * @return int of operation
     */
    int transfer_word(const uint16_t* txBuf, uint16_t* rxBuf, int length)
    {
        return (int)0;
    }
#endif

    /**
     * Change the SPI lsb mode
     *
     * @param lsb Use least significant bit transmission - 0 for msbi
     * @return int of operation
     */
    int lsbmode(bool lsb)
    {
        return (int)0;
    }

    /**
     * Set bits per mode on transaction, default is 8
     *
     * @param bits bits per word
     * @return int of operation
     */
    int bitPerWord(unsigned int bits)
    {
        return (int)0;
    }

  private:
    int         _fd;
    Spi_Mode    _mode;
    int         _freq;
    int         _bits;
};
