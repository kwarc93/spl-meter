/*
 * rcc.hpp
 *
 *  Created on: 25 paź 2020
 *      Author: kwarc
 */

#ifndef STM32L4_RCC_HPP_
#define STM32L4_RCC_HPP_

#include <cstdint>

//--------------------------------------------------------------------------------

/** @brief   Macro for initializing  @ref struct rcc::periph_bus.
 *  @example RCC_PERIPH_BUS(AHB1, GPIOA)
 */
#define RCC_PERIPH_BUS(_bus, _periph)  { rcc::bus::_bus, RCC_##_bus##ENR_##_periph##EN }

namespace drivers
{

//--------------------------------------------------------------------------------

class rcc final
{
public:
    rcc() = delete;

    enum class bus
    {
        AHB1,
        AHB2,
        AHB3,
        APB1,
        APB2,
    };

    enum class reset_source
    {
        bor = 1 << 0,     /**< Brown-out reset. */
        pin = 1 << 1,     /**< Pin reset. */
        por = 1 << 2,     /**< Power on reset. */
        sw = 1 << 3,      /**< Software reset. */
        iwdg = 1 << 4,    /**< Independent watchdog reset. */
        wwdg = 1 << 5,    /**< Window watchdog reset. */
        lpwr = 1 << 6,    /**< Low power reset. */
    };

    enum class msi_clock
    {
        CLK_100KHZ = 0,
        CLK_200KHZ,
        CLK_400KHZ,
        CLK_800KHZ,
        CLK_1MHZ,
        CLK_2MHZ,
        CLK_4MHZ,
        CLK_8MHZ,
        CLK_16MHZ,
        CLK_24MHZ,
        CLK_32MHZ,
        CLK_48MHZ,
    };

    enum class rtc_clock_source
    {
        NONE = 0,
        LSE,
        LSI,
        HSE,
    };

    /** @warning Initialize this object with @ref RCC_PERIPH_BUS() macro. */
    struct periph_bus
    {
        rcc::bus bus;
        uint32_t periph;
    };

    struct bus_presc
    {
        uint32_t ahb;       /**< AHB prescaler, use RCC_CFGR_HPRE_DIVx from CMSIS headers */
        uint32_t apb1;      /**< APB1 prescaler, use RCC_CFGR_PPRE1_DIVx from CMSIS headers */
        uint32_t apb2;      /**< APB1 prescaler, use RCC_CFGR_PPRE2_DIVx from CMSIS headers */
    };

    struct main_pll
    {
        uint32_t source;    /**< Clock source, use RCC_PLLCFGR_PLLSRC_HSx from CMSIS headers */
        uint32_t m;         /**< M divider, allowed range: 2 - 63 */
        uint32_t n;         /**< N multiplier, allowed range: 50 - 432 */
        uint32_t r;         /**< R divider, allowed range: 2, 4, 6, 8 */
    };

//--------------------------------------------------------------------------------

    /** @brief  Resets the RCC clock configuration to the default reset state. */
    static void reset(void);

    /** @brief  Resets all RCC peripherals configuration to the default reset state. */
    static void reset_all_periph(void);

    /** @brief  Resets all the RCC peripheral clocks configuration to the default reset state. */
    static void disable_all_periph_clocks(void);

    /**
     * @brief   Enables or disables selected peripheral clock.
     * @param   obj - pointer to structure created by @ref RCC_PERIPH_BUS
     * @param   state - true for enable, false for disable clock
     */
    static void toggle_periph_clock(const periph_bus &pbus, bool state);

    static void set_msi(msi_clock clock);

    static void set_main_pll(const struct main_pll &pll, const struct bus_presc &presc);

    /** @brief  Gets actual system clock frequency.
     *  @param  None
     *  @retval System clock frequency in Hz
     */
    static uint32_t get_sysclk_freq(void);

    /**
     * @brief   Gets actual frequency for given bus.
     * @param   bus - @ref bus
     * @return  0 if bus is not valid
     */
    static uint32_t get_bus_freq(bus bus);

    /**
     * @brief   Gets actual prescaler for given bus.
     * @param   bus - @ref bus
     * @return  -1 if bus is not valid
     */
    static int8_t get_bus_presc(bus bus);

    /** @brief  Enables/disables high speed internal clock.
     *  @param  state - true enables, false disables clock.
     *  @retval None
     */
    static void toggle_hsi(bool state);

    /** @brief  Enables/disables high speed external clock.
     *  @param  state - true enables, false disables clock.
     *  @retval None
     */
    static void toggle_hse(bool state);

    /** @brief  Enables/disables low speed external clock.
     *  @param  state - true enables, false disables clock.
     *  @retval None
     */
    static void toggle_lse(bool state);

    /** @brief  Enables/disables low speed internal clock.
     *  @param  state - true enables, false disables clock.
     *  @retval None
     */
    static void toggle_lsi(bool state);

    /**
     * @brief   Selects RTC clock source.
     * @param   source - @ref enum class rtc_clock_source
     * @waring  This function resets Backup Domain!
     */
    static void set_rtc_clock(rtc_clock_source source);

    /**
     * @brief   Gets reset source flags.
     * @return  reset source flags @ref enum reset_source
     */
    static reset_source get_reset_source(void);

    /** @brief  Clears reset source flags. */
    static void clear_reset_source(void);

private:
    static uint32_t get_ahb_freq(void);
    static uint32_t get_apb1_freq(void);
    static uint32_t get_apb2_freq(void);

    static uint8_t get_ahb_presc(void);
    static uint8_t get_apb1_presc(void);
    static uint8_t get_apb2_presc(void);

    static uint32_t get_sysclk_from_pll_source(void);

};

//--------------------------------------------------------------------------------

}

#endif /* STM32L4_RCC_HPP_ */
