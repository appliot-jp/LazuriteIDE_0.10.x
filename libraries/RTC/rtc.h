/* FILE NAME: rtc.h
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _RTC_H_
#define _RTC_H_
#include <lazurite.h>

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/* Argument for enableAlarm */
#define MATCH_NONE          ( 0 ) // disable alarm;
#define MATCH_SS            ( 1 ) // generate an alarm on seconds match; 
#define MATCH_MMSS          ( 2 ) // generate an alarm on minutes and seconds match; 
#define MATCH_HHMMSS        ( 3 ) // generate an alarm on hours, minutes and seconds match; 
#define MATCH_DDHHMMSS      ( 4 ) // generate an alarm on day, hours, minutes and seconds match; 
#define MATCH_MMDDHHMMSS    ( 5 ) // generate an alarm on month, day, hours, minutes and seconds match; 
#define MATCH_YYMMDDHHMMSS  ( 6 ) // generate an alarm on year, month, day, hours, minutes and seconds match; 

/*############################################################################*/
/*#                                Typedef                                   #*/
/*############################################################################*/
/*--- Date&Time setting parameter. ---*/
typedef struct {
    unsigned char   sec;    /* Number of seconds(00-59). */
    unsigned char   min;    /* Number of minutes(00-59). */
    unsigned char   hour;   /* Time from 0 o'clock(00-23). */
    unsigned char   week;   /* A day of the week(01-07). */
    unsigned char   day;    /* Date in month(01-31). */
    unsigned char   mon;    /* Month in year(01-12). */
    unsigned char   year;   /* Years(00-99). */
} tRtcTime;

/*--- callback function ---*/
typedef void (*cbfRtc)( tRtcTime *prm );

/*--- Alarm setting parameter. ---*/
typedef struct {
    cbfRtc          callBack;   /* callback function */
    unsigned char   sec;
    unsigned char   min;    /* Number of minutes(00-59). */
    unsigned char   hour;   /* Time from 0 o'clock(00-23). */
    unsigned char   day;    /* Date in month(00-31). [*]0 : The day is not used for the comparison. */
    unsigned char   mon;    /* Month in year(00-12). [*]0 : The month is not used for the comparison. */
    unsigned char   year;
} tRtcAlarm;

typedef struct {
    void (*begin)(void);
    void (*setTime)(uint8_t hours, uint8_t minutes, uint8_t seconds);
    void (*setDate)(uint8_t day, uint8_t month, uint8_t year);
    uint8_t (*getHours)(void);
    uint8_t (*getMinutes)(void);
    uint8_t (*getSeconds)(void);
    uint8_t (*getYear)(void);
    uint8_t (*getMonth)(void);
    uint8_t (*getDay)(void);
    void (*setAlarmTime)(uint8_t hours, uint8_t minutes, uint8_t seconds);
    void (*setAlarmDate)(uint8_t day, uint8_t month, uint8_t year);
    void (*enableAlarm)(uint8_t alarmMatch);
    void (*disableAlarm)(void);
    void (*attachInterrupt)(void* callback);
    void (*detachInterrupt)(void);
} st_RTC_f;

extern const st_RTC_f RTC;

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void    rtc_begin (void);
void    rtc_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void    rtc_setDate(uint8_t day, uint8_t month, uint8_t year);
uint8_t rtc_getHours(void);
uint8_t rtc_getMinutes(void);
uint8_t rtc_getSeconds(void);
uint8_t rtc_getYear(void);
uint8_t rtc_getMonth(void);
uint8_t rtc_getDay(void);
void    rtc_setAlarmTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void    rtc_setAlarmDate(uint8_t day, uint8_t month, uint8_t year);
void    rtc_enableAlarm(uint8_t alarmMatch);
void    rtc_disableAlarm(void);
void    rtc_attachInterrupt(void* callback);
void    rtc_detachInterrupt(void);

#endif // _RTC_H_
