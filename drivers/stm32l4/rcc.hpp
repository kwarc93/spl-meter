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

    /** @brief RCC peripheral definition. Initialize this object with @ref RCC_PERIPH_BUS. */
    struct periph_bus
    {
        rcc::bus bus;
        uint32_t periph;
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
     * @param   en - true for enable, false for disable clock
     */
    static void enable_periph_clock(const periph_bus &pbus, bool en);

    static void set_msi(msi_clock clock);

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
     * @brief   Gets reset source flags.
     * @return  reset source flags @ref enum reset_source
     */
    static reset_source get_reset_source(void);

    /** @brief  Clears reset source flags. */
    static void clear_reset_source(void);

};

//--------------------------------------------------------------------------------

}

#endif /* STM32L4_RCC_HPP_ */
