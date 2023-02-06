#include "include/transport_catalogue.h"

using namespace std;

namespace tc {

    using namespace domain;

    size_t CountStops(const Route *route) noexcept {
        size_t result = 0;
        if (route != nullptr) {
            result = route->stops.size();
            if (route->route_type == RouteType::LINEAR) {
                result = result  * 2 - 1;
            }
        }
        return result;
    }

    size_t CountUniqueStops(const Route *route) noexcept {
        size_t result = 0;
        if (route != nullptr) {
            std::unordered_set<std::string_view> uniques;
            for (auto stop : route->stops) {
                uniques.insert(stop->name);
            }
            result = uniques.size();
        }
        return result;
    }

    double CalculateRouteLengthGeo(const Route *route) noexcept {
        double result = 0.0;
        if (route != nullptr) {
            for (auto it_left = route->stops.begin(), it_right = it_left+1;
                 it_right < route->stops.end();
                 ++it_left, ++it_right) {
                result += ComputeDistance((*it_left)->coordinate, (*it_right)->coordinate);
            }
            if (route->route_type == RouteType::LINEAR) {
                result *= 2;
            }
        }
        return result;
    }

    size_t TransportCatalogue::CalculateRouteLength(const Route* route) const {
        size_t result = 0;
        if (route != nullptr) {
            for (auto first = route->stops.begin(), second = first+1; second < route->stops.end(); ++first, ++second) {
                result += GetStopsDistances({*first, *second});
            }
            if (route->route_type == RouteType::LINEAR) {
                for (auto first = route->stops.rbegin(), second = first+1; second < route->stops.rend(); ++first, ++second) {
                    result += GetStopsDistances((*first)->name, (*second)->name);
                }
            }
        }
        return result;
    }

    void TransportCatalogue::AddStop(Stop stop) noexcept{
        stops_.push_back(std::move(stop));
        stops_by_names_.insert({stops_.back().name, &stops_.back()});
    }

    const Stop* TransportCatalogue::FindStop(std::string_view name) const{
        if (stops_by_names_.count(name) == 0) {
            throw std::out_of_range(" Остановка "s + std::string(name) + " не найдена в каталоге "s);
        }
        return stops_by_names_.at(name);
    }

    const Stop* TransportCatalogue::FindStop(const std::string& name) const{
        return FindStop(std::string_view(name));
    }

    void TransportCatalogue::AddRoute(Route route) noexcept{
        // добавляем маршрут в хранилище и в индекс
        routes_.push_back(move(route));
        string_view route_name = routes_.back().name;
        routes_by_names_.insert({route_name, &routes_.back()});
        // добавляем информацию об автобусе в остановки по маршруту
        for (auto stop : routes_.back().stops) {
            buses_on_stops_[stop->name].insert(route_name);
        }
    }

    const domain::Route*  TransportCatalogue::FindRoute(std::string_view name) const{
        if(routes_by_names_.count(name) == 0){
            throw std::out_of_range(" Маршрут "s + std::string(name) + " не найден в каталоге "s);
        }
        return routes_by_names_.at(name);
    }

    const Route* TransportCatalogue::FindRoute(const std::string& name) const{
        return FindRoute(std::string_view(name));
    }

    RouteInfo TransportCatalogue::GetRouteInfo(const std::string& name) const {
        domain::RouteInfo result;
        auto route = FindRoute(name);
        result.name = route->name;
        result.route_type = route->route_type;
        result.stop_count = CountStops(route);
        result.unique_stop_count = CountUniqueStops(route);
        result.route_length_geo = CalculateRouteLengthGeo(route);
        result.route_length = CalculateRouteLength(route);
        result.curvature = static_cast<double>(result.route_length) / result.route_length_geo;
        return result;
    }

    std::optional<std::set<std::string_view>> TransportCatalogue::GetBussesOnStop(const std::string& name) const{
        std::optional<std::set<std::string_view>> stop_busses;
        if(stops_by_names_.count(name)){ // если найден в базе
            if(buses_on_stops_.count(name)){ // и есть автобусы
                stop_busses = buses_on_stops_.at(name);
            }else{ // и нет автобусов
                stop_busses = std::set<std::string_view>{};
            }
        }
        return stop_busses;
    }

    void TransportCatalogue::SetDistance(std::pair<const Stop*, const Stop*> pair_stops, size_t distance){
        distance_stops[pair_stops] = distance;
        auto reverse = std::pair<const Stop*,const Stop*>{pair_stops.second, pair_stops.first};
        if(distance_stops.count(reverse) == 0){
            distance_stops[reverse] = distance;
        }
    }

    void TransportCatalogue::SetDistance(const Stop* begin, const Stop* end, size_t distance){
        SetDistance({begin, end}, distance);
    }

    const std::deque<domain::Stop> TransportCatalogue::GetStops() const{
        return stops_;
    }

    const std::deque<domain::Route> TransportCatalogue::GetRoutes() const{
        return routes_;
    }

    size_t TransportCatalogue::GetAmountStops() const{
        return stops_.size();
    }

    size_t TransportCatalogue::GetAmountRoutes() const{
        return routes_.size();
    }

    size_t TransportCatalogue::GetAmountStopsDistances() const {
        return distance_stops.size();
    }

    size_t TransportCatalogue::GetStopsDistances(const std::pair<std::string, std::string>& pair) const{
        return GetStopsDistances(FindStop(pair.first), FindStop(pair.second));
    }

    size_t TransportCatalogue::GetStopsDistances(std::pair<const Stop*, const Stop*> stops) const{
        if(distance_stops.count(stops) == 0 ){
            throw std::out_of_range(" Расстояние между остановками неизвестно ");
        }
        return distance_stops.at(stops);
    }

    size_t TransportCatalogue::GetStopsDistances(const Stop* begin, const Stop* end) const{
        std::pair<const Stop*, const Stop*> stops = {begin, end};
        return GetStopsDistances(stops);
    }

    size_t TransportCatalogue::GetStopsDistances(const std::string& begin, const std::string& end) const{
        return GetStopsDistances(FindStop(begin), FindStop(end));
    }

}
