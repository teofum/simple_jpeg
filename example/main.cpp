#include <simple_jpeg.hpp>

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
