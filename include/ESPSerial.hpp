#pragma once

#include "Transport.hpp"

#include <driver/uart.h>
#include <esp_timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace AVR
{

class ESPSerial : public Transport
{
public:
    ESPSerial(uart_port_t port, uart_config_t config, size_t buffer_size, int timeout, int tx_pin, int rx_pin, int rts_pin, int cts_pin)
      : port{port},
        config{config},
        buffer_size{buffer_size},
        timeout{timeout},
        tx_pin{tx_pin},
        rx_pin{rx_pin},
        rts_pin{rts_pin},
        cts_pin{cts_pin}
    {
    }

    void connect() const override
    {
        ESP_ERROR_CHECK(uart_param_config(port, &config));
        ESP_ERROR_CHECK(uart_set_pin(port, tx_pin, rx_pin, rts_pin, cts_pin));
        ESP_ERROR_CHECK(uart_driver_install(port, buffer_size, buffer_size, 10, (void**)&queue, 0));
    }

    void write(const std::vector<uint8_t> &data) const override
    {
        ESP_ERROR_CHECK(uart_write_bytes(port, reinterpret_cast<const char *>(data.data()), data.size()));
    }

    std::vector<uint8_t> read(size_t max_length, bool wait_for_all) const override
    {
        unsigned long start_time = esp_timer_get_time();

        size_t data_available;
        bool ready_to_read = false;
        do {
            ESP_ERROR_CHECK(uart_get_buffered_data_len(port, &data_available));

            if (data_available >= max_length) {
                ready_to_read = true;
            } else if (!wait_for_all && data_available > 0) {
                ready_to_read = true;
            }
        } while ( esp_timer_get_time() - start_time < timeout * 1000
                  && !ready_to_read );

        if (!ready_to_read) {
            return std::vector<uint8_t>{};
        }

        std::vector<uint8_t> data(data_available);
        auto read_length = uart_read_bytes(port, data.data(), data.size(), timeout * portTICK_PERIOD_MS);
        data.resize(read_length < 0 ? 0 : read_length);

        return data;
    }

    void flush() const override
    {
        uart_flush(port);
    }

private:
    const uart_port_t port;
    const uart_config_t config;
    const size_t buffer_size;
    const int timeout; // ms
    const int tx_pin;
    const int rx_pin;
    const int rts_pin;
    const int cts_pin;

    QueueHandle_t queue;
};

} // namespace AVR
