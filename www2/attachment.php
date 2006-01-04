<?php
function get_mimetype($name)
{
	$dot = strrchr($name, '.');
	if ($dot == $name)
		return "text/plain";
	if (strcasecmp($dot, ".html") == 0 || strcasecmp($dot, ".htm") == 0)
		return "text/html";
	if (strcasecmp($dot, ".jpg") == 0 || strcasecmp($dot, ".jpeg") == 0)
		return "image/jpeg";
	if (strcasecmp($dot, ".gif") == 0)
		return "image/gif";
	if (strcasecmp($dot, ".png") == 0)
		return "image/png";
	if (strcasecmp($dot, ".pcx") == 0)
		return "image/pcx";
	if (strcasecmp($dot, ".css") == 0)
		return "text/css";
	if (strcasecmp($dot, ".au") == 0)
		return "audio/basic";
	if (strcasecmp($dot, ".wav") == 0)
		return "audio/wav";
	if (strcasecmp($dot, ".avi") == 0)
		return "video/x-msvideo";
	if (strcasecmp($dot, ".mov") == 0 || strcasecmp($dot, ".qt") == 0)
		return "video/quicktime";
	if (strcasecmp($dot, ".mpeg") == 0 || strcasecmp($dot, ".mpe") == 0)
		return "video/mpeg";
	if (strcasecmp($dot, ".vrml") == 0 || strcasecmp($dot, ".wrl") == 0)
		return "model/vrml";
	if (strcasecmp($dot, ".midi") == 0 || strcasecmp($dot, ".mid") == 0)
		return "audio/midi";
	if (strcasecmp($dot, ".mp3") == 0)
		return "audio/mpeg";
	if (strcasecmp($dot, ".pac") == 0)
		return "application/x-ns-proxy-autoconfig";
	if (strcasecmp($dot, ".txt") == 0)
		return "text/plain";
	if (strcasecmp($dot, ".xht") == 0 || strcasecmp($dot, ".xhtml") == 0)
		return "application/xhtml+xml";
	if (strcasecmp($dot, ".xml") == 0)
		return "text/xml";
	if (strcasecmp($dot, ".swf") == 0)
		return "application/x-shockwave-flash";
	if (strcasecmp($dot, ".pdf") == 0)
		return "application/pdf";
	return "application/octet-stream";
}

function output_attachment($filename, $attachpos, $attachname = "") {
	if (!file_exists($filename)) return;
	
	$file = fopen($filename, "rb");
	if ($attachpos != 0) {
		fseek($file,$attachpos);
		$attachname='';
		while (1) {
			$char=fgetc($file);
			if (ord($char)==0) break;
			$attachname=$attachname . $char;
		}
		$str=fread($file,4);
		$array=unpack('Nsize',$str);
		$attachsize=$array["size"];
	} else {
		$attachsize = filesize($filename);
	}
	Header("Content-Type: " . get_mimetype($attachname));
	
	Header("Accept-Ranges: bytes");
	Header("Content-Length: " . $attachsize);
	Header("Content-Disposition: inline;filename=" . $attachname);
	if ($attachsize>0) echo @fread($file,$attachsize);
	fclose($file);
}
?>
