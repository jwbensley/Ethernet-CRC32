# Ethernet CRC32


#### What is it


Calculate the CRC32 checksum for a captured Ethernet frame (and optionally  
compare it to the FCS on the frame if included in the capture).


#### How to use it

Pass one or more packet file names are arguments. Each file is a single  
Ethernet frame written as hex bytes. An example `P1.txt` is included. If the  
captures don't include an FCS as the last four bytes set `has_crc = 0` and  
recompile.

```
$ gcc -o crc32 crc32.c && ./crc32 P1.txt 
P1.txt: 0x8 0x0 0x27 0x27 0x1a 0xd5 0x52 0x54 0x0 0x12 0x35 0x2 0x8 0x0 0x45 0x0 0x0 0x54 0x1e 0x49 0x40 0x0 0x40 0x1 0x4 0x50 0xa 0x0 0x2 0x2 0xa 0x0 0x2 0xf 0x0 0x0 0x59 0xd6 0xf 0xaf 0x0 0x1 0xfd 0xb5 0xf5 0x5a 0x0 0x0 0x0 0x0 0xe1 0x95 0x3 0x0 0x0 0x0 0x0 0x0 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0xe6 0x4c 0xb4 0x86 
File length: 102 bytes
Calculated CRC: 0x86b44ce6, Frame FCS: 0x86b44ce6
Matched!
```
To capture frames with the FCS (this is usually removed by the NIC) rx-fcs  
must be enabled (not all NICs support this):  
`sudo ethtool -K enp0s25 rx-fcs on`  


#### Further Info
Further info on the EthernetCRC can be found here: https://null.53bits.co.uk/index.php?page=crc-and-checksum-error-detection
