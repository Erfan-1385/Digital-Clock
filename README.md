#ساعت دیجیتال

در این پروژه از میکرو کنترلر STM32F103C8T6 با معماری ARM استفاده شده است .
برای نمایش اطلاعات به کاربر از LCD 16*2 استفاده شده و با استفاده از درایور LCD I2C به میکروکنترلر متصل شده است.

کارایی این دستگاه:
1)نمایش ساعت
2)نمایش تاریخ
3)نمایش روز های هفته
4)اندازه گیری و نمایش دما

دو سوییچ تک پل برای قطع و وصل برق مدار و دیگری برای شروع و یا توقف ثانیه شمار ساعت دیجیتالی در نظر گرفته شده است.
علاوه بر آن از تک روتاری برای دسترسی به تنظیمات ساعت استفاده شده است.

برنامه ی آن با زبان C نوشته شده و PCB آن در نرم افزار Altium desiner طراحی شده است.

فایل های برنامه:
کد اصلی برنامه -> main.c
کتابخانه ی روتاری -> Rotary.h
کتابخانه ی LCD -> CODENIC_LCD.c & CODENIC_LCD.h
فایل های .h باید در پوشه ی inc و فایل های .c باید در پوشه ی src قرار گیرند.

#Digital Clock

This project uses an STM32F103C8T6 microcontroller with ARM architecture.
To display information to the user, a 16*2 LCD is used and is connected to the microcontroller using an I2C LCD driver.

The functionality of this device:
1) Display the time
2) Display the date
3) Display the days of the week
4) Measure and display the temperature

Two single-pole switches are provided to turn the circuit power on and off, and another to start or stop the digital clock's second counter.

In addition, a single rotary switch is used to access the clock settings.

Its program is written in C language and its PCB is designed in Altium designer software.

Program files:
Main program code -> main.c
Rotary library -> Rotary.h
LCD library -> CODENIC_LCD.c & CODENIC_LCD.h
.h files should be placed in the inc folder and .c files should be placed in the src folder.
