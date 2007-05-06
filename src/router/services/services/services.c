/*
 * services.c
 *
 * Copyright (C) 2006 Sebastian Gottschall <gottschall@dd-wrt.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>		/* AhMan  March 18 2005 */
#include <sys/socket.h>
#include <sys/mount.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <wait.h>
#include <net/route.h>		/* AhMan  March 18 2005 */
#include <sys/types.h>
#include <signal.h>

#include <bcmnvram.h>
#include <bcmconfig.h>
#include <netconf.h>
#include <shutils.h>
#include <utils.h>
#include <cy_conf.h>
#include <code_pattern.h>
#include <rc.h>
#include "mkfiles.h"
#include <wlutils.h>
#include <nvparse.h>
#include <syslog.h>


#define WL_IOCTL(name, cmd, buf, len) (wl_ioctl((name), (cmd), (buf), (len)))

#define TXPWR_MAX 251
#define TXPWR_DEFAULT 28


#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

/* AhMan  March 18 2005 */
#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)

int start_force_to_dial (void);

static int
alreadyInHost (char *host)
{
  FILE *in = fopen ("/tmp/hosts", "rb");
  if (in == NULL)
    return 0;
  char buf[100];
  while (1)
    {
      fscanf (in, "%s", buf);
      if (!strcmp (buf, host))
	{
	  fclose (in);
	  return 1;
	}
      if (feof (in))
	{
	  fclose (in);
	  return 0;
	}
    }
}

void
addHost (char *host, char *ip)
{
  char buf[100];
  char newhost[100];
  if (host == NULL)
    return;
  if (ip == NULL)
    return;
  strcpy (newhost, host);
  char *domain = nvram_safe_get ("lan_domain");
  if (domain != NULL && strlen (domain) > 0 && strcmp (host, "localhost"))
    {
      sprintf (newhost, "%s.%s", host, domain);
    }
  else
    sprintf (newhost, "%s", host);

  if (alreadyInHost (newhost))
    return;
  sprintf (buf, "echo \"%s\t%s\">>/tmp/hosts", ip, newhost);
  system2 (buf);
}

void
start_vpn_modules (void)
{
#if defined(HAVE_XSCALE) || defined(HAVE_FONERA) || defined(HAVE_WHRAG108) || defined(HAVE_X86) ||defined(HAVE_LS2)
  if ((nvram_match ("pptp_pass", "1") || nvram_match ("l2tp_pass", "1")
       || nvram_match ("ipsec_pass", "1")))
    {
      eval ("/sbin/insmod", "nf_conntrack_proto_gre");
      syslog (LOG_INFO,
	      "vpn modules : nf_conntrack_proto_gre successfully started\n");
      eval ("/sbin/insmod", "nf_nat_proto_gre");
      syslog (LOG_INFO,
	      "vpn modules : nf_nat_proto_gre successfully started\n");
    }

  if (nvram_match ("pptp_pass", "1"))
    {
      eval ("/sbin/insmod", "nf_conntrack_pptp");
      syslog (LOG_INFO,
	      "vpn modules : nf_conntrack_pptp successfully started\n");
      eval ("/sbin/insmod", "nf_nat_pptp");
      syslog (LOG_INFO, "vpn modules : nf_nat_pptp successfully started\n");
    }

#else
  if ((nvram_match ("pptp_pass", "1") || nvram_match ("l2tp_pass", "1")
       || nvram_match ("ipsec_pass", "1")))
    {
      eval ("/sbin/insmod", "ip_conntrack_proto_gre");
      syslog (LOG_INFO,
	      "vpn modules : ip_conntrack_proto_gre successfully started\n");
      eval ("/sbin/insmod", "ip_nat_proto_gre");
      syslog (LOG_INFO,
	      "vpn modules : ip_nat_proto_gre successfully started\n");
    }
  if (nvram_match ("pptp_pass", "1"))
    {
      eval ("/sbin/insmod", "ip_conntrack_pptp");
      syslog (LOG_INFO,
	      "vpn modules : ip_conntrack_pptp successfully started\n");
      eval ("/sbin/insmod", "ip_nat_pptp");
      syslog (LOG_INFO, "vpn modules : ip_nat_pptp successfully started\n");
    }
#endif
}


void
stop_vpn_modules (void)
{
#if defined(HAVE_XSCALE) || defined(HAVE_FONERA) || defined(HAVE_WHRAG108) || defined(HAVE_X86) || defined(HAVE_LS2)
  eval ("/sbin/rmmod", "nf_nat_pptp");
  syslog (LOG_INFO, "vpn modules : nf_nat_pptp successfully stopped\n");
  eval ("/sbin/rmmod", "nf_conntrack_pptp");
  syslog (LOG_INFO, "vpn modules : nf_conntrack_pptp successfully stopped\n");
  eval ("/sbin/rmmod", "nf_nat_proto_gre");
  syslog (LOG_INFO, "vpn modules : nf_nat_proto_gre successfully stopped\n");
  eval ("/sbin/rmmod", "nf_conntrack_proto_gre");
  syslog (LOG_INFO,
	  "vpn modules : nf_conntrack_proto_gre successfully stopped\n");
#else
  eval ("/sbin/rmmod", "ip_nat_proto_gre");
  syslog (LOG_INFO, "vpn modules : ip_nat_proto_gre successfully stopped\n");
  eval ("/sbin/rmmod", "ip_nat_pptp");
  syslog (LOG_INFO, "vpn modules : ip_nat_pptp successfully stopped\n");
  eval ("/sbin/rmmod", "ip_conntrack_pptp");
  syslog (LOG_INFO, "vpn modules : ip_conntrack_pptp successfully stopped\n");
  eval ("/sbin/rmmod", "ip_conntrack_proto_gre");
  syslog (LOG_INFO,
	  "vpn modules : ip_conntrack_proto_gre successfully stopped\n");

#endif
}

/* AhMan  March 18 2005 */
void start_tmp_ppp (int num);

int
softkill (char *name)
{
  killall (name, SIGKILL);
  return 0;
}


int
adjust_dhcp_range (void)
{
  struct in_addr ipaddr, netaddr, netmask;

  char *lan_ipaddr = nvram_safe_get ("lan_ipaddr");
  char *lan_netmask = nvram_safe_get ("lan_netmask");
  char *dhcp_num = nvram_safe_get ("dhcp_num");
  char *dhcp_start = nvram_safe_get ("dhcp_start");

  int legal_start_ip, legal_end_ip, legal_total_ip, dhcp_start_ip;
  int set_dhcp_start_ip = 0, set_dhcp_num = 0;
  int adjust_ip = 0, adjust_num = 0;

  inet_aton (lan_ipaddr, &netaddr);
  inet_aton (lan_netmask, &netmask);
  inet_aton (dhcp_start, &ipaddr);

  legal_total_ip = 254 - get_single_ip (lan_netmask, 3);
  legal_start_ip =
    (get_single_ip (lan_ipaddr, 3) & get_single_ip (lan_netmask, 3)) + 1;
  legal_end_ip = legal_start_ip + legal_total_ip - 1;
  dhcp_start_ip = atoi (dhcp_start);

  cprintf
    ("legal_total_ip=[%d] legal_start_ip=[%d] legal_end_ip=[%d] dhcp_start_ip=[%d]\n",
     legal_total_ip, legal_start_ip, legal_end_ip, dhcp_start_ip);

  if ((dhcp_start_ip > legal_end_ip) || (dhcp_start_ip < legal_start_ip))
    {
      cprintf ("Illegal DHCP Start IP: We need to adjust DHCP Start ip.\n");
      set_dhcp_start_ip = legal_start_ip;
      adjust_ip = 1;
      if (atoi (dhcp_num) > legal_total_ip)
	{
	  cprintf ("DHCP num is exceed, we need to adjust.");
	  set_dhcp_num = legal_total_ip;
	  adjust_num = 1;
	}
    }
  else
    {
      cprintf ("Legal DHCP Start IP: We need to check DHCP num.\n");
      if ((atoi (dhcp_num) + dhcp_start_ip) > legal_end_ip)
	{
	  cprintf ("DHCP num is exceed, we need to adjust.\n");
	  set_dhcp_num = legal_end_ip - dhcp_start_ip + 1;
	  adjust_num = 1;
	}
    }

  if (adjust_ip)
    {
      char ip[20];
      cprintf ("set_dhcp_start_ip=[%d]\n", set_dhcp_start_ip);
      snprintf (ip, sizeof (ip), "%d", set_dhcp_start_ip);
      nvram_set ("dhcp_start", ip);
    }
  if (adjust_num)
    {
      char num[5];
      cprintf ("set_dhcp_num=[%d]\n", set_dhcp_num);
      snprintf (num, sizeof (num), "%d", set_dhcp_num);
      nvram_set ("dhcp_num", num);
    }

  return 1;
}


int
write_nvram (char *name, char *nv)
{
  if (nvram_invmatch (nv, ""))
    {
      FILE *fp = fopen (name, "wb");

      char *host_key = nvram_safe_get (nv);
      int i = 0;
      do
	{
	  if (host_key[i] != 0x0D)
	    fprintf (fp, "%c", host_key[i]);
	}
      while (host_key[++i]);
      fclose (fp);
    }
  else
    return -1;
  return 0;
}


int
start_dhcpfwd (void)
{
  if (nvram_match ("wl0_mode", "wet") || nvram_match ("wl0_mode", "apstawet"))	//dont start any dhcp services in bridge mode
    {
      nvram_set ("lan_proto", "static");
      return 0;
    }
#ifdef HAVE_DHCPFORWARD
  FILE *fp;
  if (nvram_match ("dhcpfwd_enable", "1"))
    {
      mkdir ("/tmp/dhcp-fwd", 0700);
      mkdir ("/var/run/dhcp-fwd", 0700);
      fp = fopen ("/tmp/dhcp-fwd/dhcp-fwd.conf", "wb");
      fprintf (fp, "user		root\n");
      fprintf (fp, "group		root\n");
      fprintf (fp, "chroot		/var/run/dhcp-fwd\n");
      fprintf (fp, "logfile		/tmp/dhcp-fwd.log\n");
      fprintf (fp, "loglevel	1\n");
      fprintf (fp, "pidfile		/var/run/dhcp-fwd.pid\n");
      fprintf (fp, "ulimit core	0\n");
      fprintf (fp, "ulimit stack	64K\n");
      fprintf (fp, "ulimit data	32K\n");
      fprintf (fp, "ulimit rss	200K\n");
      fprintf (fp, "ulimit nproc	0\n");
      fprintf (fp, "ulimit nofile	0\n");
      fprintf (fp, "ulimit as	0\n");
      fprintf (fp, "if	%s	true	false	true\n",
	       nvram_safe_get ("lan_ifname"));

      char *wan_proto = nvram_safe_get ("wan_proto");
      char *wan_ifname = nvram_safe_get ("wan_ifname");
#ifdef HAVE_MADWIFI
      if (getSTA ())
	{
	  wan_ifname = getSTA ();	//returns eth1/eth2 for broadcom and ath0 for atheros
	}
#else
      if (nvram_match ("wl_mode", "sta"))
	{
	  wan_ifname = get_wdev ();	//returns eth1/eth2 for broadcom and ath0 for atheros
	}
#endif
#ifdef HAVE_PPPOE
      if (strcmp (wan_proto, "pppoe") == 0)
	{
	  fprintf (fp, "if	ppp0	false	true	true\n");
	}
#else
      if (0)
	{
	}
#endif
      if (getWET ())
	{
	  //nothing
	}
      else if (strcmp (wan_proto, "dhcp") == 0
	       || strcmp (wan_proto, "static") == 0)
	{
	  fprintf (fp, "if	%s	false	true	true\n", wan_ifname);
	}
#ifdef HAVE_PPTP
      else if (strcmp (wan_proto, "pptp") == 0)
	{
	  fprintf (fp, "if	ppp0	false	true	true\n");
	}
#endif
#ifdef HAVE_L2TP
      else if (strcmp (wan_proto, "l2tp") == 0)
	{
	  fprintf (fp, "if	ppp0	false	true	true\n");
	}
#endif
#ifdef HAVE_HEARTBEAT
      else if (strcmp (wan_proto, "heartbeat") == 0)
	{
	  fprintf (fp, "if	ppp0	false	true	true\n");
	}
#endif
      else
	{
	  fprintf (fp, "if	%s	false	true	true\n", wan_ifname);
	}

      fprintf (fp, "name	%s	ws-c\n",
	       nvram_safe_get ("lan_ifname"));
      fprintf (fp, "server	ip	%s\n", nvram_safe_get ("dhcpfwd_ip"));
      fclose (fp);
      eval ("dhcpfwd", "-c", "/tmp/dhcp-fwd/dhcp-fwd.conf");
      syslog (LOG_INFO,
	      "dhcpfwd : dhcp forwarder daemon successfully started\n");
      return 0;
    }
#endif
#ifdef HAVE_DHCPRELAY
  if (nvram_match ("dhcpfwd_enable", "1"))
    {
      eval ("dhcrelay", "-i", nvram_safe_get ("lan_ifname"),
	    nvram_safe_get ("dhcpfwd_ip"));
      syslog (LOG_INFO, "dhcrelay : dhcp relay successfully started\n");
    }
#endif
  return 0;
}

void
stop_dhcpfwd (void)
{
#ifdef HAVE_DHCPFORWARD
  if (pidof ("dhcpfwd") > 0)
    syslog (LOG_INFO,
	    "dhcpfwd : dhcp forwarder daemon successfully stopped\n");
  killall ("dhcpfwd", SIGTERM);	//kill also dhcp forwarder if available
#endif
#ifdef HAVE_DHCPRELAY
  if (pidof ("dhcrelay") > 0)
    syslog (LOG_INFO, "dhcrelay : dhcp relay successfully stopped\n");
  killall ("dhcrelay", SIGTERM);
#endif
}

int usejffs = 0;

int
start_udhcpd (void)
{
  FILE *fp = NULL;
  struct dns_lists *dns_list = NULL;
  int i = 0;
  if (nvram_match ("dhcpfwd_enable", "1"))
    {
      return 0;
    }
#ifndef HAVE_RB500
#ifndef HAVE_XSCALE
  if (nvram_match ("wl0_mode", "wet") || nvram_match ("wl0_mode", "apstawet"))	//dont start any dhcp services in bridge mode
    {
      nvram_set ("lan_proto", "static");
      return 0;
    }
#endif
#endif

  if (nvram_invmatch ("lan_proto", "dhcp")
      || nvram_match ("dhcp_dnsmasq", "1"))
    {
      stop_udhcpd ();
      return 0;
    }

  /* Automatically adjust DHCP Start IP and num when LAN IP or netmask is changed */
  adjust_dhcp_range ();

  cprintf ("%s %d.%d.%d.%s %s %s\n",
	   nvram_safe_get ("lan_ifname"),
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 0),
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 1),
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 2),
	   nvram_safe_get ("dhcp_start"),
	   nvram_safe_get ("dhcp_end"), nvram_safe_get ("lan_lease"));

  /* Touch leases file */

  usejffs = 0;

  if (nvram_match ("dhcpd_usejffs", "1"))
    {
      if (!(fp = fopen ("/jffs/udhcpd.leases", "a")))
	{
	  usejffs = 0;
	}
      else
	{
	  usejffs = 1;
	}
    }
  if (!usejffs)
    {
      if (!(fp = fopen ("/tmp/udhcpd.leases", "a")))
	{
	  perror ("/tmp/udhcpd.leases");
	  return errno;
	}
    }
  fclose (fp);

  /* Write configuration file based on current information */
  if (!(fp = fopen ("/tmp/udhcpd.conf", "w")))
    {
      perror ("/tmp/udhcpd.conf");
      return errno;
    }
  fprintf (fp, "pidfile /var/run/udhcpd.pid\n");
  fprintf (fp, "start %d.%d.%d.%s\n",
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 0),
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 1),
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 2),
	   nvram_safe_get ("dhcp_start"));
  fprintf (fp, "end %d.%d.%d.%d\n",
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 0),
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 1),
	   get_single_ip (nvram_safe_get ("lan_ipaddr"), 2),
	   atoi (nvram_safe_get ("dhcp_start")) +
	   atoi (nvram_safe_get ("dhcp_num")) - 1);
  int dhcp_max =
    atoi (nvram_safe_get ("dhcp_num")) +
    atoi (nvram_safe_get ("static_leasenum"));
  fprintf (fp, "max_leases %d\n", dhcp_max);
  fprintf (fp, "interface %s\n", nvram_safe_get ("lan_ifname"));
  fprintf (fp, "remaining yes\n");
  fprintf (fp, "auto_time 30\n");	// N seconds to update lease table
  if (usejffs)
    fprintf (fp, "lease_file /jffs/udhcpd.leases\n");
  else
    fprintf (fp, "lease_file /tmp/udhcpd.leases\n");
  fprintf (fp, "statics_file /tmp/udhcpd.statics\n");

  if (nvram_invmatch ("lan_netmask", "")
      && nvram_invmatch ("lan_netmask", "0.0.0.0"))
    fprintf (fp, "option subnet %s\n", nvram_safe_get ("lan_netmask"));

  if (nvram_invmatch ("lan_ipaddr", "")
      && nvram_invmatch ("lan_ipaddr", "0.0.0.0"))
    fprintf (fp, "option router %s\n", nvram_safe_get ("lan_ipaddr"));

  if (nvram_invmatch ("wan_wins", "")
      && nvram_invmatch ("wan_wins", "0.0.0.0"))
    fprintf (fp, "option wins %s\n", nvram_get ("wan_wins"));

  // Wolf add - keep lease within reasonable timeframe
  if (atoi (nvram_safe_get ("dhcp_lease")) < 10)
    {
      nvram_set ("dhcp_lease", "10");
      nvram_commit ();
    }
  if (atoi (nvram_safe_get ("dhcp_lease")) > 5760)
    {
      nvram_set ("dhcp_lease", "5760");
      nvram_commit ();
    }

  fprintf (fp, "option lease %d\n",
	   atoi (nvram_safe_get ("dhcp_lease")) ?
	   atoi (nvram_safe_get ("dhcp_lease")) * 60 : 86400);

  dns_list = get_dns_list ();

  if (!dns_list || dns_list->num_servers == 0)
    {
      if (nvram_invmatch ("lan_ipaddr", ""))
	fprintf (fp, "option dns %s\n", nvram_safe_get ("lan_ipaddr"));
    }
  else if (nvram_match ("local_dns", "1"))
    {
      if (dns_list
	  && (nvram_invmatch ("lan_ipaddr", "")
	      || strlen (dns_list->dns_server[0]) > 0
	      || strlen (dns_list->dns_server[1]) > 0
	      || strlen (dns_list->dns_server[2]) > 0))
	{
	  fprintf (fp, "option dns");

	  if (nvram_invmatch ("lan_ipaddr", ""))
	    fprintf (fp, " %s", nvram_safe_get ("lan_ipaddr"));
	  if (strlen (dns_list->dns_server[0]) > 0)
	    fprintf (fp, " %s", dns_list->dns_server[0]);
	  if (strlen (dns_list->dns_server[1]) > 0)
	    fprintf (fp, " %s", dns_list->dns_server[1]);
	  if (strlen (dns_list->dns_server[2]) > 0)
	    fprintf (fp, " %s", dns_list->dns_server[2]);

	  fprintf (fp, "\n");
	}
    }
  else
    {
      if (dns_list
	  && (strlen (dns_list->dns_server[0]) > 0
	      || strlen (dns_list->dns_server[1]) > 0
	      || strlen (dns_list->dns_server[2]) > 0))
	{
	  fprintf (fp, "option dns");
	  if (strlen (dns_list->dns_server[0]) > 0)
	    fprintf (fp, " %s", dns_list->dns_server[0]);
	  if (strlen (dns_list->dns_server[1]) > 0)
	    fprintf (fp, " %s", dns_list->dns_server[1]);
	  if (strlen (dns_list->dns_server[2]) > 0)
	    fprintf (fp, " %s", dns_list->dns_server[2]);

	  fprintf (fp, "\n");
	}
    }

  if (dns_list)
    free (dns_list);

  /* DHCP Domain */
  if (nvram_match ("dhcp_domain", "wan"))
    {
      if (nvram_invmatch ("wan_domain", ""))
	fprintf (fp, "option domain %s\n", nvram_safe_get ("wan_domain"));
      else if (nvram_invmatch ("wan_get_domain", ""))
	fprintf (fp, "option domain %s\n", nvram_safe_get ("wan_get_domain"));
    }
  else
    {
      if (nvram_invmatch ("lan_domain", ""))
	fprintf (fp, "option domain %s\n", nvram_safe_get ("lan_domain"));
    }

  if (nvram_invmatch ("dhcpd_options", ""))
    {
      char *host_key = nvram_safe_get ("dhcpd_options");
      i = 0;
      do
	{
	  if (host_key[i] != 0x0D)
	    fprintf (fp, "%c", host_key[i]);
	}
      while (host_key[++i]);
    }
  /* end Sveasoft addition */
  fclose (fp);

  /* Sveasoft addition - create static leases file */
  // Static for router
  if (!(fp = fopen ("/tmp/udhcpd.statics", "w")))
    {
      perror ("/tmp/udhcpd.statics");
      return errno;
    }

  if (nvram_match ("local_dns", "1"))
    fprintf (fp, "%s %s %s\n", nvram_safe_get ("lan_ipaddr"),
	     nvram_safe_get ("et0macaddr"), nvram_safe_get ("router_name"));

  int leasenum = atoi (nvram_safe_get ("static_leasenum"));
  if (leasenum > 0)
    {
      char *lease = nvram_safe_get ("static_leases");
      char *leasebuf = (char *) malloc (strlen (lease) + 1);
      char *cp = leasebuf;
      strcpy (leasebuf, lease);
      for (i = 0; i < leasenum; i++)
	{
	  char *mac = strsep (&leasebuf, "=");
	  char *host = strsep (&leasebuf, "=");
	  char *ip = strsep (&leasebuf, " ");
	  if (mac == NULL || host == NULL || ip == NULL)
	    continue;
	  fprintf (fp, "%s %s %s\n", ip, mac, host);
	  addHost (host, ip);
	}
      free (cp);
    }

  fclose (fp);
  /* end Sveasoft addition */

  dns_to_resolv ();

  eval ("udhcpd", "/tmp/udhcpd.conf");
  syslog (LOG_INFO, "udhcpd : udhcp daemon successfully started\n");


  /* Dump static leases to udhcpd.leases so they can be read by dnsmasq */
  /* DD-WRT (belanger) : the dump database is now handled directly in udhcpd */
//  sprintf (sigusr1, "-%d", SIGUSR1);
//killps("udhcpd",sigusr1);

//  eval ("killall", sigusr1, "udhcpd");

  cprintf ("done\n");
  return 0;
}

int
stop_udhcpd (void)
{
  if (pidof ("udhcpd") > 0)
    syslog (LOG_INFO, "udhcpd : udhcp daemon successfully stopped\n");
  softkill ("udhcpd");
  cprintf ("done\n");
  return 0;
}



int
start_dnsmasq (void)
{
  FILE *fp;
  struct dns_lists *dns_list = NULL;
  int ret;
  int i;

  if (nvram_match ("dhcp_dnsmasq", "1") && nvram_match ("lan_proto", "dhcp")
      && nvram_match ("dnsmasq_enable", "0"))
    {
      nvram_set ("dnsmasq_enable", "1");
      nvram_commit ();
    }

  // Sveasoft 2003-12-15 only start if enabled
  if (!nvram_invmatch ("dnsmasq_enable", "0"))
    {
      stop_dnsmasq ();
      return 0;
    }

  usejffs = 0;

  if (nvram_match ("dhcpd_usejffs", "1"))
    {
      if (!(fp = fopen ("/jffs/dnsmasq.leases", "a")))
	{
	  usejffs = 0;
	}
      else
	{
	  fclose (fp);
	  usejffs = 1;
	}
    }

  /* Write configuration file based on current information */
  if (!(fp = fopen ("/tmp/dnsmasq.conf", "w")))
    {
      perror ("/tmp/dnsmasq.conf");
      return errno;
    }

  if (nvram_match ("fon_enable", "1")
      || (nvram_match ("chilli_nowifibridge", "1")
	  && nvram_match ("chilli_enable", "1")))
    {
      fprintf (fp, "interface=%s,br0\n", nvram_safe_get ("wl0_ifname"));
    }
  else
    {
      if (nvram_match ("chilli_enable", "1"))
	fprintf (fp, "interface=%s\n", nvram_safe_get ("wl0_ifname"));
      else if (nvram_match ("pptpd_enable", "1"))
	fprintf (fp, "listen-address=%s,%s\n", "127.0.0.1",
		 nvram_safe_get ("lan_ipaddr"));
      else
	fprintf (fp, "interface=%s\n", nvram_safe_get ("lan_ifname"));
    }

  fprintf (fp, "resolv-file=/tmp/resolv.dnsmasq\n");

  /* Domain */
  if (nvram_match ("dhcp_domain", "wan"))
    {
      if (nvram_invmatch ("wan_domain", ""))
	fprintf (fp, "domain=%s\n", nvram_safe_get ("wan_domain"));
      else if (nvram_invmatch ("wan_get_domain", ""))
	fprintf (fp, "domain=%s\n", nvram_safe_get ("wan_get_domain"));
    }
  else
    {
      if (nvram_invmatch ("lan_domain", ""))
	fprintf (fp, "domain=%s\n", nvram_safe_get ("lan_domain"));
    }

  /* DD-WRT use dnsmasq as DHCP replacement */
  if (!nvram_match ("wl0_mode", "wet")
      && !nvram_match ("wl0_mode", "apstawet"))
    if (nvram_match ("dhcp_dnsmasq", "1") && nvram_match ("lan_proto", "dhcp")
	&& nvram_match ("dhcpfwd_enable", "0"))
      {
	/* DHCP leasefile */
	if (nvram_match ("dhcpd_usenvram", "1"))
	  {
	    fprintf (fp, "leasefile-ro\n");
	    fprintf (fp, "dhcp-script=%s\n", "/etc/lease_update.sh");
	  }
	else
	  {
	    if (usejffs)
	      fprintf (fp, "dhcp-leasefile=/jffs/dnsmasq.leases\n");
	    else
	      fprintf (fp, "dhcp-leasefile=/tmp/dnsmasq.leases\n");
	  }

	int dhcp_max =
	  atoi (nvram_safe_get ("dhcp_num")) +
	  atoi (nvram_safe_get ("static_leasenum"));
	fprintf (fp, "dhcp-lease-max=%d\n", dhcp_max);
	fprintf (fp, "dhcp-option=3,%s\n", nvram_safe_get ("lan_ipaddr"));
	if (nvram_invmatch ("wan_wins", "")
	    && nvram_invmatch ("wan_wins", "0.0.0.0"))
	  fprintf (fp, "dhcp-option=44,%s\n", nvram_safe_get ("wan_wins"));

	if (nvram_match ("dns_dnsmasq", "0"))
	  {
	    dns_list = get_dns_list ();

	    if (dns_list
		&& (strlen (dns_list->dns_server[0]) > 0
		    || strlen (dns_list->dns_server[1]) > 0
		    || strlen (dns_list->dns_server[2]) > 0))
	      {

		fprintf (fp, "dhcp-option=6");

		if (strlen (dns_list->dns_server[0]) > 0)
		  fprintf (fp, ",%s", dns_list->dns_server[0]);

		if (strlen (dns_list->dns_server[1]) > 0)
		  fprintf (fp, ",%s", dns_list->dns_server[1]);

		if (strlen (dns_list->dns_server[2]) > 0)
		  fprintf (fp, ",%s", dns_list->dns_server[2]);

		fprintf (fp, "\n");
	      }

	    if (dns_list)
	      free (dns_list);
	  }

	if (nvram_match ("auth_dnsmasq", "1"))
	  fprintf (fp, "dhcp-authoritative\n");
	fprintf (fp, "dhcp-range=");
	fprintf (fp, "%d.%d.%d.%s,",
		 get_single_ip (nvram_safe_get ("lan_ipaddr"), 0),
		 get_single_ip (nvram_safe_get ("lan_ipaddr"), 1),
		 get_single_ip (nvram_safe_get ("lan_ipaddr"), 2),
		 nvram_safe_get ("dhcp_start"));
	fprintf (fp, "%d.%d.%d.%d,",
		 get_single_ip (nvram_safe_get ("lan_ipaddr"), 0),
		 get_single_ip (nvram_safe_get ("lan_ipaddr"), 1),
		 get_single_ip (nvram_safe_get ("lan_ipaddr"), 2),
		 atoi (nvram_safe_get ("dhcp_start")) +
		 atoi (nvram_safe_get ("dhcp_num")) - 1);
	fprintf (fp, "%s,", nvram_safe_get ("lan_netmask"));
	fprintf (fp, "%sm\n", nvram_safe_get ("dhcp_lease"));
	int leasenum = atoi (nvram_safe_get ("static_leasenum"));
	if (leasenum > 0)
	  {
	    char *lease = nvram_safe_get ("static_leases");
	    char *leasebuf = (char *) malloc (strlen (lease) + 1);
	    char *cp = leasebuf;
	    strcpy (leasebuf, lease);
	    for (i = 0; i < leasenum; i++)
	      {
		char *mac = strsep (&leasebuf, "=");
		char *host = strsep (&leasebuf, "=");
		char *ip = strsep (&leasebuf, " ");
		if (mac == NULL || host == NULL || ip == NULL)
		  continue;

		fprintf (fp, "dhcp-host=%s,%s,%s,infinite\n", mac, host, ip);
		addHost (host, ip);
	      }
	    free (cp);
	  }
      }

  /* Additional options */
  if (nvram_invmatch ("dnsmasq_options", ""))
    {
      char *host_key = nvram_safe_get ("dnsmasq_options");
      i = 0;
      do
	{
	  if (host_key[i] != 0x0D)
	    fprintf (fp, "%c", host_key[i]);
	}
      while (host_key[++i]);
    }
  fclose (fp);

  dns_to_resolv ();

  chmod ("/etc/lease_update.sh", 0700);
  ret = eval ("dnsmasq", "--conf-file", "/tmp/dnsmasq.conf");
  syslog (LOG_INFO, "dnsmasq : dnsmasq daemon successfully started\n");

  cprintf ("done\n");
  return ret;
}

int
stop_dnsmasq (void)
{
  if (pidof ("dnsmasq") > 0)
    syslog (LOG_INFO, "dnsmasq : dnsmasq daemon successfully stopped\n");
  int ret = softkill ("dnsmasq");
  unlink ("/tmp/resolv.dnsmasq");

  cprintf ("done\n");
  return ret;
}

int
stop_dns_clear_resolv (void)
{
  FILE *fp_w;
  if (pidof ("dnsmasq") > 0)
    syslog (LOG_INFO, "dnsmasq : dnsmasq daemon successfully stopped\n");
  //int ret = killps("dnsmasq",NULL);
  int ret = killall ("dnsmasq", SIGTERM);

  /* Save DNS to resolv.conf */
  if (!(fp_w = fopen (RESOLV_FILE, "w")))
    {
      perror (RESOLV_FILE);
      return errno;
    }
  fprintf (fp_w, " ");
  fclose (fp_w);

  cprintf ("done\n");
  return ret;
}

int
start_httpd (void)
{
  int ret = 0;

  if (nvram_invmatch ("http_enable", "0") && !is_exist ("/var/run/httpd.pid"))
    {
      chdir ("/www");
//      if (chdir ("/tmp/www") == 0)
//      cprintf ("[HTTPD Starting on /tmp/www]\n");
//      else
      cprintf ("[HTTPD Starting on /www]\n");
      if (nvram_invmatch ("http_lanport", ""))
	{
	  char *lan_port = nvram_safe_get ("http_lanport");
	  ret = eval ("httpd", "-p", lan_port);
	}
      else
	{
	  ret = eval ("httpd");
	  syslog (LOG_INFO, "httpd : http daemon successfully started\n");
	}
      chdir ("/");
    }
#ifdef HAVE_HTTPS
  if (nvram_invmatch ("https_enable", "0")
      && !is_exist ("/var/run/httpsd.pid"))
    {

      // Generate a new certificate
      //if(!is_exist("/tmp/cert.pem") || !is_exist("/tmp/key.pem"))
      //      eval("gencert.sh", BUILD_SECS);         

      chdir ("/www");
      ret = eval ("httpd", "-S");
      syslog (LOG_INFO, "httpd : https daemon successfully started\n");
      chdir ("/");
    }
#endif

  cprintf ("done\n");
  return ret;
}

int
stop_httpd (void)
{
  if (pidof ("httpd") > 0)
    syslog (LOG_INFO, "httpd : http daemon successfully stopped\n");
  //int ret = killps("httpd",NULL);
  int ret = killall ("httpd", SIGTERM);

  unlink ("/var/run/httpd.pid");
#ifdef HAVE_HTTPS
  unlink ("/var/run/httpsd.pid");
#endif
  cprintf ("done\n");
  return ret;
}


#ifdef HAVE_SPUTNIK_APD
/* Sputnik APD Service Handling */
int
start_sputnik (void)
{
  int ret;

  // Only start if enabled
  if (!nvram_invmatch ("apd_enable", "0"))
    return 0;

  ret = eval ("sputnik");
  syslog (LOG_INFO, "sputnik : sputnik daemon successfully started\n");
  cprintf ("done\n");
  return ret;
}

int
stop_sputnik (void)
{
  if (pidof ("sputnik") > 0)
    syslog (LOG_INFO, "sputnik : sputnik daemon successfully stopped\n");
  int ret = killall ("sputnik", SIGTERM);

  cprintf ("done\n");
  return ret;
}

/* END Sputnik Service Handling */

#endif
#if 0
int
start_ntpc (void)
{
  char *servers = nvram_safe_get ("ntp_server");

  // Sveasoft 2003-12-15 only start if enabled
  if (!nvram_invmatch ("ntpd_enable", "0"))
    return 0;

  if (strlen (servers))
    {
      char *nas_argv[] =
	{ "ntpclient", "-h", servers, "-i", "5", "-l", "-s", "-c", "2",
	NULL
      };
      pid_t pid;

      _eval (nas_argv, NULL, 0, &pid);
      syslog (LOG_INFO, "ntpclient : ntp client successfully started\n");
    }

  cprintf ("done\n");
  return 0;
}
#endif
int
stop_ntpc (void)
{
  if (pidof ("ntpclient") > 0)
    syslog (LOG_INFO, "ntpclient : ntp client successfully stopped\n");
  int ret = killall ("ntpclient", SIGTERM);

  cprintf ("done\n");
  return ret;
}


/////////////////////////////////////////////////////
int
start_resetbutton (void)
{
  int ret = 0;

  ret = eval ("resetbutton");
  syslog (LOG_INFO,
	  "resetbutton : reset button daemon successfully started\n");

  cprintf ("done\n");
  return ret;
}

int
stop_resetbutton (void)
{
  int ret = 0;
  if (pidof ("resetbutton") > 0)
    syslog (LOG_INFO,
	    "resetbutton : resetbutton daemon successfully stopped\n");
  ret = killall ("resetbutton", SIGKILL);

  cprintf ("done\n");
  return ret;
}

int
start_iptqueue (void)
{
  int ret = 0;

  // Sveasoft 2003-12-15 only start if enabled
  if (!nvram_invmatch ("iptqueue_enable", "0"))
    return 0;

  ret = eval ("iptqueue");
  syslog (LOG_INFO, "iptqueue successfully started\n");

  cprintf ("done\n");
  return ret;
}

int
stop_iptqueue (void)
{
  int ret = 0;
  if (pidof ("iptqueue") > 0)
    syslog (LOG_INFO, "iptqueue : iptqueue daemon successfully stopped\n");
  ret = killall ("iptqueue", SIGKILL);

  cprintf ("done\n");
  return ret;
}

int
start_cron (void)
{
  int ret = 0;
  int i;
  struct stat buf;

  // Sveasoft 2003-12-15 only start if enabled
  if (nvram_match ("cron_enable", "0"))
    return 0;

  /* Create cron's database directory */
  if (stat ("/var/spool", &buf) != 0)
    {
      mkdir ("/var/spool", 0700);
      mkdir ("/var/spool/cron", 0700);
    }
  mkdir ("/tmp/cron.d", 0700);

  buf_to_file ("/tmp/cron.d/check_ps", "*/2 * * * * root /sbin/check_ps\n");

  /* Additional options */
  FILE *fp;
  unlink ("/tmp/cron.d/cron_jobs");

  if (nvram_invmatch ("cron_jobs", ""))
    {
      fp = fopen ("/tmp/cron.d/cron_jobs", "w");
      char *cron_job = nvram_safe_get ("cron_jobs");
      
      for (i = 0; i < (strlen(cron_job)); i++)
      {
	      fprintf (fp, "%c", cron_job[i]);
	  }
	      
	  fprintf (fp, "\n");

      fclose (fp);
    }

  cprintf ("starting cron\n");
  ret = eval ("/usr/sbin/cron");
  syslog (LOG_INFO, "cron : cron daemon successfully started\n");

  cprintf ("done\n");
  return ret;
}

int
stop_cron (void)
{
  int ret = 0;
  if (pidof ("cron") > 0)
    syslog (LOG_INFO, "cron : cron daemon successfully stopped\n");
  //ret = killps("cron","-9");
  ret = killall ("cron", SIGKILL);

  cprintf ("done\n");
  return ret;
}


int
start_syslog (void)
{
  int ret1 = 0, ret2 = 0;

  // Sveasoft 2003-12-15 only start if enabled
  if (!nvram_invmatch ("syslogd_enable", "0"))
    return 0;

  if (strlen (nvram_safe_get ("syslogd_rem_ip")) > 0)
    ret1 = eval ("/sbin/syslogd", "-R", nvram_safe_get ("syslogd_rem_ip"));
  else
    ret1 = eval ("/sbin/syslogd", "-L");

  syslog (LOG_INFO, "syslogd : syslog daemon successfully started\n");
  ret2 = eval ("/sbin/klogd");
  syslog (LOG_INFO, "klogd : klog daemon successfully started\n");

  return ret1 | ret2;
}

int
stop_syslog (void)
{
  int ret;
  if (pidof ("klogd") > 0)
    syslog (LOG_INFO, "klogd : klog daemon successfully stopped\n");
  ret = killall ("klogd", SIGKILL);
  if (pidof ("syslogd") > 0)
    syslog (LOG_INFO, "syslogd : syslog daemon successfully stopped\n");
  ret += killall ("syslogd", SIGKILL);

  cprintf ("done\n");
  return ret;
}


int
start_redial (void)
{
  int ret;
  pid_t pid;
  char *redial_argv[] = { "/tmp/ppp/redial",
    nvram_safe_get ("ppp_redialperiod"),
    NULL
  };

  symlink ("/sbin/rc", "/tmp/ppp/redial");

  ret = _eval (redial_argv, NULL, 0, &pid);
  syslog (LOG_INFO, "ppp_redial : redial process successfully started\n");

  cprintf ("done\n");
  return ret;
}

int
stop_redial (void)
{
  int ret;
  if (pidof ("redial") > 0)
    syslog (LOG_INFO, "ppp_redial : redial daemon successfully stopped\n");
  //ret = killps("redial","-9");
  ret = killall ("redial", SIGKILL);

  cprintf ("done\n");
  return ret;
}

#ifdef HAVE_RADVD
int
start_radvd (void)
{
  int ret = 0;
  int c = 0;
  char *buf, *buf2;
  int i;
  FILE *fp;
  if (!nvram_match ("radvd_enable", "1"))
    return 0;
  if (!nvram_match ("ipv6_enable", "1"))
    return 0;
  buf = nvram_safe_get ("radvd_conf");
  if (buf != NULL)
    {
      buf2 = (char *) malloc (strlen (buf) + 1);
      memcpy (buf2, buf, strlen (buf));
      buf2[strlen (buf)] = 0;

      i = 0;
      while (buf2[i++] != 0)
	{
	  cprintf (".");
	  if (buf2[i - 1] == '\r')
	    continue;
	  buf2[c++] = buf2[i - 1];
	}
      buf2[c++] = 0;
      fp = fopen ("/tmp/radvd.conf", "wb");
      fwrite (buf2, 1, c - 1, fp);
      fclose (fp);
      free (buf2);
    }
  //nvram2file("radvd_conf", "/tmp/radvd.conf");

  system2 ("sync");

  ret = eval ("/sbin/radvd");
  syslog (LOG_INFO, "radvd : RADV daemon successfully started\n");

  cprintf ("done\n");
  return ret;
}

int
stop_radvd (void)
{
  int ret = 0;
  if (pidof ("radvd") > 0)
    syslog (LOG_INFO, "radvd : RADV daemon successfully stopped\n");
  //ret = killps("radvd",NULL);
  ret = killall ("radvd", SIGKILL);

  unlink ("/var/run/radvd.pid");

  cprintf ("done\n");
  return ret;
}
#endif

int
start_ipv6 (void)
{
  int ret = 0;

  if (!nvram_invmatch ("ipv6_enable", "0"))
    return 0;

  ret = eval ("insmod", "ipv6");
  syslog (LOG_INFO, "ipv6 successfully started\n");

  cprintf ("done\n");
  return ret;
}

#ifdef HAVE_PPPOE
int
stop_pppoe (void)
{
  int ret;

  unlink ("/tmp/ppp/link");
  if (pidof ("pppoecd") > 0)
    syslog (LOG_INFO, "pppoe process successfully stopped\n");
  ret = killall ("pppoecd", SIGKILL);
  ret += killall ("ip-up", SIGKILL);
  ret += killall ("ip-down", SIGKILL);

  cprintf ("done\n");
  return ret;
}

int
stop_single_pppoe (int pppoe_num)
{
  int ret;
  char pppoe_pid[15], pppoe_ifname[15];
  char ppp_unlink[2][20] = { "/tmp/ppp/link", "/tmp/ppp/link_1" };
  char ppp_wan_dns[2][20] = { "wan_get_dns", "wan_get_dns_1" };

  sprintf (pppoe_pid, "pppoe_pid%d", pppoe_num);
  sprintf (pppoe_ifname, "pppoe_ifname%d", pppoe_num);
  dprintf ("start! stop pppoe %d, pid %s \n", pppoe_num,
	   nvram_safe_get (pppoe_pid));

  ret = eval ("kill", nvram_safe_get (pppoe_pid));
  unlink (ppp_unlink[pppoe_num]);
  nvram_unset (pppoe_ifname);

  nvram_set (ppp_wan_dns[pppoe_num], "");
  stop_dns_clear_resolv ();

  dprintf ("done\n");
  return ret;
}
#endif
int
stop_dhcpc (void)
{
  int ret = 0;
  if (pidof ("udhcpc") > 0)
    syslog (LOG_INFO, "udhcpc : udhcp client process successfully stopped\n");
  ret += killall ("udhcpc", SIGTERM);

  cprintf ("done\n");
  return ret;
}

#ifdef HAVE_PPTP
int
start_pptp (int status)
{
  int ret;
  FILE *fp;
  char *pptp_argv[] = { "pppd",
    NULL
  };
  char username[80], passwd[80];

  // Sveasoft 2003-12-15 only start if enabled
  /* if (!nvram_invmatch("pppd_enable", "0"))
     return 0; */

  stop_dhcpc ();
#ifdef HAVE_PPPOE
  stop_pppoe ();
#endif
  stop_vpn_modules ();

  if (nvram_match ("aol_block_traffic", "0"))
    {
      snprintf (username, sizeof (username), "%s",
		nvram_safe_get ("ppp_username"));
      snprintf (passwd, sizeof (passwd), "%s", nvram_safe_get ("ppp_passwd"));
    }
  else
    {
      if (!strcmp (nvram_safe_get ("aol_username"), ""))
	{
	  snprintf (username, sizeof (username), "%s",
		    nvram_safe_get ("ppp_username"));
	  snprintf (passwd, sizeof (passwd), "%s",
		    nvram_safe_get ("ppp_passwd"));
	}
      else
	{
	  snprintf (username, sizeof (username), "%s",
		    nvram_safe_get ("aol_username"));
	  snprintf (passwd, sizeof (passwd), "%s",
		    nvram_safe_get ("aol_passwd"));
	}
    }

  if (status != REDIAL)
    {
      mkdir ("/tmp/ppp", 0777);
      symlink ("/sbin/rc", "/tmp/ppp/ip-up");
      symlink ("/sbin/rc", "/tmp/ppp/ip-down");
      symlink ("/dev/null", "/tmp/ppp/connect-errors");

      /* Generate options file */
      if (!(fp = fopen ("/tmp/ppp/options", "w")))
	{
	  perror ("/tmp/ppp/options");
	  return -1;
	}
      fprintf (fp, "defaultroute\n");	//Add a default route to the system routing tables, using the peer as the gateway
      fprintf (fp, "usepeerdns\n");	//Ask the peer for up to 2 DNS server addresses
      fprintf (fp, "pty 'pptp %s --nolaunchpppd",
	       nvram_safe_get ("pptp_server_ip"));

      // PPTP client also supports synchronous mode.
      // This should improve the speeds.
      if (nvram_match ("pptp_synchronous", "1"))
	fprintf (fp, " --sync'\nsync\n");
      else
	fprintf (fp, "'\n");

      fprintf (fp, "user '%s'\n", username);
      //fprintf(fp, "persist\n");        // Do not exit after a connection is terminated.

      fprintf (fp, "mtu %s\n", nvram_safe_get ("wan_mtu"));

      if (nvram_match ("ppp_demand", "1"))
	{			//demand mode
	  fprintf (fp, "idle %d\n",
		   nvram_match ("ppp_demand",
				"1") ? atoi (nvram_safe_get ("ppp_idletime"))
		   * 60 : 0);
	  fprintf (fp, "demand\n");	// Dial on demand
	  fprintf (fp, "persist\n");	// Do not exit after a connection is terminated.
	  fprintf (fp, "%s:%s\n", PPP_PSEUDO_IP, PPP_PSEUDO_GW);	// <local IP>:<remote IP>
	  fprintf (fp, "ipcp-accept-remote\n");
	  fprintf (fp, "ipcp-accept-local\n");
	  fprintf (fp, "connect true\n");
	  fprintf (fp, "noipdefault\n");	// Disables  the  default  behaviour when no local IP address is specified
	  fprintf (fp, "ktune\n");	// Set /proc/sys/net/ipv4/ip_dynaddr to 1 in demand mode if the local address changes
	}
      else
	{			// keepalive mode
	  start_redial ();
	}
      if (nvram_match ("pptp_encrypt", "0"))
      {
		fprintf (fp, "nomppe\n");	// Disable mppe negotiation
		fprintf (fp, "noccp\n");	// Disable CCP (Compression Control Protocol)
	  }
      fprintf (fp, "default-asyncmap\n");	// Disable  asyncmap  negotiation
      fprintf (fp, "nopcomp\n");	// Disable protocol field compression
      fprintf (fp, "noaccomp\n");	// Disable Address/Control compression
      fprintf (fp, "novj\n");	// Disable Van Jacobson style TCP/IP header compression
      fprintf (fp, "nobsdcomp\n");	// Disables BSD-Compress  compression
      fprintf (fp, "nodeflate\n");	// Disables Deflate compression
      fprintf (fp, "lcp-echo-interval 0\n");	// Don't send an LCP echo-request frame to the peer
      fprintf (fp, "noipdefault\n");
      fprintf (fp, "lock\n");
      fprintf (fp, "noauth\n");

      if (nvram_invmatch ("pptp_extraoptions", ""))
	fprintf (fp, "%s\n", nvram_safe_get ("pptp_extraoptions"));

      fclose (fp);

      /* Generate pap-secrets file */
      if (!(fp = fopen ("/tmp/ppp/pap-secrets", "w")))
	{
	  perror ("/tmp/ppp/pap-secrets");
	  return -1;
	}
      fprintf (fp, "\"%s\" * \"%s\" *\n", username, passwd);
      fclose (fp);
      chmod ("/tmp/ppp/pap-secrets", 0600);

      /* Generate chap-secrets file */
      if (!(fp = fopen ("/tmp/ppp/chap-secrets", "w")))
	{
	  perror ("/tmp/ppp/chap-secrets");
	  return -1;
	}
      fprintf (fp, "\"%s\" * \"%s\" *\n", username, passwd);
      fclose (fp);
      chmod ("/tmp/ppp/chap-secrets", 0600);

      /* Enable Forwarding */
      if ((fp = fopen ("/proc/sys/net/ipv4/ip_forward", "r+")))
	{
	  fputc ('1', fp);
	  fclose (fp);
	}
      else
	perror ("/proc/sys/net/ipv4/ip_forward");
    }


  /* Bring up  WAN interface */
  if (nvram_match ("pptp_use_dhcp", "1"))
    {
//      pid_t pid;
//      char *wan_ipaddr;
//      char *wan_netmask;
//      char *wan_gateway;

//      char *pptp_server_ip = nvram_safe_get ("pptp_server_ip");
//      char *wan_hostname = nvram_safe_get ("wan_hostname");
      char *wan_ifname = nvram_safe_get ("wan_ifname");

      nvram_set ("wan_get_dns", "");

      start_dhcpc (wan_ifname);
      int timeout;

      for (timeout = 60; nvram_match ("wan_get_dns", "") && timeout > 0;
	   --timeout)
	{			/* wait for info from dhcp server */
	  sleep (1);
	}
      stop_dhcpc ();		/* we don't need dhcp client anymore */

/*    
      //this stuff has already been configured in dhcpc->bound  
      wan_ipaddr = nvram_safe_get ("wan_ipaddr");
      wan_netmask = nvram_safe_get ("wan_netmask");
      wan_gateway = nvram_safe_get ("wan_gateway");
      pptp_server_ip = nvram_safe_get ("pptp_server_ip");

      while (route_del (wan_ifname, 0, NULL, NULL, NULL) == 0);	

     
      for (timeout = 10;
	   ifconfig (wan_ifname, IFUP, wan_ipaddr, wan_netmask)
	   && timeout > 0; --timeout)
	{
	  sleep (1);
	}
      for (timeout = 10;
	   route_add (wan_ifname, 0, pptp_server_ip, wan_gateway,
		      "255.255.255.255") && timeout > 0; --timeout)
	{
	  sleep (1);
	}*/
    }
  else
    {
      ifconfig (nvram_safe_get ("wan_ifname"), IFUP,
		nvram_safe_get ("wan_ipaddr"),
		nvram_safe_get ("wan_netmask"));
    }
  ret = _eval (pptp_argv, NULL, 0, NULL);


/*	if(nvram_match("pptp_usedhcp", "1")){
                char *wan_hostname = nvram_get("wan_hostname");
		char *dhcp_argv[] = { "udhcpc",
				      "-i", nvram_safe_get("wan_ifname"),
				      "-p", "/var/run/udhcpc.pid",
				      "-s", "/tmp/udhcpc",
				      wan_hostname && *wan_hostname ? "-H" : NULL,
				      wan_hostname && *wan_hostname ? wan_hostname : NULL,
				      NULL
                };

		ifconfig(nvram_safe_get("wan_ifname"), IFUP, NULL, NULL);
    		
		symlink("/sbin/rc", "/tmp/udhcpc");
    		nvram_set("wan_get_dns","");
		//killps("udhcpc",NULL);
		
		eval("killall","udhcpc");
    		
		_eval(dhcp_argv, NULL, 0, &pid);

		// Give enough time for DHCP to get IP address.
		sleep(2);

	} else
	    ifconfig(nvram_safe_get("wan_ifname"), IFUP, nvram_safe_get("wan_ipaddr"), nvram_safe_get("wan_netmask"));

	// Start pptp client on wan interface
	ret = _eval(pptp_argv, NULL, 0, NULL);
*/
  if (nvram_match ("ppp_demand", "1"))
    {
      /* Trigger Connect On Demand if user press Connect button in Status page */
      if (nvram_match ("action_service", "start_pptp")
	  || nvram_match ("action_service", "start_l2tp"))
	{
	  start_force_to_dial ();
//                      force_to_dial(nvram_safe_get("action_service"));
	  nvram_set ("action_service", "");
	}
      /* Trigger Connect On Demand if user ping pptp server */
      else
	eval ("listen", nvram_safe_get ("lan_ifname"));
    }

  /* Sveasoft - make sure QoS comes up after pptp pppo device */
  start_wshaper ();

  cprintf ("done\n");
  return ret;
}

int
stop_pptp (void)
{
  int ret;
  route_del (nvram_safe_get ("wan_ifname"), 0,
	     nvram_safe_get ("pptp_server_ip"), NULL, NULL);

  unlink ("/tmp/ppp/link");
  //ret = killps("pppd","-9");
  //ret += killps("pptp","-9");
  //ret += killps("listen","-9");
  ret = killall ("pppd", SIGKILL);
  ret += killall ("pptp", SIGKILL);
  ret += killall ("listen", SIGKILL);

  cprintf ("done\n");
  return ret;
}

#endif

//=========================================tallest============================================
/* AhMan  March 18 2005   Start the Original Linksys PPPoE */
/*
 * This function build the pppoe instuction & execute it.
 */
#ifdef HAVE_PPPOE
int
start_pppoe (int pppoe_num)
{
  char idletime[20], retry_num[20], param[4];
  char username[80], passwd[80];

  char ppp_username[2][20] = { "ppp_username", "ppp_username_1" };
  char ppp_passwd[2][20] = { "ppp_passwd", "ppp_passwd_1" };
  char ppp_demand[2][20] = { "ppp_demand", "ppp_demand_1" };
  char ppp_service[2][20] = { "ppp_service", "ppp_service_1" };
  char ppp_ac[2][10] = { "ppp_ac", "ppp_ac_1" };
//  char wanip[2][15] = { "wan_ipaddr", "wan_ipaddr_1" };
//  char wanmask[2][15] = { "wan_netmask", "wan_netmask_1" };
//  char wangw[2][15] = { "wan_gateway", "wan_gateway_1" };
  char pppoeifname[15];
  char *wan_ifname = nvram_safe_get ("wan_ifname");
  if (isClient ())
    {
#ifdef HAVE_MADWIFI
      wan_ifname = getSTA ();
#else
      wan_ifname = get_wdev ();
#endif
    }

  pid_t pid;

  sprintf (pppoeifname, "pppoe_ifname%d", pppoe_num);
  nvram_set (pppoeifname, "");

  cprintf ("start session %d\n", pppoe_num);
  sprintf (idletime, "%d", atoi (nvram_safe_get ("ppp_idletime")) * 60);
  snprintf (retry_num, sizeof (retry_num), "%d",
	    (atoi (nvram_safe_get ("ppp_redialperiod")) / 5) - 1);

  if (nvram_match ("aol_block_traffic", "1") && pppoe_num == PPPOE0)
    {
      if (!strcmp (nvram_safe_get ("aol_username"), ""))
	{
	  snprintf (username, sizeof (username), "%s",
		    nvram_safe_get ("ppp_username"));
	  snprintf (passwd, sizeof (passwd), "%s",
		    nvram_safe_get ("ppp_passwd"));
	}
      else
	{
	  snprintf (username, sizeof (username), "%s",
		    nvram_safe_get ("aol_username"));
	  snprintf (passwd, sizeof (passwd), "%s",
		    nvram_safe_get ("aol_passwd"));
	}

    }
  else
    {
      snprintf (username, sizeof (username), "%s",
		nvram_safe_get (ppp_username[pppoe_num]));
      snprintf (passwd, sizeof (passwd), "%s",
		nvram_safe_get (ppp_passwd[pppoe_num]));
    }
  sprintf (param, "%d", pppoe_num);
  /* add here */
  char *pppoe_argv[] = { "pppoecd",
    wan_ifname,
    "-u", username,
    "-p", passwd,
    "-r", nvram_safe_get ("wan_mtu"),	//del by honor, add by tallest.
    "-t", nvram_safe_get ("wan_mtu"),
    "-i", nvram_match (ppp_demand[pppoe_num], "1") ? idletime : "0",
    "-I", "30",			// Send an LCP echo-request frame to the server every 30 seconds
    "-T", "3",			// pppd will presume the server to be dead if 5 LCP echo-requests are sent without receiving a valid LCP echo-reply
    "-P", param,		// PPPOE session number.
    "-N", retry_num,		// To avoid kill pppd when pppd has been connecting.
#if LOG_PPPOE == 2
    "-d",
#endif
    "-C", "disconnected_pppoe",	//by tallest 0407
    NULL,			/* set default route */
    NULL, NULL,			/* pppoe_service */
    NULL, NULL,			/* pppoe_ac */
    NULL,			/* pppoe_keepalive */
    NULL
  }, **arg;
  /* Add optional arguments */
  for (arg = pppoe_argv; *arg; arg++);

  /* Removed by AhMan */

  if (pppoe_num == PPPOE0)
    {				// PPPOE0 must set default route.
      *arg++ = "-R";
    }

  if (nvram_invmatch (ppp_service[pppoe_num], ""))
    {
      *arg++ = "-s";
      *arg++ = nvram_safe_get (ppp_service[pppoe_num]);
    }
  if (nvram_invmatch (ppp_ac[pppoe_num], ""))
    {
      *arg++ = "-a";
      *arg++ = nvram_safe_get (ppp_ac[pppoe_num]);
    }
  if (nvram_match ("ppp_static", "1"))
    {
      *arg++ = "-L";
      *arg++ = nvram_safe_get ("ppp_static_ip");
    }
  //if (nvram_match("pppoe_demand", "1") || nvram_match("pppoe_keepalive", "1"))
  *arg++ = "-k";

  mkdir ("/tmp/ppp", 0777);
  symlink ("/sbin/rc", "/tmp/ppp/ip-up");
  symlink ("/sbin/rc", "/tmp/ppp/ip-down");
  symlink ("/sbin/rc", "/tmp/ppp/set-pppoepid");	// tallest 1219
  unlink ("/tmp/ppp/log");

  //Clean rpppoe client files - Added by ice-man (Wed Jun 1)
  unlink ("/tmp/ppp/options.pppoe");
  unlink ("/tmp/ppp/connect-errors");

  _eval (pppoe_argv, NULL, 0, &pid);

  if (nvram_match (ppp_demand[pppoe_num], "1"))
    {
      //int timeout = 5;
      start_tmp_ppp (pppoe_num);

// This should be handled in start_wan_done
//      while (ifconfig (nvram_safe_get (pppoeifname), IFUP, NULL, NULL)
//           && timeout--)
//      sleep (1);
//      route_add (nvram_safe_get ("wan_iface"), 0, "0.0.0.0", "10.112.112.112",
//               "0.0.0.0");

    }
  cprintf ("done. session %d\n", pppoe_num);
  return 0;
}
#endif
/* AhMan  March 18 2005 */
/*
 * Get the IP, Subnetmask, Geteway from WAN interface
 * and set to NV ram.
 */
void
start_tmp_ppp (int num)
{

  int timeout = 5;
  char pppoeifname[15];
  char wanip[2][15] = { "wan_ipaddr", "wan_ipaddr_1" };
  char wanmask[2][15] = { "wan_netmask", "wan_netmask_1" };
  char wangw[2][15] = { "wan_gateway", "wan_gateway_1" };
  //char wanif[2][15]={"wan_ifname","wan_ifname_1"};
  //char *wan_ifname = nvram_safe_get("wan_ifname");
  struct ifreq ifr;
  int s;

  cprintf ("start session %d\n", num);

  sprintf (pppoeifname, "pppoe_ifname%d", num);

  if ((s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
    return;

  /* Wait for ppp0 to be created */
  while (ifconfig (nvram_safe_get (pppoeifname), IFUP, NULL, NULL)
	 && timeout--)
    sleep (1);

  strncpy (ifr.ifr_name, nvram_safe_get (pppoeifname), IFNAMSIZ);

  /* Set temporary IP address */
  timeout = 3;
  while (ioctl (s, SIOCGIFADDR, &ifr) && timeout--)
    {
      perror (nvram_safe_get (pppoeifname));
      printf ("Wait %s inteface to init (1) ...\n",
	      nvram_safe_get (pppoeifname));
      sleep (1);
    };
  nvram_set (wanip[num], inet_ntoa (sin_addr (&(ifr.ifr_addr))));
  nvram_set (wanmask[num], "255.255.255.255");

  /* Set temporary P-t-P address */
  timeout = 3;
  while (ioctl (s, SIOCGIFDSTADDR, &ifr) && timeout--)
    {
      perror (nvram_safe_get (pppoeifname));
      printf ("Wait %s inteface to init (2) ...\n",
	      nvram_safe_get (pppoeifname));
      sleep (1);
    }
  nvram_set (wangw[num], inet_ntoa (sin_addr (&(ifr.ifr_dstaddr))));

  start_wan_done (nvram_safe_get (pppoeifname));

  // if user press Connect" button from web, we must force to dial
  if (nvram_match ("action_service", "start_pppoe")
      || nvram_match ("action_service", "start_pppoe_1"))
    {
      sleep (3);
      // force_to_dial(nvram_safe_get("action_service"));
      start_force_to_dial ();
      nvram_set ("action_service", "");
    }

  close (s);
  cprintf ("done session %d\n", num);
  return;
}

//=====================================================================================================



int
start_l2tp (int status)
{
  int ret;
  FILE *fp;
  char *l2tp_argv[] = { "l2tpd",
    NULL
  };
  char l2tpctrl[64];
  char username[80], passwd[80];

  //stop_dhcpc();
#ifdef HAVE_PPPOE
  stop_pppoe ();
#endif
#ifdef HAVE_PPTP
  stop_pptp ();
#endif

  if (nvram_match ("aol_block_traffic", "0"))
    {
      snprintf (username, sizeof (username), "%s",
		nvram_safe_get ("ppp_username"));
      snprintf (passwd, sizeof (passwd), "%s", nvram_safe_get ("ppp_passwd"));
    }
  else
    {
      if (!strcmp (nvram_safe_get ("aol_username"), ""))
	{
	  snprintf (username, sizeof (username), "%s",
		    nvram_safe_get ("ppp_username"));
	  snprintf (passwd, sizeof (passwd), "%s",
		    nvram_safe_get ("ppp_passwd"));
	}
      else
	{
	  snprintf (username, sizeof (username), "%s",
		    nvram_safe_get ("aol_username"));
	  snprintf (passwd, sizeof (passwd), "%s",
		    nvram_safe_get ("aol_passwd"));
	}
    }

  if (status != REDIAL)
    {
      mkdir ("/tmp/ppp", 0777);
      symlink ("/sbin/rc", "/tmp/ppp/ip-up");
      symlink ("/sbin/rc", "/tmp/ppp/ip-down");
      symlink ("/dev/null", "/tmp/ppp/connect-errors");

      /* Generate L2TP configuration file */
      if (!(fp = fopen ("/tmp/l2tp.conf", "w")))
	{
	  perror ("/tmp/l2tp.conf");
	  return -1;
	}
      fprintf (fp, "global\n");	// Global section
      fprintf (fp, "load-handler \"sync-pppd.so\"\n");	// Load handlers
      fprintf (fp, "load-handler \"cmd.so\"\n");
      fprintf (fp, "listen-port 1701\n");	// Bind address
      fprintf (fp, "section sync-pppd\n");	// Configure the sync-pppd handler
      fprintf (fp, "section peer\n");	// Peer section
      fprintf (fp, "peer %s\n", nvram_safe_get ("l2tp_server_ip"));
      fprintf (fp, "port 1701\n");
      fprintf (fp, "lac-handler sync-pppd\n");
      fprintf (fp, "section cmd\n");	// Configure the cmd handler
      fclose (fp);

      /* Generate options file */
      if (!(fp = fopen ("/tmp/ppp/options", "w")))
	{
	  perror ("/tmp/ppp/options");
	  return -1;
	}
      fprintf (fp, "defaultroute\n");	//Add a default route to the system routing tables, using the peer as the gateway
      fprintf (fp, "usepeerdns\n");	//Ask the peer for up to 2 DNS server addresses
      //fprintf(fp, "pty 'pptp %s --nolaunchpppd'\n",nvram_safe_get("pptp_server_ip")); 
      fprintf (fp, "user '%s'\n", username);
      //fprintf(fp, "persist\n");        // Do not exit after a connection is terminated.

      if (nvram_match ("mtu_enable", "1"))
	{
	  fprintf (fp, "mtu %s\n", nvram_safe_get ("wan_mtu"));
	}

      if (nvram_match ("ppp_demand", "1"))
	{			//demand mode
	  fprintf (fp, "idle %d\n",
		   nvram_match ("ppp_demand",
				"1") ? atoi (nvram_safe_get ("ppp_idletime"))
		   * 60 : 0);
	  //fprintf(fp, "demand\n");         // Dial on demand
	  //fprintf(fp, "persist\n");        // Do not exit after a connection is terminated.
	  //fprintf(fp, "%s:%s\n",PPP_PSEUDO_IP,PPP_PSEUDO_GW);   // <local IP>:<remote IP>
	  fprintf (fp, "ipcp-accept-remote\n");
	  fprintf (fp, "ipcp-accept-local\n");
	  fprintf (fp, "connect true\n");
	  fprintf (fp, "noipdefault\n");	// Disables  the  default  behaviour when no local IP address is specified
	  fprintf (fp, "ktune\n");	// Set /proc/sys/net/ipv4/ip_dynaddr to 1 in demand mode if the local address changes
	}
      else
	{			// keepalive mode
	  start_redial ();
	}

      fprintf (fp, "default-asyncmap\n");	// Disable  asyncmap  negotiation
      fprintf (fp, "nopcomp\n");	// Disable protocol field compression
      fprintf (fp, "noaccomp\n");	// Disable Address/Control compression 
      fprintf (fp, "noccp\n");	// Disable CCP (Compression Control Protocol)
      fprintf (fp, "novj\n");	// Disable Van Jacobson style TCP/IP header compression
      fprintf (fp, "nobsdcomp\n");	// Disables BSD-Compress  compression
      fprintf (fp, "nodeflate\n");	// Disables Deflate compression
      fprintf (fp, "lcp-echo-interval 0\n");	// Don't send an LCP echo-request frame to the peer
      fprintf (fp, "lock\n");
      fprintf (fp, "noauth\n");

      fclose (fp);

      /* Generate pap-secrets file */
      if (!(fp = fopen ("/tmp/ppp/pap-secrets", "w")))
	{
	  perror ("/tmp/ppp/pap-secrets");
	  return -1;
	}
      fprintf (fp, "\"%s\" * \"%s\" *\n", username, passwd);
      fclose (fp);
      chmod ("/tmp/ppp/pap-secrets", 0600);

      /* Generate chap-secrets file */
      if (!(fp = fopen ("/tmp/ppp/chap-secrets", "w")))
	{
	  perror ("/tmp/ppp/chap-secrets");
	  return -1;
	}
      fprintf (fp, "\"%s\" * \"%s\" *\n", username, passwd);
      fclose (fp);
      chmod ("/tmp/ppp/chap-secrets", 0600);

      /* Enable Forwarding */
      if ((fp = fopen ("/proc/sys/net/ipv4/ip_forward", "r+")))
	{
	  fputc ('1', fp);
	  fclose (fp);
	}
      else
	perror ("/proc/sys/net/ipv4/ip_forward");
    }

  /* Bring up  WAN interface */
  //ifconfig(nvram_safe_get("wan_ifname"), IFUP,
  //       nvram_safe_get("wan_ipaddr"), nvram_safe_get("wan_netmask"));

  ret = _eval (l2tp_argv, NULL, 0, NULL);
  sleep (1);
  snprintf (l2tpctrl, sizeof (l2tpctrl),
	    "/usr/sbin/l2tp-control \"start-session %s\"",
	    nvram_safe_get ("l2tp_server_ip"));
  //system(l2tpctrl);

  if (nvram_match ("ppp_demand", "1"))
    {
      /* Trigger Connect On Demand if user press Connect button in Status page */
      if (nvram_match ("action_service", "start_l2tp"))
	{
	  start_force_to_dial ();
	  nvram_set ("action_service", "");
	}
      /* Trigger Connect On Demand if user ping pptp server */
      else
	eval ("listen", nvram_safe_get ("lan_ifname"));
    }
  else
    system2 (l2tpctrl);

  cprintf ("done\n");
  return ret;
}

int
start_l2tp_redial (void)
{
  return start_l2tp (REDIAL);
}

int
start_l2tp_boot (void)
{
  return start_l2tp (BOOT);
}

int
stop_l2tp (void)
{
  int ret = 0;

  unlink ("/tmp/ppp/link");
  //ret = killps("pppd","-9");
  //ret += killps("l2tpd","-9");
  //ret += killps("listen","-9");

  ret = killall ("pppd", SIGKILL);
  ret += killall ("l2tpd", SIGKILL);
  ret += killall ("listen", SIGKILL);

  cprintf ("done\n");
  return ret;
}






int
stop_wland (void)
{
  if (pidof ("wland") > 0)
    syslog (LOG_INFO, "wland : WLAN daemon successfully stopped\n");
  int ret = killall ("wland", SIGKILL);

  cprintf ("done\n");
  return ret;
}

int
start_wland (void)
{
  int ret;
  pid_t pid;
  char *wland_argv[] = { "/sbin/wland",
    NULL
  };

  stop_wland ();

//  if( nvram_match("apwatchdog_enable", "0") )
//          return 0;

  ret = _eval (wland_argv, NULL, 0, &pid);
  syslog (LOG_INFO, "wland : WLAN daemon successfully started\n");
  cprintf ("done\n");
  return ret;
}



int
start_process_monitor (void)
{
  if (nvram_match ("pmonitor_enable", "0"))
    return 0;

  pid_t pid;

  char *argv[] = { "process_monitor", NULL };
  int ret = _eval (argv, NULL, 0, &pid);
  syslog (LOG_INFO, "process_monitor successfully started\n");

  cprintf ("done");

  return ret;
}

int
stop_process_monitor (void)
{
  int ret;
  if (pidof ("process_monitor") > 0)
    syslog (LOG_INFO, "process_monitor successfully stopped\n");
  ret = killall ("process_monitor", SIGKILL);

  cprintf ("done\n");

  return ret;
}

int
start_radio_timer (void)
{
  if (nvram_match ("radio_timer_enable", "0"))
    return 0;

  pid_t pid;

  char *argv[] = { "radio_timer", NULL };
  int ret = _eval (argv, NULL, 0, &pid);
  syslog (LOG_INFO,
	  "radio_timer : radio timer daemon successfully started\n");

  cprintf ("done");

  return ret;
}

int
stop_radio_timer (void)
{
  int ret;
  if (pidof ("radio_timer") > 0)
    syslog (LOG_INFO,
	    "radio_timer : radio timer daemon successfully stopped\n");
  ret = killall ("radio_timer", SIGKILL);

  cprintf ("done\n");

  return ret;
}

extern void start_heartbeat_boot (void);


/* Trigger Connect On Demand */
int
start_force_to_dial (void)
{
//force_to_dial( char *whichone){
  int ret = 0;
  char dst[50];

  strcpy (&dst[0], nvram_safe_get ("wan_gateway"));

  char *ping_argv[] = { "ping",
    "-c", "1",
    dst,
    NULL
  };

  sleep (1);
#ifdef HAVE_L2TP
  if (nvram_match ("wan_proto", "l2tp"))
    {
      char l2tpctrl[64];

      snprintf (l2tpctrl, sizeof (l2tpctrl),
		"/usr/sbin/l2tp-control \"start-session %s\"",
		nvram_safe_get ("l2tp_server_ip"));
      system2 (l2tpctrl);
      return ret;
    }
#endif
#ifdef HAVE_HEARTBEAT
  if (nvram_match ("wan_proto", "heartbeat"))
    {
      start_heartbeat_boot ();
      return ret;
    }
#endif
  _eval (ping_argv, NULL, 3, NULL);

  return ret;
}



#ifdef HAVE_CPUTEMP

#ifdef HAVE_GATEWORX
#define TEMP_PATH "/sys/devices/platform/IXP4XX-I2C.0/i2c-0/0-0028"
#define TEMP_PREFIX "temp"
#define TEMP_MUL 100
#else
#ifdef HAVE_X86
#define TEMP_PATH "/sys/devices/platform/i2c-1/1-0048"
#else
#define TEMP_PATH "/sys/devices/platform/i2c-0/0-0048"
#endif
#define TEMP_PREFIX "temp1"
#define TEMP_MUL 1000
#endif


void
start_hwmon (void)
{
  int temp_max = atoi (nvram_safe_get ("hwmon_temp_max")) * TEMP_MUL;
  int temp_hyst = atoi (nvram_safe_get ("hwmon_temp_hyst")) * TEMP_MUL;
  char buf[128];
  sprintf (buf, "/bin/echo %d > %s/%s_max", temp_max, TEMP_PATH, TEMP_PREFIX);
  system2 (buf);
  sprintf (buf, "/bin/echo %d > %s/%s_max_hyst", temp_hyst, TEMP_PATH,
	   TEMP_PREFIX);
  system2 (buf);
  syslog (LOG_INFO, "hwmon successfully started\n");
}


#endif

#ifdef HAVE_USBHOTPLUG
int
start_hotplug_usb (void)
{
//      char *lan_ifname = nvram_safe_get("lan_ifname");
  char *interface = getenv ("INTERFACE");
  char *action = getenv ("ACTION");
  char *product = getenv ("PRODUCT");
  char *devpath = getenv ("DEVPATH");
  char *type = getenv ("TYPE");
  char *devfs = getenv ("DEVFS");
  char *device = getenv ("DEVICE");
  fprintf (stderr, "interface %s\n", interface != NULL ? interface : "");
  fprintf (stderr, "action %s\n", action != NULL ? action : "");
  fprintf (stderr, "product %s\n", product != NULL ? product : "");
  fprintf (stderr, "devpath %s\n", devpath != NULL ? devpath : "");
  fprintf (stderr, "type %s\n", type != NULL ? type : "");
  fprintf (stderr, "devfs %s\n", devfs != NULL ? devfs : "");
  fprintf (stderr, "device %s\n", device != NULL ? device : "");

  return 0;
}
#endif
