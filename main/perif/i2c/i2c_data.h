/**
 * @file i2c_data.h
 *
 * @brief i2c_data class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_PERIF_I2C_I2C_DATA_H_
#define REDCROSS_MAIN_PERIF_I2C_I2C_DATA_H_

//#include "cppEspI2C.hpp"
#include <mutex>
#include <driver/i2c.h>

namespace i2c {

/**
 * Mutexes for each i2c bus port.
 */
static std::array<std::mutex, I2C_NUM_MAX> portLocks{};
static std::array<i2c_config_t, I2C_NUM_MAX> configs{};
static std::array<bool, I2C_NUM_MAX> initializedPorts{false, false};
static std::array<unsigned, I2C_NUM_MAX> portRefs{0, 0};

}

#endif //REDCROSS_MAIN_PERIF_I2C_I2C_DATA_H_
