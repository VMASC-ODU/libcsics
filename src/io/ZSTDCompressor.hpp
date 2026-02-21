#pragma once
#include <csics/io/compression/Compressor.hpp>

namespace csics::io::compression {

class ZSTDCompressor : public ICompressor {
   public:
    explicit ZSTDCompressor();
    ~ZSTDCompressor();
    CompressionResult compress_partial(BufferView in,
                                       MutableBufferView out) override;
    CompressionResult compress_buffer(BufferView in,
                                      MutableBufferView out) override;
    CompressionResult finish(BufferView in, MutableBufferView out) override;

   private:
    void* stream_;
};
};  // namespace csics::io::compression
