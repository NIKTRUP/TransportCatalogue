#include "../include/map_renderer.h"

namespace transport {

using namespace std::literals;

DictRenderer::DictRenderer(const TransportCatalogue& catalogue, const RenderSettings& settings):
    settings_(settings),
    catalogue_(catalogue){

    if(settings.is_init){
        const auto& buses = catalogue_.GetRoutes();
        std::vector<geo::Coordinates> geo_coords;
        for (const auto& bus : buses) {
            route_names_.insert(bus.name);
        }

        for (const auto& name : route_names_) {
            const auto& route = catalogue_.FindBus(std::string(name))->stops;
            for (const auto& stop : route) {
                stop_names_.insert(stop->name);
                geo_coords.push_back(stop->coordinate);
            }
        }

        const SphereProjector projector{
                geo_coords.begin(), geo_coords.end(), settings_.width, settings_.height, settings_.padding
        };

        AddLines(projector);
        AddRouteNames(projector);
        AddStopCircles(projector);
        AddStopNames(projector);
    }

}

   void DictRenderer::AddLines(const SphereProjector& projector) {
           size_t color = 0;
           for (const auto& name : route_names_) {
               auto route = catalogue_.FindBus(std::string(name));
               if (!route->stops.empty()) {
                   svg::Polyline line;
                   line.SetStrokeColor(settings_.color_palette.at(color)).
                           SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetFillColor(svg::NoneColor).
                           SetStrokeWidth(settings_.line_width).
                           SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                   // проходим вперёд по маршруту
                   for (auto iter = route->stops.begin(); iter < route->stops.end(); ++iter) {
                       line.AddPoint(projector((*iter)->coordinate));
                   }
                   // проходим назад по некольцевому маршруту
                   if (route->route_type == domain::RouteType::LINEAR) {
                       for (auto iter = std::next(route->stops.rbegin()); iter < route->stops.rend(); ++iter) {
                           line.AddPoint(projector((*iter)->coordinate));
                       }
                   }
                    render_.Add(line);
                   ++color;
                    color = color%settings_.color_palette.size();
               }
           }
   }

   void DictRenderer::AddRouteNames(const SphereProjector& projector) {
       size_t color = 0;
       for (const auto& route_name : route_names_) {
           auto route = catalogue_.FindBus(route_name);
           if (!route->stops.empty()) {
               svg::Text name, underlayer_text;
               name.SetData(std::string(route->name)).
                       SetPosition(projector(route->stops.front()->coordinate)).
                       SetOffset(settings_.bus_label_offset).
                       SetFontFamily("Verdana"s).
                       SetFontWeight("bold").
                       SetFontSize(static_cast<std::uint32_t>(settings_.bus_label_font_size));
               underlayer_text = name;

               name.SetFillColor(settings_.color_palette.at(color));
               underlayer_text.SetFillColor(settings_.underlayer_color).SetStrokeColor(settings_.underlayer_color).
                       SetStrokeWidth(settings_.underlayer_width).
                       SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

               render_.Add(underlayer_text);
               render_.Add(name);

               if (route->route_type == domain::RouteType::LINEAR && route->stops.back() != route->stops.front()) {
                   // отрисовка у последней остановки
                   name.SetPosition(projector(route->stops.back()->coordinate));
                   underlayer_text.SetPosition(projector(route->stops.back()->coordinate));
                   render_.Add(underlayer_text);
                   render_.Add(name);
               }
               ++color;
               color = color%settings_.color_palette.size();
           }
       }
   }

   void DictRenderer::AddStopCircles(const SphereProjector& projector) {
       for (const auto& stop : stop_names_) {
           auto stop_coordinates_proj = projector(catalogue_.FindStop(std::string(stop))->coordinate);
           svg::Circle circle_stop;
           circle_stop.SetCenter(stop_coordinates_proj)
               .SetRadius(settings_.stop_radius)
               .SetFillColor("white"s);
           render_.Add(circle_stop);
       }
   }

   void DictRenderer::AddStopNames(const SphereProjector& projector) {
       for (const auto& stop : stop_names_) {
           auto stop_coordinates_proj = projector(catalogue_.FindStop(std::string(stop))->coordinate);
           svg::Text text_stop;
           text_stop.SetPosition(stop_coordinates_proj)
               .SetOffset(settings_.stop_label_offset)
               .SetFontSize(settings_.stop_label_font_size)
               .SetFontFamily("Verdana"s)
               .SetData(std::string(stop))
               .SetFillColor("black"s);

           svg::Text text_stop_shadow = text_stop;
           text_stop_shadow.SetFillColor(settings_.underlayer_color)
               .SetStrokeColor(settings_.underlayer_color)
               .SetStrokeWidth(settings_.underlayer_width)
               .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
               .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
           render_.Add(text_stop_shadow);
           render_.Add(text_stop);
       }
   }

   void DictRenderer::Print(std::ostream& out) const{
       render_.Render(out);
   }

   const RenderSettings& DictRenderer::GetSettings() const{
       return settings_;
   }

   const svg::Document& DictRenderer::GetDocument() const{
       return render_;
   }

}
