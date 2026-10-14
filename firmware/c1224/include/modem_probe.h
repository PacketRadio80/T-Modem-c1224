#ifndef C1224_MODEM_PROBE_H
#define C1224_MODEM_PROBE_H

#include <stdbool.h>

typedef struct {
    bool crystal_driven;
    bool cdt_readable;
    bool rxd_readable;
    bool ready;
} modem_probe_result_t;

modem_probe_result_t modem_probe_run(void);

#endif
