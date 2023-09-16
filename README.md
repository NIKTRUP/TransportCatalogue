# TransportCatalogue

TransportCatalogue — система обработки и хранения транспортных маршрутов. Работает через потоки ввода-вывода, считывает запросы и выдает результат в формате JSON.

## Основные функции:
 - создание базы транспортного справочника и её сериализация в файл;
 - десериализация базы из файла;
 - получение информации о маршруте;
 - получение информации об остановке;
 - поиск оптимального маршрута между двумя остановками;
 - визуализация карты маршрутов.

## Использование программы
Использование `transport_catalogue` условно делится на 2 этапа:
1. Создание базы маршрутов (если её ещё нет)
2. Отправка запросов 

Для создания базы транспортного справочника и ее сериализации в файл по запросам `base_requests` необходимо запустить программу с параметром `make_base`, указав при этом входной JSON-файл.  
Пример запуска программы для создания базы: 
`./transport_catalogue make_base < <имя файла>.json`. Само собой, можно осуществлять ввод напрямую в терминал, не используя перенаправление ввода/вывода.

Для того, чтобы использовать полученную базу и десериализовать ее для ответов на запросы `stat_requests` нужно запустить программу с параметром `process_requests`, указав входной JSON-файл, содержащий запросы к БД.
Пример запуска программы для выполнения запросов к базе:  
`./transport_catalogue process_requests < <имя файла>.json > <имя файла>.json` (первый параметр — файл с запросами,
\
второй — файл с ответами на запросы).

Файл, формирующий каталог маршрутов (будем называть его `make_base.json`), должен представлять собой словарь JSON со следующими ключами:

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

Файл, содержащий запросы (будем называть его `process_requests.json`), должен представлять собой словарь JSON со следующими ключами :
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



#### Запрос на построение маршрута между двумя остановками
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
Запустите собранную программу с ключом `make_base` : `./transport_catalogue make_base < make_base.json`\
Программа прочитает файл `make_base.json` и сформирует на его основе транспортный каталог.
В папке с программой появится файл `transport_catalogue.db` (или другой, в зависимости от того, какое название будет указано в `"serialization_settings"`). В данном файле будет сохранен каталог в двоичном виде.\
В дальнейшем этот сохраненный каталог можно будет "разворачивать" для формирования ответов на запросы, без необходимости строить его заново.

<details>
  <summary>Пример корректного файла make_base.json:</summary>

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
Запустите собранную программу с ключом `process_requests` : \
`./transport_catalogue process_requests < process_requests.json > result.json`\
Программа прочитает файл `process_requests.json`. В данном файле в настройках `"serialization_settings"` должно быть указано имя существующего файла с двоичным представлением сформированного транспортного каталога.
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

Если программа работает правильно, то вы получите такой результат:

<details>
  <summary>Пример вывода result.json:</summary>

```json
[
    {
        "curvature": 1.004,
        "request_id": 218563507,
        "route_length": 33460,
        "stop_count": 21,
        "unique_stop_count": 11
    },
    {
        "curvature": 0.953353,
        "request_id": 21854324,
        "route_length": 6500,
        "stop_count": 7,
        "unique_stop_count": 4
    },
    {
        "error_message": "not found",
        "request_id": 2185475
    },
    {
        "buses": [
            "Центральная"
        ],
        "request_id": 508658276
    },
    {
        "buses": [
            "Центральная"
        ],
        "request_id": 508658276
    },
    {
        "items": [
            {
                "stop_name": "Авиастроительная",
                "time": 5,
                "type": "Wait"
            },
            {
                "bus": "Центральная",
                "span_count": 5,
                "time": 12.2727,
                "type": "Bus"
            }
        ],
        "request_id": 1964680131,
        "total_time": 17.2727
    },
    {
        "items": [
            {
                "stop_name": "Площадь Тукая",
                "time": 5,
                "type": "Wait"
            },
            {
                "bus": "Центральная",
                "span_count": 5,
                "time": 10.5409,
                "type": "Bus"
            }
        ],
        "request_id": 324124,
        "total_time": 15.5409
    },
    {
        "items": [
            {
                "stop_name": "Авиастроительная",
                "time": 5,
                "type": "Wait"
            },
            {
                "bus": "Центральная",
                "span_count": 10,
                "time": 22.8136,
                "type": "Bus"
            }
        ],
        "request_id": 98765235,
        "total_time": 27.8136
    },
    {
        "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"923.696,748.584 994,681.204 990.651,621.416 993.775,561.964 990.651,621.416 994,681.204 923.696,748.584\" fill=\"none\" stroke=\"blue\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"45.6295,30 40.0655,118.883 30,227.478 133.392,301.865 185.664,436.992 303.082,499.201 418.735,556.457 572.924,633.407 729.664,664.898 844.531,731.209 911.506,771.49 844.531,731.209 729.664,664.898 572.924,633.407 418.735,556.457 303.082,499.201 185.664,436.992 133.392,301.865 30,227.478 40.0655,118.883 45.6295,30\" fill=\"none\" stroke=\"red\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"923.696\" y=\"748.584\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Строящийся участок</text>\n  <text fill=\"blue\" x=\"923.696\" y=\"748.584\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Строящийся участок</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"993.775\" y=\"561.964\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Строящийся участок</text>\n  <text fill=\"blue\" x=\"993.775\" y=\"561.964\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Строящийся участок</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"45.6295\" y=\"30\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Центральная</text>\n  <text fill=\"red\" x=\"45.6295\" y=\"30\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Центральная</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"911.506\" y=\"771.49\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Центральная</text>\n  <text fill=\"red\" x=\"911.506\" y=\"771.49\" dx=\"7\" dy=\"15\" font-size=\"15\" font-family=\"Verdana\" font-weight=\"bold\">Центральная</text>\n  <circle cx=\"923.696\" cy=\"748.584\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"45.6295\" cy=\"30\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"994\" cy=\"681.204\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"572.924\" cy=\"633.407\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"729.664\" cy=\"664.898\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"911.506\" cy=\"771.49\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"990.651\" cy=\"621.416\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"133.392\" cy=\"301.865\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"185.664\" cy=\"436.992\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"303.082\" cy=\"499.201\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"844.531\" cy=\"731.209\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"40.0655\" cy=\"118.883\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"418.735\" cy=\"556.457\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"993.775\" cy=\"561.964\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"30\" cy=\"227.478\" r=\"5\" fill=\"white\"/>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"923.696\" y=\"748.584\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">100-летие ТАССР</text>\n  <text fill=\"black\" x=\"923.696\" y=\"748.584\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">100-летие ТАССР</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"45.6295\" y=\"30\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Авиастроительная</text>\n  <text fill=\"black\" x=\"45.6295\" y=\"30\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Авиастроительная</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"994\" y=\"681.204\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Академическая</text>\n  <text fill=\"black\" x=\"994\" y=\"681.204\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Академическая</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"572.924\" y=\"633.407\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Аметьево</text>\n  <text fill=\"black\" x=\"572.924\" y=\"633.407\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Аметьево</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"729.664\" y=\"664.898\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Горки</text>\n  <text fill=\"black\" x=\"729.664\" y=\"664.898\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Горки</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"911.506\" y=\"771.49\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Дубравная</text>\n  <text fill=\"black\" x=\"911.506\" y=\"771.49\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Дубравная</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"990.651\" y=\"621.416\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Зилант</text>\n  <text fill=\"black\" x=\"990.651\" y=\"621.416\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Зилант</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"133.392\" y=\"301.865\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Козья Слобода</text>\n  <text fill=\"black\" x=\"133.392\" y=\"301.865\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Козья Слобода</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"185.664\" y=\"436.992\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Кремлёвская</text>\n  <text fill=\"black\" x=\"185.664\" y=\"436.992\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Кремлёвская</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"303.082\" y=\"499.201\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Площадь Тукая</text>\n  <text fill=\"black\" x=\"303.082\" y=\"499.201\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Площадь Тукая</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"844.531\" y=\"731.209\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Проспект Победы</text>\n  <text fill=\"black\" x=\"844.531\" y=\"731.209\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Проспект Победы</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"40.0655\" y=\"118.883\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Северный вокзал</text>\n  <text fill=\"black\" x=\"40.0655\" y=\"118.883\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Северный вокзал</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"418.735\" y=\"556.457\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Суконная Слобода</text>\n  <text fill=\"black\" x=\"418.735\" y=\"556.457\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Суконная Слобода</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"993.775\" y=\"561.964\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Тулпар</text>\n  <text fill=\"black\" x=\"993.775\" y=\"561.964\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Тулпар</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"30\" y=\"227.478\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Яшьлек</text>\n  <text fill=\"black\" x=\"30\" y=\"227.478\" dx=\"7\" dy=\"-2\" font-size=\"15\" font-family=\"Verdana\">Яшьлек</text>\n</svg>",
        "request_id": 1359372752
    }
]
``` 
</details>

Если вы разэкранируете `map` и вставите его в файл с расширением `.svg` (или воспользуетесь [сервисом](https://www.freecodeformat.com/svg-editor.php)), то получите `svg` изображение:
<details>
<summary>Разэкранированный map</summary>
 
```xml
<?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
  <polyline points="923.696,748.584 994,681.204 990.651,621.416 993.775,561.964 990.651,621.416 994,681.204 923.696,748.584" fill="none" stroke="blue" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="45.6295,30 40.0655,118.883 30,227.478 133.392,301.865 185.664,436.992 303.082,499.201 418.735,556.457 572.924,633.407 729.664,664.898 844.531,731.209 911.506,771.49 844.531,731.209 729.664,664.898 572.924,633.407 418.735,556.457 303.082,499.201 185.664,436.992 133.392,301.865 30,227.478 40.0655,118.883 45.6295,30" fill="none" stroke="red" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="923.696" y="748.584" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Строящийся участок</text>
  <text fill="blue" x="923.696" y="748.584" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Строящийся участок</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="993.775" y="561.964" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Строящийся участок</text>
  <text fill="blue" x="993.775" y="561.964" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Строящийся участок</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="45.6295" y="30" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Центральная</text>
  <text fill="red" x="45.6295" y="30" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Центральная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="911.506" y="771.49" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Центральная</text>
  <text fill="red" x="911.506" y="771.49" dx="7" dy="15" font-size="15" font-family="Verdana" font-weight="bold">Центральная</text>
  <circle cx="923.696" cy="748.584" r="5" fill="white"/>
  <circle cx="45.6295" cy="30" r="5" fill="white"/>
  <circle cx="994" cy="681.204" r="5" fill="white"/>
  <circle cx="572.924" cy="633.407" r="5" fill="white"/>
  <circle cx="729.664" cy="664.898" r="5" fill="white"/>
  <circle cx="911.506" cy="771.49" r="5" fill="white"/>
  <circle cx="990.651" cy="621.416" r="5" fill="white"/>
  <circle cx="133.392" cy="301.865" r="5" fill="white"/>
  <circle cx="185.664" cy="436.992" r="5" fill="white"/>
  <circle cx="303.082" cy="499.201" r="5" fill="white"/>
  <circle cx="844.531" cy="731.209" r="5" fill="white"/>
  <circle cx="40.0655" cy="118.883" r="5" fill="white"/>
  <circle cx="418.735" cy="556.457" r="5" fill="white"/>
  <circle cx="993.775" cy="561.964" r="5" fill="white"/>
  <circle cx="30" cy="227.478" r="5" fill="white"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="923.696" y="748.584" dx="7" dy="-2" font-size="15" font-family="Verdana">100-летие ТАССР</text>
  <text fill="black" x="923.696" y="748.584" dx="7" dy="-2" font-size="15" font-family="Verdana">100-летие ТАССР</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="45.6295" y="30" dx="7" dy="-2" font-size="15" font-family="Verdana">Авиастроительная</text>
  <text fill="black" x="45.6295" y="30" dx="7" dy="-2" font-size="15" font-family="Verdana">Авиастроительная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="994" y="681.204" dx="7" dy="-2" font-size="15" font-family="Verdana">Академическая</text>
  <text fill="black" x="994" y="681.204" dx="7" dy="-2" font-size="15" font-family="Verdana">Академическая</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="572.924" y="633.407" dx="7" dy="-2" font-size="15" font-family="Verdana">Аметьево</text>
  <text fill="black" x="572.924" y="633.407" dx="7" dy="-2" font-size="15" font-family="Verdana">Аметьево</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="729.664" y="664.898" dx="7" dy="-2" font-size="15" font-family="Verdana">Горки</text>
  <text fill="black" x="729.664" y="664.898" dx="7" dy="-2" font-size="15" font-family="Verdana">Горки</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="911.506" y="771.49" dx="7" dy="-2" font-size="15" font-family="Verdana">Дубравная</text>
  <text fill="black" x="911.506" y="771.49" dx="7" dy="-2" font-size="15" font-family="Verdana">Дубравная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="990.651" y="621.416" dx="7" dy="-2" font-size="15" font-family="Verdana">Зилант</text>
  <text fill="black" x="990.651" y="621.416" dx="7" dy="-2" font-size="15" font-family="Verdana">Зилант</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="133.392" y="301.865" dx="7" dy="-2" font-size="15" font-family="Verdana">Козья Слобода</text>
  <text fill="black" x="133.392" y="301.865" dx="7" dy="-2" font-size="15" font-family="Verdana">Козья Слобода</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="185.664" y="436.992" dx="7" dy="-2" font-size="15" font-family="Verdana">Кремлёвская</text>
  <text fill="black" x="185.664" y="436.992" dx="7" dy="-2" font-size="15" font-family="Verdana">Кремлёвская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="303.082" y="499.201" dx="7" dy="-2" font-size="15" font-family="Verdana">Площадь Тукая</text>
  <text fill="black" x="303.082" y="499.201" dx="7" dy="-2" font-size="15" font-family="Verdana">Площадь Тукая</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="844.531" y="731.209" dx="7" dy="-2" font-size="15" font-family="Verdana">Проспект Победы</text>
  <text fill="black" x="844.531" y="731.209" dx="7" dy="-2" font-size="15" font-family="Verdana">Проспект Победы</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="40.0655" y="118.883" dx="7" dy="-2" font-size="15" font-family="Verdana">Северный вокзал</text>
  <text fill="black" x="40.0655" y="118.883" dx="7" dy="-2" font-size="15" font-family="Verdana">Северный вокзал</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="418.735" y="556.457" dx="7" dy="-2" font-size="15" font-family="Verdana">Суконная Слобода</text>
  <text fill="black" x="418.735" y="556.457" dx="7" dy="-2" font-size="15" font-family="Verdana">Суконная Слобода</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="993.775" y="561.964" dx="7" dy="-2" font-size="15" font-family="Verdana">Тулпар</text>
  <text fill="black" x="993.775" y="561.964" dx="7" dy="-2" font-size="15" font-family="Verdana">Тулпар</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="30" y="227.478" dx="7" dy="-2" font-size="15" font-family="Verdana">Яшьлек</text>
  <text fill="black" x="30" y="227.478" dx="7" dy="-2" font-size="15" font-family="Verdana">Яшьлек</text>
</svg>
```
</details>

<figure>
  <img
  src="https://github.com/NIKTRUP/cpp-transport-catalogue/assets/72292425/a60d2401-c36c-4342-96f7-245471b8ae6d"
  alt="Svg изображение не отобразилось">
  <figcaption>Схема будущего метро города Казань</figcaption>
</figure>

