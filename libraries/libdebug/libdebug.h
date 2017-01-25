


#ifndef _LIB_DEBUG_H_
#define	_LIB_DEBUG_H_



#ifdef LIB_DEBUG
#ifdef BREAK
	#define BREAK(m);		Serial.println(m);				\
							while(Serial.read() != '\r');
	#define BREAKL(m,a,b);	Serial.print(m);				\
							Serial.println_long(a,b);		\
							while(Serial.read() != '\r');
	#define BREAKD(m,a,b);	Serial.print(m);				\
							Serial.println_double(a,b);		\
							while(Serial.read() != '\r');
#else  // BREAK
	#define BREAK(m);		Serial.println(m);				
	#define BREAKL(m,a,b);	Serial.print(m);				\
							Serial.println_long(a,b);		
	#define BREAKD(m,a,b);	Serial.print(m);				\
							Serial.println_double(a,b);		
#endif // BREAK
#else
	#define BREAK(m);
	#define BREAKL(m,a,b);
	#define BREAKF(m,a,b);
#endif



#endif //_LIB_DEBUG_H_

