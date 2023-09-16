# TransportCatalogur

TransportCatalogue — система обработки и хранения транспортных маршрутов. Работает через потоки ввода-ввывода, считывает запросы и выдает результат в формате JSON.

## Основные функции:
 - создание базы транспортного справочника и её сериализация в файл;
 - десериализация базы из файла;
 - получение информации о маршруте;
 - получение информации об остановке;
 - поиск оптимального маршрута между двумя остановками;
 - визуализация карты маршрутов.

## Использование программы
Использование `transport_catalogue` условно делится на 2 этапа:
1. Создание базы маршрутов (если его ещё нет)
2. Отправка запросов 

Для создания базы транспортного справочника и ее сериализации в файл по запросам `base_requests` необходимо запустить программу с параметром `make_base`, указав при этом входной JSON-файл.  
Пример запуска программы для создания базы: 
`./transport_catalogue make_base < <имя файла>.json`. Само собой, можно осуществлять ввод напрямую в терминал, не используя перенаправление ввода/вывода.

Для того, чтобы использовать полученную базу и десериализовать ее для ответов на запросы `stat_requests` нужно запустить программу с параметром `process_requests`, указав входной JSON-файл, содержащий запросы к БД.
Пример запуска программы для выполнения запросов к базе:  
`transport_catalogue.exe process_requests <process_requests.json > response.txt` (`response.json` — файл с ответами на запросы).

Файл `make_base.json`, формирующий каталог маршрутов, должен представлять собой словарь JSON со следующими разделами (ключами) :

<details> <summary> 
  
  ### `serialization_settings` — настройки сериализации
  </summary>

`file` содержит название файла, в который сериализуется (или из которого десериализуется в случае с командой `process_requests`) база.
``` json
      "serialization_settings": {
          "file": "transport_catalogue.db"
      }
```
  
</details>

<details> <summary> 
  
  ### `base_requests` — массив данных об остановках и маршрутах
  </summary>
  
`base_requests` содержит в себе данные остановок и маршрутов, порядок их перечисления — произволен. 
#### Описание остановки — словарь с ключами:  
`type` — строка, равная `Stop`, означает, что объект описывает остановку;  
`name` — название остановки;  
`latitude` и `longitude` задают координаты широты и долготы остановки;  
`road_distances` — словарь, задающий расстояние до соседних остановок. Ключ — название остановки, значение — целое число в метрах. 
``` json
{
  "type": "Stop",
  "name": "Электросети",
  "latitude": 43.598701,
  "longitude": 39.730623,
  "road_distances": {
    "Улица Докучаева": 3000,
    "Улица Лизы Чайкиной": 4300
  }
}
```

#### Описание маршрута — словарь с ключами:  
`type` — строка `Bus`, означающая, что объект описывает маршрут;  
`name` — название маршрута;  
`stops` — массив с названиями остановок, через которые проходит маршрут. У кольцевого маршрута название последней остановки дублирует название первой. Например: `["stop1", "stop2", "stop3", "stop1"]`;  
`is_roundtrip` — значение типа bool. Указывает, кольцевой маршрут или нет.  

``` json
{
  "type": "Bus",
  "name": "14",
  "stops": [
    "Улица Лизы Чайкиной",
    "Электросети",
    "Улица Докучаева",
    "Улица Лизы Чайкиной"
  ],
  "is_roundtrip": true
} 
```
</details>

<details> <summary> 
  
  ### `routing_settings` — настройки маршрутизации
  </summary>

``` json
"routing_settings": {
      "bus_wait_time": 6,
      "bus_velocity": 40
} 
```
`bus_wait_time` — время ожидания автобуса на остановке, в минутах. Считайте, что когда бы человек ни пришёл на остановку и какой бы ни была эта остановка, он будет ждать любой автобус в точности указанное количество минут. Значение — целое число `от 1 до 1000`.  
`bus_velocity` — скорость автобуса, в км/ч. Считайте, что скорость любого автобуса постоянна и в точности равна указанному числу. Время стоянки на остановках не учитывается, время разгона и торможения тоже. Значение — вещественное число `от 1 до 1000`.  
Данная конфигурация задаёт время ожидания, равным 6 минутам, и скорость автобусов, равной 40 километрам в час.

  
</details>

<details> <summary> 
  
  ### `render_settings` — настройки отрисовки  
  </summary>

`width` и `height` — ключи, которые задают ширину и высоту в пикселях. Вещественное число в диапазоне `от 0 до 100000`.  
`padding` — отступ краёв карты от границ `SVG`-документа. Вещественное число не меньше 0 и меньше `min(width, height)/2`.  
`line_width` — толщина линий, которыми рисуются автобусные маршруты. Вещественное число в диапазоне `от 0 до 100000`.  
`stop_radius` — радиус окружностей, которыми обозначаются остановки. Вещественное число в диапазоне `от 0 до 100000`.  
`bus_label_font_size` — размер текста, которым написаны названия автобусных маршрутов. Целое число в диапазоне `от 0 до 100000`.  
`bus_label_offset` — смещение надписи с названием маршрута относительно координат конечной остановки на карте. Массив из двух элементов типа double. Задаёт значения свойств `dx` и `dy` `SVG`-элемента `text`. Элементы массива — числа в диапазоне от `–100000 до 100000`.  
`stop_label_font_size` — размер текста, которым отображаются названия остановок. Целое число в диапазоне `от 0 до 100000`.  
`stop_label_offset` — смещение названия остановки относительно её координат на карте. Массив из двух элементов типа double. Задаёт значения свойств `dx` и `dy` SVG-элемента `text`. Числа в диапазоне `от –100000 до 100000`.  
`underlayer_color` — цвет подложки под названиями остановок и маршрутов.  
`underlayer_width` — толщина подложки под названиями остановок и маршрутов. Задаёт значение атрибута `stroke-width` элемента `<text>`. Вещественное число в диапазоне `от 0 до 100000`.
`color_palette` — цветовая палитра. Непустой массив.  
Цвет можно указать:  
- в виде строки, например, `"red"` или `"black"`;  
- в массиве из трёх целых чисел диапазона `[0, 255]`. Они определяют `r`, `g` и `b` компоненты цвета в формате `svg::Rgb`. Цвет `[255, 16, 12]` нужно вывести как `rgb(255, 16, 12)`;  
- в массиве из четырёх элементов: три целых числа в диапазоне от `[0, 255]` и одно вещественное число в диапазоне от `[0.0, 1.0]`. Они задают составляющие `red`, `green`, `blue` и `opacity` цвета формата `svg::Rgba`. Цвет, заданный как `[255, 200, 23, 0.85]`, должен быть выведен как `rgba(255, 200, 23, 0.85)`.

``` json
{
  "width": 1200.0,
  "height": 1200.0,

  "padding": 50.0,

  "line_width": 14.0,
  "stop_radius": 5.0,

  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 15.0],

  "stop_label_font_size": 20,
  "stop_label_offset": [7.0, -3.0],

  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,

  "color_palette": [
    "green",
    [255, 160, 0],
    "red"
  ]
} 
```
</details>

\
Файл `process_requests.json` должен представлять собой словарь JSON со следующими разделами (ключами) :
<details> <summary> 
  
  ### `serialization_settings` — настройки сериализации  
  </summary>

  Аналогично `make_base.json`
</details>

<details> <summary> 
  
  ### `stat_requests` — массив запросов к каталогу  
  </summary>

#### Запрос на получение информации о маршруте:
``` json
{
  "id": 12345678,
  "type": "Bus",
  "name": "14"
} 
```
Ключ `name` задаёт название маршрута, для которого приложение должно вывести статистическую информацию.  
В ответ на этот запрос выдается в виде словаря:
``` json
{
  "curvature": 2.18604,
  "request_id": 12345678,
  "route_length": 9300,
  "stop_count": 4,
  "unique_stop_count": 3
} 
```
В словаре содержатся ключи:
`curvature` — число типа double, задающее извилистость маршрута. Извилистость равна отношению длины дорожного расстояния маршрута к длине географического расстояния;  
`request_id` — целое число, равное `id` соответствующего запроса `Bus`;  
`route_length` — целое число, равное длине маршрута в метрах;  
`stop_count` — количество остановок на маршруте;  
`unique_stop_count` — количество уникальных остановок на маршруте.  
На кольцевом маршруте, заданном остановками `A, B, C, A`, количество остановок равно четырём, а количество уникальных остановок равно трём.  
На некольцевом маршруте, заданном остановками `A, B и C`, количество остановок равно пяти `(A, B, C, B, A)`, а уникальных — равно трём.  

#### Запрос на получение информации об остановке:
``` json
{
  "id": 12345,
  "type": "Stop",
  "name": "Улица Докучаева"
} 
```
Ключ `name` задаёт название остановки.  
Ответ на запрос:
``` json
{
  "buses": [
      "14", "22к"
  ],
  "request_id": 12345
} 
```
#### Запрос на получение изображения:
``` json
{
  "type": "Map",
  "id": 11111
}
```
Ответ на запрос:
``` json
{
  "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">...\n</svg>",
  "request_id": 11111
} 
```
Ключ `map` — строка с изображением карты в формате `SVG`
![124977269-f6e62380-e038-11eb-80de-2ba6bfa1e1d8](https://github.com/NIKTRUP/cpp-transport-catalogue/assets/72292425/a6d69885-57f9-4df6-8f9f-a9125788c170)



### Запрос на построение маршрута между двумя остановками
Помимо стандартных свойств `id` и `type`, запрос содержит ещё два:  
`from` — остановка, где нужно начать маршрут.  
`to` — остановка, где нужно закончить маршрут.  
Оба значения — названия существующих в базе остановок. Однако они, возможно, не принадлежат ни одному автобусному маршруту.
``` json
{
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Universam",
      "id": 4
}
```
</details>

## Сборка
Для сборки программы необходим  C++, поддерживающий стандарт C++17 и выше, Cmake не ниже версии 3.10, библиотека Protobuf.

> 0. Скачайте и соберите [Google Protobuf](https://protobuf.dev/downloads/) под вашу версию компилятора
> 1. Создайте папку `build` для сборки проекта
> 2. Откройте консоль в папке `build` и введите в консоли : `cmake ..` с параметром `-DCMAKE_PREFIX_PATH= <путь к собранной библиотеке Protobuf>`, если `Protobuf` не указан в переменной `Path`
> 3. Введите команду `make` или `make -j<кол-во ядер процессора>` для ускорения процесса сборки 
> 4. В папке `build` появится исполняемый файл `transport_catalogue` (`transport_catalogue.exe` для Windows)

После этого можно приступать к работе с программой.
### Формирование транспортного каталога
Запустите собранную программу с ключом `make_base` : `./transport_catalogue make_base < base.json`\
Программа прочитает файл `base.json` и сформирует на его основе транспортный каталог.
В папке с программой появится файл `transport_catalogue.db` (или другой, в зависимости от того, какое название будет указано в `"serialization_settings"`). В данном файле будет сохранен каталог в двоичном виде.\
В дальнейшем этот сохраненный каталог можно будет "разворачивать" для формирования ответов на запросы, без необходимости строить его заново.

<details>
  <summary>Пример корректного файла для формирования каталога:</summary>

```json 
      {
      "serialization_settings": {
          "file": "transport_catalogue.db"
      },
      "base_requests": [
          {
              "is_roundtrip": false,
              "name": "Центральная",
              "stops": [
                  "Авиастроительная",
                  "Северный вокзал",
                  "Яшьлек",
                  "Козья Слобода",
                  "Кремлёвская",
                  "Площадь Тукая",
                  "Суконная Слобода",
                  "Аметьево",
                  "Горки",
                  "Проспект Победы",
                  "Дубравная"
              ],
              "type": "Bus"
          },
          {
              "is_roundtrip": false,
              "name": "Строящийся участок",
              "stops": [
                  "100-летие ТАССР",
                  "Академическая",
                  "Зилант",
                  "Тулпар"
              ],
              "type": "Bus"
          },
          {
              "latitude": 55.858825,
              "longitude": 49.084693,
              "name": "Авиастроительная",
              "road_distances": {
                  "Северный вокзал": 1800
              },
              "type": "Stop"
          },
          {
              "latitude": 55.845023,
              "longitude": 49.083829,
              "name": "Северный вокзал",
              "road_distances": {
                  "Яшьлек": 1500
              },
              "type": "Stop"
          },
    	  {
              "latitude": 55.828160,
              "longitude": 49.082266,
              "name": "Яшьлек",
              "road_distances": {
                  "Козья Слобода": 1600
              },
              "type": "Stop"
          },
          {
              "latitude": 55.816609,
              "longitude": 49.098321,
              "name": "Козья Слобода",
              "road_distances": {
                  "Кремлёвская": 2500
              },
              "type": "Stop"
          },
          {
              "latitude": 55.795626,
              "longitude": 49.106438,
              "name": "Кремлёвская",
              "road_distances": {
                  "Площадь Тукая": 1600
              },
              "type": "Stop"
          },
          {
              "latitude": 55.785966,
              "longitude": 49.124671,
              "name": "Площадь Тукая",
              "road_distances": {
                  "Суконная Слобода": 1500
              },
              "type": "Stop"
          },
          {
              "latitude": 55.777075,
              "longitude": 49.142630,
              "name": "Суконная Слобода",
              "road_distances": {
                  "Аметьево": 2000
              },
              "type": "Stop"
          },
          {
              "latitude": 55.765126,
              "longitude": 49.166573,
              "name": "Аметьево",
              "road_distances": {
                  "Горки": 1650
              },
              "type": "Stop"
          },
          {
              "latitude": 55.760236,
              "longitude": 49.190912,
              "name": "Горки",
              "road_distances": {
                  "Проспект Победы": 1580
              },
              "type": "Stop"
          },
          {
              "latitude": 55.749939,
              "longitude": 49.208749,
              "name": "Проспект Победы",
              "road_distances": {
                  "Дубравная": 1000
              },
              "type": "Stop"
          },
          {
              "latitude": 55.743684,
              "longitude": 49.219149,
              "name": "Дубравная",
              "road_distances": {
              	"100-летие ТАССР": 500
              },
              "type": "Stop"
          },
          {
              "latitude": 55.747241,
              "longitude": 49.221042,
              "name": "100-летие ТАССР",
              "road_distances": {
              	"Академическая": 1250
              },
              "type": "Stop"
          },
          {
              "latitude": 55.757704,
              "longitude": 49.231959,
              "name": "Академическая",
              "road_distances": {
              	"Зилант": 1000
              },
              "type": "Stop"
          },
          {
              "latitude": 55.766988,
              "longitude": 49.231439,
              "name": "Зилант",
              "road_distances": {
              	"Тулпар": 1000
              },
              "type": "Stop"
          },
    	  {
              "latitude": 55.776220,
              "longitude": 49.231924,
              "name": "Тулпар",
              "road_distances": {},
              "type": "Stop"
          }
      ],
      "render_settings": {
          "bus_label_font_size": 15,
          "bus_label_offset": [
              7,
              15
          ],
          "color_palette": [
              "blue",
              "red",
              "green",
              [
                  255,
                  160,
                  0
              ]
          ],
          "height": 1024,
          "line_width": 14,
          "padding": 30,
          "stop_label_font_size": 15,
          "stop_label_offset": [
              7,
              -2
          ],
          "stop_radius": 5,
          "underlayer_color": [
              255,
              255,
              255,
              0.85
          ],
          "underlayer_width": 3,
          "width": 1024
      },
      "routing_settings": {
          "bus_velocity": 44,
          "bus_wait_time": 5
      }
  }
```
</details>

### Использование сформированного транспортного каталога
Запустите собранную программу с ключом `process_requests` : `./transport_catalogue process_requests < requests.json > result.json`\
Программа прочитает файл `requests.json`. В данном файле в настройках `"serialization_settings"` должно быть указано имя существующего файла с двоичным представлением сформированного транспортного каталога.
После "развертывания" каталога из двоичного файла, программа последовательно обойдет запросы из `"stat_requests"` и сохранит сформированные ответы в файл `result.json`

<details>
  <summary>Пример корректного файла process_requests.json:</summary>

```json 
  {
      "serialization_settings": {
          "file": "transport_catalogue.db"
      },
      "stat_requests": [
          {
              "id": 218563507,
              "type": "Bus",
              "name": "Центральная"
          },
          {
              "id": 21854324,
              "type": "Bus",
              "name": "Строящийся участок"
          },
	  {
              "id": 2185475,
              "type": "Bus",
              "name": "Новый"
          },
          {
              "id": 508658276,
              "type": "Stop",
              "name": "Авиастроительная"
          },
    	  {
              "id": 508658276,
              "type": "Stop",
              "name": "Площадь Тукая"
          },
          {
              "id": 1964680131,
              "type": "Route",
              "from": "Авиастроительная",
              "to": "Площадь Тукая"
          },
          {
              "id": 324124,
              "type": "Route",
              "from": "Площадь Тукая",
              "to": "Дубравная"
          },
    	  {
              "id": 98765235,
              "type": "Route",
              "from": "Авиастроительная",
              "to": "Дубравная"
          },
          {
              "id": 1359372752,
              "type": "Map"
          }
      ]
  }
```
</details>

<details>
  <summary>Пример вывода result.json:</summary>

```json
[
    {
        "curvature": 1.60481,
        "request_id": 218563507,
        "route_length": 11230,
        "stop_count": 8,
        "unique_stop_count": 7
    },
    {
        "buses": [
            "14",
            "24"
        ],
        "request_id": 508658276
    },
    {
        "items": [
            {
                "stop_name": "Морской вокзал",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "114",
                "span_count": 1,
                "time": 1.7,
                "type": "Bus"
            },
            {
                "stop_name": "Ривьерский мост",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "14",
                "span_count": 4,
                "time": 6.06,
                "type": "Bus"
            },
            {
                "stop_name": "Улица Докучаева",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "24",
                "span_count": 1,
                "time": 2.2,
                "type": "Bus"
            }
        ],
        "request_id": 1964680131,
        "total_time": 15.96
    },
    {
        "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"125.25,382.708 74.2702,281.925 125.25,382.708\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"592.058,238.297 311.644,93.2643 74.2702,281.925 267.446,450 317.457,442.562 365.599,429.138 367.969,320.138 592.058,238.297\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"367.969,320.138 350.791,243.072 311.644,93.2643 50,50 311.644,93.2643 350.791,243.072 367.969,320.138\" fill=\"none\" stroke=\"red\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"green\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"green\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <text fill=\"rgb(255,160,0)\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"red\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"red\" x=\"50\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <circle cx=\"267.446\" cy=\"450\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"317.457\" cy=\"442.562\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"125.25\" cy=\"382.708\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"350.791\" cy=\"243.072\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"365.599\" cy=\"429.138\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"74.2702\" cy=\"281.925\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"50\" cy=\"50\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"367.969\" cy=\"320.138\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"592.058\" cy=\"238.297\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"311.644\" cy=\"93.2643\" r=\"5\" fill=\"white\"/>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"267.446\" y=\"450\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Гостиница Сочи</text>\n  <text fill=\"black\" x=\"267.446\" y=\"450\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Гостиница Сочи</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"317.457\" y=\"442.562\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Кубанская улица</text>\n  <text fill=\"black\" x=\"317.457\" y=\"442.562\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Кубанская улица</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Морской вокзал</text>\n  <text fill=\"black\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Морской вокзал</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"350.791\" y=\"243.072\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Параллельная улица</text>\n  <text fill=\"black\" x=\"350.791\" y=\"243.072\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Параллельная улица</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"365.599\" y=\"429.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">По требованию</text>\n  <text fill=\"black\" x=\"365.599\" y=\"429.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">По требованию</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Ривьерский мост</text>\n  <text fill=\"black\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Ривьерский мост</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Санаторий Родина</text>\n  <text fill=\"black\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Санаторий Родина</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Улица Докучаева</text>\n  <text fill=\"black\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Улица Докучаева</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Улица Лизы Чайкиной</text>\n  <text fill=\"black\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Улица Лизы Чайкиной</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"311.644\" y=\"93.2643\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Электросети</text>\n  <text fill=\"black\" x=\"311.644\" y=\"93.2643\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Электросети</text>\n</svg>",
        "request_id": 1359372752
    }
]
```
</details>
