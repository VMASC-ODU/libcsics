#pragma once
#include <csics/io/compression/Compressor.hpp>

namespace csics::io::compression {

class ZSTDCompressor {
   public:
    explicit ZSTDCompressor();
    ~ZSTDCompressor();
    CompressionResult compress_partial(BufferView in, BufferView out);
    CompressionResult compress_buffer(BufferView in, BufferView out);
    CompressionResult finish(BufferView in, BufferView out);

    inline CompressionResult operator()(BufferView in, BufferView out) { return compress_buffer(in,out); }

   private:
    void* stream_;
};
};  // namespace csics::io::compression
