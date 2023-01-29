
#ifndef TRANSPORT_CATALOGUE_H
#define TRANSPORT_CATALOGUE_H

#include <string>
#include <cstdint>
#include <string_view>
#include <deque>
#include <set>
#include <vector>
#include <execution>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include "include/geo.h"

namespace tc {

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
        size_t num_of_stops = 0;
        size_t num_of_unique_stops = 0;
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

    size_t CountStops(const Route *route) noexcept;

    size_t CountUniqueStops(const Route *route) noexcept;

    double CalculateRouteLengthGeo(const Route *route) noexcept;

    // Транспортный справочник
    class TransportCatalogue{

    public:

       void AddStop(Stop stop) noexcept;

       const Stop* FindStop(const std::string& name) const;

       void AddRoute(Route route) noexcept;

       const Route*  FindRoute(const std::string& name) const;

       RouteInfo GetRouteInfo(const std::string& name) const ;

       std::optional<std::set<std::string_view>> GetBussesOnStop(const std::string& name) const;

       void SetDistance(std::pair<const Stop*,const Stop*> pair_stops, size_t distance);

       size_t GetAmountStops() const;

       size_t GetAmountRoutes() const;

       size_t GetStopsDistances(std::pair<const Stop*, const Stop*>) const;
       size_t GetStopsDistances(const Stop*, const Stop*) const;
       size_t GetStopsDistances(const std::string& begin, const std::string& end) const;

       size_t CalculateRouteLength(const Route* route) const;

    private:
       // остановки
       std::deque<Stop> stops_;
       // Формат хранения: имя_остановки - указатель_на_остановку
       std::unordered_map<std::string_view, const Stop*> stops_by_names_;
       // автобусы на каждой остановке. // Формат хранения: имя_остановки - множество_автобусов
       std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stops_;
       // маршруты
       std::deque<Route> routes_;
       // Формат хранения для поиска: имя_маршрута(совпадает с именем автобуса) - указатель на маршрут
       std::unordered_map<std::string_view, const Route*> routes_by_names_;

       // расчёт расстояний между остановками .
       std::unordered_map<std::pair<const Stop*, const Stop*>, size_t, StopsHasher> distance_stops;
    };

}

#endif
