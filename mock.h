#include <trompeloeil.hpp>
#include "class.h"

struct Mock : public Class {
MAKE_MOCK0(bar, void());
};
