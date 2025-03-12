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

/*
 * Per-channel pixel format of the input buffer
 * Bit depth for writing is always 8bpc. Integer values will be truncated to 8 bits,
 * and floating point values quantized.
 */
enum class PixelFormat {
  Uint8,
  Uint16,
  Float16,
  Uint32,
  Float32,
  Uint64,
  Float64,
};

/*
 * Parameters for encoding a JPEG image
 */
struct EncodeParams {
  // Width of the image in pixels
  uint32_t width;
  // Height of the image in pixels
  uint32_t height;

  /*
   * Input color mode
   * Currently only RGB and grayscale are supported. JPEG does not support
   * transparency: for RGBA buffers, use RGB mode and inChannels = 4.
   */
  ColorMode colorMode = ColorMode::RGB;

  /*
   * Pixel format of the input data
   */
  PixelFormat pixelFormat = PixelFormat::Uint8;

  /*
   * Number of channels in input data (-1 = auto)
   * Auto: three channels are assumed for RGB and one for grayscale.
   */
  int32_t inChannels = -1;

  /*
   * Channel offset when reading pixel data
   * Can be used to read channels other than the first, for example, write a
   * grayscale image from the alpha channel of a RGBA buffer
   */
  uint32_t inChannelOffset = 0;

  /*
   * Pixel stride, in bytes (-1 = auto)
   * Auto: inChannels * <format size>
   */
  int32_t inPixelStride = -1;

  /*
   * Pixel (horizontal) offset
   * Offset each row by this many pixels. Use in combination with width and
   * inRowStride to read a horizontal slice of a buffer.
   */
  uint32_t inPixelOffset = 0;

  /*
   * Row stride, in bytes (-1 = auto)
   * Auto: inPixelStride * width
   */
  int32_t inRowStride = -1;

  /*
   * Row (vertical) offset
   * Offset the first read by this many rows. Use in combination with height to
   * read a vertical slice of a buffer.
   */
  uint32_t inRowOffset = 0;

  /*
   * Output file path
   */
  fs::path outPath = fs::current_path() / "out.jpeg";

  [[nodiscard]] constexpr size_t channelStride() const {
    switch (pixelFormat) {
      case PixelFormat::Uint8: return 1;
      case PixelFormat::Uint16:
      case PixelFormat::Float16: return 2;
      case PixelFormat::Uint32:
      case PixelFormat::Float32: return 3;
      case PixelFormat::Uint64:
      case PixelFormat::Float64: return 4;
    }
  }
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
