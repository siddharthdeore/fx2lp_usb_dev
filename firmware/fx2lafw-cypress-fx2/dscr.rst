ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 1.
Hexadecimal [24-Bits]



                                      1 ;;
                                      2 ;; This file is part of the sigrok-firmware-fx2lafw project.
                                      3 ;;
                                      4 ;; Copyright (C) 2011-2012 Uwe Hermann <uwe@hermann-uwe.de>
                                      5 ;; Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
                                      6 ;;
                                      7 ;; This program is free software; you can redistribute it and/or modify
                                      8 ;; it under the terms of the GNU General Public License as published by
                                      9 ;; the Free Software Foundation; either version 2 of the License, or
                                     10 ;; (at your option) any later version.
                                     11 ;;
                                     12 ;; This program is distributed in the hope that it will be useful,
                                     13 ;; but WITHOUT ANY WARRANTY; without even the implied warranty of
                                     14 ;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                                     15 ;; GNU General Public License for more details.
                                     16 ;;
                                     17 ;; You should have received a copy of the GNU General Public License
                                     18 ;; along with this program; if not, see <http://www.gnu.org/licenses/>.
                                     19 ;;
                                     20 
                           00B404    21 VID = 0xb404	; Manufacturer ID (0x04b4)
                           001386    22 PID = 0x1386	; Product ID (0x8613)
                                     23 
ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 2.
Hexadecimal [24-Bits]



                                     24 .include "dscr.inc"
                                      1 ;;
                                      2 ;; This file is part of the sigrok-firmware-fx2lafw project.
                                      3 ;;
                                      4 ;; Copyright (C) 2011-2012 Uwe Hermann <uwe@hermann-uwe.de>
                                      5 ;; Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
                                      6 ;;
                                      7 ;; This program is free software; you can redistribute it and/or modify
                                      8 ;; it under the terms of the GNU General Public License as published by
                                      9 ;; the Free Software Foundation; either version 2 of the License, or
                                     10 ;; (at your option) any later version.
                                     11 ;;
                                     12 ;; This program is distributed in the hope that it will be useful,
                                     13 ;; but WITHOUT ANY WARRANTY; without even the implied warranty of
                                     14 ;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                                     15 ;; GNU General Public License for more details.
                                     16 ;;
                                     17 ;; You should have received a copy of the GNU General Public License
                                     18 ;; along with this program; if not, see <http://www.gnu.org/licenses/>.
                                     19 ;;
                                     20 
ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 3.
Hexadecimal [24-Bits]



                                     21 .include "common.inc"
                                      1 ;;
                                      2 ;; This file is part of the sigrok-firmware-fx2lafw project.
                                      3 ;;
                                      4 ;; Copyright (C) 2016 Stefan Brüns <stefan.bruens@rwth-aachen.de>
                                      5 ;;
                                      6 ;; This program is free software; you can redistribute it and/or modify
                                      7 ;; it under the terms of the GNU General Public License as published by
                                      8 ;; the Free Software Foundation; either version 2 of the License, or
                                      9 ;; (at your option) any later version.
                                     10 ;;
                                     11 ;; This program is distributed in the hope that it will be useful,
                                     12 ;; but WITHOUT ANY WARRANTY; without even the implied warranty of
                                     13 ;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                                     14 ;; GNU General Public License for more details.
                                     15 ;;
                                     16 ;; You should have received a copy of the GNU General Public License
                                     17 ;; along with this program; if not, see <http://www.gnu.org/licenses/>.
                                     18 ;;
                                     19 
                                     20 .macro string_descriptor_a n,str
                                     21 _string'n:
                                     22 	.nchr	len,"'str"
                                     23 	.db	len * 2 + 2
                                     24 	.db	3
                                     25 	.irpc	i,^"'str"
                                     26 		.db	''i, 0
                                     27 	.endm
                                     28 .endm
                                     29 
                                     30 .macro string_descriptor_lang n,l
                                     31 _string'n:
                                     32 	.db	4
                                     33 	.db	3
                                     34 	.dw	>l + (<l * 0x100)
                                     35 .endm
ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 4.
Hexadecimal [24-Bits]



                                     22 
                                     23 .module DEV_DSCR
                                     24 
                                     25 ; Descriptor types
                           000001    26 DSCR_DEVICE_TYPE	= 1
                           000002    27 DSCR_CONFIG_TYPE	= 2
                           000003    28 DSCR_STRING_TYPE	= 3
                           000004    29 DSCR_INTERFACE_TYPE	= 4
                           000005    30 DSCR_ENDPOINT_TYPE	= 5
                           000006    31 DSCR_DEVQUAL_TYPE	= 6
                                     32 
                                     33 ; Descriptor lengths
                           000009    34 DSCR_INTERFACE_LEN	= 9
                           000007    35 DSCR_ENDPOINT_LEN	= 7
                                     36 
                                     37 ; Endpoint types
                           000000    38 ENDPOINT_TYPE_CONTROL	= 0
                           000001    39 ENDPOINT_TYPE_ISO	= 1
                           000002    40 ENDPOINT_TYPE_BULK	= 2
                           000003    41 ENDPOINT_TYPE_INT	= 3
                                     42 
                                     43 .globl _dev_dscr, _dev_qual_dscr, _highspd_dscr, _fullspd_dscr, _dev_strings, _dev_strings_end
                                     44 .area DSCR_AREA (CODE)
                                     45 
                                     46 ; -----------------------------------------------------------------------------
                                     47 ; Device descriptor
                                     48 ; -----------------------------------------------------------------------------
      001E00                         49 _dev_dscr:
      001E00 12                      50 	.db	dev_dscr_end - _dev_dscr
      001E01 01                      51 	.db	DSCR_DEVICE_TYPE
      001E02 00 02                   52 	.dw	0x0002			; USB 2.0
      001E04 FF                      53 	.db	0xff			; Class (vendor specific)
      001E05 FF                      54 	.db	0xff			; Subclass (vendor specific)
      001E06 FF                      55 	.db	0xff			; Protocol (vendor specific)
      001E07 40                      56 	.db	64			; Max. EP0 packet size
      001E08 B4 04                   57 	.dw	VID			; Manufacturer ID
      001E0A 13 86                   58 	.dw	PID			; Product ID
      001E0C 01 00                   59 	.dw	0x0100			; Product version (0.01)
      001E0E 01                      60 	.db	1			; Manufacturer string index
      001E0F 02                      61 	.db	2			; Product string index
      001E10 03                      62 	.db	3			; Serial number string index (none)
      001E11 01                      63 	.db	1			; Number of configurations
      001E12                         64 dev_dscr_end:
                                     65 
                                     66 ; -----------------------------------------------------------------------------
                                     67 ; Device qualifier (for "other device speed")
                                     68 ; -----------------------------------------------------------------------------
      001E12                         69 _dev_qual_dscr:
      001E12 0A                      70 	.db	dev_qualdscr_end - _dev_qual_dscr
      001E13 06                      71 	.db	DSCR_DEVQUAL_TYPE
      001E14 00 02                   72 	.dw	0x0002			; USB 2.0
      001E16 FF                      73 	.db	0xff			; Class (vendor specific)
      001E17 FF                      74 	.db	0xff			; Subclass (vendor specific)
      001E18 FF                      75 	.db	0xff			; Protocol (vendor specific)
      001E19 40                      76 	.db	64			; Max. EP0 packet size
ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 5.
Hexadecimal [24-Bits]



      001E1A 01                      77 	.db	1			; Number of configurations
      001E1B 00                      78 	.db	0			; Extra reserved byte
      001E1C                         79 dev_qualdscr_end:
                                     80 
                                     81 ; -----------------------------------------------------------------------------
                                     82 ; High-Speed configuration descriptor
                                     83 ; -----------------------------------------------------------------------------
      001E1C                         84 _highspd_dscr:
      001E1C 09                      85 	.db	highspd_dscr_end - _highspd_dscr
      001E1D 02                      86 	.db	DSCR_CONFIG_TYPE
                                     87 	; Total length of the configuration (1st line LSB, 2nd line MSB)
      001E1E 19                      88 	.db	(highspd_dscr_realend - _highspd_dscr) % 256
      001E1F 00                      89 	.db	(highspd_dscr_realend - _highspd_dscr) / 256
      001E20 01                      90 	.db	1			; Number of interfaces
      001E21 01                      91 	.db	1			; Configuration number
      001E22 00                      92 	.db	0			; Configuration string (none)
      001E23 80                      93 	.db	0x80			; Attributes (bus powered, no wakeup)
      001E24 32                      94 	.db	0x32			; Max. power (100mA)
      001E25                         95 highspd_dscr_end:
                                     96 
                                     97 	; Interfaces (only one in our case)
      001E25 09                      98 	.db	DSCR_INTERFACE_LEN
      001E26 04                      99 	.db	DSCR_INTERFACE_TYPE
      001E27 00                     100 	.db	0			; Interface index
      001E28 00                     101 	.db	0			; Alternate setting index
      001E29 01                     102 	.db	1			; Number of endpoints
      001E2A FF                     103 	.db	0xff			; Class (vendor specific)
      001E2B FF                     104 	.db	0xff			; Subclass (vendor specific)
      001E2C FF                     105 	.db	0xff			; Protocol (vendor specific)
      001E2D 00                     106 	.db	0			; String index (none)
                                    107 
                                    108 	; Endpoint 2 (IN)
      001E2E 07                     109 	.db	DSCR_ENDPOINT_LEN
      001E2F 05                     110 	.db	DSCR_ENDPOINT_TYPE
      001E30 82                     111 	.db	0x82			; EP number (2), direction (IN)
      001E31 02                     112 	.db	ENDPOINT_TYPE_BULK	; Endpoint type (bulk)
      001E32 00                     113 	.db	0x00			; Max. packet size, LSB (512 bytes)
      001E33 02                     114 	.db	0x02			; Max. packet size, MSB (512 bytes)
      001E34 00                     115 	.db	0x00			; Polling interval (ignored for bulk)
                                    116 
      001E35                        117 highspd_dscr_realend:
                                    118 
      001E36                        119 	.even
                                    120 
                                    121 ; -----------------------------------------------------------------------------
                                    122 ; Full-Speed configuration descriptor
                                    123 ; -----------------------------------------------------------------------------
      001E36                        124 _fullspd_dscr:
      001E36 09                     125 	.db	fullspd_dscr_end - _fullspd_dscr
      001E37 02                     126 	.db	DSCR_CONFIG_TYPE
                                    127 	; Total length of the configuration (1st line LSB, 2nd line MSB)
      001E38 19                     128 	.db	(fullspd_dscr_realend - _fullspd_dscr) % 256
      001E39 00                     129 	.db	(fullspd_dscr_realend - _fullspd_dscr) / 256
      001E3A 01                     130 	.db	1			; Number of interfaces
      001E3B 01                     131 	.db	1			; Configuration number
ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 6.
Hexadecimal [24-Bits]



      001E3C 00                     132 	.db	0			; Configuration string (none)
      001E3D 80                     133 	.db	0x80			; Attributes (bus powered, no wakeup)
      001E3E 32                     134 	.db	0x32			; Max. power (100mA)
      001E3F                        135 fullspd_dscr_end:
                                    136 
                                    137 	; Interfaces (only one in our case)
      001E3F 09                     138 	.db	DSCR_INTERFACE_LEN
      001E40 04                     139 	.db	DSCR_INTERFACE_TYPE
      001E41 00                     140 	.db	0			; Interface index
      001E42 00                     141 	.db	0			; Alternate setting index
      001E43 01                     142 	.db	1			; Number of endpoints
      001E44 FF                     143 	.db	0xff			; Class (vendor specific)
      001E45 FF                     144 	.db	0xff			; Subclass (vendor specific)
      001E46 FF                     145 	.db	0xff			; Protocol (vendor specific)
      001E47 00                     146 	.db	0			; String index (none)
                                    147 
                                    148 	; Endpoint 2 (IN)
      001E48 07                     149 	.db	DSCR_ENDPOINT_LEN
      001E49 05                     150 	.db	DSCR_ENDPOINT_TYPE
      001E4A 82                     151 	.db	0x82			; EP number (2), direction (IN)
      001E4B 02                     152 	.db	ENDPOINT_TYPE_BULK	; Endpoint type (bulk)
      001E4C 40                     153 	.db	0x40			; Max. packet size, LSB (64 bytes)
      001E4D 00                     154 	.db	0x00			; Max. packet size, MSB (64 bytes)
      001E4E 00                     155 	.db	0x00			; Polling interval (ignored for bulk)
                                    156 
      001E4F                        157 fullspd_dscr_realend:
                                    158 
      001E50                        159 	.even
                                    160 
                                    161 ; -----------------------------------------------------------------------------
                                    162 ; Strings
                                    163 ; -----------------------------------------------------------------------------
                                    164 
      001E50                        165 _dev_strings:
                                    166 
                                    167 ; See http://www.usb.org/developers/docs/USB_LANGIDs.pdf for the full list.
      001E50                        168 string_descriptor_lang 0 0x0409 ; Language code 0x0409 (English, US)
      000050                          1 _string0:
      001E50 04                       2 	.db	4
      001E51 03                       3 	.db	3
      001E52 09 04                    4 	.dw	>0x0409 + (<0x0409 * 0x100)
                                    169 
      001E54                        170 string_descriptor_a 1,^"sigrok"
      000054                          1 _string1:
                           000006     2 	.nchr	len,"sigrok"
      001E54 0E                       3 	.db	len * 2 + 2
      001E55 03                       4 	.db	3
                                      5 	.irpc	i,^"sigrok"
                                      6 		.db	''i, 0
                                      7 	.endm
      001E56 73 00                    1 		.db	's, 0
      001E58 69 00                    1 		.db	'i, 0
      001E5A 67 00                    1 		.db	'g, 0
      001E5C 72 00                    1 		.db	'r, 0
      001E5E 6F 00                    1 		.db	'o, 0
ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 7.
Hexadecimal [24-Bits]



      001E60 6B 00                    1 		.db	'k, 0
      001E62                        171 string_descriptor_a 2,^"fx2lafw"
      000062                          1 _string2:
                           000007     2 	.nchr	len,"fx2lafw"
      001E62 10                       3 	.db	len * 2 + 2
      001E63 03                       4 	.db	3
                                      5 	.irpc	i,^"fx2lafw"
                                      6 		.db	''i, 0
                                      7 	.endm
      001E64 66 00                    1 		.db	'f, 0
      001E66 78 00                    1 		.db	'x, 0
      001E68 32 00                    1 		.db	'2, 0
      001E6A 6C 00                    1 		.db	'l, 0
      001E6C 61 00                    1 		.db	'a, 0
      001E6E 66 00                    1 		.db	'f, 0
      001E70 77 00                    1 		.db	'w, 0
ASxxxx Assembler V02.00 + NoICE + SDCC mods  (Intel 8051), page 8.
Hexadecimal [24-Bits]



      001E72                         25 string_descriptor_a 3,^"Cypress FX2"
      000072                          1 _string3:
                           00000B     2 	.nchr	len,"Cypress FX2"
      001E72 18                       3 	.db	len * 2 + 2
      001E73 03                       4 	.db	3
                                      5 	.irpc	i,^"Cypress FX2"
                                      6 		.db	''i, 0
                                      7 	.endm
      001E74 43 00                    1 		.db	'C, 0
      001E76 79 00                    1 		.db	'y, 0
      001E78 70 00                    1 		.db	'p, 0
      001E7A 72 00                    1 		.db	'r, 0
      001E7C 65 00                    1 		.db	'e, 0
      001E7E 73 00                    1 		.db	's, 0
      001E80 73 00                    1 		.db	's, 0
      001E82 20 00                    1 		.db	' , 0
      001E84 46 00                    1 		.db	'F, 0
      001E86 58 00                    1 		.db	'X, 0
      001E88 32 00                    1 		.db	'2, 0
      001E8A                         26 _dev_strings_end:
      001E8A 00 00                   27 	.dw	0x0000
