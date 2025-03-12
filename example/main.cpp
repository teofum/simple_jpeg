#include <simple_jpeg.hpp>

int main() {
  jpeg::Encoder enc;

  constexpr uint32_t size = 1024;

  uint8_t data[size * size * 3];
  for (uint32_t i = 0; i < size; i++) {
    for (uint32_t j = 0; j < size; j++) {
      data[(i * size + j) * 3 + 0] = j % 256;
      data[(i * size + j) * 3 + 1] = i % 256;
      data[(i * size + j) * 3 + 2] = 255;
    }
  }

  enc.encode(
    &data, {
      .width = size,
      .height = size,
    }
  );
}
