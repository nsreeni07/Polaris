/* Ring buffer in RAM, flushed to SD periodically via FatFS. Requires
 * FatFS middleware added on top of SDMMC1 in CubeMX. */
#include "datalog.h"
#include "app_config.h"
#include "fatfs.h"
#include <string.h>

static uint8_t  s_ring[LOG_RING_BYTES];
static uint32_t s_head, s_tail;
static FIL      s_file;
static uint8_t  s_file_ok;

int datalog_init(void)
{
    s_head = s_tail = 0;
    s_file_ok = 0;

    if (f_open(&s_file, LOG_FILENAME, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
        return -1;

    s_file_ok = 1;
    return 0;
}

void datalog_write(const log_record_t *rec)
{
    if (!s_file_ok)
        return;

    uint32_t len = sizeof(*rec);
    uint32_t free = LOG_RING_BYTES - (s_head - s_tail);
    if (len > free)
        return;   /* ring full - drop, don't stall the control loop */

    for (uint32_t i = 0; i < len; i++)
        s_ring[(s_head + i) % LOG_RING_BYTES] = ((const uint8_t *)rec)[i];
    s_head += len;
}

void datalog_flush(void)
{
    if (!s_file_ok || s_head == s_tail)
        return;

    uint32_t len = s_head - s_tail;
    uint32_t start = s_tail % LOG_RING_BYTES;
    uint32_t first_chunk = LOG_RING_BYTES - start;
    if (first_chunk > len) first_chunk = len;

    UINT written;
    f_write(&s_file, &s_ring[start], first_chunk, &written);
    if (len > first_chunk)
        f_write(&s_file, &s_ring[0], len - first_chunk, &written);

    f_sync(&s_file);
    s_tail = s_head;
}
