//
// Created by tim on 2/2/20.
//

#ifndef REDCROSS_LIB_ACCELGYRO_SRC_ACCELGYRO_H
#define REDCROSS_LIB_ACCELGYRO_SRC_ACCELGYRO_H
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>
#include <Sensor.h>
#include <type_traits>
#include <array>


/**
 * A simple handler for a series of data points.
 */
class Coords : Printable {
private:
  int size;
  int16_t* points = nullptr;
public:
  /**
   * Constructor for size coordinate points. Default is 3 indicating x, y, z
   * coordinates.
   *
   * @param size the number of coordinate points
   */
  explicit Coords(int size = 3)
  : size(size)
  , points(new int16_t[size]) {}

  /**
   * Copy constructor so this can be passed as value.
   *
   * @param original the original coordinates
   */
  Coords(const Coords& original)
  : size(original.size)
  , points(new int16_t[size]) {
    for (int i = 0; i < size; ++i)
      points[i] = original.points[i];
  }

  /**
   * Destructor. Must delete new'ed coordinate array.
   */
  ~Coords() override {
    if (points != nullptr) {
      delete[] points;
      points = nullptr;
    }
  }

  /**
   * Convert the coordinates to a String representation.
   *
   * @return String representation of Coords
   */
  String toString() const {
    String result = String("(");
    for (int i = 0; i < size - 1; ++i)
      result += String(points[i]) + ", ";
    result += String(points[size-1]) + ")";
    return result;
  }

  /**
   * Index operator to easily obtain individual coordinate points. Returns by
   * reference so coordinates can be passed by address.
   *
   * @param i the index of the coordinate
   * @return the coordinate at i
   */
  int16_t& operator[](int i) const {
    return points[i];
  }

private:
  /**
   * Allows for using Serial.print without having to convert this to another
   * type first.
   *
   * @param p the printer
   * @return the size of the text printed
   */
  size_t printTo(Print &p) const override {
    return p.print(toString());
  }
};

// define sda and scl pins for different boards
#ifdef ARDUINO_ARCH_ESP32
#define MPU_SDA_PIN     21
#define MPU_SCL_PIN     22
#define MPU_INT_PIN     15
#elif ARDUINO_AVR_UNO
#define MPU_SDA_PIN     A4
#define MPU_SCL_PIN     A5
//#define MPU_INT_PIN
#endif

/**
 * Accelerometer and gyroscope wrapper around MPU6050 library by Jeff Rowberg.
 */
class AccelGyro : public Sensor<Coords> {
private:
  MPU6050 mpu;
  uint8_t sda;
  uint8_t scl;
  Coords data;

  /**
   * Starts the I2C interface for the accelerometer and gyro.
   */
  void start() {
    // only ESP32 needs to specify pins for I2C since Arduino UNO only has one
    Wire.begin(
#ifdef ARDUINO_ARCH_ESP32
        sda, scl
#endif
    );
    mpu.reset();
    delay(1000);
    mpu.initialize();
  }

public:
  /**
   * Constructor with sda and scl pins.
   *
   * @param sdaPin the sda pin
   * @param sclPin the scl pin
   */
  explicit AccelGyro(int sdaPin = MPU_SDA_PIN, int sclPin = MPU_SCL_PIN) :
  mpu(),
  sda(sdaPin),
  scl(sclPin),
  data(6) {
    start();
  }

  /**
   * Obtains the acceleration data.
   *
   * @return acceleration coords (x, y, z)
   */
  Coords getAcceleration();
  /**
   * Obtains the gyroscope rotation data.
   *
   * @return rotation coords (x, y, z)
   */
  Coords getRotation();
  /**
   * The name of this sensor.
   *
   * @return the sensor name
   */
  String name() override;
  /**
   * Gets both acceleration and rotation data. (Magnetometer WIP).
   *
   * @return acceleration and rotation data
   */
  Coords getData() override;
  /**
   * Convert coordinate data to a String.
   *
   * @param cData coordinate data
   * @return String representation of coords
   */
  String dataAsString(const Coords& cData) override;
};

#endif //REDCROSS_LIB_ACCELGYRO_SRC_ACCELGYRO_H
