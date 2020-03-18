/**
 * @file SdFileIo.cpp
 *
 * @brief SDFileIO class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "SDFileIO.h"

SDFileIO::SDFileIO(const char* prefix)
    : prefix(prefix),
      host(SDSPI_HOST_DEFAULT()),
      slot_config(SDSPI_SLOT_CONFIG_DEFAULT()),
      ready(false) {
  // set the SD card SPI pins
  slot_config.gpio_miso = PIN_MISO;
  slot_config.gpio_mosi = PIN_MOSI;
  slot_config.gpio_sck = PIN_CLK;
  slot_config.gpio_cs = PIN_CS;
}

SDFileIO::~SDFileIO() {
  // unmount SD card and disable vfs driver
  esp_vfs_fat_sdmmc_unmount();
}

bool SDFileIO::exists(const char* fileName) {
  // obtains detailed file info, but this only cares if it found anything
  struct stat st;
  return stat(absolute(fileName).c_str(), &st) == 0;
}

bool SDFileIO::isReady() {
  return ready;
}

bool SDFileIO::mount() {
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

template<const ios_base::openmode& mode,
    const ios_base::openmode& ... modes>
ios::openmode SDFileIO::orModes() {
  // without this, it won't compile
  if (sizeof...(modes) == 0)
    return mode;
  // perform bitwise OR on all open modes
  return mode | orModes<modes...>();
}

void SDFileIO::printCardInfo() {
  // prints SD info to standard out (not sure if this works with Arduino)
  sdmmc_card_print_info(stdout, card);
}

template<typename FStream, const ios_base::openmode& mode, const ios_base::openmode& ... modes>
bool SDFileIO::openFile(const char* fileName, bool (* fileOp)(FStream&)) {
  // creates a stream and opens the file
  FStream file(absolute(fileName), orModes<mode, modes...>());
  // if the file isn't open, there's nothing to do
  if (!file.is_open())
    return false;
  // perform the file operations
  bool res = fileOp(file);
  // close the file to sync any modifications
  file.close();
  return res;
}

template<const ios_base::openmode... modes>
bool SDFileIO::appendFile(const char* fileName,
                          const vector<const char*>& data) {
  // opens the file with out and append modes
  return openFile<ofstream, ios::out, ios::app, modes...>(
      fileName, [&data](ofstream& f) {
        // the data is a single row in a CSV, although multiple rows can be
        // added manually
        for (const char* col : data) {
          // ensure the ofstream is good
          if (f.bad() || f.fail()) return false;
          f << col << ",";
        }
        // create a new line for the next row
        if (!f.bad() && !f.fail())
          f << endl;
        return true;
      });
}

template<const ios_base::openmode... modes>
bool SDFileIO::appendBinFile(const char* fileName, const string& data) {
  // opens the file with out, truncate and binary modes
  return openFile<ofstream, ios::out, ios::app, ios::binary, modes...>(
      fileName, [&data](ofstream& f) {
        // write the binary data without formatting or character encoding
        f.write(data.c_str(), data.length());
        return true;
      });
}

template<const ios_base::openmode... modes>
bool SDFileIO::readFile(const char* fileName, string& data) {
  // clear in case the passed reference is being reused (may change later)
  if (data.size() > 0)
    data.clear();
  // opens the file for reading starting at the end
  // slightly inefficient, but there doesn't seem to be any other way to
  // check file size
  return openFile<ifstream, ios::in, ios::ate, modes...>(
      fileName, [&data](ifstream& f) {
        // reserve space for the entire file so it doesn't reallocate
        // multiple times while reading
        data.reserve(f.tellg());
        // move the get pointer to the beginning
        f.seekg(0);

        // append everything to the data reference
        while (f.good())
          data += f.get();

        return true;
      });
}

template<const ios_base::openmode... modes>
bool SDFileIO::readBinFile(const char* fileName, string& data) {
  // clear in case the passed reference is being reused (may change later)
  if (data.size() > 0)
    data.clear();
  // opens the file for reading starting at the end
  // slightly inefficient, but there doesn't seem to be a better way to
  // check file size
  return openFile<ifstream, ios::in, ios::ate, ios::binary, modes...>(
      fileName, [&data](ifstream& f) {
        // create a temporary buffer to hold binary data
        streamsize size = f.tellg();
        char* buf = new char[size];
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

template<const ios_base::openmode& ... modes>
bool SDFileIO::writeFile(const char* fileName,
                         const vector<const char*>& data) {
  // opens the file with out and truncate modes
  return openFile<ofstream, ios::out, ios::trunc, modes...>(
      fileName, [&data](ofstream& f) {
        // the data is a single row in a CSV, although multiple rows can be
        // added manually
        for (const char* col : data) {
          // ensure the ofstream is good
          if (f.bad() || f.fail()) return false;
          f << col << ",";
        }
        // create a new line for the next row
        if (!f.bad() && !f.fail())
          f << endl;
        return true;
      });
}

template<const ios_base::openmode... modes>
bool SDFileIO::writeBinFile(const char* fileName, const string& data) {
  // opens the file with out, truncate and binary modes
  return openFile<ofstream, ios::out, ios::trunc, ios::binary, modes...>(
      fileName, [&data](ofstream& f) {
        // write the binary data without formatting or character encoding
        f.write(data.c_str(), data.length());
        return true;
      });
}
