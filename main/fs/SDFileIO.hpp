/**
 * @file SDFileIO.h
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
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define PIN_MISO GPIO_NUM_2
#define PIN_MOSI GPIO_NUM_15
#define PIN_CLK  GPIO_NUM_14
#define PIN_CS   GPIO_NUM_13

#define absolute(fName) (prefix + "/" + fName)

using namespace std;

/**
 * A utility for safely operating on an SD card asynchronously over the SPI
 * interface.
 */
class SDFileIO {
 private:
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
 public:
  /**
   * Explicit value constructor to specify the mount point of the SD card.
   *
   * @param prefix the SD card mount point
   */
  explicit SDFileIO(const char *prefix = "/sdcard")
	  : prefix(prefix),
		host(SDSPI_HOST_DEFAULT()),
		slot_config(SDSPI_SLOT_CONFIG_DEFAULT()),
		card(nullptr),
		ready(false) {
	// set the SD card SPI pins
	slot_config.gpio_miso = PIN_MISO;
	slot_config.gpio_mosi = PIN_MOSI;
	slot_config.gpio_sck = PIN_CLK;
	slot_config.gpio_cs = PIN_CS;
  }
  /**
   * Destructor. Unmounts the SD card and disables the driver since it's no
   * longer needed.
   */
  ~SDFileIO() {
	// unmount SD card and disable vfs driver
	esp_vfs_fat_sdmmc_unmount();
  }
  /**
   * Checks if the specified file exists on the SD card.
   *
   * @param fileName the name of the file
   * @return whether the file exists
   */
  bool exists(const string &fileName) {
	// obtains detailed file info, but this only cares if it found anything
	struct stat st;
	return stat(absolute(fileName).c_str(), &st) == 0;
  }
  bool isReady() {
	return ready;
  }
  /**
   * Mounts the SD card and virtual file systems to be accessed.
   *
   * @return whether mount succeeded
   */
  bool mount() {
	// set up the mount configuration
	// TODO allow more mount customization
	esp_vfs_fat_sdmmc_mount_config_t mntCfg = {
		.format_if_mount_failed = true,
		.max_files = 10,
		.allocation_unit_size = 16 * 1024
	};
	// mount the SD card and obtain its information
	ready = (esp_vfs_fat_sdmmc_mount(
		prefix.c_str(),
		&host, &slot_config,
		&mntCfg, &card) == ESP_OK);
	return ready;
  }
  /**
   * Prints information about the SD card.
   */
  void printCardInfo() {
	// prints SD info to standard out
	sdmmc_card_print_info(stdout, card);
  }
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
  template<class FStream>
  inline bool openFile(const string &fileName,
					   ios_base::openmode mode,
					   std::function<bool(FStream &)> &&fileOp) {
	// creates a stream and opens the file
	FStream file(absolute(fileName), mode);
	// if the file isn't open, there's nothing to do
	if (!file.is_open())
	  return false;
	// perform the file operations
	bool res = fileOp(file);
	// close the file to sync any modifications
	file.close();
	return res;
  }
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
  bool writeFile(const string &fileName, const vector<string> &data) {
	// opens the file with out and truncate modes
	return openFile<ofstream>(
		fileName, ios_base::out | ios_base::trunc,
		[&data](ofstream &f) {
		  // the data is a single row in a CSV, although multiple rows can be
		  // added manually
		  for (const string &col : data) {
			// ensure the ofstream is good
			if (!f) break;
			f << col << ",";
		  }
		  // create a new line for the next row
		  if (f) f << endl;
		  return true;
		});
  }
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
  bool writeBinFile(const string &fileName, const string &data) {
	// opens the file with out, truncate and binary modes
	return openFile<ofstream>(
		fileName, ios_base::out | ios_base::trunc | ios_base::binary,
		[&data](ofstream &f) {
		  // write the binary data without formatting or character encoding
		  f.write(data.c_str(), data.length());
		  return true;
		});
  }
  /**
   * Appends the provided data to the file specified by file name.
   *
   * @tparam modes file open modes, excl. ios::out | ios::app
   * @param fileName the name of the file
   * @param data the data to append
   * @return whether the data was successfully written
   */
  bool appendFile(const string &fileName, const vector<string> &data) {
	// opens the file with out and append modes
	return openFile<ofstream>(
		fileName, ios_base::out | ios_base::app,
		[&data](ofstream &f) {
		  // the data is a single row in a CSV, although multiple rows can be
		  // added manually
		  for (const string &col : data) {
			// ensure the ofstream is good
			if (!f) break;
			f << col << ',';
		  }
		  // create a new line for the next row
		  if (f) f << endl;
		  return true;
		});
  }
  /**
   * Appends the provided binary data to the file specified by file name.
   *
   * @tparam modes file open modes, excl. ios::out | ios::app | ios::binary
   * @param fileName the name of the file
   * @param data the data to write
   * @return whether the data was successfully written
   */
  bool appendBinFile(const string &fileName, const string &data) {
	// opens the file with out, truncate and binary modes
	return openFile<ofstream>(
		fileName, ios_base::out | ios_base::app | ios_base::binary,
		[&data](ofstream &f) {
		  // write the binary data without formatting or character encoding
		  f.write(data.c_str(), data.length());
		  return true;
		});
  }
  /**
   * Reads content from the specified file into the data parameter.
   *
   * @tparam modes file open modes, excl. ios::in | ios::ate
   * @param fileName the name of the file
   * @param data the destination
   * @return whether the data was successfully read
   */
  bool readFile(const string &fileName, string &data) {
	// clear in case the passed reference is being reused (may change later)
	if (data.size() > 0)
	  data.clear();
	// opens the file for reading starting at the end
	// slightly inefficient, but there doesn't seem to be any other way to
	// check file size
	return openFile<ifstream>(
		fileName, ios_base::in | ios_base::ate,
		[&data](ifstream &f) {
		  // reserve space for the entire file so it doesn't reallocate
		  // multiple times while reading
		  data.reserve(f.tellg());
		  // move the get pointer to the beginning
		  f.seekg(0, ios::beg);

		  // append everything to the data reference
		  string line;
		  while (getline(f, line))
			data.append(line + '\n');

		  return true;
		});
  }
  /**
   * Reads binary content from the specified file into the data parameter.
   *
   * @tparam modes file open modes, excl. ios::in | ios::ate | ios::binary
   * @param fileName the name of the file
   * @param data the destination
   * @return whether the data was successfully read
   */
  bool readBinFile(const string &fileName, string &data) {
	// clear in case the passed reference is being reused (may change later)
	if (data.size() > 0)
	  data.clear();
	// opens the file for reading starting at the end
	// slightly inefficient, but there doesn't seem to be a better way to
	// check file size
	return openFile<ifstream>(
		fileName, ios_base::in | ios_base::ate | ios_base::binary,
		[&data](ifstream &f) {
		  // create a temporary buffer to hold binary data
		  streamsize size = f.tellg();
		  char *buf = new char[size];
		  // move the get pointer to the beginning
		  f.seekg(0);
		  // read the binary data without formatting or character encoding
		  f.read(buf, size);
		  // copy's the content from the buffer
		  data = buf;
		  // memory leaks are bad
		  delete[] buf;
		  return true;
		});
  }
};

#endif // REDCROSS_LIB_IO_SRC_SDFILEIO_H_
