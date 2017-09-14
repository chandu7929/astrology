OVERVIEW
--------

This module creates a default astrology named "Zodiac", which consists 12 
astrological signs,namely Aries, Taurus, Gemini, Cancer, Leo, Virgo, Libra,
 Scorpio, Sagittarius, Capricorn, Aquarius and Pisces.

In order to use this module functionality, site administrator need to add text
for each sign and for each format.


It also creates a block to display astrological data.

Default format can be selected from settings page:
http://example.com/admin/config/astrology/list.

User can create custom astrology if default doesn't fit to their requirement and
can also create associated astrological sign for it,
custom astrology can be added from:
http://example.com/admin/config/astrology/add.

Only administrators are allowed use the features described above.

 Default format			 
 * Day.
 
 Default astrology               
 * Zodiac


 Available format
 * Day.
 * Week.
 * Month.
 * Year.

 Available astrology
 * Zodiac
 * More can be created.
 
 1. Information already available for each sign belonging to zodiac astrology.
 2. There is a block named "Astrology", which can be use to show default(zodiac)
  selected astrology.
 3. Default format is 'day, zodiac', while other can be chosen by admin to show
  default format.
 4. User can check their astrology for day, week, month, and year.
 5. User can find their astrological star sign by simply providing their 
 date of birth.

INSTALLATION
------------

Install module from https://drupal.org/project/astrology or
you directly put your extracted astrology module folder to
"site/modules/" directory and enable module by visiting the URL:
"http://example.com/admin/modules".

After installing module, you can visit astrology setting page:
URL: http://.example/admin/config/astrology/list,
where you can adjust the settings for your site.

Now place the block named "Astrology" to your site region.

CREATE CUSTOM ASTROLOGY  & CONCEPT
----------------------------------
By clicking on "Add astrology" action link you can add a custom astrology, and
after creating it, there will be links to  add sign, list text, edit and delete
for your action. 

Every astrology have some "star sign" like zodiac has Aries, Taurus etc, which 
can be seen by clicking on "list sign" link.

First add all star sign for newly created astrology and later, add text for each
sign and for each format.

As astrological sign needs data(text) to show for every day, every week, every 
year, data(text) can be added for particular sign and for particular format like
day, week, month, and year by clicking "add text" link.

Format can be chosen by www.example.com/admin/config/astrology/list.


ADMINISTRATIVE INTERFACE
------------------------

 1. Multiple astrology and their associated sign can be add/edit/delete.
 2. Select default format to display and perform operation for a particular
    astrology.
 3. You can only create/add text for the current year for default selected
    astrology and format.
 4. If you add text "A" for January 1, in January 1 of the next year you might
    get text "A".
 5. When year changed; and if you creating/adding text "A" for current year 1st
    January then it will update the text "B"  of 1st January of previous year if 
    text "B" exists.
 6. It will also update the content of a particular date if its being added
    twice.
 7. Text can be added for current year only.
 8. Entered text for particular astrology signs can be viewed sign wise
    for the selected date (format will be default selected).