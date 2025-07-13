#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/usb/usb_device.h>

#define MCP3204_CHANNELS 4

static const struct device *adc_dev;
static int16_t sample_buf;

int main(void)
{
    int err;

    (void)usb_enable(NULL);	// Enable USB serial

    k_sleep(K_MSEC(1000));

    adc_dev = DEVICE_DT_GET_ANY(microchip_mcp3204);
    if (!device_is_ready(adc_dev)) {
        printk("Error: MCP3204 device not ready\n");
        return -1;
    }

    printk("Starting ADC reads from MCP3204\n");

    while (1) {
        for (int ch = 0; ch < MCP3204_CHANNELS; ++ch) {
            struct adc_sequence sequence = {
                .channels     = BIT(ch),
                //.resolution   = 12,
                .resolution   = 12,
                .buffer       = &sample_buf,
                .buffer_size  = sizeof(sample_buf),
            };

            err = adc_read(adc_dev, &sequence);
            if (err < 0) {
                printk("CH%d: %d Error!\n", ch, err);
            } else {
                printk("CH%d: %d\n", ch, sample_buf);
            }
        }

        k_sleep(K_MSEC(500));
    }

    return 0;
}

