/*
 * lcd_gh08172.hpp
 *
 *  Created on: 27 cze 2021
 *      Author: kwarc
 */

#ifndef LCD_GH08172_HPP_
#define LCD_GH08172_HPP_

#include <string>
#include <map>

/**
  @verbatim
================================================================================
                              GLASS LCD MAPPING
================================================================================
LCD allows to display informations on six 14-segment digits and 4 bars:

  1       2       3       4       5       6
-----   -----   -----   -----   -----   -----
|\|/| o |\|/| o |\|/| o |\|/| o |\|/|   |\|/|   BAR3
-- --   -- --   -- --   -- --   -- --   -- --   BAR2
|/|\| o |/|\| o |/|\| o |/|\| o |/|\|   |/|\|   BAR1
----- * ----- * ----- * ----- * -----   -----   BAR0

LCD segment mapping:
--------------------
  -----A-----        _
  |\   |   /|   COL |_|
  F H  J  K B
  |  \ | /  |        _
  --G-- --M--   COL |_|
  |  / | \  |
  E Q  P  N C
  |/   |   \|        _
  -----D-----   DP  |_|

 An LCD character coding is based on the following matrix:
COM           0   1   2     3
SEG(n)      { E , D , P ,   N   }
SEG(n+1)    { M , C , COL , DP  }
SEG(23-n-1) { B , A , K ,   J   }
SEG(23-n)   { G , F , Q ,   H   }
with n positive even number.

 The character 'A' for example is:
  -------------------------------
LSB   { 1 , 0 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
MSB   { 1 , 1 , 0 , 0   }
      -------------------
  'A' =  F    E   0   0 hexa

Bar segment mapping:
 - Bar 0: COM3, LCD_SEG11
 - Bar 1: COM2, LCD_SEG11
 - Bar 2: COM3, LCD_SEG9
 - Bar 3: COM2, LCD_SEG9

  @endverbatim
*/

namespace drivers
{

class lcd_gh08172
{
public:

    lcd_gh08172();
    ~lcd_gh08172();
    bool write(const std::string_view &s);
private:
    const uint8_t segments = 16;    /* Number of segments in one LCD position (character) */
    const uint8_t positions = 6;    /* Number of LCD positions */
    const uint8_t bars = 4;         /* Number of bar segments */

    static const std::map<const char, uint16_t> character_map;

    void set_character(uint16_t value, uint8_t position);
    void set_bar(bool value, uint8_t bar);
};

}

#endif /* LCD_GH08172_HPP_ */
