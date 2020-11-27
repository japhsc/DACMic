/*      Header
        little endian   '<L4s4sLHHLLHH4sL'
        type    byte    bit     value
        4s      4       32      "RIFF"
        L       4       32      headlength
        4s      4       32      "WAVE"
        4s      4       32      "fmt "
        L       4       32      16
        H       2       16      WAVE_FORMAT_PCM
        H       2       16      nchannels
        L       4       32      framerate
        L       4       32      datasize
        H       2       16      width
        H       2       16      depth
        4s      4       32      "data"
        L       4       32      datalength
        Length  40
*/

#ifndef RIFF_WAVE_H
#define  RIFF_WAVE_H

    #include "endian.h"
    #include <cstring>
    
    int wav_header(uint16_t nchannels, uint16_t sampwidth, uint32_t framerate, const size_t &size, uint8_t* &header) {
        const size_t length = 44;
        header = (uint8_t*) malloc(length);
        if (!header) return 0;
        
        uint32_t nframes        = size / (sampwidth * nchannels);
        uint32_t datalength     = nframes * nchannels * sampwidth;
        uint32_t headlength     = 36 + datalength;
        uint32_t fmt            = 16;
        uint16_t format_pcm     = 0x0001;
        uint32_t datasize       = nchannels * framerate * sampwidth;
        uint16_t width          = nchannels * sampwidth;
        uint16_t depth          = sampwidth*8;
        
        headlength  = htole32(headlength);
        fmt         = htole16(fmt);
        format_pcm  = htole16(format_pcm);
        nchannels   = htole16(nchannels);
        framerate   = htole32(framerate);
        datasize    = htole32(datasize);
        width       = htole16(width);
        depth       = htole16(depth);
        datalength  = htole32(datalength);
        
        uint8_t* header_ptr = header;
        memcpy(header_ptr, "RIFF", 4);
        header_ptr+=4;
        memcpy(header_ptr, &headlength, 4);
        header_ptr+=4;
        memcpy(header_ptr, "WAVE", 4);
        header_ptr+=4;
        memcpy(header_ptr, "fmt ", 4);
        header_ptr+=4;
        memcpy(header_ptr, &fmt, 4);
        header_ptr+=4;
        memcpy(header_ptr, &format_pcm, 2);
        header_ptr+=2;
        memcpy(header_ptr, &nchannels, 2);
        header_ptr+=2;
        memcpy(header_ptr, &framerate, 4);
        header_ptr+=4;
        memcpy(header_ptr, &datasize, 4);
        header_ptr+=4;
        memcpy(header_ptr, &width, 2);
        header_ptr+=2;
        memcpy(header_ptr, &depth, 2);
        header_ptr+=2;
        memcpy(header_ptr, "data", 4);
        header_ptr+=4;
        memcpy(header_ptr, &datalength, 4);
        header_ptr+=4;
        return length;
    }
    
    int wav_encode (const uint16_t &nchannels, 
                    const uint16_t &sampwidth, 
                    const uint32_t &framerate,
                    const uint8_t* data, 
                    const size_t &size, 
                    uint8_t* &out) 
    {
        size_t length = 0;
        if (data && size>0) {
            uint8_t* header = NULL;
            size_t headerlength = wav_header(nchannels, sampwidth, framerate, size, header);
            if (headerlength) {
                length = headerlength + size;
                out = (uint8_t*) malloc(length);
                if (out) {
                    memcpy(             out,  header, headerlength);
                    memcpy(out+headerlength,    data,         size);
                } else {
                    length = 0;
                }
                free(header);
            }
        }
        return length;
    }
#endif
