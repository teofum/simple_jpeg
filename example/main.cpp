#include <simple_jpeg.hpp>

// IEEE-754 half-precision float to 32-bit float conversion
// https://stackoverflow.com/a/60047308
static uint32_t as_uint(const float x) { return *(uint32_t*) &x; }
uint16_t float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
  const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
  const uint32_t e = (b & 0x7F800000) >> 23; // exponent
  const uint32_t m = b &
                     0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
  return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) |
         ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) |
         (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

int main() {
  jpeg::Encoder enc;

  constexpr uint32_t size = 1024;

  std::vector<uint8_t> data(size * size * 3);
  for (uint32_t i = 0; i < size; i++) {
    for (uint32_t j = 0; j < size; j++) {
      data[(i * size + j) * 3 + 0] = j % 256;
      data[(i * size + j) * 3 + 1] = i % 256;
      data[(i * size + j) * 3 + 2] = 128;
    }
  }

  enc.encode(
    data.data(), {
      .width = size,
      .height = size,
    }
  );

  // Integer pixel format
  std::vector<uint16_t> data16(size * size * 3);
  for (uint32_t i = 0; i < size; i++) {
    for (uint32_t j = 0; j < size; j++) {
      data16[(i * size + j) * 3 + 0] = j * 64;
      data16[(i * size + j) * 3 + 1] = i * 64;
      data16[(i * size + j) * 3 + 2] = 1 << 15;
    }
  }

  enc.encode(
    data16.data(), {
      .width = size,
      .height = size,
      .pixelFormat = jpeg::PixelFormat::Uint16,
      .outPath = fs::current_path() / "out_16.jpeg",
    }
  );

  // Float pixel format
  std::vector<float> dataF(size * size * 3);
  for (uint32_t i = 0; i < size; i++) {
    for (uint32_t j = 0; j < size; j++) {
      dataF[(i * size + j) * 3 + 0] = float(j) / size;
      dataF[(i * size + j) * 3 + 1] = float(i) / size;
      dataF[(i * size + j) * 3 + 2] = 0.5f;
    }
  }

  enc.encode(
    dataF.data(), {
      .width = size,
      .height = size,
      .pixelFormat = jpeg::PixelFormat::Float32,
      .outPath = fs::current_path() / "out_f32.jpeg",
    }
  );

  // Half precision float pixel format
  std::vector<uint16_t> dataF16(size * size * 3);
  for (uint32_t i = 0; i < size; i++) {
    for (uint32_t j = 0; j < size; j++) {
      dataF16[(i * size + j) * 3 + 0] = float_to_half(float(j) / size);
      dataF16[(i * size + j) * 3 + 1] = float_to_half(float(i) / size);
      dataF16[(i * size + j) * 3 + 2] = float_to_half(0.5f);
    }
  }

  enc.encode(
    dataF16.data(), {
      .width = size,
      .height = size,
      .pixelFormat = jpeg::PixelFormat::Float16,
      .outPath = fs::current_path() / "out_f16.jpeg",
    }
  );

  // Buffer slice
  enc.encode(
    data.data(), {
      .width = size / 2,
      .height = size / 2,
      .inPixelOffset = 128,
      .inRowStride = sizeof(uint8_t) * 3 * size,
      .inRowOffset = 128,
      .outPath = fs::current_path() / "out_slice.jpeg",
    }
  );

  enc.encode(
    data.data(), {
      .width = size,
      .height = size,
      .colorMode = jpeg::ColorMode::Grayscale,
      .inChannels = 3,
      .inChannelOffset = 1,
      .outPath = fs::current_path() / "out_gray.jpeg",
    }
  );
}
