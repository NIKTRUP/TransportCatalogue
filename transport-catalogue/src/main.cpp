#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>

using namespace std::literals;
using namespace svg;

namespace shapes {

Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays) {
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}


class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

private:
    svg::Point p1_, p2_, p3_;
};

class Star final : public svg::Drawable{
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays):
        center_(center),
        outer_rad_(outer_rad),
        inner_rad_(inner_rad),
        num_rays_(num_rays)
    {}

    void Draw(svg::ObjectContainer& container) const override {
        container.Add(CreateStar(center_, outer_rad_, inner_rad_, num_rays_).
                      SetFillColor("red").
                      SetStrokeColor("black"));
    }

private:

    svg::Point center_;
    double outer_rad_;
    double inner_rad_;
    int num_rays_;
};
class Snowman final: public svg::Drawable {
public:
    Snowman(Point center_head, double radius_head):
        center_head_(center_head), radius_head_(radius_head)
    {}

    void Draw(svg::ObjectContainer& container) const override {
        auto head = Circle().SetCenter(center_head_).SetRadius(radius_head_);
        auto torso = Circle().SetCenter({center_head_.x, center_head_.y + 2*radius_head_ }).SetRadius(radius_head_*1.5);
        auto footing = Circle().SetCenter({center_head_.x, center_head_.y + 5*radius_head_ }).SetRadius(radius_head_*2);
        container.Add(footing.SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(torso.SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(head.SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    }
private:
    Point center_head_;
    double radius_head_;
};

} // namespace shapes

template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

// Выполняет линейную интерполяцию значения от from до to в зависимости от параметра t.
uint8_t Lerp(uint8_t from, uint8_t to, double t) {
    return static_cast<uint8_t>(std::round((to - from) * t + from));
}

// Выполняет линейную интерполяцию Rgb цвета от from до to в зависимости от параметра t
svg::Rgb Lerp(svg::Rgb from, svg::Rgb to, double t) {
    return {Lerp(from.red, to.red, t), Lerp(from.green, to.green, t), Lerp(from.blue, to.blue, t)};
}

int main() {
    using namespace svg;
    using namespace std;

    Rgb start_color{0, 255, 30};
    Rgb end_color{20, 20, 150};

    const int num_circles = 10;
    Document doc;
    for (int i = 0; i < num_circles; ++i) {
        const double t = double(i) / (num_circles - 1);

        const Rgb fill_color = Lerp(start_color, end_color, t);

        doc.Add(Circle()
                    .SetFillColor(fill_color)
                    .SetStrokeColor("black"s)
                    .SetCenter({i * 20.0 + 40, 40.0})
                    .SetRadius(15));
    }
    doc.Render(cout);
}
