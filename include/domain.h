#ifndef DOMAIN_H
#define DOMAIN_H

#include <string>
#include <vector>
#include <string_view>
#include <unordered_set>
#include "../include/geo.h"

namespace transport {

    namespace domain {

        ///bus_wait_time — время ожидания автобуса на остановке, в минутах.
        /// Считайте, что когда бы человек ни пришёл на остановку и какой бы ни была эта остановка,
        /// он будет ждать любой автобус в точности указанное количество минут. Значение — целое число от 1 до 1000.

        ///bus_velocity — скорость автобуса, в км/ч. Считайте, что скорость любого автобуса постоянна и в точности равна указанному числу.
        /// Время стоянки на остановках не учитывается, время разгона и торможения тоже. Значение — вещественное число от 1 до 1000.

        struct RoutingSettings{
            size_t bus_wait_time;
            double  bus_velocity;
        };

        struct Stop{
            std::string name;
            geo::Coordinates coordinate;
        };

        bool operator==(const Stop &lhs, const Stop &rhs);

        enum class RouteType {
            UNKNOWN,
            LINEAR,
            CIRCLE,
        };

        struct RouteInfo {
            std::string name;
            RouteType route_type;
            size_t stop_count = 0;
            size_t unique_stop_count = 0;
            double route_length_geo = 0;
            double route_length = 0;
            double curvature = 0.0;
        };

        struct Route{
            std::string name;
            RouteType route_type = RouteType::UNKNOWN;
            std::vector<const Stop*> stops; // Остановки маршрута
        };

            bool operator==(const Route &lhs, const Route &rhs);

        struct StopsHasher{
            size_t operator() (const Stop* lhs, const Stop* rhs) const{
                size_t lhs_link = hasher(lhs);
                size_t rhs_link = hasher(rhs);

                return lhs_link + rhs_link * 37;
            }

            size_t operator() (std::pair<const Stop*, const Stop*> pair) const{
                return operator()(pair.first, pair.second);
            }
        private:
            std::hash<const void*> hasher;
        };

        struct StopHasher{
            size_t operator() (const Stop* stop) const{
                return hasher(stop);
            }
        private:
            std::hash<const void*> hasher;
        };
    }
}

#endif // DOMAIN_H
