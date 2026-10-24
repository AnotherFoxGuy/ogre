/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreSDLImageCodec.h"
#include "OgreLogManager.h"
#include "OgreDataStream.h"
#include "OgreImage.h"

#include "OgrePlatformInformation.h"
#include "OgreString.h"
#include "OgreRoot.h"

#include <vector>

#include <SDL.h>
#include <SDL_image.h>

namespace Ogre {

    namespace
    {
        void appendToVector(void* context, void* data, int size)
        {
            auto* out = static_cast<std::vector<uchar>*>(context);
            auto* bytes = static_cast<uchar*>(data);
            out->insert(out->end(), bytes, bytes + size);
        }
    }

    SDLImageCodec::RegisteredCodecList SDLImageCodec::msCodecList;
    //---------------------------------------------------------------------
    void SDLImageCodec::startup(void)
    {
        // Register codecs
        String exts = "jpeg,jpg,png,bmp,psd,tga,gif,pic,ppm,pgm,hdr";
        StringVector extsVector = StringUtil::split(exts, ",");
        for (auto & v : extsVector)
        {
            ImageCodec* codec = OGRE_NEW SDLImageCodec(v);
            msCodecList.push_back(codec);
            Codec::registerCodec(codec);
        }

        LogManager::getSingleton().logMessage("Supported formats: " + exts);
        // IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    }
    //---------------------------------------------------------------------
    void SDLImageCodec::shutdown(void)
    {
        for (auto & i : msCodecList)
        {
            Codec::unregisterCodec(i);
            OGRE_DELETE i;
        }
        msCodecList.clear();
    }
    //---------------------------------------------------------------------
    SDLImageCodec::SDLImageCodec(const String &type):
        mType(type)
    { 
    }
    //---------------------------------------------------------------------
    DataStreamPtr SDLImageCodec::encode(const Any& input) const
    {
        return nullptr;
    }
    //---------------------------------------------------------------------
    void SDLImageCodec::encodeToFile(const Any& input, const String& outFileName) const
    {

    }
    //---------------------------------------------------------------------
    void SDLImageCodec::decode(const DataStreamPtr& input, const Any& output) const
    {
        LogManager::getSingleton().logMessage("AAAAAAAAAAAAA " + input->getName());
        MemoryDataStreamPtr mem(new MemoryDataStream(input));
        SDL_RWops* rw = SDL_RWFromConstMem(mem->getPtr(), static_cast<int>(mem->size()));
        SDL_Surface *surface = IMG_Load_RW(rw, 0);

        if (!surface) {
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, SDL_GetError());
            return;
        }

        const auto image = any_cast<Image*>(output);
        // Normalize to a known byte-order RGBA layout, whatever the source format was
        SDL_Surface* conv = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        if (!conv)
           OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, SDL_GetError());

        const int w = conv->w;
        const int h = conv->h;
        const size_t rowBytes = size_t(w) * 4;

        // Copy row by row, since SDL's pitch may include padding
        MemoryDataStreamPtr stream(new MemoryDataStream(rowBytes * h));
        SDL_LockSurface(conv);
        for (int y = 0; y < h; ++y)
        {
            memcpy(stream->getPtr() + y * rowBytes,
                   static_cast<uint8_t*>(conv->pixels) + y * conv->pitch,
                   rowBytes);
        }

        image->loadRawData(stream, surface->w, surface->h, 1, PF_BYTE_RGBA);
        SDL_UnlockSurface(conv);
        SDL_FreeSurface(conv);

        SDL_UnlockSurface(surface);
        SDL_FreeSurface(surface);
    }
    //---------------------------------------------------------------------    
    String SDLImageCodec::getType() const
    {
        return mType;
    }

#ifndef OGRE_STATIC_LIB
    extern "C" void _OgreSTBICodecExport dllStartPlugin();
    extern "C" void _OgreSTBICodecExport dllStopPlugin();

    extern "C" void _OgreSTBICodecExport dllStartPlugin()
    {
        SDLImageCodec::startup();
    }
    extern "C" void _OgreSTBICodecExport dllStopPlugin()
    {
        SDLImageCodec::shutdown();
    }
#endif
}
