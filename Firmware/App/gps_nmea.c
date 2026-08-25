/* USART6 ISR-fed ring buffer + minimal GGA parser. Compiled out
 * unless ENABLE_GPS=1; USART6 must be added to the .ioc first. */
#include "gps_nmea.h"
#include "board.h"
#include <string.h>
#include <stdlib.h>

#define RX_BUF_LEN 128
static volatile uint8_t s_rx_buf[RX_BUF_LEN];
static volatile uint16_t s_rx_head;
static uint16_t s_rx_tail;
static char s_line[96];
static uint16_t s_line_len;
static gps_fix_t s_fix;

void gps_init(void)
{
    s_rx_head = s_rx_tail = 0;
    s_line_len = 0;
    memset(&s_fix, 0, sizeof(s_fix));
    HAL_UART_Receive_IT(&huart6, (uint8_t *)&s_rx_buf[0], 1);
}

/* call from HAL_UART_RxCpltCallback for huart6 */
void gps_rx_isr_push(uint8_t byte)
{
    uint16_t next = (s_rx_head + 1) % RX_BUF_LEN;
    if (next != s_rx_tail) {
        s_rx_buf[s_rx_head] = byte;
        s_rx_head = next;
    }
    HAL_UART_Receive_IT(&huart6, (uint8_t *)&s_rx_buf[s_rx_head], 1);
}

static float nmea_to_deg(const char *field, char dir)
{
    float raw = (float)atof(field);
    int deg_len = (dir == 'N' || dir == 'S') ? 2 : 3;
    char deg_buf[4] = {0};
    strncpy(deg_buf, field, deg_len);
    float deg = (float)atoi(deg_buf);
    float min = raw - deg * (deg_len == 2 ? 100.0f : 100.0f) / 100.0f * 100.0f;
    float result = deg + (raw - deg * 100.0f) / 60.0f;
    (void)min;
    if (dir == 'S' || dir == 'W') result = -result;
    return result;
}

static void parse_gga(char *line)
{
    char *tok = strtok(line, ",");
    int field = 0;
    char lat[16] = {0}, lon[16] = {0};
    char lat_dir = 0, lon_dir = 0;
    int fix_q = 0;
    char alt[16] = {0};

    while (tok) {
        switch (field) {
            case 2: strncpy(lat, tok, sizeof(lat) - 1); break;
            case 3: lat_dir = tok[0]; break;
            case 4: strncpy(lon, tok, sizeof(lon) - 1); break;
            case 5: lon_dir = tok[0]; break;
            case 6: fix_q = atoi(tok); break;
            case 9: strncpy(alt, tok, sizeof(alt) - 1); break;
        }
        tok = strtok(NULL, ",");
        field++;
    }

    s_fix.fix = (fix_q > 0);
    if (s_fix.fix) {
        s_fix.lat = nmea_to_deg(lat, lat_dir);
        s_fix.lon = nmea_to_deg(lon, lon_dir);
        s_fix.alt_m = (float)atof(alt);
    }
}

void gps_poll(void)
{
    while (s_rx_tail != s_rx_head) {
        uint8_t c = s_rx_buf[s_rx_tail];
        s_rx_tail = (s_rx_tail + 1) % RX_BUF_LEN;

        if (c == '\n') {
            s_line[s_line_len] = '\0';
            if (strncmp(s_line, "$GPGGA", 6) == 0 || strncmp(s_line, "$GNGGA", 6) == 0)
                parse_gga(s_line);
            s_line_len = 0;
        } else if (c != '\r' && s_line_len < sizeof(s_line) - 1) {
            s_line[s_line_len++] = (char)c;
        }
    }
}

gps_fix_t gps_get_fix(void) { return s_fix; }
