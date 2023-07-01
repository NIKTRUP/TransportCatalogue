#include "../include/transport_router.h"

namespace transport{

    //TODO:: Отредачить все похожести

    TransportRouter::TransportRouter(const TransportCatalogue& catalogue, domain::RoutingSettings settings):
            catalogue_(catalogue), settings_(settings){
        graph_ = std::move(BuildGraph());
        router_ = std::make_unique<Router>(graph_);
    }

    TransportRouter::TransportRouter(const TransportCatalogue& catalogue, domain::RoutingSettings settings, const Graph& graph):
            catalogue_(catalogue), settings_(settings), graph_(graph){
        router_ = std::make_unique<Router>(graph_);
    }

    domain::RoutingSettings TransportRouter::GetSettings() const{
        return settings_;
    }

    void TransportRouter::InitVertexes() {
        size_t cnt_stops = 0;
        const auto &stops = catalogue_.GetStopsByNames();
        id_to_stop_name_.reserve(stops.size());
        stop_name_to_id_.reserve(stops.size());
        for (auto [name, _] : stops) {
            id_to_stop_name_[cnt_stops] = name;
            stop_name_to_id_[name] = cnt_stops++;
        }
    }

    graph::Edge<RouteWeight> TransportRouter::MakeEdge(const domain::Route *route,
                                                       int stop_from_index, int stop_to_index) {
        graph::Edge<RouteWeight> edge;
        edge.from = stop_name_to_id_.at(route->stops.at(static_cast<size_t>(stop_from_index))->name);
        edge.to = stop_name_to_id_.at(route->stops.at(static_cast<size_t>(stop_to_index))->name);
        edge.weight.bus_name = route->name;
        edge.weight.span_count = static_cast<int>(stop_to_index - stop_from_index);
        return edge;
    }

    double TransportRouter::ComputeRouteTime(const domain::Route *route, int stop_from_index, int stop_to_index) {
        auto split_distance =
                catalogue_.GetStopsDistance(route->stops.at(static_cast<size_t>(stop_from_index))->name,
                                       route->stops.at(static_cast<size_t>(stop_to_index))->name);
        return split_distance.value()/settings_.bus_velocity;
    }

    TransportRouter::Graph TransportRouter::BuildGraph(){
        InitVertexes();
        Graph graph(catalogue_.GetAmountStops());

        // проходим по всем маршрутам
        for (const auto& [route_name, route] : catalogue_.GetRoutesByNames()) {
            int cnt_stops = static_cast<int>(route->stops.size());
            // перебираем все пары остановок на маршруте и строим ребра
            for(int i = 0; i < cnt_stops - 1; ++i) {
                // общее время движения по ребру с учетом ожидания автобуса в минутах
                double route_time = settings_.bus_wait_time,
                   route_time_back = settings_.bus_wait_time;
                for(int j = i + 1; j < cnt_stops; ++j) {
                    graph::Edge<RouteWeight> edge = MakeEdge(route, i, j);
                    route_time += ComputeRouteTime(route, j - 1, j);
                    edge.weight.time = route_time;
                    graph.AddEdge(edge);

                    // если маршрут линейный, строим ребра так же для обратного направления
                    if (route->route_type == domain::RouteType::LINEAR) {
                        int i_back = cnt_stops - 1 - i;
                        int j_back = cnt_stops - 1 - j;
                        graph::Edge<RouteWeight> edge_back = MakeEdge(route, i_back, j_back);
                        route_time_back += ComputeRouteTime(route, j_back + 1, j_back);
                        edge_back.weight.time = route_time_back;
                        graph.AddEdge(edge_back);
                    }
                }
            }
        }
        return graph;
    }

    std::optional<std::vector<UserActivity>> TransportRouter::FindRoute(const std::string& from, const std::string& to) const{
        // если начальная и конечная остановка одинаковые - возвращаем пустой результат
        std::vector<UserActivity> found_route;

        if (from == to) { return found_route; }

        auto route = router_->BuildRoute(stop_name_to_id_.at(from), stop_name_to_id_.at(to));
        if (!route.has_value()) {
            return std::nullopt;
        }

        // проходим по всем ребрам маршрута
        for (auto edge_id : route->edges) {
            const auto &edge = graph_.GetEdge(edge_id);
            UserActivity route_edge;
            route_edge.bus_name = edge.weight.bus_name;
            route_edge.stop_name = id_to_stop_name_.at(edge.from);
            route_edge.span_count = edge.weight.span_count;
            route_edge.time = edge.weight.time;
            found_route.push_back(route_edge);
        }
        return found_route;
    }

    bool operator<(const RouteWeight &left, const RouteWeight &right) {
        return left.time < right.time;
    }

    RouteWeight operator+(const RouteWeight &left, const RouteWeight &right) {
        RouteWeight result;
        result.time = left.time + right.time;
        return result;
    }

    bool operator>(const RouteWeight &left, const RouteWeight &right) {
        return left.time > right.time;
    }

}