/* stdint.h - ANSI C99 規格の stdint.h の代わり
 * Last modified: Fri Jan 16 18:37:35 JST 2015 (kobayasy)
 * 
 *   ANSI C99 規格に準拠していないコンパイラでビルドする場合に使用。
 *
 *   最低限必要な宣言のみしか実装しておらず ANSI C99 規格には準拠してい
 * ない仮実装なので注意。 コンパイラにANSI準拠の stdint.h が準備されて
 * いるならばそちらを使う事。
 */

#ifndef _INCLUDE_STDINT_H
#define _INCLUDE_STDINT_H


typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned long uint32_t;
typedef signed long int32_t;


#endif  /* #ifndef _INCLUDE_STDINT_H */
