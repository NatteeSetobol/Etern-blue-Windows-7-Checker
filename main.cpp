/*
License: MIT License

Copyright (c) 2020 Nomications

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "required/intrinsic.h"
#include "required/nix.h"
#include "nix_socket.h"
#include "required/memory.h"
#include "required/platform.h"
#include "required/assert.h"
#include "bucket.h"
#include "marray.h"
#include "stringz.h"

enum 
{
	SMB_STATUS_OK = 0,
	STATUS_MORE_PROCESSING_REQUIRED = 0xC0000016,
	STATUS_INSUFF_SERVER_RESOURCES = 0xC0000205,
};

struct __attribute__((__packed__))net_bios
{
	ui32 length;
};

struct __attribute__((__packed__))smb_header
{
	us32 protocol[4];
	us32 command;
	ui32 smbError;
	us32 flag;
	ushort flag2;
	ushort PIDHigh;
	us32 securityFeature[8];
	ushort reserves;
	ushort tid;
	ushort pid;
	ushort uid;
	ushort mid;
};

struct __attribute__((__packed__))neg_prot_req
{
	us32 wordCount;
	ushort byteCount;
	us32 bufferFormat;
	us32 name[12]; // This depends on the byte count;
};

struct __attribute__((__packed__)) neg_prot_resp
{
	us32 wordCount;
	ushort selectedIndex;
	us32 securityMode;
	ushort maxMpxCount;
	ushort maxVC;
	ui32 maxBufferSize;
	ui32 maxRawBuffer;
	ui32 sessionKey;
	ui32 capabilities;
	ui32 systemTimeHigh;
	ui32 systemTimeLow;
	ushort serverTimeZone;
	us32 contentLength;
	ushort byteCount;
	us32 serverGUI[16];
	us32 securityBlock[26];
};

struct __attribute__((__packed__)) Simple_Protected_Negotiation
{
	uchar sign[2];
	uchar negTokenInit[6];
	uchar mechTypes[2];
	uchar mechType[10];
	uchar unknown[4];
	uchar ntlmssp[8];
	uchar ntlmMsgType[4];
	uchar negFlags[4];
	uchar workStationDoman[8];
	uchar workStationName[8];
};

struct __attribute__((__packed__)) GSS_API
{
	uchar sign[4];
	uchar oID[6];
	struct Simple_Protected_Negotiation simpleProtectNegotiation;
};

struct __attribute__((__packed__)) SPN_negTokenTarg
{
	uchar sign[2];
	uchar negTokSign[6];
	uchar NTLMSSPID[8];
	ui32  NTLMMessageType;
	ushort LANLen;
	ushort LANmaxLen;
	ui32 LANOffset;
	uchar NTLMResp[8];
	uchar domainName[8];
	uchar username[8];
	uchar hostName[8];
	uchar sessionKey[8];
	uchar negFlags[4];
	char LanManagerResp;
};

struct __attribute__((__packed__)) Setup_AndX_Request 
{
	uchar wordCount;
	uchar andXCommand;
	uchar reserved;
	ushort andXOffset;
	ushort maxBuffer;
	ushort maxMpxCount;
	ushort vcNumber;
	s32 sessionKey[4];
	ushort securityBlobLen;
	uint32 reserved2;
	uint32 capabilities;
	ushort byteCount;
};



struct __attribute__((__packed__)) Setup_AndX_Response
{
	uchar wordCount;
	uchar andXCommand;
	uchar reserved;
	ushort andXOffset;
	ushort action;
	ushort securityBlobLen;
	ushort byteCount;
};

struct __attribute__((__packed__)) Tree_Connect_AndX_Response
{
	uchar wordCount;
	uchar andXCommand;
	uchar reserved;
	ushort andXOffset;
	ushort flags;
	ushort passLen;
	ushort byteCount;
};


struct __attribute__((__packed__)) Trans_Response
{
	uchar wordCount;
	ushort totalParameterCount;
	ushort totalDataCount;
	ushort maxParameterCount;
	ushort maxDataCount;
	uchar maxSetupCount;
	uchar reserved;
	ushort flags;
	i32 timeout;
	ushort reserved2;
	ushort parameterCount;
	ushort parameterOffset;
	ushort dataCount;
	ushort dataOffset;
	uchar setupCount;
	uchar reserved3;
	ushort unknown;
	ushort byteCount;
};

struct __attribute__((__packed__)) Tree_Disconnect_Request
{
	uchar wordCount;
	ushort byteCount;
};

struct __attribute__((__packed__)) Logoff_AndX_Request
{
	uchar wordCount;
	uchar andXCommand;
	uchar reserved;
	ushort andXOffset;
	ushort byteCount;
};

struct __attribute__((__packed__)) Logoff_AndX_Response
{
	uchar wordCount;
	uchar andXCommand;
	uchar reserved;
	ushort andXOffset;
	ushort byteCount;
};


uint32 SwapBytes(uint32 bytesToSwap)
{
	ui32 result = 0;
	ui32 leftBits = 0;
	ui32 rightBits = 0;
	
	leftBits = bytesToSwap & 0x0000FF00;
	leftBits >>= 8;

	rightBits  = bytesToSwap & 0x000000FF;
	rightBits <<= 8;

	result = rightBits | leftBits;

	return result;
}


ui32 BigToLittleEndian(ui32 bigEndian)
{
	ui32 result = 0;
	ui32 bytesToSwap = 0;
	ui32 topBytes = 0; 
	ui32 bottomBytes =  0;
	ui32 bottomResult = 0;
	ui32 topResult = 0;

	topBytes = bigEndian & 0xFFFF0000;
	bottomBytes = bigEndian & 0x0000FFFF;

	topBytes >>= 16;

	bottomResult = SwapBytes(bottomBytes);
	topResult = SwapBytes(topBytes);

	bottomResult <<= 16;

	result = bottomResult | topResult;
	
	return result;
}


ushort  BigToLittleEndian(ushort bigEndian)
{
	ui32 toUInt32 = 0;
	ushort result = 0;

	toUInt32 = (ui32) bigEndian;
	
	result =  (ushort) ( (ui32) (BigToLittleEndian(toUInt32) >> 16));

	return result; 
}

void *SMB_RecievePacket(struct platform_socket* socket, ui32* packetSize)
{
	uint32 totalPacketSize = 0;
	void *recvPacket=NULL;
	uint32 totalBytesRecv = 0;
	uint32 rotationCount = 0;
	int retCount = 0;

	do
	{
		char recvData[256]={};

		retCount = RecvFromSocket(socket,recvData,256);
		if (retCount > 0)
		{
			if (rotationCount == 0)
			{
				struct net_bios *netbiosRecv = NULL;
				netbiosRecv = (struct net_bios*) recvData;
				totalPacketSize = BigToLittleEndian(netbiosRecv->length);
				totalPacketSize += sizeof(net_bios);

				recvPacket = MemoryRaw(totalPacketSize);

				memcpy(recvPacket,recvData,retCount);

				totalBytesRecv += retCount;
			} else {

				memcpy(recvPacket+totalBytesRecv,recvData,retCount);
				totalBytesRecv += retCount;
			}

			if (totalBytesRecv >= totalPacketSize)
			{
				break;
			}
			rotationCount++;
		}

	} while (retCount != 0);

	*packetSize  =  totalBytesRecv;
	return recvPacket;

}

ui32 SMB_NegotiateRequest(struct platform_socket *socket, ushort *randomPid)
{
	struct smb_header smbHeader = {};
	struct neg_prot_req negRequest={};
	int totalLength = 0;
	struct net_bios netbios= {};
	void *packet = NULL;
	void *smbPacket = NULL;
	ui32 packetSize = 0;
	struct net_bios *nps_netBios= NULL;
	struct smb_header *nps_smbHeader = NULL;
	struct neg_prot_resp *nps_prot_resp = NULL;
	ui32 result = 0;

	srand(time(NULL));

	*randomPid = rand() % 6000 + 4000;


	smbHeader.protocol[0] = 0xFF;
	smbHeader.protocol[1] = 'S';
	smbHeader.protocol[2] = 'M';
	smbHeader.protocol[3] = 'B';

	smbHeader.command = 0x72;
	smbHeader.smbError = 0;
	smbHeader.flag = 0x18;
	smbHeader.flag2 = 0x4801;
	smbHeader.PIDHigh = 0;
	//smbHeader.securityFeature = 0;
	smbHeader.reserves = 0;
	smbHeader.tid = 0xFFFF;
	smbHeader.pid = *randomPid;
	smbHeader.uid = 0;
	smbHeader.mid = 0;

	negRequest.wordCount = 0;
	negRequest.byteCount = 12;
	negRequest.bufferFormat = 0x02;
	strcpy( (char*) negRequest.name,"NT LM 0.12");

	totalLength = sizeof(smbHeader) + sizeof(negRequest) ;
	netbios.length =(ui32) BigToLittleEndian((uint32)totalLength);

	packet = MemoryRaw(sizeof(netbios)+totalLength);

	memcpy(packet,(void*) &netbios,sizeof(netbios));
	memcpy(packet+sizeof(netbios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netbios)+sizeof(smbHeader),(void*) &negRequest,sizeof(negRequest));

	SendToSocket(socket,(char*) packet,(sizeof(netbios) + totalLength));

	#ifndef MEM_DEBUG
	smbPacket = SMB_RecievePacket(socket,&packetSize);

	if (smbPacket)
	{
		nps_netBios = (struct net_bios*) smbPacket;
		nps_smbHeader = (struct smb_header*) (sizeof(struct net_bios) + smbPacket);
		nps_prot_resp = (struct neg_prot_resp*) (sizeof(struct net_bios) + sizeof(smb_header) + smbPacket);

		result = nps_smbHeader->smbError;
	}
	#else 
	//Insert Fake Results
	result = SMB_STATUS_OK;
	#endif

	return  result;
}

ui32 SetupAndXRequestChallenge(struct platform_socket *socket, ushort *userID, ushort randomPID)
{
	struct smb_header smbHeader = {};
	struct Setup_AndX_Request setupAndXRequest = {};
	struct net_bios netBios = {};
	ui32 packetTotalSize = 0;
	void* packet=NULL;
	void* recvPacket = NULL;
	ui32 recvPacketSize=0;
	struct smb_header *recvSmbHeader = {};
	ui32 result = 0;

	smbHeader.protocol[0] = 0xFF;
	smbHeader.protocol[1] = 'S';
	smbHeader.protocol[2] = 'M';
	smbHeader.protocol[3] = 'B';

	smbHeader.command = 0x73;
	smbHeader.smbError = 0;
	smbHeader.flag = 0x18;
	smbHeader.flag2 = 0x4801;
	smbHeader.PIDHigh = 0;
	smbHeader.reserves = 0;
	smbHeader.tid = 0xFFFF;
	smbHeader.pid = randomPID;;
	smbHeader.uid = 0;
	smbHeader.mid = 0;

	setupAndXRequest.wordCount = 0xC;
	setupAndXRequest.andXCommand = 0xff;
	setupAndXRequest.reserved = 0x00;
	setupAndXRequest.andXOffset = 0x0000;
	setupAndXRequest.maxBuffer = (ushort) 61440;
	setupAndXRequest.maxMpxCount = (ushort)  2;
	setupAndXRequest.vcNumber = (ushort) 1;
	setupAndXRequest.securityBlobLen =  66;
	setupAndXRequest.reserved = 0x00000000;
	setupAndXRequest.capabilities = (ui32) 0x8000c044;
	// NOTES(): Byte count is security blob Length + OS Length + LAN Manager Length
	setupAndXRequest.byteCount = (ushort)  0x004d; //77  


	struct  GSS_API gssAPI ={};
	char OS[5] = {};
	char LANManager[6] = {};

	gssAPI.sign[0] = 0x60;
	gssAPI.sign[1] = 0x40;
	gssAPI.sign[2] = 0x06;
	gssAPI.sign[3] = 0x06;

	gssAPI.oID[0]  = 0x2b;
	gssAPI.oID[1]  = 0x06;
	gssAPI.oID[2]  = 0x01;
	gssAPI.oID[3]  = 0x05;
	gssAPI.oID[4]  = 0x05;
	gssAPI.oID[5]  = 0x02;

	gssAPI.simpleProtectNegotiation.sign[0] = 0xa0; 
	gssAPI.simpleProtectNegotiation.sign[1] = 0x36;
	gssAPI.simpleProtectNegotiation.negTokenInit[0] = 0x30;
	gssAPI.simpleProtectNegotiation.negTokenInit[1] = 0x34;
	gssAPI.simpleProtectNegotiation.negTokenInit[2] = 0xa0;
	gssAPI.simpleProtectNegotiation.negTokenInit[3] = 0x0e;
	gssAPI.simpleProtectNegotiation.negTokenInit[4] = 0x30;
	gssAPI.simpleProtectNegotiation.negTokenInit[5] = 0x0c;

	gssAPI.simpleProtectNegotiation.mechTypes[0] = 0x06;
	gssAPI.simpleProtectNegotiation.mechTypes[1] = 0x0a;
	gssAPI.simpleProtectNegotiation.mechType[0] = 0x2b;
	gssAPI.simpleProtectNegotiation.mechType[1] = 0x06;
	gssAPI.simpleProtectNegotiation.mechType[2] = 0x01;
	gssAPI.simpleProtectNegotiation.mechType[3] = 0x04;
	gssAPI.simpleProtectNegotiation.mechType[4] = 0x01;
	gssAPI.simpleProtectNegotiation.mechType[5] = 0x82;
	gssAPI.simpleProtectNegotiation.mechType[6] = 0x37;
	gssAPI.simpleProtectNegotiation.mechType[7] = 0x02;
	gssAPI.simpleProtectNegotiation.mechType[8] = 0x02;
	gssAPI.simpleProtectNegotiation.mechType[9] = 0x0a;

	gssAPI.simpleProtectNegotiation.unknown[0] = 0xa2;
	gssAPI.simpleProtectNegotiation.unknown[1] = 0x22;
	gssAPI.simpleProtectNegotiation.unknown[2] = 0x04;
	gssAPI.simpleProtectNegotiation.unknown[3] = 0x20;

	gssAPI.simpleProtectNegotiation.ntlmssp[0] = 0x4e;
	gssAPI.simpleProtectNegotiation.ntlmssp[1] = 0x54;
	gssAPI.simpleProtectNegotiation.ntlmssp[2] = 0x4c;
	gssAPI.simpleProtectNegotiation.ntlmssp[3] = 0x4d;
	gssAPI.simpleProtectNegotiation.ntlmssp[4] = 0x53;
	gssAPI.simpleProtectNegotiation.ntlmssp[5] = 0x53;
	gssAPI.simpleProtectNegotiation.ntlmssp[6] = 0x50;
	gssAPI.simpleProtectNegotiation.ntlmssp[7] = 0x00;

	gssAPI.simpleProtectNegotiation.ntlmMsgType[0] = 0x01;
	gssAPI.simpleProtectNegotiation.ntlmMsgType[1] = 0x00;
	gssAPI.simpleProtectNegotiation.ntlmMsgType[2] = 0x00;
	gssAPI.simpleProtectNegotiation.ntlmMsgType[3] = 0x00;


	gssAPI.simpleProtectNegotiation.negFlags[0] = 0x05;
	gssAPI.simpleProtectNegotiation.negFlags[1] = 0x02;
	gssAPI.simpleProtectNegotiation.negFlags[2] = 0x88;
	gssAPI.simpleProtectNegotiation.negFlags[3] = 0xa0;

	memcpy(OS,"Unix",4);
	memcpy(LANManager,"Samba",5);

	packetTotalSize = sizeof(struct smb_header) + sizeof(struct Setup_AndX_Request) +  sizeof(gssAPI) + 5 + 6;
	netBios.length = BigToLittleEndian(packetTotalSize);

	packet = MemoryRaw(packetTotalSize+sizeof(struct net_bios) );

	memcpy(packet,(void*) &netBios,sizeof(netBios));
	memcpy(packet+sizeof(netBios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader),(void*) &setupAndXRequest,sizeof(setupAndXRequest));	

	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(setupAndXRequest),(void*) &gssAPI, sizeof(gssAPI));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(setupAndXRequest)+sizeof(gssAPI),(void*) OS, 5);
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(setupAndXRequest)+sizeof(gssAPI)+5,(void*) LANManager, 6);

	#ifndef MEM_DEBUG
	SendToSocket(socket,(char*) packet,(sizeof(net_bios) + packetTotalSize));

	recvPacket = SMB_RecievePacket(socket,&recvPacketSize);

	if (recvPacket)
	{
		recvSmbHeader = (smb_header*) ( sizeof(net_bios)+((i8*)recvPacket));

		*userID = recvSmbHeader->uid;
		result = recvSmbHeader->smbError;

	}
	#else
	result = STATUS_MORE_PROCESSING_REQUIRED;
	#endif

	return result;
}

ui32 SetupAndXRequestAUTHUSER(struct platform_socket *socket, ui32 userID, ushort randomPID)
{
	FILE* samplePacketFile= NULL;
	struct smb_header smbHeader = {};
	struct Setup_AndX_Request setupAndXRequest = {};
	struct SPN_negTokenTarg spnNegTokenTArg = {};
	struct net_bios netBios = {};
	uint32 spnNegLen = 0;
	char OS[5] = {};
	char LANManager[6] = {};
	uint packetTotalSize = 0;
	void *packet = NULL;
	ui32 result = 0;
	void *recvPacket=NULL;
	ui32 recvPacketSize = 0;
	struct net_bios *recv_netBios = NULL;
	struct smb_header *recv_smbHeader = NULL;

	spnNegLen = sizeof(SPN_negTokenTarg);

	smbHeader.protocol[0] = 0xFF;
	smbHeader.protocol[1] = 'S';
	smbHeader.protocol[2] = 'M';
	smbHeader.protocol[3] = 'B';

	smbHeader.command = 0x73;
	smbHeader.smbError = 0;
	smbHeader.flag = 0x18;
	smbHeader.flag2 = 0x4801;
	smbHeader.PIDHigh = 0;
	smbHeader.reserves = 0;
	smbHeader.tid = 0xFFFF;
	smbHeader.pid = randomPID;
	smbHeader.uid = userID;
	smbHeader.mid = 0;

	setupAndXRequest.wordCount = 0xC;
	setupAndXRequest.andXCommand = 0xff;
	setupAndXRequest.reserved = 0x00;
	setupAndXRequest.andXOffset = 0x0000;
	setupAndXRequest.maxBuffer = (ushort) 61440;
	setupAndXRequest.maxMpxCount = (ushort)  2;
	setupAndXRequest.vcNumber = (ushort) 1;
	setupAndXRequest.securityBlobLen =  spnNegLen;
	setupAndXRequest.reserved = 0x00000000;
	setupAndXRequest.capabilities = (ui32) 0x8000c044;
	// NOTES(): Byte count is security blob Length + OS Length + LAN Manager Length
	setupAndXRequest.byteCount = spnNegLen+5+6; //77  

	spnNegTokenTArg.sign[0] = 0xa1;
	spnNegTokenTArg.sign[1] = 0x47;

	spnNegTokenTArg.negTokSign[0] = 0x30;
	spnNegTokenTArg.negTokSign[1] = 0x45;
	spnNegTokenTArg.negTokSign[2] = 0xa2;
	spnNegTokenTArg.negTokSign[3] = 0x43;
	spnNegTokenTArg.negTokSign[4] = 0x04;
	spnNegTokenTArg.negTokSign[5] = 0x41;

	strcpy((char*) spnNegTokenTArg.NTLMSSPID,"NTLMSSP");

	spnNegTokenTArg.NTLMMessageType = 3;
	spnNegTokenTArg.LANLen = 1;
	spnNegTokenTArg.LANmaxLen = 1;
	spnNegTokenTArg.LANOffset = 64;

	spnNegTokenTArg.NTLMResp[0] = 0x00;
	spnNegTokenTArg.NTLMResp[1] = 0x00;
	spnNegTokenTArg.NTLMResp[2] = 0x00;
	spnNegTokenTArg.NTLMResp[3] = 0x00;
	spnNegTokenTArg.NTLMResp[4] = 0x41;
	spnNegTokenTArg.NTLMResp[5] = 0x00;
	spnNegTokenTArg.NTLMResp[6] = 0x00;
	spnNegTokenTArg.NTLMResp[7] = 0x00;

	spnNegTokenTArg.domainName[0] = 0x00;
	spnNegTokenTArg.domainName[1] = 0x00;
	spnNegTokenTArg.domainName[2] = 0x00;
	spnNegTokenTArg.domainName[3] = 0x00;
	spnNegTokenTArg.domainName[4] = 0x40;
	spnNegTokenTArg.domainName[5] = 0x00;
	spnNegTokenTArg.domainName[6] = 0x00;
	spnNegTokenTArg.domainName[7] = 0x00;


	spnNegTokenTArg.username[0] = 0x00;
	spnNegTokenTArg.username[1] = 0x00;
	spnNegTokenTArg.username[2] = 0x00;
	spnNegTokenTArg.username[3] = 0x00;
	spnNegTokenTArg.username[4] = 0x40;
	spnNegTokenTArg.username[5] = 0x00;
	spnNegTokenTArg.username[6] = 0x00;
	spnNegTokenTArg.username[7] = 0x00;

	spnNegTokenTArg.hostName[0] = 0x00;
	spnNegTokenTArg.hostName[1] = 0x00;
	spnNegTokenTArg.hostName[2] = 0x00;
	spnNegTokenTArg.hostName[3] = 0x00;
	spnNegTokenTArg.hostName[4] = 0x40;
	spnNegTokenTArg.hostName[5] = 0x00;
	spnNegTokenTArg.hostName[6] = 0x00;
	spnNegTokenTArg.hostName[7] = 0x00;

	spnNegTokenTArg.sessionKey[0] = 0x00;
	spnNegTokenTArg.sessionKey[1] = 0x00;
	spnNegTokenTArg.sessionKey[2] = 0x00;
	spnNegTokenTArg.sessionKey[3] = 0x00;
	spnNegTokenTArg.sessionKey[4] = 0x41;
	spnNegTokenTArg.sessionKey[5] = 0x00;
	spnNegTokenTArg.sessionKey[6] = 0x00;
	spnNegTokenTArg.sessionKey[7] = 0x00;

	spnNegTokenTArg.negFlags[0] = 0x05;
	spnNegTokenTArg.negFlags[1] = 0x02;
	spnNegTokenTArg.negFlags[2] = 0x88;
	spnNegTokenTArg.negFlags[3] = 0xa0;

	spnNegTokenTArg.LanManagerResp = 0x00;

	memcpy(OS,"Unix",4);
	memcpy(LANManager,"Samba",5);


	packetTotalSize = sizeof(struct smb_header) + sizeof(struct Setup_AndX_Request) +  sizeof(spnNegTokenTArg) + 5 + 6;
	netBios.length = BigToLittleEndian(packetTotalSize);
	packet = MemoryRaw(packetTotalSize+sizeof(struct net_bios) );

	memcpy(packet,(void*) &netBios,sizeof(netBios));
	memcpy(packet+sizeof(netBios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader),(void*) &setupAndXRequest,sizeof(setupAndXRequest));	

	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(setupAndXRequest),(void*) &spnNegTokenTArg, sizeof(spnNegTokenTArg));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(setupAndXRequest)+sizeof(spnNegTokenTArg),(void*) OS, 5);
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(setupAndXRequest)+sizeof(spnNegTokenTArg)+5,(void*) LANManager, 6);

	#ifndef MEM_DEBUG
	SendToSocket(socket,(char*) packet,(sizeof(net_bios) + packetTotalSize));

	recvPacket = SMB_RecievePacket(socket,&recvPacketSize);

	if (recvPacket)
	{
		recv_netBios = (struct net_bios*) packet;
		recv_smbHeader = (struct smb_header*) (sizeof(struct net_bios) + packet);

		result = recv_smbHeader->smbError;
	}
	#else
		result = SMB_STATUS_OK;
	#endif


	return result;
}

ui32 TreeConnectAndXRequest(struct platform_socket *socket, ui32 userID, char* targetIP, ushort *returnTreeID, ui32 randomPid)
{
	struct smb_header smbHeader = {};
	struct net_bios netBios = {};
	struct Tree_Connect_AndX_Response treeConnectAndXResp= {};
	uint32 spnNegLen = 0;
	s32 *path = NULL;
	s32 *service = NULL;
	s32 *password = NULL;
	ui32 packetTotalSize = 0;
	void *packet = NULL;
	void *recvPacket=NULL;
	ui32 recvPacketSize = 0;
	struct net_bios *recv_netBios = NULL;
	struct smb_header *recv_smbHeader = NULL;
	ui32 result = 0;


	smbHeader.protocol[0] = 0xFF;
	smbHeader.protocol[1] = 'S';
	smbHeader.protocol[2] = 'M';
	smbHeader.protocol[3] = 'B';

	smbHeader.command = 0x75;
	smbHeader.smbError = 0;
	smbHeader.flag = 0x18;
	smbHeader.flag2 = 0x4801;
	smbHeader.PIDHigh = 0;
	smbHeader.reserves = 0;
	smbHeader.tid = 0xFFFF;
	smbHeader.pid = randomPid;
	smbHeader.uid = userID;
	smbHeader.mid = 0;

	treeConnectAndXResp.wordCount = 4;
	treeConnectAndXResp.andXCommand = 0xff;
	treeConnectAndXResp.reserved = 0x00;
	treeConnectAndXResp.andXOffset = 0;
	treeConnectAndXResp.flags = 0x0000;
	treeConnectAndXResp.passLen = 1;

	password = S32("\0");
	path = CS32Cat(4,"\\\\", targetIP, "\\","IPC$");
	service = S32("?????");


	treeConnectAndXResp.byteCount = Strlen(password) + 1 + Strlen(path) + 1 + Strlen(service) + 1; 

	packetTotalSize = sizeof(smbHeader) + sizeof(treeConnectAndXResp) + Strlen(password)+1 + Strlen(path) + 1 + Strlen(service) + 1;
	packet = MemoryRaw(packetTotalSize+sizeof(struct net_bios) );

	netBios.length = BigToLittleEndian(packetTotalSize);

	memcpy(packet,(void*) &netBios,sizeof(netBios));
	memcpy(packet+sizeof(netBios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader),(void*) &treeConnectAndXResp,sizeof(treeConnectAndXResp));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(treeConnectAndXResp),password,Strlen(password)+1);
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(treeConnectAndXResp)+Strlen(password)+1,path,Strlen(path)+1);
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader)+sizeof(treeConnectAndXResp)+Strlen(password)+1+Strlen(path)+1,service,Strlen(service)+1);


	SendToSocket(socket,(char*) packet,(sizeof(net_bios) + packetTotalSize));

#ifndef MEM_DEBUG
	recvPacket = SMB_RecievePacket(socket,&recvPacketSize);

	if (recvPacket)
	{
		recv_netBios = (struct net_bios*) packet;
		recv_smbHeader = (struct smb_header*) (sizeof(struct net_bios) + recvPacket);

		result = recv_smbHeader->smbError;
		*returnTreeID = recv_smbHeader->tid;
	}
#else
		result = SMB_STATUS_OK;
#endif

	return result;

}


ui32 TransRequest(struct platform_socket *socket, ui32 userID, ushort treeID, ui32 randomPID)
{
	struct smb_header smbHeader = {};
	struct net_bios netBios = {};
	struct Trans_Response transResp = {};
	ui32 packetTotalSize = 0;
	void* packet = NULL;
	struct net_bios *recv_netBios = NULL;
	struct smb_header *recv_smbHeader = NULL;
	ui32 recvPacketSize = 0;
	void *recvPacket = NULL;
	ui32 result = 0;
	ushort newMID = 0;


	srand(time(NULL));

	newMID = 6000 + rand() * (6900 - 6000)/RAND_MAX;

	smbHeader.protocol[0] = 0xFF;
	smbHeader.protocol[1] = 'S';
	smbHeader.protocol[2] = 'M';
	smbHeader.protocol[3] = 'B';

	smbHeader.command = 0x25;
	smbHeader.smbError = 0;
	smbHeader.flag = 0x18;
	smbHeader.flag2 = 0x4801;
	smbHeader.PIDHigh = 0;
	smbHeader.reserves = 0;
	smbHeader.tid = treeID;
	smbHeader.pid = randomPID;
	smbHeader.uid = userID;
	smbHeader.mid = newMID;

	transResp.wordCount = 0x0f;
	transResp.totalParameterCount = 0x00;
	transResp.totalDataCount  = 0x00;
	transResp.maxParameterCount = 0xffff;
	transResp.maxDataCount = 2048;
	transResp.maxSetupCount = 0x02;
	transResp.reserved = 0x00;
	transResp.flags = 0x00;
	transResp.timeout = 0xffffffff;
	transResp.reserved2 = 0x0000;
	transResp.parameterCount = 0x0;
	transResp.parameterOffset = 0x0;
	transResp.dataCount = 0;
	transResp.dataOffset = 0;
	transResp.setupCount = 1;
	transResp.reserved3 = 0;
	transResp.unknown = 0x23;
	transResp.byteCount = 0;


	packetTotalSize = sizeof(smbHeader) + sizeof(transResp);


	netBios.length = BigToLittleEndian(packetTotalSize);

	packet = MemoryRaw(packetTotalSize+sizeof(netBios) );

	memcpy(packet,(void*) &netBios,sizeof(netBios));
	memcpy(packet+sizeof(netBios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader),&transResp,sizeof(transResp));


	#ifndef MEM_DEBUG
	SendToSocket(socket,(char*) packet,(sizeof(netBios) + packetTotalSize));

	recvPacket = SMB_RecievePacket(socket,&recvPacketSize);

	if (recvPacket)
	{
		recv_netBios = (struct net_bios*) packet;
		recv_smbHeader = (struct smb_header*) (sizeof(struct net_bios) + recvPacket);

		

		result = recv_smbHeader->smbError;

	}
	#else
		result = STATUS_INSUFF_SERVER_RESOURCES;
	#endif

	return result;

}

ui32 SendDisconnectRequest(struct platform_socket *socket, ushort treeID, ushort userID,ushort randomPID)
{
	ui32 result = 0;
	struct smb_header smbHeader = {};
	struct net_bios netBios = {};
	struct Tree_Disconnect_Request treeDisconnectReq = {};
	ui32 packetTotalSize = 0;
	void* packet = NULL;
	struct net_bios *recv_netBios = NULL;
	struct smb_header *recv_smbHeader = NULL;
	ui32 recvPacketSize = 0;
	void *recvPacket = NULL;

	smbHeader.protocol[0] = 0xFF;
	smbHeader.protocol[1] = 'S';
	smbHeader.protocol[2] = 'M';
	smbHeader.protocol[3] = 'B';

	smbHeader.command = 0x71;
	smbHeader.smbError = 0;
	smbHeader.flag = 0x18;
	smbHeader.flag2 = 0x4801;
	smbHeader.PIDHigh = 0;
	smbHeader.reserves = 0;
	smbHeader.tid = treeID;
	smbHeader.pid = randomPID;
	smbHeader.uid = userID;
	smbHeader.mid = 0;

	treeDisconnectReq.wordCount = 0;
	treeDisconnectReq.byteCount = 0;

	packetTotalSize = sizeof(smbHeader) + sizeof(treeDisconnectReq);
	netBios.length = BigToLittleEndian(packetTotalSize);

	packet = MemoryRaw(packetTotalSize+sizeof(netBios) );

	memcpy(packet,(void*) &netBios,sizeof(netBios));
	memcpy(packet+sizeof(netBios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader),&treeDisconnectReq,sizeof(treeDisconnectReq));

	#ifndef MEM_DEBUG
	SendToSocket(socket,(char*) packet,(sizeof(netBios) + packetTotalSize));

	recvPacket = SMB_RecievePacket(socket,&recvPacketSize);

	if (recvPacket)
	{
		recv_netBios = (struct net_bios*) packet;
		recv_smbHeader = (struct smb_header*) (sizeof(struct net_bios) + recvPacket);

		result = recv_smbHeader->smbError;
	} 
	#else
		result = SMB_STATUS_OK;
	#endif

	return result;

}

ui32 LogoffAndXRequest(struct platform_socket *socket, ushort treeID, ushort userID,ushort randomPID)
{
	ui32 result = 0;
	struct smb_header smbHeader = {};
	struct net_bios netBios = {};
	struct Logoff_AndX_Request logoffAndXRequest = {};
	ui32 packetTotalSize = 0;
	void* packet = NULL;
	struct net_bios *recv_netBios = NULL;
	struct smb_header *recv_smbHeader = NULL;
	ui32 recvPacketSize = 0;
	void *recvPacket = NULL;

	smbHeader.protocol[0] = 0xFF;
	smbHeader.protocol[1] = 'S';
	smbHeader.protocol[2] = 'M';
	smbHeader.protocol[3] = 'B';

	smbHeader.command = 0x74;
	smbHeader.smbError = 0;
	smbHeader.flag = 0x18;
	smbHeader.flag2 = 0x4801;
	smbHeader.PIDHigh = 0;
	smbHeader.reserves = 0;
	smbHeader.tid = treeID;
	smbHeader.pid = randomPID;
	smbHeader.uid = userID;
	smbHeader.mid = 0;

	logoffAndXRequest.wordCount = 0;
	logoffAndXRequest.andXCommand = 0xff;
	logoffAndXRequest.reserved = 0x00;
	logoffAndXRequest.andXOffset = 0x0;
	logoffAndXRequest.byteCount = 0;


	packetTotalSize = sizeof(smbHeader) + sizeof(logoffAndXRequest);
	packet = MemoryRaw(packetTotalSize+sizeof(netBios) );

	memcpy(packet,(void*) &netBios,sizeof(netBios));
	memcpy(packet+sizeof(netBios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netBios)+sizeof(smbHeader),&logoffAndXRequest,sizeof(logoffAndXRequest));

	#ifndef MEM_DEBUG
	SendToSocket(socket,(char*) packet,(sizeof(netBios) + packetTotalSize));

	recvPacket = SMB_RecievePacket(socket,&recvPacketSize);

	if (recvPacket)
	{
		recv_netBios = (struct net_bios*) packet;
		recv_smbHeader = (struct smb_header*) (sizeof(struct net_bios) + recvPacket);

		result = recv_smbHeader->smbError;
	}
	#else
		result = SMB_STATUS_OK; 
	#endif

	return result;
}

int main()
{
	struct platform_socket socket;
	ushort userID = 0;
	s32 *targetIP = NULL;
	ushort treeID = 0;
	ushort randomPID = 0;

	targetIP = S32("10.10.53.203");

	printf("Connecting...");
	#ifndef MEM_DEBUG
	socket = CreateSocket(targetIP, 445);

	if (socket.connected)
	#endif
	{
		
		printf("Ok\n");
		printf("Sending Negotiate Request..");
		if (SMB_NegotiateRequest(&socket,&randomPID) == SMB_STATUS_OK)
		{

			printf("ok!\n");
			printf("Random pid = %i\n", randomPID);
			printf("Sending SetUpAndXRequest..");
			if (SetupAndXRequestChallenge(&socket,&userID,randomPID) == STATUS_MORE_PROCESSING_REQUIRED)
			{
			
				printf("ok!\n");
				printf("Sending SetUpAndXAUTHUSER..");
				if (SetupAndXRequestAUTHUSER(&socket, userID,randomPID) == SMB_STATUS_OK)
				{
					
					printf("ok!\n");
					printf("Sending Tree Connect AndX Request..");

					if (TreeConnectAndXRequest(&socket, userID, targetIP, &treeID,randomPID) == SMB_STATUS_OK)
					{
						printf("ok!\n");
						if (TransRequest(&socket, userID, treeID, randomPID) == STATUS_INSUFF_SERVER_RESOURCES)
						{
							printf("This machine is vulnerable!\n");
						} else {
							printf("This machine is NOT vulnerable!\n");
						}

						printf("Sending Tree Disconnect request...");

						if (SendDisconnectRequest(&socket, treeID, userID,randomPID) == SMB_STATUS_OK)
						{
							printf("ok\n");
						}

					} else {
						printf("This machine is NOT vulnerable!\n");
					}

				} else {
					printf("This machine is NOT vulnerable!\n");
				}

				printf("Sending logoffAndXRequest..");
				if (LogoffAndXRequest(&socket, treeID, userID,randomPID) == SMB_STATUS_OK)
				{
					printf("ok!\n");
				}
			} else {
				printf("This machine is NOT vulnerable!\n");
			}

		 } else {
			 printf("This machine is NOT vulnerable!\n");
		 }
	}
	#ifdef MEM_DEBUG
		MemoryResults();
	#endif
}

