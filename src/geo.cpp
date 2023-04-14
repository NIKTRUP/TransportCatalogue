#include "include/geo.h"

namespace tc {
    namespace geo {

        // Считает расстояние между двумя точками на земной поверхности (Для упрощения считаем, что земля имеет форму сферы)
        double ComputeDistance(Coordinates from, Coordinates to) {
            using namespace std;
            if (from == to) {
                return 0;
            }
            static const double dr = M_PI / 180.;
            return acos(sin(from.lat * dr) * sin(to.lat * dr)
                        + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
                * 6371000;
        }
    }
}
