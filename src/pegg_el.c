/*
	
	Pegg_el 0.12 - Linux support for Casio label printers.

	pegg_el.c
	
	Copyright (c) 2003-2005 Daniel Amkreutz, <daniel.amkreutz@tu-harburg.de>
		  (c) 2005 Alex Perry


        IMPORTANT NOTICE:

	* This software was designed using data made available by and is released
          with the permission of CASIO COMPUTER CO., LTD.
        * CASIO COMPUTER Co., LTD. assumes no responsibility for the content of
          this software.
        * Please do not contact CASIO COMPUTER CO., LTD. with any inquiries
          concerning this software.

	A large amount of code is borrowed from the libusb documentation.
	The communication API of the printer was greatly documented
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



        ----------------------------------------------------------------------------
	EL-5000W users:
	Before you can use pegg_el you will have to change the ProductID in line 333
	to match the EL-5000W. 
        ----------------------------------------------------------------------------

*/

#include "pegg_el.h"
#include <ctype.h>

main (int argc, char *argv[]) {		/* The Main Program handles commandline args 
				        and processes the image-file */
   int i, n=0;
   int c=0;
   FILE * raw_file;
   
   printf("Pegg_EL Version 0.12 - (c)2003-2005 Daniel Amkreutz, (c)2004 Alex Perry\n\n");
   
   if (argc < 2) {
     
     printf("WRONG NUMBER OF ARGUMENTS\n\n"); 
     printf("\tUSAGE:\n\n");	     
     printf("\tpegg_el -w -h (-C) (-t) (-o) (-d) (-c) (-s) (-f) (-H) (-O) PEGG-FILE\n\n");		
     printf("\t USING THE OPTIONS:\n\n");
     printf("\t\t-w<width>\tprinting width (variable eg. -w512 = 512px width)\n");
     printf("\t\t-h<height>\tprinting heigt (fixed to 64,128,256,384px)\n");
     printf("\t\t-o<offset>\toffset (variable from 0..255px)\n");
     printf("\t\t-t\t\tTests the printer connection\n");
     printf("\t\t-C\t\tJust cut the label\n");
     printf("\t\t-d<dens>\tSets the density (-d0=light..-d5=dark)\n");
     printf("\t\t-c<mode>\tOperates cutter (-c0=normal,-c1=half cut, -c2=no cut)\n");
     printf("\t\t-s<speed>\tSpeed (-s0=normal, -s1=high speed)\n");
     printf("\t\t-f<feed>\tFeed (-f0=no feed, -f1=large margin, -f2=small margin, -f3=medium margin)\n");
     printf("\t\t-H<head>\tSelect Printhead (only EL-5000W) -H0=256dot head, -H1=64dot head\n");
     printf("\t\t-O<file>\tExport to <file> instead of printing\n");
     exit(-1);
   }
     
     
       
    for (i=0; i < argc; i++) {		/* Handle command line argunemts*/
    
      if ((strstr(argv[i],"-w") != 0x00) and (strtok(argv[i], "-w") != NULL)) {
         width = chr2int(strtok(argv[i], "-w"));
      }
       
      if ((strstr(argv[i],"-h") != 0x00) and (strtok(argv[i], "-h") != NULL)) {
         height = chr2int(strtok(argv[i], "-h"));
      }

      if ((strstr(argv[i],"-o") != 0x00) and (strtok(argv[i], "-o") != NULL)) {
         offset = chr2int(strtok(argv[i], "-o"));
      }

      if ((strstr(argv[i],"-O") != 0x00) and (strtok(argv[i], "-O") != NULL)) {
         pnm_filename = 2+strstr(argv[i], "-O");
      }
          
      if ((strstr(argv[i],"-c") != 0x00) and (strtok(argv[i], "-c") != NULL)) {
         switch(chr2int(strtok(argv[i], "-c"))) {
             case 0: cut = 0x00;
	     break;
     
	     case 1: cut = 0x01;
	     break;
	  
	     case 2: cut = 0xFF;
             break;
         }
      }
    
    if (strstr(argv[i],"-H") != 0x00) {
      if(strtok(argv[i], "-H") != NULL) { 
       switch(chr2int(strtok(argv[i], "-H"))) {
          case 0: head = 0x00;
	  break;
	  
	  case 1: head = 0x01;
          break;
       }
      }
     }

     
    if (strstr(argv[i],"-d") != 0x00) {
      if(strtok(argv[i], "-d") != NULL) { 
       switch(chr2int(strtok(argv[i], "-d"))) {
          case 0: density = 0xFF;
	  break;
          
	  case 1: density = 0xFE;
	  break;
	  
	  case 2: density = 0x00;
          break;
	  
	  case 3: density = 0x01;
          break;
	  
	  case 4: density = 0x02;
          break;
       }
      }
     }
    if (strstr(argv[i],"-s") != 0x00) {
      if(strtok(argv[i], "-s") != NULL) { 
       switch(chr2int(strtok(argv[i], "-s"))) {
          case 0: speed = 0x00;
	  break;
          
	  case 1: speed = 0x01;
	  break;
       }
      }
     }
    if (strstr(argv[i],"-f") != 0x00) {
      if(strtok(argv[i], "-f") != NULL) { 
       switch(chr2int(strtok(argv[i], "-f"))) {
          case 0: margin = 0x01;
	  break;
          
	  case 1: margin = 0x02;
	  break;
	  
	  case 2: margin = 0x40;
          break;
	  
	  case 3: margin = 0x80;
          break;
       }
      }
     }
           
    if (strstr(argv[i],"-t") != 0x00) testmode = 1;   
    
    if (strstr(argv[i],"-C") != 0x00) {
        if(open_printer() < 0){
         return -1;
        }
 
        _write_buffer[0] = 0x07;		/* Cut the tape */
          
        write_to_printer(1);
        close_printer();
        exit(0);
      }
    }
    
    switch(height) {
     case 64 : print_height = 0x01;
     break;
     case 128: print_height = 0x00;
     break;
     case 256: print_height = 0x04;
     break;
     case 384: print_height = 0x05;
     break;

    }
    						/* End commandline parsing */
    
    char data[width*height-1];    		/* Now we know the size of the imagefile*/
    
    if (testmode == 0) {    
     raw_file = fopen(argv[i-1],"rb");		/* The raw-file is the LAST argument so it can
    						   be read here */
     if (raw_file==NULL){
       printf("Error opening file: %s !\n", argv[i-1]);
       exit(-1);
     } else {
      for (n=0; n < (width*height)/8; n++) {
        c = getc(raw_file);
        data[n] = c;
      }
     fclose (raw_file);     
     printf ("%i Bytes read.\n",n);
     }

     if (pnm_filename == NULL) {
      print_label(data); 
      exit(0);
     }
    
     printf ( "*%s*\n", pnm_filename );
     raw_file = fopen ( pnm_filename, "wb" );
     if ( raw_file == NULL ) {
        perror ( "Failed to open pnm output file" );
     exit(-2);
     }

				      		/* This generates PNMPLAIN format output */
     fprintf ( raw_file, "P1 %i %i\n", width, height );
      for ( i=0; i<height; i++ )
        for ( c=0; c<width; c++ )
          fprintf ( raw_file, "%i%c",
            0 != ( ( (unsigned int) data[(c*(height/8))+(i/8)] )
             & ( ((unsigned int)1)<<(i%8) )
             ),
           ( ((c%35)==134) || (c==width-1) ) ? '\n' : ' ' );

     fclose ( raw_file );

      
    } else {
     test_printer();
    }
    exit(0);
  
}

int chr2int (char s[]) {
  int i,n;
  n=0;
  for (i=0;isdigit(s[i]); ++i) {      
      n = 10 * n + (s[i]-'0');
  }
  return n;
}


int write_to_printer(int bytes) {	/* write the date in _write_buffer to the printer */
  					/* Printer accepts only Packets site of 1,2,16,64 bytes */

  if (bytes == 1 || bytes == 2 || bytes == 16 || bytes == 64){

    if (usb_bulk_write(_eggprinter, _endpoint_w, (void *)_write_buffer ,bytes ,500) == bytes) {
      bzero(_write_buffer, 64);
      return 0;
      
    }
    bzero(_write_buffer, 64);
    return -2;			/* Error code -1 = wrong size */
  }
  bzero(_write_buffer, 64);
  return -1;			/* Error code -2 = transmission failed */

}


int recv_from_printer(int bytes) {	/* read data from the printer and write it to _read_buffer */

  int n,rd=0, loop=0;
  bzero(_read_buffer, 8);

  while ( rd < bytes && loop < 10 ) {	/* Read loop, in order to handle delays btw. transmission */

    n = usb_bulk_read(_eggprinter, _endpoint_r, (void *)_read_buffer, bytes, 50);

    if ( n == 0 ) {
      usleep(20000);
      continue;
    }

    if ( n <= 0) {
      if (ignore_read_error == 0) {
       printf("USB_BULK_READ ERROR ! Use strace for more information. \n");
      }
      return -1;
    }

    rd += n;
    loop++;
  }

  return n;

}



int scan_usb (void) {		/* This function scans the USB and sets up the Printer communication */

  struct usb_config_descriptor *cdesc;
  struct usb_interface *interface;
  struct usb_interface_descriptor *idesc;
  struct usb_endpoint_descriptor *epdesc;

  usb_init();
  usb_find_busses();
  usb_find_devices();

  busses = usb_get_busses();

  for (bus = busses; bus; bus = bus->next) {		/* Scanning trough all Busses and Devices */
    for (dev = bus->devices; dev; dev = dev->next) {

      if (dev->descriptor.idVendor == 0x07cf) {		/* CASIO's Vendor ID*/

        if (dev->descriptor.idProduct == 0x4006) {	/* Product ID of the EL-700*/
        
	/* EL-5000W users: 
	   ---------------
	   change the if-statement above to match the product ID of the EL-5000W Printer.
           For the EL-700 the product id is (as you can see) 0x4006. You can get the
	   product ID of the EL-5000W printer by looking at your system-log while
	   plugging the printer into the usb port.   
	
	*/  
	
	  cdesc = dev->config;				/* Setting all necessariy device variables */
	  interface = cdesc->interface;
	  idesc = interface->altsetting;
	  epdesc = idesc->endpoint;
          
	  _endpoint_w = epdesc->bEndpointAddress;	/* Writing Endpoint  */
          
	  epdesc = &idesc->endpoint[1];			/* Switch endpoint to get the ... */
	  _endpoint_r = epdesc->bEndpointAddress;	/* ...reading EndPoint */
	  
	  _eggprinter = usb_open(dev);


	  if (usb_claim_interface(_eggprinter, _interfacenumber) < 0) {
	    printf("ERROR: Could not open Interface! Make sure you have the appopriate rights. \n");
	    return -1;
  	  }
	  return 0;
 	}
      }

    }
  }

 return 1;

}


int open_printer () {

  int found;

  found = scan_usb();
  printf("Scanning USB ...\n");

  if (found == 0) {
    printf("Found Casio EL - Label Printer.\n");
     }

  if (found != 0) {
    printf("No device(s) found. - Maybe uhci.o module not loaded ?\n\n");
     return -1;
  }

  return 0;

}
int close_printer () {			/* Closes the Printer */

  usb_release_interface(_eggprinter, _interfacenumber);
  usb_close(_eggprinter);
  //usb_reset(_eggprinter);	       /* This reset is due to some problems with users of SuSe Linux */
  printf("Printer closed. \n\n");
  return 0;
}


int test_printer() {

  int ret = 0;			

  ret = open_printer();

  if(ret < 0){
    return -1;
  }

  
  _write_buffer[0] = STX;		/* Get Mount tape information */
  _write_buffer[1] = _MTI;
   
  write_to_printer(16);
  recv_from_printer(8);

  printf("Currently mounted tape: \n\n");
  
  if(_read_buffer[4] == 0x00) printf("   No tape mounted\n\n");
  if(_read_buffer[4] == 0x0F) printf("   Decorative Tape 18mm\n\n");
  if(_read_buffer[4] == 0x1E) printf("   Kraft lables 24mm\n\n");
  if(_read_buffer[4] == 0x3A) printf("   Monochrome 6mm\n\n");
  if(_read_buffer[4] == 0x3B) printf("   Monochrome 12mm\n\n");
  if(_read_buffer[4] == 0x3D) printf("   Monochrome 9mm\n\n");
  if(_read_buffer[4] == 0x3E) printf("   Monochrome 24mm\n\n");
  if(_read_buffer[4] == 0x3F) printf("   Monochrome 18mm\n\n");
  if(_read_buffer[4] == 0x6F) printf("   Long tape 18mm\n\n");
  printf("Transfer complete.\n");
  close_printer();
  return 0;
 
  
}


int print_label(char printdata[]) {
  
  
  int full_blocks = 0,		/* Counter for Fileposition */
      fraction,
      pages = 1,
      count_bytes = 0,
      i = 0,
      n = 0;
      
       
  char mounted_tape = 0x00;
  int ret = 0;		/* For communication sheme look at the Casio Command Spec */

  
  pages = ((width*height)/8) / 16384;
  
  
  full_blocks = (((width*height)/8)-(pages*16384)) / 60;
  fraction    = (((width*height)/8)-(pages*16384)) % 60;

  ret = open_printer();

  if(ret < 0) {
    return -1;
  }
 
  printf("Getting mount tape information... "); /* Get Mount tape information */
  _write_buffer[0] = STX;	
  _write_buffer[1] = _MTI;
   
  write_to_printer(16);
  recv_from_printer(8);
  
  mounted_tape = _read_buffer[4];
  printf("Tape ID: %X OK\nChecking for EL-5000W... ",mounted_tape);

  _write_buffer[0] = STX;	/*  ----------------- Product type check EL-5000W */
  _write_buffer[1] = _PTC;
  _write_buffer[2] = 0x05;
  _write_buffer[3] = 0x00;
  _write_buffer[4] = 0x5A;
  _write_buffer[5] = 0x58;
  _write_buffer[6] = 0x32;
  _write_buffer[7] = 0x31;
  _write_buffer[8] = 0x32;
  
  write_to_printer(16);
  recv_from_printer(1);

  if(_read_buffer[0] == ACK) {
     printf("FOUND\n");
     el700=0;
  } else {
   printf("NOT FOUND\n");
  }
  
  if(el700 == 1) {
   printf("Checking for EL-700... ");
   _write_buffer[0] = STX;	/*  ------------------- Product type check EL-700 */
   _write_buffer[1] = _PTC;
   _write_buffer[2] = 0x05;
   _write_buffer[3] = 0x00;
   _write_buffer[4] = 0x5A;
   _write_buffer[5] = 0x58;
   _write_buffer[6] = 0x32;
   _write_buffer[7] = 0x30;
   _write_buffer[8] = 0x32;
  
   write_to_printer(16);
   recv_from_printer(1);

   if(_read_buffer[0] == ACK) printf("FOUND\n"); else {
   printf("NOT FOUND\n");
   exit(-1);
   }
  }
     
  printf("Mount tape check... ");
  _write_buffer[0] = STX;	/* -------------------------- Mount tape check */
  _write_buffer[1] = _MTC;
  _write_buffer[2] = 0x01;
  _write_buffer[3] = 0x00;
  _write_buffer[4] = mounted_tape;
  
  write_to_printer(16);
  recv_from_printer(1);

  if(_read_buffer[0] == ACK) printf("OK\n"); else {
   printf("ERROR\n");
   exit(-1);
  }

     
  printf("Resetting printer... ");
  _write_buffer[0] = STX;	/* ---------------------------------------Reset */
  _write_buffer[1] = _RST;
  
  write_to_printer(16);
  recv_from_printer(1);
  
  if(_read_buffer[0] == ACK) printf("OK\n"); else {
   printf("ERROR\n");
   exit(-1);
  }

  if(el700 == 1) {
   printf("Print speed adjust... ");
   _write_buffer[0] = STX;	/* -------------------------- Mount tape check */
   _write_buffer[1] = _PSA;
   _write_buffer[2] = 0x01;
   _write_buffer[3] = 0x00;
   _write_buffer[4] = speed;
  
   write_to_printer(16);
   recv_from_printer(1);

   if(_read_buffer[0] == ACK) printf("OK\n"); else {
    printf("ERROR\n");
    exit(-1);
   }
  }
  
  printf("Automatic feed setting... ");
  _write_buffer[0] = STX;	/* -------------------------- Automatic feed setting */
  _write_buffer[1] = _AFS;
  _write_buffer[2] = 0x01;
  _write_buffer[3] = 0x00;
  _write_buffer[4] = margin;
  
  write_to_printer(16);
  recv_from_printer(1);

  if(_read_buffer[0] == ACK) printf("OK\n"); else {
   printf("ERROR\n");
   exit(-1);
  }

  
  printf("Deployment setting... ");
  _write_buffer[0] = STX;	/* -------------------------- Deployment setting */
  _write_buffer[1] = _DMS;
  _write_buffer[2] = 0x06;
  _write_buffer[3] = 0x00;
  _write_buffer[4] = print_height;
  _write_buffer[5] = offset;
  _write_buffer[6] = 0x01;
  _write_buffer[7] = 0x00;
  _write_buffer[8] = density;
  _write_buffer[9] = head;  
  write_to_printer(16);
  recv_from_printer(1);

  if(_read_buffer[0] == ACK) printf("OK\n"); else {
   printf("ERROR\n");
   exit(-1);
  }
  
    if (cut == 0x01) {
    cut = 0x00;
    printf("Performing half cut...");
    fflush( stdout );
    _write_buffer[0] = STX;  
    _write_buffer[1] = 0x1B;  
    _write_buffer[2] = 0x01;  
    _write_buffer[3] = 0x00;  
    _write_buffer[4] = 0x10;    
    write_to_printer(16);
    wait_for_ready();
  
    _write_buffer[0] = 0x09;
    write_to_printer(1);
    
    wait_for_ready();
   }

  printf("Cut mode Setting setting... ");
  
  _write_buffer[0] = STX;	/* -------------------------- Cut mode setting */
  _write_buffer[1] = _CMS;
  _write_buffer[2] = 0x01;
  _write_buffer[3] = 0x00;
  _write_buffer[4] = cut;
  
  write_to_printer(16);
  recv_from_printer(1);

  if(_read_buffer[0] == ACK) printf("OK\n"); else {
   printf("ERROR\n");
   exit(-1);
  }

  
  /*DATA TRANSFER ------------------------------------------------------------- */
  
  printf("\nTransferring %d Bytes (%i Pages, %i Blocks + %i Bytes\n\n", 
        (width*height)/8, pages, full_blocks, fraction); 
  

  /* FULL PAGE PRINTING ------------------------------------------------------- */ 
  for(n=0; n < pages; n++) {
   printf("Sending page %d of %d :\n",n+1,pages); 
   for(i=0; i < 273; i++) {
  
    _write_buffer[0] = STX;
    _write_buffer[1] = _PDT;
    _write_buffer[2] = 0x3C;
    _write_buffer[3] = 0x00;
   
    for(count_bytes = 0; count_bytes < 60; count_bytes++) {
      _write_buffer[count_bytes + 4] = printdata[(i*60)+count_bytes+n*16384];
    }
    
    write_to_printer(64);
    recv_from_printer(1);

    
    if(_read_buffer[0] == ACK){
       if ((i+1)%10 == 0) {
        printf("%d",i+1); 
        fflush( stdout );
        if ((i+1)%50 == 0) {
         printf("\n"); 
         fflush( stdout );
       	}
       } else {
        printf("."); 
        fflush( stdout );
       }
       
    }else {
      printf("No ACK recieved. STOP\n");
      exit(-1);
    }

   }

    _write_buffer[0] = STX;
    _write_buffer[1] = _PDT;
    _write_buffer[2] = 0x04;
    _write_buffer[3] = 0x00;
      
    for(count_bytes = 0; count_bytes < 4; count_bytes++) {
      _write_buffer[count_bytes + 4] = printdata[(272*60)+count_bytes+n*16384];
    }
    write_to_printer(64);
    recv_from_printer(1);
     
     if (!(n == (pages -1) && fraction == 0 && full_blocks == 0)) {
      printf("\nPrint start ");
      _write_buffer[0] = 0x0C;
      write_to_printer(1);
      printf(".");  

      wait_for_ready();
     }

 
  }
  
  /* PRINTING < 16kB    ------------------------------------------------------- */ 
  
  if(full_blocks != 0) {
  
  printf("\nSending %d blocks of data \n", full_blocks);     
     
  for(i=0; i < full_blocks; i++) {
  
    _write_buffer[0] = STX;
    _write_buffer[1] = _PDT;
    _write_buffer[2] = 0x3C;
    _write_buffer[3] = 0x00;
   
    for(count_bytes = 0; count_bytes < 60; count_bytes++) {
      _write_buffer[count_bytes + 4] = printdata[(i*60)+count_bytes+pages*16384];
    }
    
    write_to_printer(64);
    recv_from_printer(1);

    
    if(_read_buffer[0] == ACK){
       if ((i+1)%10 == 0) {
        printf("%d",i+1); 
        fflush( stdout );
        if ((i+1)%50 == 0) {
         printf("\n"); 
         fflush( stdout );
       	}
       
       } else {
        printf("."); 
        fflush( stdout );
       }
       
    }else {
      printf("No ACK recieved. STOP\n");
      exit(-1);
    }

  }
 

  }
  /* FRACION PRINTING < 60 Bytes  ---------------------------------------------------- */ 
  
  if (fraction != 0) {
  
  printf("\nSending %d bytes of print data ... ",fraction);     
  _write_buffer[0] = STX;
  _write_buffer[1] = _PDT;
  _write_buffer[2] = fraction;
  _write_buffer[3] = 0x00;
  
  for(count_bytes = 0; count_bytes <= fraction; count_bytes++) {
      _write_buffer[count_bytes + 4] = printdata[(full_blocks*60)+count_bytes+pages*16384];
  }  

  write_to_printer(64);
  recv_from_printer(1);

    
  if(_read_buffer[0] == ACK){
       printf("OK\n"); 
       fflush( stdout );
       }else {
      printf("ERROR\n");
      exit(-1);
   }
   

  }
 
  /* FINISHING ---------------------------------------------------------------- */
  printf("Send print data end ... ");
  
  _write_buffer[0] = STX;
  _write_buffer[1] = _PDE;
  
  write_to_printer(16);
  recv_from_printer(1);
  if(_read_buffer[0] == ACK) printf("OK\n"); else {
   printf("ERROR\n");
   exit(-1);
  }
    
  printf("Print start ");
  fflush( stdout );
  
  _write_buffer[0] = 0x0C;
  write_to_printer(1);
   
   
  printf("Sending Platen-up ... OK\n");
  _write_buffer[0] = PUP;
  
  write_to_printer(1);

  close_printer();
  return 0;
}


void wait_for_ready() {

int ready = 0;
  
  do {
   sleep(1);
   ignore_read_error = 1;
   _write_buffer[0] = SYN;
   write_to_printer(1);
   recv_from_printer(1);
   if(_read_buffer[0] == ACK) {
     ready = 1;
     printf(" OK\n");
     fflush( stdout );
     ignore_read_error = 0;
   } else {
     printf(".");
     fflush( stdout );
   }

  } while (ready == 0);

}