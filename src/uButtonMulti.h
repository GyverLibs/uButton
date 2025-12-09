#pragma once
#include "uButton.h"

class uButtonMulti : public uButtonVirt {
   public:
    bool tick(uButton& b0, uButton& b1) {
        if (_press) {
            if (!b0.pressing() && !b1.pressing()) _press = 0;
            if (!b0.pressing()) b0.reset();
            if (!b1.pressing()) b1.reset();
            return pollRaw(true);
        } else {
            if (b0.pressing() && b1.pressing()) {
                b0.skipEvents();
                b1.skipEvents();
                _press = 1;
            }
            return pollRaw(false);
        }
    }

   private:
    using uButtonVirt::poll;
    using uButtonVirt::pollRaw;
};