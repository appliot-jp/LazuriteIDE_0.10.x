

#include "LedDotMatrix.h"

#define D_port      P50D
#define C_port      P53D
#define B_port      P42D
#define A_port      P43D
#define G_port      P32D
#define DI_port     P33D
#define CLK_port    P57D
#define Latch_port  P52D

typedef struct {
	uint8_t chain_number;
	uint8_t __far *up;
	uint8_t __far *lo;
	uint16_t up_size;
	uint16_t lo_size;
	uint8_t up_cashe[9*8];
	uint8_t lo_cashe[9*8];
} LcdDotMatrixParam;

static LcdDotMatrixParam param;

 const uint8_t font_reverse[]={
255	,
127	,
191	,
63	,
223	,
95	,
159	,
31	,
239	,
111	,
175	,
47	,
207	,
79	,
143	,
15	,
247	,
119	,
183	,
55	,
215	,
87	,
151	,
23	,
231	,
103	,
167	,
39	,
199	,
71	,
135	,
7	,
251	,
123	,
187	,
59	,
219	,
91	,
155	,
27	,
235	,
107	,
171	,
43	,
203	,
75	,
139	,
11	,
243	,
115	,
179	,
51	,
211	,
83	,
147	,
19	,
227	,
99	,
163	,
35	,
195	,
67	,
131	,
3	,
253	,
125	,
189	,
61	,
221	,
93	,
157	,
29	,
237	,
109	,
173	,
45	,
205	,
77	,
141	,
13	,
245	,
117	,
181	,
53	,
213	,
85	,
149	,
21	,
229	,
101	,
165	,
37	,
197	,
69	,
133	,
5	,
249	,
121	,
185	,
57	,
217	,
89	,
153	,
25	,
233	,
105	,
169	,
41	,
201	,
73	,
137	,
9	,
241	,
113	,
177	,
49	,
209	,
81	,
145	,
17	,
225	,
97	,
161	,
33	,
193	,
65	,
129	,
1	,
254	,
126	,
190	,
62	,
222	,
94	,
158	,
30	,
238	,
110	,
174	,
46	,
206	,
78	,
142	,
14	,
246	,
118	,
182	,
54	,
214	,
86	,
150	,
22	,
230	,
102	,
166	,
38	,
198	,
70	,
134	,
6	,
250	,
122	,
186	,
58	,
218	,
90	,
154	,
26	,
234	,
106	,
170	,
42	,
202	,
74	,
138	,
10	,
242	,
114	,
178	,
50	,
210	,
82	,
146	,
18	,
226	,
98	,
162	,
34	,
194	,
66	,
130	,
2	,
252	,
124	,
188	,
60	,
220	,
92	,
156	,
28	,
236	,
108	,
172	,
44	,
204	,
76	,
140	,
12	,
244	,
116	,
180	,
52	,
212	,
84	,
148	,
20	,
228	,
100	,
164	,
36	,
196	,
68	,
132	,
4	,
248	,
120	,
184	,
56	,
216	,
88	,
152	,
24	,
232	,
104	,
168	,
40	,
200	,
72	,
136	,
8	,
240	,
112	,
176	,
48	,
208	,
80	,
144	,
16	,
224	,
96	,
160	,
32	,
192	,
64	,
128	,
0
};

static void HC595_Data_Send(uint8_t daH, uint8_t daL, uint8_t han, bool output)
{
    uint8_t i;
    Latch_port = 0; 
    CLK_port = 0;
    for(i=0;i<8;i++){
        if((daH&0x80)!=0) DI_port = 1;
        else DI_port = 0;
        CLK_port = 1;
        CLK_port = 0;
        daH <<= 1;
        }
    for(i=0;i<8;i++){
        if((daL&0x80)!=0) DI_port = 1;
        else DI_port = 0;
        CLK_port = 1;
        CLK_port = 0;
        daL <<= 1;
        }
	if(output == true)
	{
	    G_port = 1; 
	    switch (han)
		{
			case 0: 
	            A_port = 0; B_port = 0; C_port = 0; D_port = 0;
	            break;
	        case 1:
	            A_port = 1; B_port = 0; C_port = 0; D_port = 0;
	            break;
	        case 2:
	            A_port = 0; B_port = 1; C_port = 0; D_port = 0;
	            break;
	        case 3: 
	            A_port = 1; B_port = 1; C_port = 0; D_port = 0;
	            break;
	        case 4: 
	            A_port = 0; B_port = 0; C_port = 1; D_port = 0;
	            break;
	        case 5: 
	            A_port = 1; B_port = 0; C_port = 1; D_port = 0;
	            break;
	        case 6: 
	            A_port = 0; B_port = 1; C_port = 1; D_port = 0;
	            break;
	        case 7: 
	            A_port = 1; B_port = 1; C_port = 1; D_port = 0;
	            break;
	        case 8: 
	            A_port = 0; B_port = 0; C_port = 0; D_port = 1;
	            break;
	        case 9: 
	            A_port = 1; B_port = 0; C_port = 0; D_port = 1;
	            break;
	        case 10: 
	            A_port = 0; B_port = 1; C_port = 0; D_port = 1;
	            break;
	        case 11: 
	            A_port = 1; B_port = 1; C_port = 0; D_port = 1;
	            break;
	        case 12: 
	            A_port = 0; B_port = 0; C_port = 1; D_port = 1;
	            break;
	        case 13: 
	            A_port = 1; B_port = 0; C_port = 1; D_port = 1;
	            break;
	        case 14: 
	            A_port = 0; B_port = 1; C_port = 1; D_port = 1;
	            break;
	        case 15: 
	            A_port = 1; B_port = 1; C_port = 1; D_port = 1;
	            break;
			default:
	    		break;
		}
		
	    Latch_port = 1; 
	    G_port = 0;
	    Latch_port = 0;
	}
}
bool LedDotMatrix_init(void)
{
    pinMode(2,OUTPUT);            // setting of LED
    pinMode(3,OUTPUT);            // setting of LED
    pinMode(4,OUTPUT);            // setting of LED
    pinMode(5,OUTPUT);            // setting of LED
    pinMode(6,OUTPUT);            // setting of LED
    pinMode(7,OUTPUT);            // setting of LED
    pinMode(8,OUTPUT);            // setting of LED
    pinMode(9,OUTPUT);            // setting of LED

//    digitalWrite(0,HIGH);
//    digitalWrite(1,HIGH);
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    digitalWrite(4,HIGH);
    digitalWrite(5,HIGH);
    digitalWrite(6,HIGH);
    digitalWrite(7,HIGH);
    digitalWrite(8,HIGH);
    digitalWrite(9,HIGH);
	
//	param.chain_number = number;
	
	return true;
}
void LedDotMatrix_setMemory(uint8_t *up, uint16_t up_size, uint8_t *lo, uint16_t lo_size)
{
	if(up_size)
	{
		param.up = (uint8_t __far *)up;
		param.up_size = up_size;
	}
	if(lo_size)
	{	
		param.lo = (uint8_t __far *)lo;
		param.lo_size = lo_size;
	}
}

void LedDotMatrix_setFlash(uint8_t up_sector, uint16_t up_offset, uint16_t up_size, uint8_t lo_sector,uint16_t lo_offset, uint16_t lo_size)
{
	if(up_size)
	{
		param.up_size = up_size;
		switch(up_sector)
		{
		case 0:
			param.up = (uint8_t __far *)(0x000F0000+up_offset);
			break;
		case 1:
			param.up = (uint8_t __far *)0x000F0400+up_offset;
			break;
		default:
			break;
		}
	}
	if(lo_size)
	{
		param.lo_size = lo_size;
		switch(lo_sector)
		{
		case 0:
			param.lo = (uint8_t __far *)(0x000F0000+lo_offset);
			break;
		case 1:
			param.lo = (uint8_t __far *)(0x000F0400+lo_offset);
			break;
		default:
			break;
		}
	}
	Serial.print("set\t");
	Serial.print_long((long)param.up_size, DEC);
	Serial.print("\t");
	Serial.print_long((long)param.lo_size, DEC);
	Serial.println("");
}

void LedDotMatrix_shift(int speed, bool up_shift, bool lo_shift)
{
	int16_t		i,j;
	uint16_t	up_index = 0;
	uint16_t	lo_index = 0;
	uint8_t		shift = 0;
	int16_t		cycle = 1;				//表示データを変更する回数
	uint8_t		ci[8],co[8];
	uint8_t __far *tmp;
	//シフト系の終了判定
	if(up_shift)
	{
		cycle = param.up_size/8+1;
	}
	if(lo_shift)
	{
		if(param.lo_size>param.up_size) cycle = param.lo_size/8+1;
	}
	if(cycle==0) cycle = 1;
	Serial.print("init\t");
	Serial.print_long((long)cycle, HEX);
	Serial.print("\t");
	Serial.print_long((long)param.up_size, HEX);
	Serial.print("\t");
	Serial.print_long((long)param.lo_size, HEX);
	Serial.println("");
	while(cycle)
	{
		// 表示するデータをメモリからキャッシュにコピーする
		for(i=0;i<9*8;i++)
		{
			tmp = param.up + up_index + i;
			param.up_cashe[i] = (up_index >= param.up_size) ? 0 : *tmp;
			if(i==0)
			{
				Serial.print_long((long)tmp,HEX);
				Serial.print("\t");
				Serial.print_long((long)up_index,HEX);
				Serial.print("\t");
				Serial.print_long((long)param.up_size,HEX);
				Serial.print("\t");
				Serial.print_long((long)*tmp,HEX);
				Serial.print("\t");
				Serial.print_long((long)param.up_cashe[i],HEX);
				Serial.print("\t");
			}
			tmp = param.lo + lo_index + i;
			param.lo_cashe[i] = (lo_index >= param.lo_size) ? 0 : *tmp;
			if(i==0)
			{
				Serial.print_long((long)tmp,HEX);
				Serial.print("\t");
				Serial.print_long((long)lo_index,HEX);
				Serial.print("\t");
				Serial.print_long((long)param.lo_size,HEX);
				Serial.print("\t");
				Serial.print_long((long)*tmp,HEX);
				Serial.print("\t");
				Serial.print_long((long)param.lo_cashe[i],HEX);
				Serial.println("");
			}
		}
		for(shift=0;shift<8;shift++)
		{
			for(j=0;j<speed;j++)
			{
				for(i=0;i<8;i++)		// 上段の表示
				{
					HC595_Data_Send(font_reverse[param.up_cashe[7*8+i]],font_reverse[param.up_cashe[6*8+i]],i,false);
					HC595_Data_Send(font_reverse[param.up_cashe[5*8+i]],font_reverse[param.up_cashe[4*8+i]],i,false);
					HC595_Data_Send(font_reverse[param.up_cashe[3*8+i]],font_reverse[param.up_cashe[2*8+i]],i,false);
					HC595_Data_Send(font_reverse[param.up_cashe[1*8+i]],font_reverse[param.up_cashe[0*8+i]],i,true);
					delayMicroseconds(100);          //表示を変える時間の設定
				}
				for(i=0;i<8;i++)		//下段の表示
				{
					HC595_Data_Send(font_reverse[param.lo_cashe[7*8+i]],font_reverse[param.lo_cashe[6*8+i]],i+8,false);
					HC595_Data_Send(font_reverse[param.lo_cashe[5*8+i]],font_reverse[param.lo_cashe[4*8+i]],i+8,false);
					HC595_Data_Send(font_reverse[param.lo_cashe[3*8+i]],font_reverse[param.lo_cashe[2*8+i]],i+8,false);
					HC595_Data_Send(font_reverse[param.lo_cashe[1*8+i]],font_reverse[param.lo_cashe[0*8+i]],i+8,true);
					delayMicroseconds(100);          //表示を変える時間の設定
				}
			}
			// データをシフトする。
			if(up_shift)
			{
//				Serial.println("upper shift");
				memset(ci,0,sizeof(ci));
				memset(co,0,sizeof(co));
				for(i=8;i>=0;i--)
				{
					for(j=0;j<8;j++)
					{
						ci[j] = co[j];
						co[j] = (param.up_cashe[i*8+j]&0x80) ? 1:0;
						param.up_cashe[i*8+j] = (param.up_cashe[i*8+j]<<1) + ci[j];
					}
				}
			}
			if(lo_shift)
			{
//				Serial.println("lower shift");
				memset(ci,0,8);
				memset(co,0,8);
				for(i=8;i>=0;i--)
				{
					for(j=0;j<8;j++)
					{
						ci[j] = co[j];
						co[j] = (param.lo_cashe[i*8+j]&0x80) ? 1:0;
						param.lo_cashe[i*8+j] = (param.lo_cashe[i*8+j]<<1) + ci[j];
					}
				}
			}
			Serial.print("shift\t");
			Serial.println_long((long)shift, DEC);
		}
		// 文字を更新
		if(up_shift) up_index+=8;
		if(lo_shift) lo_index+=8;
		cycle--;
		Serial.print("cycle\t");
		Serial.println_long((long)cycle, HEX);
	}
}

const LED_DOT_MATRIX LedDotMatrix = {
	LedDotMatrix_init,
	LedDotMatrix_setMemory,
	LedDotMatrix_setFlash,
	LedDotMatrix_shift
};


