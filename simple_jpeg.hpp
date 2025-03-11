#ifndef SIMPLE_JPEG_SIMPLE_JPEG_HPP
#define SIMPLE_JPEG_SIMPLE_JPEG_HPP

#include <filesystem>
#include <iostream>
#include <fstream>

#include <jpeglib.h>

namespace fs = std::filesystem;

namespace jpeg {

enum class ColorMode {
  RGB,
  Grayscale,
};

struct EncodeParams {
  uint32_t width = 0;
  uint32_t height = 0;

  ColorMode colorMode = ColorMode::RGB;

  fs::path outPath = fs::current_path() / "out.jpeg";
};

class Encoder {
public:
  Encoder() noexcept;
  ~Encoder();

  Encoder(const Encoder& enc) = delete;
  Encoder(Encoder&& enc) noexcept;

  Encoder& operator=(const Encoder& enc) = delete;
  Encoder& operator=(Encoder&& enc) noexcept;

  void encode(void* data, const EncodeParams& params);

private:
  jpeg_compress_struct m_cinfo{};
  jpeg_error_mgr m_jerr{};
};

}

#endif //SIMPLE_JPEG_SIMPLE_JPEG_HPP
