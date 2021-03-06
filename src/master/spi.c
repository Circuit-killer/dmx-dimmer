// spi.c

#include "spi.h"

#include "master.h"

inline void spi_master_init (void) {
    // debug: pull up so no fake interrupt occurs
    output_high(SPI_SLAVES_PORT, SPI_OUT_SS1);
    output_high(SPI_SLAVES_PORT, SPI_OUT_SS2);
    output_high(SPI_SLAVES_PORT, SPI_OUT_SS3);

    // set up slave select, channel select and "ok i'm ready" lines
    set_output(SPI_SLAVES_DDR, SPI_OUT_CHAN0_DDR);
    set_output(SPI_SLAVES_DDR, SPI_OUT_CHAN1_DDR);
    set_output(SPI_SLAVES_DDR, SPI_OUT_SS1_DDR);
    set_output(SPI_SLAVES_DDR, SPI_OUT_SS2_DDR);
    set_output(SPI_SLAVES_DDR, SPI_OUT_SS3_DDR);
    set_input(SPI_SLAVES_DDR, SPI_OUT_OK1_DDR);
    set_input(SPI_SLAVES_OUTOK_DDR, SPI_OUT_OK2_DDR);
    set_input(SPI_SLAVES_OUTOK_DDR, SPI_OUT_OK3_DDR);

    // pull-ups on inputs (hi-z otherwise)
    input_pullup(SPI_SLAVES_PORT, SPI_OUT_OK1);
    input_pullup(SPI_SLAVES_OUTOK_PORT, SPI_OUT_OK2);
    input_pullup(SPI_SLAVES_OUTOK_PORT, SPI_OUT_OK3);

    // there are pull-ups on slave end, pull high here, too, so no
    // current flows pointlessly
    output_high(SPI_SLAVES_PORT, SPI_OUT_CHAN0);
    output_high(SPI_SLAVES_PORT, SPI_OUT_CHAN1);    

    // MOSI, SCK, ~SS are outputs, MISO remains input
    SPI_DDR |= _BV(SPI_MOSI_DDR) | _BV(SPI_SCK_DDR) | _BV(SPI_SS_DDR);
    /* set_output(SPI_DDR, SPI_MOSI_DDR); */
    /* set_output(SPI_DDR, SPI_SCK_DDR); */
    /* set_output(SPI_DDR, SPI_SS_DDR); */
    output_high(SPI_PORT, SPI_SS);  // ~SS inactive

    // Enable SPI, Master, SCK=fosc/128 (TODO: increase SCK)
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
}

uint8_t spi_master_transmit (uint8_t data) {
    SPDR = data;                  // start transmission
    while( !(SPSR & _BV(SPIF)) ); // wait for transmission complete 
    return SPDR;
}

inline void spi_chan_select (uint8_t c) {
    switch (c) {
    case 0:
	output_low(SPI_SLAVES_PORT, SPI_OUT_CHAN0);
	output_low(SPI_SLAVES_PORT, SPI_OUT_CHAN1);
	break;
    case 1:
	output_high(SPI_SLAVES_PORT, SPI_OUT_CHAN0);
	output_low(SPI_SLAVES_PORT, SPI_OUT_CHAN1);
	break;
    case 2:
	output_low(SPI_SLAVES_PORT, SPI_OUT_CHAN0);
	output_high(SPI_SLAVES_PORT, SPI_OUT_CHAN1);
	break;
    case 3:
	output_high(SPI_SLAVES_PORT, SPI_OUT_CHAN0);
	output_high(SPI_SLAVES_PORT, SPI_OUT_CHAN1);
	break;
    default: break;
    }
}

inline void spi_request_interrupt (uint8_t slave) {
    switch (slave) {
    case 0: output_toggle(SPI_SLAVES_PORT, SPI_OUT_SS1); break;
    case 1: output_toggle(SPI_SLAVES_PORT, SPI_OUT_SS2); break;
    case 2: output_toggle(SPI_SLAVES_PORT, SPI_OUT_SS3); break;
    default: break;
    }
}

inline void cfg_init (void) {
    set_output(SPI_DDR, SPI_SCK_DDR);
    set_output(SPI_DDR, SPI_SS_DDR);
    set_output(SPI_CFG_DDR, SPI_CFG_RESET_DDR);
    set_output(SPI_CFG_DDR, SPI_CFG_MODE_DDR);

    /* cfg_reset_disable(); */
    /* cfg_deselect(); */
}
