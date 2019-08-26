/*
	
	Pegg_el 0.11 - Linux support for Casio label printers.

	pegg_el.h
	
	Copyright (c) 2003,2004 Daniel Amkreutz, <daniel.amkreutz@tu-harburg.de>
	          (c) 2004 Alex Perry
		  

        IMPORTANT NOTICE:

	* This software was designed using data made available by and is released
          with the permission of CASIO COMPUTER CO., LTD.
        * CASIO COMPUTER Co., LTD. assumes no responsibility for the content of
          this software.
        * Please do not contact CASIO COMPUTER CO., LTD. with any inquiries
          concerning this software.

	A large amount of code is borrowed from the libusb Documentation.
	The communication API of the Printer has been greatly documented
	and published by CASIO COMPUTER CO., LTD

							Many Thanks.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

	You may use this program for whatever you want as long the above written
	text remains.

	Please support Pegg_el by testing it and reporting bugs. !
*/

#include <strings.h>
#include <stdio.h>
#include <usb.h>

const char 	WIT = 0x1f,	/* The Printer's contol codes*/
     		STX = 0x02,
     		EOT = 0x04,	/* Reffer to the Command Scpec's by Casio */
     		ENQ = 0x05,
     		ACK = 0x06,
     		NAK = 0x15,
     		SYN = 0x16,
     		CAN = 0x18,
		PUP = 0x0E,
     		EOS = 0x1e;


const char	_TC   = 0x07, 	/* The Printer's command codes for the EL Series*/
     		_PTC  = 0x00,
		_RST  = 0x01,
     		_DMS  = 0x09,
     		_DPS  = 0x0A,
     		_AFS  = 0x0D,
		_MTC  = 0x17,
		_CMS  = 0x19,
		_MTI  = 0x1A,
		_TF   = 0x1B,
		_PSA  = 0x1C,
		_PDT  = 0xFE,
		_PDE  = 0x04;

int 		width   = 512,  /* Default width & Height */
		height  = 64,
		offset  = 0,
		print_height = 0x00,
		testmode = 0,
		ignore_read_error = 0,
		el700 = 1;

char		density = 0x00, /* Default printing parameters */
		speed   = 0x00,
		margin  = 0x02,
		cut     = 0x00,
		head	= 0x00;
		

int _interfacenumber, 	/* Interface and Endpoints of the printer*/
    _endpoint_r,
    _endpoint_w;
    
char _read_buffer[8],	/* Read and Write buffer */
     _write_buffer[64];

char *pnm_filename = NULL;

struct usb_bus *busses;		/* libUSB structures */
struct usb_bus *bus;
struct usb_device *dev;
usb_dev_handle *_eggprinter;



