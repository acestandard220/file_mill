#pragma once
#include "pdf_mill_core.h"


namespace PDF_MILL
{

    struct _box
    {
        float _x, _y, _w, _h;

        _box() = default;
        _box(float x, float y, float w, float h)
            : _x(x), _y(y), _w(w), _h(h)
        {
        }

        std::array<float, 4> asArray() { return std::array<float, 4>{_x, _y, _w, _h}; }
    };

    struct media_box :public _box
    {

        media_box() = default;

        media_box(float x, float y, float w, float h)
            :_box(x, y, w, h)
        {
        }
    };

    struct FontBox :public _box
    {
        FontBox() = default;
        FontBox(float x, float y, float w, float h)
            :_box(x, y, w, h)
        {

        }

        FontBox(std::vector<float> xywh)
            :_box(xywh[0], xywh[1], xywh[2], xywh[3])
        {

        }
    };

}