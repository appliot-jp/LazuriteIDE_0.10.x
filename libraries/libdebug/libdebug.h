/* FILE NAME: libdebug.c
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
 
#ifndef _LIB_DEBUG_H_
#define	_LIB_DEBUG_H_



#ifdef LIB_DEBUG
#ifdef BREAK_MODE
	#define BREAK(m);		Serial.println(m);				\
							while(Serial.read() != '\r');
	#define BREAKS(m,a);	Serial.print(m);				\
							Serial.println(a);				\
							while(Serial.read() != '\r');
	#define BREAKE(m,F);	Serial.println(m);				\
							while(Serial.read() != '\r')	\
							{								\
								F;							\
							}
	#define BREAKL(m,a,b);	Serial.print(m);				\
							Serial.println_long(a,b);		\
							while(Serial.read() != '\r');
	#define BREAKD(m,a,b);	Serial.print(m);				\
							Serial.println_double(a,b);		\
							while(Serial.read() != '\r');
#else
	#define BREAK(m);		Serial.println(m);				
	#define BREAKS(m,a);	Serial.print(m);				\
							Serial.println(a);
	#define BREAKE(m,E);	Serial.println(m);				
	#define BREAKL(m,a,b);	Serial.print(m);				\
							Serial.println_long(a,b);		
	#define BREAKD(m,a,b);	Serial.print(m);				\
							Serial.println_double(a,b);		
#endif // BREAK_MODE
#else	// LIB_DEBUG
	#define BREAK(m)
	#define BREAKS(m,a)
	#define BREAKE(m,E)
	#define BREAKL(m,a,b)
	#define BREAKD(m,a,b)
#endif // LIB_DEBUG

#endif //_LIB_DEBUG_H_

