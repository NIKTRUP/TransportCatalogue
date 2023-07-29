#include "../include/serialization.h"

namespace serialize {

    namespace {
        struct ReinterpretToSerializedColor {
            [[nodiscard]] transport_serialize::Color operator() (std::monostate) const {
                return transport_serialize::Color{};
            }
            [[nodiscard]] transport_serialize::Color operator() (const std::string& color) {
                transport_serialize::Color color_serialize;
                color_serialize.set_is_rgba(false);
                color_serialize.set_name(color);
                return color_serialize;
            }
            [[nodiscard]] transport_serialize::Color operator() (const svg::Rgb& color) {
                transport_serialize::Rgba rgb;
                rgb.set_red(color.red);
                rgb.set_green(color.green);
                rgb.set_blue(color.blue);

                transport_serialize::Color color_serialize;
                color_serialize.set_is_rgba(false);
                *color_serialize.mutable_rgba() = std::move(rgb);
                return color_serialize;
            }
            [[nodiscard]] transport_serialize::Color operator()(const svg::Rgba& color) {
                transport_serialize::Rgba rgba;
                rgba.set_red(color.red);
                rgba.set_green(color.green);
                rgba.set_blue(color.blue);
                rgba.set_opacity(color.opacity);

                transport_serialize::Color color_serialize;
                color_serialize.set_is_rgba(true);
                *color_serialize.mutable_rgba() = std::move(rgba);
                return color_serialize;
            }
        };
    }

    svg::Color ReinterpretToSvgColor(const transport_serialize::Color& color_deserialized){
        svg::Color color;
        if(!color_deserialized.name().empty()){
            color = color_deserialized.name();
        }else if(color_deserialized.is_rgba()){
            color = svg::Rgba{static_cast<std::uint8_t>(color_deserialized.rgba().red()),
                              static_cast<std::uint8_t>(color_deserialized.rgba().green()),
                              static_cast<std::uint8_t>(color_deserialized.rgba().blue()),
                              color_deserialized.rgba().opacity()};
        }else{
            color = svg::Rgb{static_cast<std::uint8_t>(color_deserialized.rgba().red()),
                             static_cast<std::uint8_t>(color_deserialized.rgba().green()),
                             static_cast<std::uint8_t>(color_deserialized.rgba().blue())};
        }
        return color;
    }
    
    void Serializer::Serialize(const transport::TransportCatalogue& tc, const transport::RenderSettings& render,
                               const std::optional<transport::domain::RoutingSettings>& routing_settings ,std::ostream& ostream){
        transport_serialize::DataBase db;
        auto& tc_serialize = *db.mutable_catalogue();
        AddStopsToSerialize(tc_serialize, tc.GetStops());
        AddRoutesToSerialize(tc_serialize, tc.GetRoutes());
        AddDistancesToSerialize(tc_serialize, tc.GetDistancesStops());
        AddRenderSettingsToSerialize(*db.mutable_render_settings(), render);
        if(routing_settings.has_value()){
            transport::TransportRouter router(tc, routing_settings.value());
            AddTransportRouterToSerialize(*db.mutable_router(), router);
        }

        db.SerializeToOstream(&ostream);
    }

    void Serializer::Deserialize(transport::TransportCatalogue& tc, transport::RenderSettings& settings,
                                 std::unique_ptr<transport::TransportRouter>& router, std::istream& istream){
        transport_serialize::DataBase db;
        db.ParseFromIstream(&istream);
        tc = std::move(DeserializeTransportCatalogue(*db.mutable_catalogue()));
        settings = std::move(DeserializeRenderSettings(*db.mutable_render_settings()));
        if (db.has_router()) {
            auto& tr_deserialize = *db.mutable_router();
            auto& settings_deserialize = *tr_deserialize.mutable_settings();
            transport::domain::RoutingSettings routing_settings = {settings_deserialize.bus_wait_time(), settings_deserialize.bus_velocity()};
            const auto& [id_to_stop_name, stop_name_to_id] = DeserializeIndexMaps(tc, tr_deserialize);
            auto graph = DeserializeGraph(tc, tr_deserialize);
            auto routes_internal_data =  DeserializeInternalData(tr_deserialize);
            router = std::make_unique<transport::TransportRouter>(tc, id_to_stop_name, stop_name_to_id, graph, routes_internal_data, routing_settings);
        }
    }

    transport::TransportCatalogue Serializer::DeserializeTransportCatalogue(transport_serialize::TransportCatalogue& tc_deserialize) {
        transport::TransportCatalogue tc;
        auto &stops = *tc_deserialize.mutable_stops();
        auto &routes = *tc_deserialize.mutable_routes();
        auto &distances = *tc_deserialize.mutable_distances();

        for (auto& stop: stops) {
            tc.AddStop(std::move(transport::domain::Stop{stop.name(), stop.coordinates().lat(), stop.coordinates().lng()}));
            id_to_stop_name_[stop.id()] = stop.name();
        }

        for (auto& distance: distances) {
            tc.SetDistance({tc.FindStop(id_to_stop_name_.at(distance.id_from())),
                            tc.FindStop(id_to_stop_name_.at(distance.id_to()))},
                           distance.distance());
        }

        for (auto& route_deserialize: routes) {
            transport::domain::Route route;
            route.name = route_deserialize.name();
            route.route_type = ReinterpretToDomainRouteType(route_deserialize.type());
            route.stops.reserve(route_deserialize.stops_id_size());
            for (auto& stop_id: *route_deserialize.mutable_stops_id()) {
                route.stops.push_back(tc.FindStop(id_to_stop_name_.at(stop_id)));
            }
            tc.AddRoute(route);
            id_to_route_name_[route_deserialize.id()] = route_deserialize.name();
        }
        return tc;
    }

    transport::RenderSettings Serializer::DeserializeRenderSettings(transport_serialize::RenderSettings& render_deserialize){
        transport::RenderSettings settings;
        settings.width = render_deserialize.width();
        settings.height = render_deserialize.height();
        settings.padding = render_deserialize.padding();
        settings.line_width = render_deserialize.line_width();
        settings.stop_radius = render_deserialize.stop_radius();
        settings.bus_label_font_size = render_deserialize.bus_label_font_size();
        settings.bus_label_offset = {render_deserialize.bus_label_offset().x(), render_deserialize.bus_label_offset().y()};
        settings.stop_label_font_size = render_deserialize.stop_label_font_size();
        settings.stop_label_offset = {render_deserialize.stop_label_offset().x(), render_deserialize.stop_label_offset().y()};
        settings.underlayer_color = ReinterpretToSvgColor(*render_deserialize.mutable_underlayer_color());
        settings.underlayer_width = render_deserialize.underlayer_width();
        for(const auto& color: render_deserialize.color_palette()){
            settings.color_palette.push_back(std::move(ReinterpretToSvgColor(color)));
        }
        settings.is_init = render_deserialize.is_init();
        return settings;
    }

    std::pair<std::unordered_map<graph::VertexId, std::string_view>,
            std::unordered_map<std::string_view, graph::VertexId>> Serializer::DeserializeIndexMaps(transport::TransportCatalogue& tc,
                                                                                        transport_serialize::TransportRouter& tr_deserialize){
        std::unordered_map<graph::VertexId, std::string_view> id_to_stop_name;
        std::unordered_map<std::string_view, graph::VertexId> stop_name_to_id;
        for (auto& p_stop_by_id : *tr_deserialize.mutable_stop_by_id()) {
            const auto& stop = tc.FindStop(id_to_stop_name_.at(p_stop_by_id.stop_id()));
            id_to_stop_name[p_stop_by_id.id()] = stop->name;
            stop_name_to_id[stop->name] = p_stop_by_id.id();
        }
        return {id_to_stop_name,stop_name_to_id };
    }

    graph::DirectedWeightedGraph<transport::TransportWeight> Serializer::DeserializeGraph(transport::TransportCatalogue& tc,
                                                                                          transport_serialize::TransportRouter& tr_deserialize){
        std::vector<graph::Edge<transport::TransportWeight>> edges;
        auto& graph_deserialize = *tr_deserialize.mutable_graph();
        for (const auto& edge_deserialize : graph_deserialize.edges()) {
            edges.push_back({edge_deserialize.from() , edge_deserialize.to(),
                             { tc.FindBus(id_to_route_name_.at(edge_deserialize.weight().bus_id()))->name,
                               edge_deserialize.weight().span_count(),
                               edge_deserialize.weight().time()}});
        }

        std::vector<std::vector<graph::EdgeId>> incidence_lists;
        for (auto i = 0; i < graph_deserialize.incidence_lists_size(); ++i) {
            std::vector<graph::EdgeId> list;
            auto &list_serialize = graph_deserialize.incidence_lists(i);
            for (auto j = 0; j < list_serialize.edge_id_size(); ++j) {
                list.push_back(list_serialize.edge_id(j));
            }
            incidence_lists.push_back(list);
        }
        return {edges, incidence_lists};
    }

    graph::Router<transport::TransportWeight>::RoutesInternalData Serializer::DeserializeInternalData(transport_serialize::TransportRouter& tr_deserialize){

        using RouteInternalData = graph::Router<transport::TransportWeight>::RouteInternalData;

        auto& router = *tr_deserialize.mutable_router();
        graph::Router<transport::TransportWeight>::RoutesInternalData routes_internal_data;
        routes_internal_data.reserve(router.rows_size());
        for (auto& row : *router.mutable_rows()) {
            std::vector<std::optional<RouteInternalData>> internal_data;
            internal_data.reserve(row.cols_size());
            for (auto& col : *row.mutable_cols()) {
                if (col.optional_route_internal_data_case() == transport_serialize::OptionalRouteInternalData::kRouteInternalData) {;
                    RouteInternalData route_data;
                    auto& value = *col.mutable_route_internal_data();
                    route_data.weight.time = value.time();
                    if (value.optional_prev_edge_case() == transport_serialize::RouteInternalData::kEdgeId) {
                        route_data.prev_edge = value.edge_id();
                    }else{
                        route_data.prev_edge = std::nullopt;
                    }
                    internal_data.emplace_back(route_data);
                }else {
                    internal_data.emplace_back(std::nullopt);
                }
            }
            routes_internal_data.push_back(internal_data);
        }
        return routes_internal_data;
    }

    void Serializer::AddStopsToSerialize(transport_serialize::TransportCatalogue& catalogue_serialize,
                                         const std::deque<transport::domain::Stop>& stops) {
        uint32_t id = 0;
        for (auto& [name, coordinate] : stops) {
            auto& stop = *catalogue_serialize.add_stops();
            stop.set_id(id);
            stop.set_name(name);
            auto& coord = *stop.mutable_coordinates();
            coord.set_lat(coordinate.lat);
            coord.set_lng(coordinate.lng);
            stop_name_to_id_[name] = id;
           ++id;
        }
    }

    void Serializer::AddRoutesToSerialize(transport_serialize::TransportCatalogue& catalogue_serialize,
                                          const std::deque<transport::domain::Route>& routers) {
        uint32_t id = 0;
        for (auto& [name, type, stops] : routers) {
            auto& route_serialize = *catalogue_serialize.add_routes();
            route_serialize.set_id(id);
            route_serialize.set_name(name);
            route_serialize.set_type(ReinterpretToSerializeRouteType(type));

            for (const auto& stop : stops) {
                route_serialize.add_stops_id(stop_name_to_id_.at(stop->name));
            }
            route_name_to_id_[name] = id;
            ++id;
        }
    }

    void Serializer::AddDistancesToSerialize(transport_serialize::TransportCatalogue& catalogue_serialize,
                                             const std::unordered_map<std::pair<const transport::domain::Stop*,
                                                     const transport::domain::Stop*>,
                                                     size_t, transport::domain::StopsHasher>& distances){
        for (auto& [stops, distance] : distances) {
            auto& distance_serialize = *catalogue_serialize.add_distances();
            distance_serialize.set_id_from(stop_name_to_id_.at(stops.first->name));
            distance_serialize.set_id_to(stop_name_to_id_.at(stops.second->name));
            distance_serialize.set_distance(distance);
        }
    }

    void Serializer::AddTransportRouterToSerialize(transport_serialize::TransportRouter& router_serialize, const transport::TransportRouter& router){
        AddGraphToSerialize(*router_serialize.mutable_graph(), router.GetGraph());
        AddRouterToSerialize(router_serialize, router);
    }

    void Serializer::AddGraphToSerialize(transport_serialize::Graph& graph_serialize, const graph::DirectedWeightedGraph<transport::TransportWeight>& graph){
        for (const auto& edge : graph.GetEdges()) {
            auto& edge_serialize = *graph_serialize.add_edges();
                edge_serialize.set_from(edge.from);
                edge_serialize.set_to(edge.to);
                auto& weight = *edge_serialize.mutable_weight();
                    weight.set_bus_id(route_name_to_id_.at(edge.weight.bus_name));
                    weight.set_span_count(edge.weight.span_count);
                    weight.set_time(edge.weight.time);
        }

        for (const auto& list : graph.GetIncidenceLists()) {
            auto& list_serialize = *graph_serialize.add_incidence_lists();
            for (const auto id : list) {
                list_serialize.add_edge_id(id);
            }
        }
    }

    void Serializer::AddRouterToSerialize(transport_serialize::TransportRouter& router_serialize, const transport::TransportRouter& router){
        auto& settings = *router_serialize.mutable_settings();
        settings.set_bus_wait_time(router.GetSettings().bus_wait_time);
        settings.set_bus_velocity(router.GetSettings().bus_velocity);

        for (const auto& [name, id] : router.GetStopNameToId()) {
            auto& stop_id = *router_serialize.add_stop_by_id();
            stop_id.set_id(id);
            stop_id.set_stop_id(stop_name_to_id_.at(name));
        }

        for (const auto &rows : router.GetRoutesInternalData()) {
            auto& p_data = *router_serialize.mutable_router()->add_rows();
            for (const auto &cols : rows) {
                auto& p_internal = *p_data.add_cols();
                if (cols.has_value()) {
                    auto &value = cols.value();
                    auto& value_serialize = *p_internal.mutable_route_internal_data();
                    value_serialize.set_time(value.weight.time);
                    if (value.prev_edge.has_value()) {
                        value_serialize.set_edge_id(value.prev_edge.value());
                    }
                }
            }
        }
    }

    void Serializer::AddRenderSettingsToSerialize(transport_serialize::RenderSettings& render_serialize,
                                                  const transport::RenderSettings& render){
        render_serialize.set_width(render.width);
        render_serialize.set_height(render.height);
        render_serialize.set_padding(render.padding);
        render_serialize.set_line_width(render.line_width);
        render_serialize.set_stop_radius(render.stop_radius);
        render_serialize.set_bus_label_font_size(render.bus_label_font_size);
        auto& bus_label_offset = *render_serialize.mutable_bus_label_offset();
        bus_label_offset.set_x(render.bus_label_offset.x);
        bus_label_offset.set_y(render.bus_label_offset.y);
        render_serialize.set_stop_label_font_size(render.stop_label_font_size);
        auto& stop_label_offset = *render_serialize.mutable_stop_label_offset();
        stop_label_offset.set_x(render.stop_label_offset.x);
        stop_label_offset.set_y(render.stop_label_offset.y);
        *render_serialize.mutable_underlayer_color() = std::move(std::visit(ReinterpretToSerializedColor(), render.underlayer_color));
        render_serialize.set_underlayer_width(render.underlayer_width);
        for(const auto& color: render.color_palette){
            *render_serialize.add_color_palette() = std::move(std::visit(ReinterpretToSerializedColor(), color));
        }
        render_serialize.set_is_init(render.is_init);
    }

    transport_serialize::RouteType Serializer::ReinterpretToSerializeRouteType(const transport::domain::RouteType& type){
        transport_serialize::RouteType type_serialized;
        switch (type) {
            case transport::domain::RouteType::LINEAR :
                type_serialized = transport_serialize::RouteType::LINEAR;
                break;
            case transport::domain::RouteType::CIRCLE :
                type_serialized = transport_serialize::RouteType::CIRCLE;
                break;
            default:
                type_serialized = transport_serialize::RouteType::UNKNOWN;
                break;
        }
        return type_serialized;
    }

    transport::domain::RouteType Serializer::ReinterpretToDomainRouteType(const transport_serialize::RouteType& type_serialized){
        transport::domain::RouteType type;
        switch (type_serialized) {
            case transport_serialize::RouteType::LINEAR :
                type = transport::domain::RouteType::LINEAR;
                break;
            case transport_serialize::RouteType::CIRCLE :
                type = transport::domain::RouteType::CIRCLE;
                break;
            default:
                type = transport::domain::RouteType::UNKNOWN;
                break;
        }
        return type;
    }
}