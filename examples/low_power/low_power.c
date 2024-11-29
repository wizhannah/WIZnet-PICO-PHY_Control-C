/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>

#include "port_common.h"

#include "wizchip_conf.h"
#include "w5x00_spi.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 11, 2},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 11, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

/* Loopback */
static uint8_t g_loopback_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void);

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{

    int chip_ver = 0;
    int chip_phy_reg = 0;

    wiz_PhyConf phy_conf, get_conf;

    /* Initialize */
    int retval = 0;

    set_clock_khz();

    stdio_init_all();

    sleep_ms(1000);

    wizchip_spi_initialize();
    wizchip_cris_initialize();
    wizchip_reset();

    wizchip_initialize();
    wizchip_check();

    network_initialize(g_net_info);

    /* Get network information */
    print_network_information(g_net_info);

    /* config init massage */
    phy_conf.by = PHY_CONFBY_SW;        // Use software config
    phy_conf.mode = PHY_MODE_MANUAL;    // User config mode 
    phy_conf.duplex = PHY_DUPLEX_FULL;  // Full duplex
    phy_conf.speed = PHY_SPEED_100;     // Speed

    /* setting phy 100M and full-duplex mode */
    ctlwizchip(CW_SET_PHYCONF, &phy_conf);
    ctlwizchip(CW_GET_PHYCONF, &get_conf);

    printf("The current Mbtis speed : %d\r\n", get_conf.speed == PHY_SPEED_100 ? 100 : 10);
    printf("The current Duplex Mode : %s\r\n", get_conf.duplex == PHY_DUPLEX_HALF ? "Half-Duplex" : "Full-Duplex");

    /* setting phy 10M and half-duplex mode */
    phy_conf.speed = PHY_SPEED_10;
    phy_conf.duplex = PHY_DUPLEX_HALF;
    ctlwizchip(CW_SET_PHYCONF, &phy_conf);
    ctlwizchip(CW_GET_PHYCONF, &get_conf);

    printf("The current Mbtis speed : %d\r\n", get_conf.speed == PHY_SPEED_100 ? 100 : 10);
    printf("The current Duplex Mode : %s\r\n", get_conf.duplex == PHY_DUPLEX_HALF ? "Half-Duplex" : "Full-Duplex");

    /* setting phy low power mode */
#if (_WIZCHIP_ == W5100S)
    wizphy_setphypmode(PHY_POWER_DOWN);
    printf("The current phy is : %s\r\n", (read_phy_pwdn(PHYCR1) & (1 << 5)) ? "normal mode" : "power down mode");
    printf("PHY is in power down state and cannot be ping reply.\r\n");
#elif (_WIZCHIP_ == W5500)
    setPHYCFGR((uint8_t) PHYCFGR_RST);
    setPHYCFGR(PHYCFGR_OPMDC_PDOWN);
    printf("The current phy is : %s\r\n", (getPHYCFGR() & PHYCFGR_OPMDC_PDOWN) ? "power down mode" : "normal mode");
    printf("PHY is in power down state and cannot be ping reply.\r\n");
#endif

    /* Infinite loop */
    while (1)
    {

    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}
