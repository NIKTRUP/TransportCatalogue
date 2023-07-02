#ifndef GEO_H
#define GEO_H
#define _USE_MATH_DEFINES

#include <cmath>

namespace transport {
    namespace geo {

        struct Coordinates {
            double lat;
            double lng;
            bool operator==(const Coordinates& other) const {
                return lat == other.lat && lng == other.lng;
            }
            bool operator!=(const Coordinates& other) const {
                return !(*this == other);
            }
        };

        double ComputeDistance(Coordinates from, Coordinates to);
    }
}
#endif // GEO_H
