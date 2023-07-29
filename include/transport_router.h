#ifndef TRANSPORTCATALOGUE_TRANSPORT_ROUTER_H
#define TRANSPORTCATALOGUE_TRANSPORT_ROUTER_H

#include "../include/router.h"
#include "../include/transport_catalogue.h"
namespace transport {

    struct TransportWeight {
        std::string_view bus_name;
        size_t span_count = 0;
        double time = 0;
    };

    TransportWeight operator+(const TransportWeight &lhs, const TransportWeight &rhs);
    bool operator<(const TransportWeight &lhs, const TransportWeight &rhs);
    bool operator>(const TransportWeight &lhs, const TransportWeight &rhs);


    struct UserActivity{
        std::string stop_name,
                bus_name;
        size_t span_count = 0;
        double time = 0;
    };

    class TransportRouter {
        using Router = graph::Router<TransportWeight>;
        using Graph = graph::DirectedWeightedGraph<TransportWeight>;

    public:
        TransportRouter(const TransportCatalogue& catalogue, domain::RoutingSettings settings);

        TransportRouter(const transport::TransportCatalogue& catalogue,
                                 const std::unordered_map<graph::VertexId, std::string_view>& id_to_stop_name,
                                 const std::unordered_map<std::string_view, graph::VertexId>& stop_name_to_id,
                                 const Graph& graph,
                                 const Router::RoutesInternalData & internal_data,
                                 domain::RoutingSettings settings);



        domain::RoutingSettings GetSettings() const;

        const std::unordered_map<graph::VertexId, std::string_view>& GetIdToStopName() const;

        const std::unordered_map<std::string_view, graph::VertexId> & GetStopNameToId() const;

        const Graph& GetGraph() const;

        const Router::RoutesInternalData& GetRoutesInternalData() const;

        std::optional<std::vector<UserActivity>> FindRoute(const std::string& from, const std::string& to) const;

    private:
        const TransportCatalogue& catalogue_;
        domain::RoutingSettings settings_;
        std::unordered_map<graph::VertexId, std::string_view> id_to_stop_name_;
        std::unordered_map<std::string_view, graph::VertexId> stop_name_to_id_;
        Graph graph_;
        std::unique_ptr<Router> router_;

    private:
        void InitVertexes();
        graph::Edge<TransportWeight> CreateEdge(const domain::Route *route, size_t from_id, size_t to_id);
        double CalculateRouteTime(const domain::Route *route, size_t from_id, size_t to_id);
        Graph BuildGraph();
    };

} // namespace router

#endif //TRANSPORTCATALOGUE_TRANSPORT_ROUTER_H
