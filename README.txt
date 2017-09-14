OVERVIEW
--------

This module creates a default astrology named "Zodiac", which consists 12 
astrological signs,namely Aries, Taurus, Gemini, Cancer, Leo, Virgo, Libra,
Scorpio, Sagittarius, Capricorn, Aquarius and Pisces.

In order to use this module functionality, site administrator need to add text
for each sign and for each format.


It also creates a block to display astrological data named "Astrology".

Default format can be selected from settings page:
http://example.com/admin/config/astrology/list.

User can add new astrology if default(Zodiac) doesn't fit to their requirement
and can also create associated astrological sign for it,

New astrology can be added from:
http://example.com/admin/config/astrology/add.

 Default format			 
 * Day
 
 Default astrology               
 * Zodiac


 Available format
 * Day
 * Week
 * Month
 * Year

 Available astrology
 * Zodiac
 * While more can be created.

 Note: There are two types of format defined, one for displaying astrological
 data site wide and another for administrate work like adding text for astrology
 sign, listing editing etc.

 1. There is already information for each sign belonging to zodiac astrology.
 2. Default format is 'day, zodiac', while this can be changed from 
    "Site wide settings".
 4. Provides astrological data for day, week, month, and year.
 5. Site visitor can find their astrological star sign by simply providing their 
    date of birth.

INSTALLATION
------------

1. Install module from https://drupal.org/project/astrology or you directly put
   your extracted astrology module folder to "sites/all/modules/" directory and 
   enable it by visiting the URL: "http://example.com/admin/modules".

2. Visit astrology setting page and select the format you want to display data
   on site from "Site wide settings" tab.

3. Now place the block named "Astrology" to your site region.

ADMINISTRATIVE INTERFACE
------------------------

 1. Multiple astrology and their associated sign can be add/edit/delete.
 2. Select format from "Administer settings" for adding and listing of text in
    that format for particular astrological sign.
 3. You can only add text for the current year for default selected format.
 4. If you add text "A" for January 1, in January 1 of the next year you might
    get text "A".
 5. When year changed; and if you adding text "A" for current year 1st
    January then it will update the text "B"  of 1st January of previous year if 
    text "B" exists.
 6. It will replace the content of a particular date if its being added twice.
 8. Entered text for particular astrology signs can be viewed from text link
    adjacent to each astrology name.
