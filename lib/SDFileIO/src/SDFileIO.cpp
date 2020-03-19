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

bool SDFileIO::exists(const string& fileName) {
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

void SDFileIO::printCardInfo() {
  // prints SD info to standard out (not sure if this works with Arduino)
  sdmmc_card_print_info(stdout, card);
}

template<class FStream>
bool SDFileIO::openFile(const string& fileName,
                        ios_base::openmode mode,
                        std::function<bool(FStream&)>&& fileOp) {
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

bool SDFileIO::appendFile(const string& fileName,
                          const vector<string>& data) {
  // opens the file with out and append modes
  return openFile<ofstream>(
      fileName, ios_base::out | ios_base::app,
      [&data](ofstream& f) {
        // the data is a single row in a CSV, although multiple rows can be
        // added manually
        for (const string& col : data) {
          // ensure the ofstream is good
          if (!f) break;
          f << col << ',';
        }
        // create a new line for the next row
        if (f) f << endl;
        return true;
      });
}

bool SDFileIO::appendBinFile(const string& fileName, const string& data) {
  // opens the file with out, truncate and binary modes
  return openFile<ofstream>(
      fileName, ios_base::out | ios_base::app | ios_base::binary,
      [&data](ofstream& f) {
        // write the binary data without formatting or character encoding
        f.write(data.c_str(), data.length());
        return true;
      });
}

bool SDFileIO::readFile(const string& fileName, string& data) {
  // clear in case the passed reference is being reused (may change later)
  if (data.size() > 0)
    data.clear();
  // opens the file for reading starting at the end
  // slightly inefficient, but there doesn't seem to be any other way to
  // check file size
  return openFile<ifstream>(
      fileName, ios_base::in | ios_base::ate,
      [&data](ifstream& f) {
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

bool SDFileIO::readBinFile(const string& fileName, string& data) {
  // clear in case the passed reference is being reused (may change later)
  if (data.size() > 0)
    data.clear();
  // opens the file for reading starting at the end
  // slightly inefficient, but there doesn't seem to be a better way to
  // check file size
  return openFile<ifstream>(
      fileName, ios_base::in | ios_base::ate | ios_base::binary,
      [&data](ifstream& f) {
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

bool SDFileIO::writeFile(const string& fileName,
                         const vector<string>& data) {
  // opens the file with out and truncate modes
  return openFile<ofstream>(
      fileName, ios_base::out | ios_base::trunc,
      [&data](ofstream& f) {
        // the data is a single row in a CSV, although multiple rows can be
        // added manually
        for (const string& col : data) {
          // ensure the ofstream is good
          if (!f) break;
          f << col << ",";
        }
        // create a new line for the next row
        if (f) f << endl;
        return true;
      });
}

bool SDFileIO::writeBinFile(const string& fileName, const string& data) {
  // opens the file with out, truncate and binary modes
  return openFile<ofstream>(
      fileName, ios_base::out | ios_base::trunc | ios_base::binary,
      [&data](ofstream& f) {
        // write the binary data without formatting or character encoding
        f.write(data.c_str(), data.length());
        return true;
      });
}
