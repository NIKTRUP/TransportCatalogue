#include "../include/transport_router.h"

namespace transport{

    TransportWeight operator+(const TransportWeight &lhs, const TransportWeight &rhs) {
        TransportWeight result;
        result.time = lhs.time + rhs.time;
        return result;
    }

    bool operator<(const TransportWeight &lhs, const TransportWeight &rhs) {
        return lhs.time < rhs.time;
    }

    bool operator>(const TransportWeight &lhs, const TransportWeight &rhs) {
        return lhs.time > rhs.time;
    }

    TransportRouter::TransportRouter(const TransportCatalogue& catalogue, domain::RoutingSettings settings):
            catalogue_(catalogue), settings_(settings){
        graph_ = std::move(BuildGraph());
        router_ = std::make_unique<Router>(graph_);
    }

    TransportRouter::TransportRouter(const transport::TransportCatalogue & catalogue,
                    const std::unordered_map<graph::VertexId, std::string_view>& id_to_stop_name,
                    const std::unordered_map<std::string_view, graph::VertexId>& stop_name_to_id,
                    const Graph& graph,
                    const Router::RoutesInternalData & internal_data, domain::RoutingSettings settings)
                    : catalogue_(catalogue), id_to_stop_name_(id_to_stop_name),
                    stop_name_to_id_(stop_name_to_id), graph_(graph), settings_(settings)
    {
        router_ = std::make_unique<Router>(graph_, internal_data);
    }

    domain::RoutingSettings TransportRouter::GetSettings() const{
        return settings_;
    }

    const std::unordered_map<graph::VertexId, std::string_view>& TransportRouter::GetIdToStopName() const{
        return id_to_stop_name_;
    }

    const std::unordered_map<std::string_view, graph::VertexId> & TransportRouter::GetStopNameToId() const{
        return stop_name_to_id_;
    }

    const TransportRouter::Graph& TransportRouter::GetGraph() const{
        return graph_;
    }

    const TransportRouter::Router::RoutesInternalData& TransportRouter::GetRoutesInternalData() const{
        return router_->GetRoutesInternalData();
    }

    std::optional<std::vector<UserActivity>> TransportRouter::FindRoute(const std::string& from, const std::string& to) const{
        std::vector<UserActivity> found_route;

        if (from == to || id_to_stop_name_.empty() || stop_name_to_id_.empty()) { return found_route; }

        auto route = router_->BuildRoute(stop_name_to_id_.at(from), stop_name_to_id_.at(to));
        if (!route.has_value()) {
            return std::nullopt;
        }

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

    void TransportRouter::InitVertexes() {
        const auto& stops = catalogue_.GetStopsByNames();
        id_to_stop_name_.reserve(stops.size());
        stop_name_to_id_.reserve(stops.size());
        size_t cnt_stops = 0;
        for (auto [name, _] : stops) {
            id_to_stop_name_[cnt_stops] = name;
            stop_name_to_id_[name] = cnt_stops++;
        }
    }

    graph::Edge<TransportWeight> TransportRouter::CreateEdge(const domain::Route *route,
                                                             size_t from_id, size_t to_id) {
        graph::Edge<TransportWeight> edge;
        edge.from = stop_name_to_id_.at(route->stops.at(from_id)->name);
        edge.to = stop_name_to_id_.at(route->stops.at(to_id)->name);
        edge.weight.span_count = to_id - from_id;
        edge.weight.bus_name = route->name;
        return edge;
    }

    double TransportRouter::CalculateRouteTime(const domain::Route *route, size_t from_id, size_t to_id) {
        auto distance = catalogue_.GetStopsDistance(route->stops.at(from_id)->name, route->stops.at(to_id)->name);
        return static_cast<double>(distance.value()) / settings_.bus_velocity;
    }

    TransportRouter::Graph TransportRouter::BuildGraph(){
        InitVertexes();
        Graph graph(catalogue_.GetAmountStops());

        for (const auto& [_, route] : catalogue_.GetRoutesByNames()) {
            size_t number_stops = route->stops.size();
            for(size_t i = 0; i + 1 < number_stops; ++i) {
                double route_time = static_cast<double>(settings_.bus_wait_time),
                        route_time_reverse = route_time;
                for(size_t j = i + 1; j < number_stops; ++j) {
                    graph::Edge<TransportWeight> edge = CreateEdge(route, i, j);
                    route_time += CalculateRouteTime(route, j - 1, j);
                    edge.weight.time = route_time;
                    graph.AddEdge(edge);

                    if (route->route_type == domain::RouteType::LINEAR) {
                        size_t last_edge = number_stops - 1;
                        graph::Edge<TransportWeight> edge_reverse = CreateEdge(route, last_edge - i, last_edge - j);
                        route_time_reverse += CalculateRouteTime(route, number_stops - j, last_edge - j);
                        edge_reverse.weight.time = route_time_reverse;
                        graph.AddEdge(edge_reverse);
                    }
                }
            }
        }
        return graph;
    }
}