/*
 * Broadcom UPnP module, xml_x_lanhostconfigmanagement.c
 *
 * Copyright (C) 2008, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: xml_x_lanhostconfigmanagement.c,v 1.5 2007/11/23 09:51:55 Exp $
 */
#include <upnp.h>

char xml_x_lanhostconfigmanagement[]=
	"<?xml version=\"1.0\"?>\r\n"
	"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n"
	"\t<specVersion>\r\n"
	"\t\t<major>1</major>\r\n"
	"\t\t<minor>0</minor>\r\n"
	"\t</specVersion>\r\n"
	"\t<actionList>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetDHCPServerConfigurable</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDHCPServerConfigurable</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DHCPServerConfigurable</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetDHCPServerConfigurable</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDHCPServerConfigurable</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DHCPServerConfigurable</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetDHCPRelay</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDHCPRelay</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DHCPRelay</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetDHCPRelay</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDHCPRelay</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DHCPRelay</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetSubnetMask</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewSubnetMask</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>SubnetMask</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetSubnetMask</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewSubnetMask</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>SubnetMask</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetIPRouter</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewIPRouters</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>IPRouters</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>DeleteIPRouter</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewIPRouters</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>IPRouters</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetIPRoutersList</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewIPRouters</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>IPRouters</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetDomainName</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDomainName</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DomainName</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetDomainName</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDomainName</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DomainName</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetAddressRange</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewMinAddress</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>MinAddress</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewMaxAddress</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>MaxAddress</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetAddressRange</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewMinAddress</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>MinAddress</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewMaxAddress</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>MaxAddress</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetReservedAddress</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewReservedAddresses</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>ReservedAddresses</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>DeleteReservedAddress</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewReservedAddresses</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>ReservedAddresses</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetReservedAddresses</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewReservedAddresses</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>ReservedAddresses</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetDNSServer</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDNSServers</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DNSServers</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>DeleteDNSServer</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDNSServers</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DNSServers</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetDNSServers</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewDNSServers</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>DNSServers</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t</actionList>\r\n"
	"\t<serviceStateTable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>DHCPServerConfigurable</name>\r\n"
	"\t\t\t<dataType>boolean</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>DHCPRelay</name>\r\n"
	"\t\t\t<dataType>boolean</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>SubnetMask</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>IPRouters</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>DNSServers</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>DomainName</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>MinAddress</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>MaxAddress</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>ReservedAddresses</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t</serviceStateTable>\r\n"
	"</scpd>\r\n"
	"\r\n";
