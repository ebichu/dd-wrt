<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
	<head>
		<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=iso-8859-1" />
		<title><% nvram_get("router_name"); %> - Routing Table</title>
		<link type="text/css" rel="stylesheet" href="style/<% nvram_get("router_style"); %>/style.css" />
		<!--[if IE]><link type="text/css" rel="stylesheet" href="style/<% nvram_get("router_style"); %>/style_ie.css" /><![endif]-->
		<script type="text/javascript" src="common.js"></script>
	</head>

	<body>
		<form>
			<h2>Routing Table Entry List</h2>
			<table>
				<tr>
					<th>Destination LAN IP</th>
					<th>Subnet Mask</th>
					<th>Gateway</th>
					<th>Interface</th>
				</tr>
<script language="JavaScript">

var table = new Array(<% dump_route_table(""); %>);

if(table.length == 0) {
	document.write("<tr><td align=\"center\" colspan=\"4\">- None -</td></tr>");
} else {
	for(var i = 0; i < table.length; i = i+4) {
		if(table[i+3] == "LAN")
			table[i+3] = "LAN &amp; Wireless";
		else if(table[i+3] == "WAN")
			table[i+3] = "WAN (Internet)";
		document.write("<tr><td>"+table[i]+"</td><td>"+table[i+1]+"</td><td>"+table[i+2]+"</td><td>"+table[i+3]+"</td></tr>");
	}
}
</script>
			</table><br />
			<div class="submitFooter">
				<input name="button" type="button" onclick="window.location.reload()" value=" Refresh "/>
				<input onclick="self.close()" type="reset" value="Close"/>
			</div>
		</form>
	</body>
</html>