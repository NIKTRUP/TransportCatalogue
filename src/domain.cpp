#include "include/domain.h"

namespace tc {
    namespace domain {

        bool operator==(const Route &lhs, const Route &rhs) {
            return (lhs.name == rhs.name);
        }

        bool operator==(const Stop &lhs, const Stop &rhs) {
            return (lhs.name == rhs.name && lhs.coordinate == rhs.coordinate);
        }
    }
}
