/**
 * @file cppEspI2C.hpp
 *
 * @brief cppEspI2C class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_PERIF_I2C_CPPESPI2C_HPP_
#define REDCROSS_MAIN_PERIF_I2C_CPPESPI2C_HPP_

#include <driver/i2c.h>
#include <functional>

template<typename... Args>
using idfFunc = esp_err_t(*)(Args...);

namespace i2c {

enum class Port { PORT0 = I2C_NUM_0, PORT1 = I2C_NUM_1 };
enum class Mode {
  SLAVE = I2C_MODE_SLAVE,
  MASTER = I2C_MODE_MASTER
};

template<Mode mode>
class Driver {
 protected:
  Port port;
  explicit Driver(Port port) : port(port) {}
};

class Master : Driver<Mode::MASTER> {
  explicit Master(Port port, int intrFlags = 0) : Driver(port) {

	i2c_driver_install(static_cast<i2c_port_t>(port),
					   static_cast<i2c_mode_t>(Mode::MASTER),
					   0, 0, intrFlags);
  }

  ~Master() {
	i2c_driver_delete(static_cast<i2c_port_t >(port));
  }

};

#endif //REDCROSS_MAIN_PERIF_I2C_CPPESPI2C_HPP_
