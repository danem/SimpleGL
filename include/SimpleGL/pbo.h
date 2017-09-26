#ifndef PBO_H
#define PBO_H

#include <SimpleGL/sglconfig.h>
#include <SimpleGL/resource.h>
#include <SimpleGL/texture.h>

namespace sgl {

class PBOUploader {
private:
    sgl::Texture2D& _destTexture;
    sgl::GLResourceArray<GL_PIXEL_UNPACK_BUFFER> _pbos;
    size_t _delay;
    size_t _pboIdx;
    size_t _pboIdxNext;
    size_t _frameSize;

public:

    PBOUploader (sgl::Texture2D& texture, size_t delay = 2) :
        _destTexture(texture),
        _pbos(delay),
        _delay(delay),
        _pboIdx(0),
        _pboIdxNext(1),
        _frameSize(texture.attrs.size())
    {
        for (size_t i = 0; i < _delay; i++){
            sgl::bufferData(_pbos[i], static_cast<const char*>(0), _frameSize, GL_STREAM_DRAW);
        }
        sglDbgCatchGLError();
    }

    ~PBOUploader () {
        _pbos.release();
    }

    template <class T, size_t len>
    void update (std::array<T,len>& data) {
        update(&data[0], len);
    }

    template <class T>
    void update (std::vector<T>& data) {
        update(&data[0], data.size());
    }

    template <class T>
    void update (T * data, size_t count) {
        _pboIdx = (_pboIdx + 1) % _delay;
        _pboIdxNext = (_pboIdxNext + 1 ) % _delay;

        auto pbo1 = _pbos[_pboIdx];
        auto pbo2 = _pbos[_pboIdxNext];

        _destTexture.bind();
        pbo1.bind();

        glTexSubImage2D(_destTexture.type, 0, 0, 0, _destTexture.attrs.width, _destTexture.attrs.height, _destTexture.attrs.format, _destTexture.attrs.data_type, 0);

        sglDbgCatchGLError();

        pbo2.bind();
        void * dest = glMapBufferRange(pbo2.type, 0, sizeof(T) * count, GL_MAP_WRITE_BIT);
        memcpy(dest, data, sizeof(T) * count);
        glUnmapBuffer(pbo2.type);
        pbo2.unbind();
        sglDbgCatchGLError();
    }
};



} // end namespace

#endif // PBO_H
