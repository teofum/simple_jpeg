#include "simple_jpeg.hpp"

#define min(x, y) ((x) < (y)) ? (x) : (y)

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
   * Calculate parameters
   */
  uint32_t nChannels = params.inChannels == -1
                       ? m_cinfo.input_components
                       : params.inChannels;
  uint32_t channelStride = params.channelStride();
  uint32_t pixelStride = params.inPixelStride == -1
                         ? channelStride * nChannels
                         : params.inPixelStride;
  uint32_t rowStride = params.inRowStride == -1
                       ? pixelStride * params.width
                       : params.inRowStride;

  /*
   * Write JPEG data
   */
  jpeg_start_compress(&m_cinfo, true);

  std::vector<uint8_t> rowBuffer(sizeof(uint8_t) * m_cinfo.input_components * params.width);
  uint8_t* rowBufferRaw = rowBuffer.data();

  while (m_cinfo.next_scanline < m_cinfo.image_height) {
    /*
     * Copy a scanline's worth of data from the input buffer to the internal row
     * buffer, adapting to the expected 8bpc integer format
     */
    size_t scanlineOffset = rowStride * (m_cinfo.next_scanline + params.inRowOffset);
    for (size_t iPixel = 0; iPixel < params.width; iPixel++) {
      size_t pixelOffset = pixelStride * (iPixel + params.inPixelOffset);
      for (size_t iChannel = 0; iChannel < m_cinfo.input_components; iChannel++) {
        size_t channelOffset = channelStride * (iChannel + params.inChannelOffset);

        size_t offset = scanlineOffset + pixelOffset + channelOffset;
        size_t iWrite = iPixel * m_cinfo.input_components + iChannel;
        switch (params.pixelFormat) {
          case PixelFormat::Uint8: {
            // This is the simplest operation: just copy the data per channel
            rowBufferRaw[iWrite] = ((uint8_t*) data)[offset];
            break;
          }
          case PixelFormat::Uint16: {
            // For >8bit integer formats, simply dump the lower bits
            // TODO: possibly add dithering support?
            rowBufferRaw[iWrite] = uint8_t(((uint16_t*) data)[offset / 2] >> 8);
            break;
          }
          case PixelFormat::Uint32: {
            rowBufferRaw[iWrite] = uint8_t(((uint32_t*) data)[offset / 4] >> 24);
            break;
          }
          case PixelFormat::Uint64: {
            rowBufferRaw[iWrite] = uint8_t(((uint64_t*) data)[offset / 8] >> 56);
            break;
          }
          case PixelFormat::Float16: {
            break; // TODO handle half-precision floating point
          }
          case PixelFormat::Float32: {
            float v = ((float*) data)[offset / 4];
            rowBufferRaw[iWrite] = uint8_t(min(v * 256, 255));
            break;
          }
          case PixelFormat::Float64: {
            double v = ((double*) data)[offset / 8];
            rowBufferRaw[iWrite] = uint8_t(min(v * 256, 255));
            break;
          }
        }
      }
    }

    jpeg_write_scanlines(&m_cinfo, &rowBufferRaw, 1);
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
