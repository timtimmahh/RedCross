/**
 * @file SdFileIo.h
 *
 * @brief SDFileIO class declaration.
 *
 * This work complies with the JMU honor code.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_IO_SRC_SDFILEIO_H_
#define REDCROSS_LIB_IO_SRC_SDFILEIO_H_

#include <esp_vfs_fat.h>
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define PIN_MISO GPIO_NUM_2
#define PIN_MOSI GPIO_NUM_15
#define PIN_CLK  GPIO_NUM_14
#define PIN_CS   GPIO_NUM_13

#define absolute(fName) (prefix + fName)

using namespace std;

/**
 * A utility for safely operating on an SD card asynchronously over the SPI
 * interface.
 */
class SDFileIO {
 private:
  static SDFileIO* sdInst;
  /**
   * The name of the mount point on the vfs. All file names must be prepended
   * with this in order to be found.
   */
  string prefix;
  /**
   * The SD/MMC driver configuration.
   */
  sdmmc_host_t host;
  /**
   * The SD SPI driver configuration.
   */
  sdspi_slot_config_t slot_config;
  /**
   * The connected SD card.
   */
  sdmmc_card_t* card;
  /**
   * Whether the SD card is mounted and ready to perform IO operations.
   */
  bool ready;
  /**
   * Explicit value constructor to specify the mount point of the SD card.
   *
   * @param prefix the SD card mount point
   */
  explicit SDFileIO(const char* prefix = "/sdcard");
  /**
   * Processes variadic template packs of file open modes such as in, out,
   * append, binary, truncate, etc.
   *
   * @tparam mode the first specified open mode
   * @tparam modes the rest of the open modes
   * @return the bitwise OR of all specified open modes
   */
  template<const ios::openmode& mode, const ios::openmode& ... modes>
  ios::openmode orModes();
 public:
  static SDFileIO& instance() {
    if (sdInst == nullptr)
      sdInst = new SDFileIO();
    return *sdInst;
  }
  /**
   * Destructor. Unmounts the SD card and disables the driver since it's no
   * longer needed.
   */
  ~SDFileIO();
  /**
   * Checks if the specified file exists on the SD card.
   *
   * @param fileName the name of the file
   * @return whether the file exists
   */
  bool exists(const char* fileName);
  bool isReady();
  /**
   * Mounts the SD card and virtual file systems to be accessed.
   *
   * @return whether mount succeeded
   */
  bool mount();
  /**
   * Prints information about the SD card.
   */
  void printCardInfo();
  /**
   * Opens a file with the specified file name and modes, then executes the
   * provided operations on the file. After all operations are complete, the
   * file gets closed safely to ensure all operations are synced with the
   * virtual file system.
   *
   * @tparam FStream the type of stream for the file
   * @tparam mode the first open mode
   * @tparam modes any additional open modes
   * @param fileName the name of the file
   * @param fileOp the operations to perform on the opened file
   * @return whether all operations completed successfully
   */
  template<typename FStream,
      const ios::openmode& mode,
      const ios::openmode& ... modes>
  inline bool openFile(const char* fileName, bool(* fileOp)(FStream&));
  /**
   * Writes the provided data to the file specified by file name. This method
   * truncates the entire file upon opening so the only data that will be in
   * the file is what was passed as a parameter here.
   *
   * @tparam modes file open modes, excl. ios::out | ios::trunc
   * @param fileName the name of the file
   * @param data the data to write
   * @return whether the data was successfully written
   */
  template<const ios::openmode& ... modes>
  bool writeFile(const char* fileName, const vector<const char*>& data);
  /**
   * Writes the provided binary data to the file specified by file name. This
   * method truncates the entire file upon opening so the only data that will
   * be in the file is what was passed as a parameter here.
   *
   * @tparam modes file open modes, excl. ios::out | ios::trunc | ios::binary
   * @param fileName the name of the file
   * @param data the data to write
   * @return whether the data was successfully written
   */
  template<const ios::openmode... modes>
  bool writeBinFile(const char* fileName, const string& data);
  /**
   * Appends the provided data to the file specified by file name.
   *
   * @tparam modes file open modes, excl. ios::out | ios::app
   * @param fileName the name of the file
   * @param data the data to append
   * @return whether the data was successfully written
   */
  template<const ios::openmode... modes>
  bool appendFile(const char* fileName, const vector<const char*>& data);
  /**
   * Appends the provided binary data to the file specified by file name.
   *
   * @tparam modes file open modes, excl. ios::out | ios::app | ios::binary
   * @param fileName the name of the file
   * @param data the data to write
   * @return whether the data was successfully written
   */
  template<const ios::openmode... modes>
  bool appendBinFile(const char* fileName, const string& data);
  /**
   * Reads content from the specified file into the data parameter.
   *
   * @tparam modes file open modes, excl. ios::in | ios::ate
   * @param fileName the name of the file
   * @param data the destination
   * @return whether the data was successfully read
   */
  template<const ios::openmode... modes>
  bool readFile(const char* fileName, string& data);
  /**
   * Reads binary content from the specified file into the data parameter.
   *
   * @tparam modes file open modes, excl. ios::in | ios::ate | ios::binary
   * @param fileName the name of the file
   * @param data the destination
   * @return whether the data was successfully read
   */
  template<const ios::openmode... modes>
  bool readBinFile(const char* fileName, string& data);
};

#endif // REDCROSS_LIB_IO_SRC_SDFILEIO_H_