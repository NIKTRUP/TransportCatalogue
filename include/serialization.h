#ifndef TRANSPORTCATALOGUE_SERIALIZATION_H
#define TRANSPORTCATALOGUE_SERIALIZATION_H
#include <filesystem>
#include <fstream>
#include "../include/transport_catalogue.h"
#include "../include/transport_router.h"
#include "../include/map_renderer.h"
#include <transport_catalogue.pb.h>
#include <transport_router.pb.h>
#include <transport_router.pb.h>
#include <utility>

namespace serialize{
    class Serializer {
    public:
        Serializer() = default;

        void Serialize(const transport::TransportCatalogue& tc, const transport::RenderSettings& render,
                                   const std::optional<transport::domain::RoutingSettings>& routing_settings ,std::ostream& ostream);

        void Deserialize(transport::TransportCatalogue& tc, transport::RenderSettings& settings,
                         std::unique_ptr<transport::TransportRouter>& router , std::istream& istream);
    private:

        transport::TransportCatalogue DeserializeTransportCatalogue(transport_serialize::TransportCatalogue& tc_deserialize);

        transport::RenderSettings DeserializeRenderSettings(transport_serialize::RenderSettings& render_deserialize);

        std::pair<std::unordered_map<graph::VertexId, std::string_view>,
                std::unordered_map<std::string_view, graph::VertexId>> DeserializeIndexMaps(transport::TransportCatalogue& tc,
                                                                                            transport_serialize::TransportRouter& tr_deserialize);

        graph::DirectedWeightedGraph<transport::TransportWeight> DeserializeGraph(transport::TransportCatalogue& tc,
                                                                                  transport_serialize::TransportRouter& tr_deserialize);

        graph::Router<transport::TransportWeight>::RoutesInternalData DeserializeInternalData(transport_serialize::TransportRouter& tr_deserialize);

        void AddStopsToSerialize(transport_serialize::TransportCatalogue& catalogue_serialize,
                                 const std::deque<transport::domain::Stop>& stops);

        void AddRoutesToSerialize(transport_serialize::TransportCatalogue& catalogue_serialize,
                                  const std::deque<transport::domain::Route>& routers);

        void AddDistancesToSerialize(transport_serialize::TransportCatalogue& catalogue_serialize,
                                     const std::unordered_map<std::pair<const transport::domain::Stop*,
                                             const transport::domain::Stop*>,
                                             size_t, transport::domain::StopsHasher>& distances);

        void AddTransportRouterToSerialize(transport_serialize::TransportRouter& router_serialize, const transport::TransportRouter& router);

        void AddGraphToSerialize(transport_serialize::Graph& graph_serialize, const graph::DirectedWeightedGraph<transport::TransportWeight>& graph);

        void AddRouterToSerialize(transport_serialize::TransportRouter& router_serialize, const transport::TransportRouter& router);

        static void AddRenderSettingsToSerialize(transport_serialize::RenderSettings& render_serialize,
                                                      const transport::RenderSettings& render);

        static transport_serialize::RouteType ReinterpretToSerializeRouteType(const transport::domain::RouteType& type);

        static transport::domain::RouteType ReinterpretToDomainRouteType(const transport_serialize::RouteType& type_serialized);

        std::unordered_map<std::size_t , std::string_view> id_to_stop_name_;
        std::unordered_map<std::string_view, std::size_t > stop_name_to_id_;

        std::unordered_map<std::size_t , std::string_view> id_to_route_name_;
        std::unordered_map<std::string_view, std::size_t > route_name_to_id_;
    };
}

#endif //TRANSPORTCATALOGUE_SERIALIZATION_H
