#include "board.h"

#include "picoRTOS.h"
#include "picoRTOS_device.h"

#include "clock-lgt8fx8p.h"
#include "gpio-avr.h"
#include "mux-avr.h"
#include "spi-avr_irqdriven.h"
#include "wd-avr.h"

static void init_clock(void)
{
  struct clock_settings CLOCK_settings = {
    0, /* no xtal freq */
    CLOCK_LGT8FX8P_MCLK_HFRC,
    CLOCK_LGT8FX8P_PS_1,
    CLOCK_LGT8FX8P_WDCLKS_LFRC
  };

  (void)clock_lgt8fx8p_init(&CLOCK_settings);
}

static void init_mux(void)
{
    struct mux PORTB;
    struct mux PORTD;

    (void)mux_avr_init(&PORTB, ADDR_PORTB);
    (void)mux_avr_init(&PORTD, ADDR_PORTD);

    (void)mux_avr_output(&PORTB, (size_t)5, false); /* SCK */
    (void)mux_avr_input(&PORTB, (size_t)4);         /* MISO */
    (void)mux_avr_output(&PORTB, (size_t)3, false); /* MOSI */
    (void)mux_avr_output(&PORTB, (size_t)2, true);  /* _SS */

    (void)mux_avr_output(&PORTD, (size_t)2, false); /* BLK */
    (void)mux_avr_output(&PORTD, (size_t)3, false); /* DC */
    (void)mux_avr_output(&PORTD, (size_t)4, false); /* RES */
}

static void init_spi(/*@partial@*/ struct board *ctx)
{
    static struct spi SPI;
    struct spi_settings SPI_settings = {
        2000000ul,                           /* bitrate */
        SPI_MODE_MASTER,
        SPI_CLOCK_MODE_0,
        (size_t)16,                           /* frame size */
        SPI_CS_POL_IGNORE,
        (size_t)0,
    };

    (void)spi_avr_init(&SPI, ADDR_SPI, CLOCK_LGT8FX8P_PERI_CLK);
    (void)spi_setup(&SPI, &SPI_settings);
    
#if CONFIG_SPI_AVR_IRQDRIVEN == 1
    static uint8_t rx_buf[32];
    static uint8_t tx_buf[32];

    /* IRQ mode */
    struct spi_avr_irqdriven_settings IRQ_settings = {
      (picoRTOS_irq_t)IRQ_SPI_STC,
      rx_buf,
      tx_buf,
      sizeof(rx_buf) - 1,
    };
    
    (void)spi_avr_setup_irqdriven(&SPI, &IRQ_settings);
#endif
    
    /* physical layer */
    ctx->lcd_phys.spi = &SPI;
}

static void init_gpio(/*@partial@*/ struct board *ctx)
{
    static struct gpio LED;
    static struct gpio DC;
    static struct gpio RST;
    static struct gpio CS;
    static struct gpio BL;

    struct gpio_settings LED_settings = {
        GPIO_INVERT_ENABLE,
    };

    /* LEDs */
    (void)gpio_avr_init(&LED, ADDR_PORTD, (size_t)5); /* don't care */
    (void)gpio_setup(&LED, &LED_settings);
    /* LCD */
    (void)gpio_avr_init(&DC, ADDR_PORTD, (size_t)3);
    (void)gpio_avr_init(&CS, ADDR_PORTB, (size_t)2);
    (void)gpio_avr_init(&RST, ADDR_PORTD, (size_t)4);
    (void)gpio_avr_init(&BL, ADDR_PORTD, (size_t)2);

    /* leds */
    ctx->LED = &LED;

    /* physical layer */
    ctx->lcd_phys.dc = &DC;
    ctx->lcd_phys.rst = &RST;
    ctx->lcd_phys.cs = &CS;

    /* force BL on */
    gpio_write(&BL, true);
}

static void init_wd(/*@partial@*/ struct board *ctx)
{
    static struct wd WD;
    struct wd_avr_settings WD_settings = {
        WD_AVR_CYCLES_16K,
    };

    (void)wd_avr_init(&WD, ADDR_WDT);
    (void)wd_avr_setup(&WD, &WD_settings);

    (void)wd_start(&WD);
    ctx->WD = &WD;
}

int board_init(/*@out@*/ struct board *ctx)
{
  // init_clock();
    init_mux();
    
    init_spi(ctx);
    init_gpio(ctx);
    init_wd(ctx);

    return 0;
}
