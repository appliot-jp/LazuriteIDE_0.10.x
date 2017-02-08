


#ifndef _LIB_DEBUG_H_
#define	_LIB_DEBUG_H_



#ifdef LIB_DEBUG
#ifdef BREAK
	#define BREAK(m);		Serial.println(m);				\
							while(Serial.read() != '\r');
	#define BREAKE(m,E);	Serial.println(m);				\
							while(Serial.read() != '\r')	\
							{								\
								E;							\
							}
	#define BREAKL(m,a,b);	Serial.print(m);				\
							Serial.println_long(a,b);		\
							while(Serial.read() != '\r');
	#define BREAKD(m,a,b);	Serial.print(m);				\
							Serial.println_double(a,b);		\
							while(Serial.read() != '\r');
#else
	#define BREAK(m);		Serial.println(m);				
	#define BREAKE(m,E);	Serial.println(m);				
	#define BREAKL(m,a,b);	Serial.print(m);				\
							Serial.println_long(a,b);		
	#define BREAKD(m,a,b);	Serial.print(m);				\
							Serial.println_double(a,b);		
#endif // BREAK



#endif //_LIB_DEBUG_H_

