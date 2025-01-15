#include "image-loader.h"
#include <vector>
#include "turbojpeg.h"
#include "file-loader.h"
#include <stdexcept>

G3DHostImage ImageLoader::loadJPEGImage(const char *path, uint32_t byteAlignment)
{
    G3DHostImage image;
    
    image.m_byteAlignment = byteAlignment;

    tjhandle turboJpegHandle = tj3Init(TJINIT_DECOMPRESS);
    
    char* jpegFileData = nullptr;
    uint32_t jpegFileSize = 0U;
    G3DFileLoader::Binary::loadFile(path, &jpegFileData, &jpegFileSize);
    if (jpegFileSize == 0) throw std::runtime_error("Failed to open jpeg image");
    const unsigned char* jpegData = static_cast<const unsigned char*>(static_cast<void*>(jpegFileData));
    
    int pixelFormat = TJPF_RGBA;

    // Retrieve image parameters
    tj3DecompressHeader(turboJpegHandle, jpegData, jpegFileSize);

    // Returns the image height and width in pixels, respectively
    int imageHeight = tj3Get(turboJpegHandle, TJPARAM_JPEGHEIGHT);
    int imageWidth = tj3Get(turboJpegHandle, TJPARAM_JPEGWIDTH);

    image.m_width = imageWidth;
    image.m_height = imageHeight;

    // Returns the number of bits used per sample
    int precision = tj3Get(turboJpegHandle, TJPARAM_PRECISION);

    int samplesPerPixel = tjPixelSize[pixelFormat];

    // Determine the number of padded pixels needed per row to satisfy memory alignment requirements
    int paddingPixelsPerRow = byteAlignment == 0 ? 0 : (imageWidth % byteAlignment);

    // Determine the number of samples per row, aka pitch
    int pitch = (imageWidth + paddingPixelsPerRow) * samplesPerPixel;

    // Create a buffer big enough to hold the image
    int outputBufferSize = imageHeight * pitch;
    image.m_byteSize = outputBufferSize;
    image.m_data = std::make_unique<char[]>(outputBufferSize);

    tj3Decompress8(turboJpegHandle, jpegData, jpegFileSize, static_cast<unsigned char*>(static_cast<void*>(image.m_data.get())), pitch, pixelFormat);
    delete[] jpegFileData;
    return image;
}
