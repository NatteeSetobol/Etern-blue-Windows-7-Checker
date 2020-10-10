#include <stdio.h>
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
	struct  GSS_API gssAPI;
	s32 OS[5];
	s32 LANManger[6];
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

ui32 SMB_NegotiateRequest(struct platform_socket *socket)
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
	smbHeader.pid =BigToLittleEndian( (ushort) 0x1e17);
	smbHeader.uid = 0;
	smbHeader.mid = 0;

	negRequest.wordCount = 0;
	negRequest.byteCount = 12;
	negRequest.bufferFormat = 0x02;
	strcpy( (char*) negRequest.name,"NT LM 0.12");

	totalLength = sizeof(smbHeader) + sizeof(negRequest) - 1;
	netbios.length =(ui32) BigToLittleEndian((uint32)totalLength);

	packet = MemoryRaw(sizeof(netbios)+totalLength);

	memcpy(packet,(void*) &netbios,sizeof(netbios));
	memcpy(packet+sizeof(netbios),(void*) &smbHeader,sizeof(smbHeader));
	memcpy(packet+sizeof(netbios)+sizeof(smbHeader),(void*) &negRequest,sizeof(negRequest));

	SendToSocket(socket,(char*) packet,(sizeof(netbios) + totalLength));

	smbPacket = SMB_RecievePacket(socket,&packetSize);

	if (smbPacket)
	{
		//printf("packet recv, size is %i\n", packetSize);

		nps_netBios = (struct net_bios*) smbPacket;
		nps_smbHeader = (struct smb_header*) (sizeof(struct net_bios) + smbPacket);
		nps_prot_resp = (struct neg_prot_resp*) (sizeof(struct net_bios) + sizeof(smb_header) + smbPacket);
		/*
		for (int securityBlockIndex = 0; securityBlockIndex < 16; securityBlockIndex++)
		{
			printf("%x", nps_prot_resp->securityBlock[securityBlockIndex]);
		}
		

		printf("\n");
		*/
	}


	return nps_smbHeader->smbError;
}

int main()
{
	struct platform_socket socket;
	FILE* testPacket = NULL;

	/*
	testPacket = fopen("test_packet.raw", "wb");

	if (testPacket)
	{
		fwrite(packet,1, sizeof(netbios) + totalLength,testPacket);
		fclose(testPacket);
	}
	
	*/

	printf("Connecting...\n");
	socket = CreateSocket("10.10.120.51", 445);

	if (socket.connected)
	{
		printf("Connected!\n");
		printf("Sending Negotiate Request\n");
		 if (SMB_NegotiateRequest(&socket) == SMB_STATUS_OK)
		 {

			 printf("Negotiate Request..ok!\n");
			
			 struct smb_header smbHeader = {};
			 struct Setup_AndX_Request setupAndXRequest = {};
			 struct net_bios netBios = {};
			 ui32 packetTotalSize = 0;
			 void* packet=NULL;
			 void* recvPacket = NULL;
			 ui32 recvPacketSize=0;
			struct smb_header *recvSmbHeader = {};

			 smbHeader.protocol[0] = 0xFF;
			 smbHeader.protocol[1] = 'S';
			 smbHeader.protocol[2] = 'M';
			 smbHeader.protocol[3] = 'B';

			 smbHeader.command = 0x73;
			 smbHeader.smbError = 0;
			 smbHeader.flag = 0x18;
			 smbHeader.flag2 = 0x4801;
			 smbHeader.PIDHigh = 0;
			 //smbHeader.securityFeature = 0;
			 smbHeader.reserves = 0;
			 smbHeader.tid = 0xFFFF;
			 smbHeader.pid = BigToLittleEndian( (ushort) 0x1e17);
			 smbHeader.uid = 0;
			 smbHeader.mid = 0;

			 setupAndXRequest.wordCount = 0xC;
			 setupAndXRequest.andXCommand = 0xff;
			 setupAndXRequest.reserved = 0x00;
			 setupAndXRequest.andXOffset = 0x0000;
			 setupAndXRequest.maxBuffer = (ushort) 61440;
			 setupAndXRequest.maxMpxCount = (ushort)  2;
			 setupAndXRequest.vcNumber = (ushort) 1;
			 //setupAndXRequest.sessionKey = 0;
			 setupAndXRequest.securityBlobLen =  66;
			 setupAndXRequest.reserved = 0x00000000;
			 setupAndXRequest.capabilities = (ui32) 0x8000c044;
			 // NOTES(): Byte count is security blob Length + OS Length + LAN Manager Length
			 setupAndXRequest.byteCount = (ushort)  0x004d; //77  
			 // NOTES(): This will be static for now!
			 char  blobStatic[] = { 
					0x60, 0x40,0x06,0x06,0x2b,0x06,0x01,0x05,0x05,0x02,0xa0,0x36,0x30,
					0x34,0xa0,0x0e,0x30,0x0c,0x06,0x0a,0x2b,0x06,0x01,0x04,0x01,0x82,0x37,0x02,0x02,
					0x0a,0xa2,0x22,0x04,0x20,0x4e,0x54,0x4c,0x4d,0x53,0x53,0x50,0x00,0x01,0x00,0x00,
					0x00,0x05,0x02,0x88,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00, 0x00
					};

			 setupAndXRequest.gssAPI.sign[0] = 0x60;
			 setupAndXRequest.gssAPI.sign[1] = 0x40;
			 setupAndXRequest.gssAPI.sign[2] = 0x06;
			 setupAndXRequest.gssAPI.sign[3] = 0x06;

			 setupAndXRequest.gssAPI.oID[0]  = 0x2b;
			 setupAndXRequest.gssAPI.oID[1]  = 0x06;
			 setupAndXRequest.gssAPI.oID[2]  = 0x01;
			 setupAndXRequest.gssAPI.oID[3]  = 0x05;
			 setupAndXRequest.gssAPI.oID[4]  = 0x05;
			 setupAndXRequest.gssAPI.oID[5]  = 0x02;

			 setupAndXRequest.gssAPI.simpleProtectNegotiation.sign[0] = 0xa0; 
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.sign[1] = 0x36;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negTokenInit[0] = 0x30;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negTokenInit[1] = 0x34;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negTokenInit[2] = 0xa0;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negTokenInit[3] = 0x0e;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negTokenInit[4] = 0x30;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negTokenInit[5] = 0x0c;

			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechTypes[0] = 0x06;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechTypes[1] = 0x0a;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[0] = 0x2b;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[1] = 0x06;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[2] = 0x01;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[3] = 0x04;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[4] = 0x01;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[5] = 0x82;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[6] = 0x37;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[7] = 0x02;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[8] = 0x02;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.mechType[9] = 0x0a;

			 setupAndXRequest.gssAPI.simpleProtectNegotiation.unknown[0] = 0xa2;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.unknown[1] = 0x22;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.unknown[2] = 0x04;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.unknown[3] = 0x20;

			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[0] = 0x4e;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[1] = 0x54;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[2] = 0x4c;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[3] = 0x4d;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[4] = 0x53;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[5] = 0x53;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[6] = 0x50;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmssp[7] = 0x00;

			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmMsgType[0] = 0x01;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmMsgType[1] = 0x00;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmMsgType[2] = 0x00;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.ntlmMsgType[3] = 0x00;


			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negFlags[0] = 0x05;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negFlags[1] = 0x02;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negFlags[2] = 0x88;
			 setupAndXRequest.gssAPI.simpleProtectNegotiation.negFlags[3] = 0xa0;



	//		 memcpy(setupAndXRequest.securityBlob,blobStatic,66);
			 memcpy(setupAndXRequest.OS,"Unix",4);
			 memcpy(setupAndXRequest.LANManger,"Samba",5);
			
			 packetTotalSize = sizeof(struct smb_header) + sizeof(struct Setup_AndX_Request);
			 netBios.length = BigToLittleEndian(packetTotalSize);

			packet = MemoryRaw(packetTotalSize+sizeof(struct net_bios));

			memcpy(packet,(void*) &netBios,sizeof(netBios));
			memcpy(packet+sizeof(netBios),(void*) &smbHeader,sizeof(smbHeader));
			memcpy(packet+sizeof(netBios)+sizeof(smbHeader),(void*) &setupAndXRequest,sizeof(setupAndXRequest));	
	

			SendToSocket(&socket,(char*) packet,(sizeof(net_bios) + packetTotalSize));

	
			recvPacket = SMB_RecievePacket(&socket,&recvPacketSize);

			if (recvPacket)
			{
				recvSmbHeader = (smb_header*) ( sizeof(net_bios)+((i8*)recvPacket));
				if (recvSmbHeader->smbError == 0xc0000016)
				{
					printf("works!, %x\n",recvSmbHeader->smbError);
				} else {
					printf("this doesn't work!, %x\n",recvSmbHeader->smbError);
				}
			}
			
			
		
			FILE *samplePacketFile = NULL;

			samplePacketFile = fopen("sample_smb_setupXAndReq.raw","wb");;
			fwrite(packet,1,packetTotalSize+sizeof(struct net_bios),samplePacketFile);
			fclose(samplePacketFile);

		 }
	}
	


	#if 0
	FILE *samplePacketFile = NULL;
	i32 fileSize = 0;
	void* fileContent = NULL;
	struct net_bios *netBios = NULL;
	struct smb_header *smbHeader = NULL;
	struct neg_prot_req *negProtReq = NULL;
	void* chunk = NULL;
	struct neg_prot_resp *negProtRes = NULL;

	samplePacketFile = fopen("sample_smb_response.raw","rb");;

	if (samplePacketFile)
	{
		fseek(samplePacketFile,0,SEEK_END);
		fileSize = ftell(samplePacketFile);
		rewind(samplePacketFile);

		fileContent = MemoryRaw(fileSize);

		fread(fileContent,1,fileSize,samplePacketFile);

		
		netBios = (net_bios*) fileContent;
		smbHeader = (smb_header*) (sizeof(net_bios) + fileContent);
		negProtRes = (neg_prot_resp*) (sizeof(net_bios)  + sizeof(smb_header) + fileContent);

		printf("%x\n", negProtRes->serverGUI[1]);
		/*
		negProtReq = (neg_prot_req*) (sizeof(net_bios) + sizeof(smb_header) + fileContent);



		chunk = MemoryRaw(sizeof(smbHeader));

		if (chunk)
		{
			struct smb_header  *newNetBios = NULL;
			chunk = memcpy(chunk,(void*) smbHeader, sizeof(netBios));

			newNetBios = (struct smb_header*) chunk;

			printf("%s\n", newNetBios->protocol);
		}
		*/



		fclose(samplePacketFile);


		
	}
	#endif
}

