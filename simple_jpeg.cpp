#include "simple_jpeg.hpp"

jpeg::Encoder::Encoder() noexcept {
  m_cinfo.err = jpeg_std_error(&m_jerr);
  jpeg_create_compress(&m_cinfo);
}

jpeg::Encoder::~Encoder() {
  jpeg_destroy_compress(&m_cinfo);
}

jpeg::Encoder::Encoder(jpeg::Encoder&& enc) noexcept {
  m_cinfo = enc.m_cinfo;
  m_jerr = enc.m_jerr;
}

jpeg::Encoder& jpeg::Encoder::operator=(jpeg::Encoder&& enc) noexcept {
  m_cinfo = enc.m_cinfo;
  m_jerr = enc.m_jerr;

  return *this;
}

void jpeg::Encoder::encode(void* data, const EncodeParams& params) {
  // Set up a simple buffer
  // TODO use a writer object
  unsigned char* buf = nullptr;
  unsigned long size = 0;
  jpeg_mem_dest(&m_cinfo, &buf, &size);

  /*
   * Configure the compression object with image parameters
   */
  m_cinfo.image_width = params.width;
  m_cinfo.image_height = params.height;

  switch (params.colorMode) {
    case ColorMode::RGB: {
      m_cinfo.input_components = 3;
      m_cinfo.in_color_space = JCS_RGB;
      break;
    }
    case ColorMode::Grayscale: {
      m_cinfo.input_components = 1;
      m_cinfo.in_color_space = JCS_GRAYSCALE;
      break;
    }
  }

  /*
   * Set default settings for libjpeg. This needs to be done *after* setting the
   * color mode ("colorspace" in libjpeg terms).
   */
  jpeg_set_defaults(&m_cinfo);

  /*
   * Write JPEG data
   */
  jpeg_start_compress(&m_cinfo, true);

  JSAMPROW rowPtr[1];
  constexpr size_t stride = 3 * sizeof(uint8_t);
  while (m_cinfo.next_scanline < m_cinfo.image_height) {
    size_t offset = stride * params.width * m_cinfo.next_scanline;
    rowPtr[0] = reinterpret_cast<uint8_t*>(data) + offset;

    jpeg_write_scanlines(&m_cinfo, rowPtr, 1);
  }

  jpeg_finish_compress(&m_cinfo);

  // Write to file
  // TODO use a writer object
  auto file = std::ofstream(params.outPath, std::ios_base::out | std::ios_base::binary);
  file.write((char*) buf, std::streamsize(size));

  // Release resources
  free(buf);
  file.close();
}
