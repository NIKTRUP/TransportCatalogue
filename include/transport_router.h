#ifndef TRANSPORTCATALOGUE_TRANSPORT_ROUTER_H
#define TRANSPORTCATALOGUE_TRANSPORT_ROUTER_H

#include "../include/router.h"
#include "../include/transport_catalogue.h"
namespace transport {

    struct RouteWeight {
        std::string_view bus_name;
        size_t span_count = 0;
        double time = 0;
    };

    bool operator<(const RouteWeight &left, const RouteWeight &right);
    bool operator>(const RouteWeight &left, const RouteWeight &right);
    RouteWeight operator+(const RouteWeight &left, const RouteWeight &right);

    struct UserActivity{
        std::string stop_name,
                bus_name;
        size_t span_count = 0;
        double time = 0;
    };

    class TransportRouter {
        using Router = graph::Router<RouteWeight>;
        using Graph = graph::DirectedWeightedGraph<RouteWeight>;

    public:

        TransportRouter(const TransportCatalogue& catalogue, domain::RoutingSettings settings);

        TransportRouter(const TransportCatalogue& catalogue, domain::RoutingSettings settings, const Graph& graph);

        domain::RoutingSettings GetSettings() const;

        std::optional<std::vector<UserActivity>> FindRoute(const std::string& from, const std::string& to) const;

    private:

        const TransportCatalogue& catalogue_;
        domain::RoutingSettings settings_;
        std::unordered_map<graph::VertexId, std::string_view> id_to_stop_name_;
        std::unordered_map<std::string_view, graph::VertexId> stop_name_to_id_;
        Graph graph_;
        std::unique_ptr<Router> router_;

    private:

        Graph BuildGraph();
        void InitVertexes();
        double ComputeRouteTime(const domain::Route *route, int stop_from_index, int stop_to_index);
        graph::Edge<RouteWeight> MakeEdge(const domain::Route *route,
                                          int stop_from_index, int stop_to_index);
    };
} // namespace router

#endif //TRANSPORTCATALOGUE_TRANSPORT_ROUTER_H
