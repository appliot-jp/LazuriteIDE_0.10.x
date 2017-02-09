/* FILE NAME: rtc.c
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

#include "rtc.h"
#include "driver_timer.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*--- Setting value. ---*/
#define RTC_SEC_INVALID     ( 0xFF )
#define RTC_MIN_INVALID     ( 0xFF )
#define RTC_HOUR_INVALID    ( 0xFF )
#define RTC_DAY_INVALID     ( 0xFF )
#define RTC_MON_INVALID     ( 0xFF )
#define RTC_YEAR_INVALID    ( 0xFF )
/*---   Carrying result ---*/
#define RTC_CARRY           ( 1 )
#define RTC_NO_CARRY        ( 0 )
/*---   Carrying at time.   ---*/
#define RTC_SEC_CARRY       ( 60 )
#define RTC_MIN_CARRY       ( 60 )
#define RTC_HOUR_CARRY      ( 24 )
#define RTC_DAY_31TH_CARRY  ( 32 )
#define RTC_DAY_30TH_CARRY  ( 31 )
#define RTC_DAY_29TH_CARRY  ( 30 )
#define RTC_DAY_28TH_CARRY  ( 29 )
#define RTC_MON_CARRY       ( 13 )
#define RTC_YEAR_CARRY      ( 100 )
#define RTC_WEEK_CARRY      ( 8 )
/*---   Initialize value at time.   ---*/
#define RTC_SEC_INI         ( 0 )
#define RTC_MIN_INI         ( 0 )
#define RTC_HOUR_INI        ( 0 )
#define RTC_DAY_INI         ( 1 )
#define RTC_MON_INI         ( 1 )
#define RTC_YEAR_INI        ( 17 )
#define RTC_WEEK_INI        ( 1 )
/*=== Week day value. ===*/
#define RTC_WEEK_DAY_SUN    ( 1 )    
#define RTC_WEEK_DAY_MON    ( 2 )    
#define RTC_WEEK_DAY_TUE    ( 3 )    
#define RTC_WEEK_DAY_WED    ( 4 )    
#define RTC_WEEK_DAY_THU    ( 5 )    
#define RTC_WEEK_DAY_FRI    ( 6 )    
#define RTC_WEEK_DAY_SAT    ( 7 )    
/*=== Return value. ===*/  
#define RTC_R_OK            (   0 )    
#define RTC_R_ERR_SEC       (  -1 )    
#define RTC_R_ERR_MIN       (  -2 )    
#define RTC_R_ERR_HOUR      (  -3 )    
#define RTC_R_ERR_WEEK      (  -4 )    
#define RTC_R_ERR_DAY       (  -5 )    
#define RTC_R_ERR_MON       (  -6 )    
#define RTC_R_ERR_YEAR      (  -7 )    
#define RTC_R_ERR_ALEN      (  -8 )    
#define RTC_R_ERR_RIN       (  -9 )    
#define RTC_R_ERR_GETTIME   ( -10 )    

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
/* global variable for RTC class */
const st_RTC_f RTC = {
    rtc_begin,
    rtc_setTime,
    rtc_setDate,
    rtc_getHours,
    rtc_getMinutes,
    rtc_getSeconds,
    rtc_getYear,
    rtc_getMonth,
    rtc_getDay,
    rtc_setAlarmTime,
    rtc_setAlarmDate,
    rtc_enableAlarm,
    rtc_disableAlarm,
    rtc_attachInterrupt,
    rtc_detachInterrupt
};
/*--- Time parameter. ---*/
static tRtcTime _rtc_timePrm = { 0, 0, 0, RTC_WEEK_DAY_SUN, 1, 1, 17 }; // 0:0:0 1st Jan, 2017 SUN
/*--- Parameter of alarm. ---*/
static tRtcAlarm _rtc_timePrm_Alarm = { (void*)0, RTC_SEC_INVALID, \
       RTC_MIN_INVALID, RTC_HOUR_INVALID, RTC_DAY_INVALID, RTC_MON_INVALID, RTC_YEAR_INVALID };
static _rtc_alarmMatch = MATCH_NONE;

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static unsigned char _bcd2dec( unsigned char bcd );
static unsigned char _inc_Nadj( unsigned char * ptr_inc , unsigned char value_N , unsigned char value_init );
static int _setTime( tRtcTime *prm );
static int _getTime( tRtcTime *prm );
static void _updateTime( uint32_t sys_timer_count );
static int _setAlarm( tRtcAlarm* prm );

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void rtc_begin (void) {
    set_timer0_function( _updateTime );
}

void rtc_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    tRtcTime time;

    _getTime(&time);
    time.hour = hours;
    time.min = minutes;
    time.sec = (uint8_t)(seconds & 0xFE); // rounds to even number
    _setTime(&time);
}

void rtc_setDate(uint8_t day, uint8_t month, uint8_t year) {
    tRtcTime date;

    _getTime(&date);
    date.day = day;
    date.mon = month;
    date.year = year;
    _setTime(&date);
}

uint8_t rtc_getHours(void) { return _rtc_timePrm.hour; }

uint8_t rtc_getMinutes(void) { return _rtc_timePrm.min; }

uint8_t rtc_getSeconds(void) { return _rtc_timePrm.sec; }

uint8_t rtc_getYear(void) { return _rtc_timePrm.year; }

uint8_t rtc_getMonth(void) { return _rtc_timePrm.mon; }

uint8_t rtc_getDay(void) { return _rtc_timePrm.day; }

void rtc_setAlarmTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    tRtcAlarm alarm;

    alarm.year = rtc_getYear();
    alarm.mon = rtc_getMonth();
    alarm.day = rtc_getDay();
    alarm.hour = hours;
    alarm.min = minutes;
    alarm.sec = (uint8_t)(seconds & 0xFE); // rounds to even number
    _setAlarm(&alarm);
}

void rtc_setAlarmDate(uint8_t day, uint8_t month, uint8_t year) {
    tRtcAlarm alarm;

    alarm.year = year;
    alarm.mon = month;
    alarm.day = day;
    alarm.hour = rtc_getHours();
    alarm.min = rtc_getMinutes();
    alarm.sec = rtc_getSeconds();
    _setAlarm(&alarm);
}

void rtc_enableAlarm(uint8_t alarmMatch) {
    if ((alarmMatch >= MATCH_SS) && (alarmMatch <= MATCH_YYMMDDHHMMSS)) {
        _rtc_alarmMatch = alarmMatch;
    } else {
        _rtc_alarmMatch = MATCH_NONE;   // out of range
    }
}

void rtc_disableAlarm(void) {
    _rtc_alarmMatch = MATCH_NONE;
}

void rtc_attachInterrupt(void* callback) {
    _rtc_timePrm_Alarm.callBack = callback;
}

void rtc_detachInterrupt(void) {
    _rtc_timePrm_Alarm.callBack = NULL;
    rtc_disableAlarm();                 // Just in case, disables alarm when detached.
                                        // When attached again, alarm must be enabled.
}

/*############################################################################*/
/*#                              Subroutine                                  #*/
/*############################################################################*/

static int _setTime( tRtcTime *prm )
{
    unsigned char   maxDay;
    
    /*=== parameter check. ===*/
    /*--- check [year](00-99). ---*/
    if( prm->year  > (unsigned char)99 ) {
        return ( RTC_R_ERR_YEAR );
    }
    /*--- check [mon](01-12). ---*/
    if( ( prm->mon < (unsigned char)1 ) ||
        ( prm->mon > (unsigned char)12 ) ) {
        return ( RTC_R_ERR_MON );
    }
    /*--- check [day]. ---*/
    /*- month : 2 => day : 28 or 29 -*/
    if( prm->mon == (unsigned char)2 ) {
        if( ( prm->year & (unsigned char)3 ) == (unsigned char)0 ) {
            maxDay = (unsigned char)29;
        }else {
            maxDay = (unsigned char)28;
        }
    }
    /*- month : 4 or 6 or 9 or 11 => day : 30 -*/
    else if( ( prm->mon == (unsigned char)4 ) ||
             ( prm->mon == (unsigned char)6 ) ||
             ( prm->mon == (unsigned char)9 ) ||
             ( prm->mon == (unsigned char)11 ) ) {
        maxDay = (unsigned char)30;
    }
    /*- month : 1 or 3 or 5 or 7 or 8 or 10 or 12 => day : 31 -*/
    else {
        maxDay = (unsigned char)31;
    }
    if( ( prm->day < (unsigned char)1 ) ||
        ( prm->day > maxDay ) ) {
        return ( RTC_R_ERR_DAY );
    }
    /*--- check [week](01-07). ---*/
    if( ( prm->week < (unsigned char)1 ) ||
        ( prm->week > (unsigned char)7 ) ) {
        return ( RTC_R_ERR_WEEK );
    }
    /*--- check [hour](00-23). ---*/
    if( prm->hour > (unsigned char)23 ) {
        return ( RTC_R_ERR_HOUR );
    }
    /*--- check [min](00-59). ---*/
    if( prm->min > (unsigned char)59 ) {
        return ( RTC_R_ERR_MIN );
    }
    /*--- check [sec](00-59). ---*/
    if( prm->sec > (unsigned char)58 ) {
        return ( RTC_R_ERR_SEC );
    }
    /*--- Set time. ---*/
    _rtc_timePrm.year = prm->year;
    _rtc_timePrm.mon  = prm->mon;
    _rtc_timePrm.day  = prm->day;
    _rtc_timePrm.week = prm->week;
    _rtc_timePrm.hour = prm->hour;
    _rtc_timePrm.min  = prm->min;
    _rtc_timePrm.sec  = prm->sec;

    return ( RTC_R_OK );
}

static int _getTime( tRtcTime *prm )
{
    prm->sec  = _rtc_timePrm.sec;
    prm->min  = _rtc_timePrm.min;
    prm->hour = _rtc_timePrm.hour;
    prm->week = _rtc_timePrm.week;
    prm->day  = _rtc_timePrm.day;
    prm->mon  = _rtc_timePrm.mon;
    prm->year = _rtc_timePrm.year;

    return ( RTC_R_OK );
}

static int _setAlarm( tRtcAlarm* prm )
{
    unsigned char   maxDay;
    
    /*=== parameter check. ===*/
    if( prm->year  > (unsigned char)99 ) {
        return ( RTC_R_ERR_YEAR );
    }
    if( prm->mon > (unsigned char)12 ) {
        return ( RTC_R_ERR_MON );
    }
    if( prm->mon == (unsigned char)2 ) {
        maxDay = (unsigned char)29;
    }
    /*- month : 4 or 6 or 9 or 11 => day : 30 -*/
    else if( ( prm->mon == (unsigned char)4 ) ||
             ( prm->mon == (unsigned char)6 ) ||
             ( prm->mon == (unsigned char)9 ) ||
             ( prm->mon == (unsigned char)11 ) ) {
        maxDay = (unsigned char)30;
    }
    /*- month : 0 or 1 or 3 or 5 or 7 or 8 or 10 or 12 => day : 31 -*/
    else {
        maxDay = (unsigned char)31;
    }
    if( prm->day > maxDay ) {
        return ( RTC_R_ERR_DAY );
    }
    /*--- check [hour](00-23). ---*/
    if( prm->hour > (unsigned char)23 ) {
        return ( RTC_R_ERR_HOUR );
    }
    /*--- check [min](00-59). ---*/
    if( prm->min > (unsigned char)59 ) {
        return ( RTC_R_ERR_MIN );
    }
    /*--- check [sec](00-59). ---*/
    if( prm->sec > (unsigned char)58 ) {
        return ( RTC_R_ERR_SEC );
    }
    /*=== Alarm setting. ===*/
    _rtc_timePrm_Alarm.year = prm->year;
    _rtc_timePrm_Alarm.mon  = prm->mon;
    _rtc_timePrm_Alarm.day  = prm->day;
    _rtc_timePrm_Alarm.hour = prm->hour;
    _rtc_timePrm_Alarm.min  = prm->min;
    _rtc_timePrm_Alarm.sec  = prm->sec;

    return ( RTC_R_OK );
}

static void _updateTime( uint32_t sys_timer_count )
{
    int             ret;
    unsigned char   carryOfDay;
    unsigned char   execCallback = 0;

    /*=== Update second [0 to 58]. ===*/
    _rtc_timePrm.sec++;                             /* Count up second. */
    _rtc_timePrm.sec++;                             /* Count up second. This RTC library must be called per 2 seconds. */
    if( _inc_Nadj( &_rtc_timePrm.sec, RTC_SEC_CARRY, RTC_SEC_INI ) == RTC_CARRY ) {

        /*=== Update minute [0 to 59]. ===*/
        _rtc_timePrm.min++;                         /* Count up minute. */
        if( _inc_Nadj( &_rtc_timePrm.min, RTC_MIN_CARRY, RTC_MIN_INI ) == RTC_CARRY ) {

            /*=== Update hour [0 to 23]. ===*/
            _rtc_timePrm.hour++;                    /* Count up hour. */
            if( _inc_Nadj( &_rtc_timePrm.hour, RTC_HOUR_CARRY, RTC_HOUR_INI ) == RTC_CARRY ) {

                /*=== Judgement max number of day [1 to 31]. ===*/
                /*- month : 2 => day : 28 or 29 -*/
                if( _rtc_timePrm.mon == (unsigned char)2 ) {
                    if( ( _rtc_timePrm.year & (unsigned char)3 ) == (unsigned char)0 ) {
                        carryOfDay = (unsigned char)RTC_DAY_29TH_CARRY;
                    }else {
                        carryOfDay = (unsigned char)RTC_DAY_28TH_CARRY;
                    }
                }
                /*- month : 4 or 6 or 9 or 11 => day : 30 -*/
                else if( ( _rtc_timePrm.mon == (unsigned char)4 ) ||
                         ( _rtc_timePrm.mon == (unsigned char)6 ) ||
                         ( _rtc_timePrm.mon == (unsigned char)9 ) ||
                         ( _rtc_timePrm.mon == (unsigned char)11 ) ) {
                    carryOfDay = (unsigned char)RTC_DAY_30TH_CARRY;
                }
                /*- month : 1 or 3 or 5 or 7 or 8 or 10 or 12 => day : 31 -*/
                else {
                    carryOfDay = (unsigned char)RTC_DAY_31TH_CARRY;
                }

                /*=== Update date. ===*/
                /* Update day [1 to (carryOfDay-1)]. */
                _rtc_timePrm.day++;             /* Count up day. */
                if( _inc_Nadj( &_rtc_timePrm.day, carryOfDay, RTC_DAY_INI ) == RTC_CARRY ) {

                    /* Update month [1 to 12]. */
                    _rtc_timePrm.mon++;         /* Count up month. */
                    if( _inc_Nadj( &_rtc_timePrm.mon, RTC_MON_CARRY, RTC_MON_INI ) == RTC_CARRY ) {

                        /* Update year [0 to 99]. */
                        _rtc_timePrm.year++;    /* Count up year. */
                        /* The returned value from _inc_Nadj function is not used. */
                        ret = _inc_Nadj( &_rtc_timePrm.year, RTC_YEAR_CARRY, RTC_YEAR_INI );
                    }
                }
                /* Update week [1 to 7] */
                _rtc_timePrm.week++;            /* Count up week. */
                if( _rtc_timePrm.week >= RTC_WEEK_CARRY ) {
                    _rtc_timePrm.week = RTC_WEEK_INI;
                }
            }
        }
    }

    /*=== Check alarm setting. ===*/
    /* Callback alarm. */
    if ( _rtc_timePrm_Alarm.callBack != (void*)0 ) {
        switch (_rtc_alarmMatch) {
        case MATCH_SS:
            if ((_rtc_timePrm_Alarm.sec == _rtc_timePrm.sec)) {
                execCallback = 1;
            }
            break;
        case MATCH_MMSS:
            if ((_rtc_timePrm_Alarm.sec == _rtc_timePrm.sec) && \
                (_rtc_timePrm_Alarm.min == _rtc_timePrm.min)) {
                execCallback = 1;
            }
            break;
        case MATCH_HHMMSS:
            if ((_rtc_timePrm_Alarm.sec == _rtc_timePrm.sec) && \
                (_rtc_timePrm_Alarm.min == _rtc_timePrm.min) && \
                (_rtc_timePrm_Alarm.hour == _rtc_timePrm.hour)) {
                execCallback = 1;
            }
            break;
        case MATCH_DDHHMMSS:
            if ((_rtc_timePrm_Alarm.sec == _rtc_timePrm.sec) && \
                (_rtc_timePrm_Alarm.min == _rtc_timePrm.min) && \
                (_rtc_timePrm_Alarm.hour == _rtc_timePrm.hour) && \
                (_rtc_timePrm_Alarm.day == _rtc_timePrm.day)) {
                execCallback = 1;
            }
            break;
        case MATCH_MMDDHHMMSS:
            if ((_rtc_timePrm_Alarm.sec == _rtc_timePrm.sec) && \
                (_rtc_timePrm_Alarm.min == _rtc_timePrm.min) && \
                (_rtc_timePrm_Alarm.hour == _rtc_timePrm.hour) && \
                (_rtc_timePrm_Alarm.day == _rtc_timePrm.day) && \
                (_rtc_timePrm_Alarm.mon == _rtc_timePrm.mon)) {
                execCallback = 1;
            }
            break;
        case MATCH_YYMMDDHHMMSS:
            if ((_rtc_timePrm_Alarm.sec == _rtc_timePrm.sec) && \
                (_rtc_timePrm_Alarm.min == _rtc_timePrm.min) && \
                (_rtc_timePrm_Alarm.hour == _rtc_timePrm.hour) && \
                (_rtc_timePrm_Alarm.day == _rtc_timePrm.day) && \
                (_rtc_timePrm_Alarm.mon == _rtc_timePrm.mon) && \
                (_rtc_timePrm_Alarm.year == _rtc_timePrm.year)) {
                execCallback = 1;
            }
            break;
        default:
            break;
        }
        if (execCallback) {
            execCallback = 0;
            _rtc_timePrm_Alarm.callBack( &_rtc_timePrm );
        }
    }
}
// the below warning is no problem because the argument of the callback is not used daringly.
// Warning : W5025 : 'sys_timer_count': unreferenced formal parameter 

static unsigned char _inc_Nadj( unsigned char * ptr_inc , unsigned char value_N , unsigned char value_init )
{
    if( *ptr_inc >= value_N ) {
        *ptr_inc = value_init;
        return( RTC_CARRY );        /* There is carry.      */
    }

    return( RTC_NO_CARRY );         /* There is no carry.   */
}
