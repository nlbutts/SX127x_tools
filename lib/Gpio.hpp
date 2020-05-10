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
#include <gpiod.h>
// These enums must match the enums in gpio.h

/**
 * Gpio Output modes
 */
typedef enum {
    MODE_STRONG = 0,   /**< Default. Strong High and Low */
    MODE_PULLUP = 1,   /**< Resistive High */
    MODE_PULLDOWN = 2, /**< Resistive Low */
    MODE_HIZ = 3       /**< High Z State */
} Mode;

/**
 * Gpio Direction options
 */
typedef enum {
    DIR_OUT = 0,      /**< Output. A Mode can also be set */
    DIR_IN = 1,       /**< Input */
    DIR_OUT_HIGH = 2, /**< Output. Init High */
    DIR_OUT_LOW = 3   /**< Output. Init Low */
} Dir;

/**
 * Gpio Edge types for interrupts
 */
typedef enum {
    EDGE_NONE = 0,   /**< No interrupt on Gpio */
    EDGE_BOTH = 1,   /**< Interrupt on rising & falling */
    EDGE_RISING = 2, /**< Interrupt on rising only */
    EDGE_FALLING = 3 /**< Interrupt on falling only */
} Edge;

/**
 * Gpio Input modes
 */
typedef enum {
    MODE_IN_ACTIVE_HIGH = 0, /**< Resistive High */
    MODE_IN_ACTIVE_LOW = 1,  /**< Resistive Low */
} InputMode;

/**
 * Gpio output driver modes
 */
typedef enum {
    MODE_OUT_OPEN_DRAIN = 0, /**< Open Drain Configuration */
    MODE_OUT_PUSH_PULL = 1,  /**< Push Pull Configuration */
} OutputMode;

/**
 * @brief API to General Purpose IO
 *
 * This file defines the gpio interface for libmraa
 *
 * @snippet gpio.cpp Interesting
 */
class Gpio
{
  public:
    /**
     * Instantiates a Gpio object
     *
     * @param pin pin number to use
     * @param owner (optional) Set pin owner, default behaviour is to 'own'
     * the pin if we exported it. This means we will close it on destruct.
     * Otherwise it will get left open. This is only valid in sysfs use
     * cases
     * @param raw (optional) Raw pins will use gpiolibs pin numbering from
     * the kernel module. Note that you will not get any muxers set up for
     * you so this may not always work as expected.
     */
    Gpio(int pin, Dir direction = DIR_IN)
    {
        _chip = gpiod_chip_open_by_name("gpiochip0");
        _line = gpiod_chip_get_line(_chip, pin);
        _pin = pin;
        _mode = MODE_STRONG;
        _dir  = direction;
        dir(_dir);
    }
    /**
     * Gpio object destructor, this will only unexport the gpio if we where
     * the owner
     */
    ~Gpio()
    {
        //gpiod_line_close_chip(_line);
        //gpiod_chip_close(_chip);
        _line = nullptr;
        _chip = nullptr;
    }
    /**
     * Change Gpio mode
     *
     * @param mode The mode to change the gpio into
     * @return int of operation
     */
    int mode(Mode mode)
    {
        switch (mode)
        {
            case MODE_STRONG:
                break;
            case MODE_PULLUP:
                break;
            case MODE_PULLDOWN:
                break;
            case MODE_HIZ:
                break;
        }

        return (int )42;
    }
    /**
     * Change Gpio direction
     *
     * @param dir The direction to change the gpio into
     * @return int of operation
     */
    int dir(Dir dir)
    {
        _dir = dir;
        switch (dir)
        {
            case DIR_OUT:
            case DIR_OUT_LOW:
                gpiod_line_request_output(_line, "lora", 0);
            case DIR_OUT_HIGH:
                break;
                gpiod_line_request_output(_line, "lora", 1);
                break;
            case DIR_IN:
                gpiod_line_request_input(_line, "lora");
                break;
        }

        return (int )0;
    }

    /**
     * Read Gpio direction
     *
     * @throw std::runtime_error in case of failure
     * @return int of operation
     */
    Dir readDir()
    {
        return _dir;
    }

    /**
     * Read value from Gpio
     *
     * @return Gpio value
     */
    int read()
    {
        return gpiod_line_get_value(_line);
    }
    /**
     * Write value to Gpio
     *
     * @param value Value to write to Gpio
     * @return int of operation
     */
    int write(int value)
    {
        gpiod_line_set_value(_line, value);
        return (int)0;
    }
    /**
     * Get pin number of Gpio. If raw param is True will return the
     * number as used within sysfs. Invalid will return -1.
     *
     * @param raw (optional) get the raw gpio number.
     * @return Pin number
     */
    int getPin()
    {
        return _pin;
    }

    /**
     * Change Gpio input mode
     *
     * @param mode The mode to change the gpio input
     * @return int of operation
     */
    int inputMode(InputMode mode)
    {
        return (int )42;
    }

    /**
     * Change Gpio output driver mode
     *
     * @param mode Set output driver mode
     * @return int of operation
     */
    int outputMode(OutputMode mode)
    {
        return (int)42;
    }

  private:
    gpiod_chip * _chip;
    gpiod_line * _line;
    Dir          _dir;
    Mode         _mode;
    int          _pin;
};
