<?php
	/**
	 * This file displays article to user.
	 * $Id$
	 */
	require("funcs.php");
function get_mimetype($name)
{
	$dot = strrchr($name, '.');
	if ($dot == $name)
		return "text/plain; charset=gb2312";
	if (strcasecmp($dot, ".html") == 0 || strcasecmp($dot, ".htm") == 0)
		return "text/html; charset=gb2312";
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
		return "text/plain; charset=gb2312";
	if (strcasecmp($dot, ".xht") == 0 || strcasecmp($dot, ".xhtml") == 0)
		return "application/xhtml+xml";
	if (strcasecmp($dot, ".xml") == 0)
		return "text/xml";
	return "application/octet-stream";
}

function display_navigation_bar_in($brdarr, $articles, $num, $brdnum )
{
	global $currentuser;

	$brd_encode = urlencode($brdarr["NAME"]);
	$PAGE_SIZE = 20;
?>
<table width="100%" border="0" cellspacing="0" cellpadding="0" class="b7">
<tr><td>
<a href="/bbspst.php?board=<?php echo $brd_encode; ?>&reid=<?php echo $articles[1]["ID"];?>"><img src="images/reply.gif" border="0" alt="�ظ�����" align="absmiddle"></a>
<a href="bbspst.php?board=<?php echo $brd_encode; ?>"><img src="images/postnew.gif" border="0" alt="������" align="absmiddle"></a>
</td><td align="right">
[<a class="b7" href="<?php echo $_SERVER["PHP_SELF"]; ?>?bid=<?php echo $brdnum; ?>&id=<?php echo $articles[1]["ID"]; ?>&p=p"><font class="b7">��һƪ</font></a>]
[<a class="b7" href="<?php echo $_SERVER["PHP_SELF"]; ?>?bid=<?php echo $brdnum; ?>&id=<?php echo $articles[1]["ID"]; ?>&p=n"><font class="b7">��һƪ</font></a>]
[<a class="b7" href="<?php echo $_SERVER["PHP_SELF"]; ?>?bid=<?php echo $brdnum; ?>&id=<?php echo $articles[1]["ID"]; ?>&p=tp"><font class="b7">ͬ������ƪ</font></a>]
[<a class="b7" href="<?php echo $_SERVER["PHP_SELF"]; ?>?bid=<?php echo $brdnum; ?>&id=<?php echo $articles[1]["ID"]; ?>&p=tn"><font class="b7">ͬ������ƪ</font></a>]
</td></tr></table>
<?php
}

function display_navigation_bar_out($brdarr, $articles, $num, $brdnum)
{
	global $currentuser;

	$brd_encode = urlencode($brdarr["NAME"]);
	$PAGE_SIZE = 20;

	if( $articles[1]["ATTACHPOS"] == 0)
	{
?>
[<a href="/bbsfwd.php?board=<?php echo $brd_encode; ?>&id=<?php echo $articles[1]["ID"]; ?>">ת��</a>]
[<a href="/cgi-bin/bbs/bbsccc?board=<?php echo $brd_encode; ?>&file=<?php echo $articles[1]["FILENAME"]; ?>">ת��</a>]
<?php
	}
?>
[<a href="/bbspstmail.php?board=<?php echo $brd_encode; ?>&file=<?php echo $articles[1]["FILENAME"]; ?>&userid=<?php echo $articles[1]["OWNER"]; ?>&title=<?php if(strncmp($articles[1]["TITLE"],"Re:",3)) echo "Re: "; ?><?php echo urlencode($articles[1]["TITLE"]); ?>">���Ÿ�����</a>]
[<a href="/cgi-bin/bbs/bbsedit?board=<?php echo $brd_encode; ?>&file=<?php echo $articles[1]["FILENAME"]; ?>">�޸�����</a>]
[<a onclick="return confirm('�����Ҫɾ��������?')" href="bbsdel.php?board=<?php echo $brd_encode; ?>&file=<?php echo $articles[1]["FILENAME"]; ?>">ɾ������</a>]
[<a href="/bbstcon.php?board=<?php echo $brd_encode; ?>&gid=<?php echo $articles[1]["GROUPID"]; ?>">ͬ�����Ķ�</a>]
[<a href="/bbstcon.php?board=<?php echo $brd_encode; ?>&gid=<?php echo $articles[1]["GROUPID"]; ?>&start=<?php echo $articles[1]["ID"]; ?>">�Ӵ˴�չ��</a>]
[<a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>&page=<?php echo intval(($num + $PAGE_SIZE - 1) / $PAGE_SIZE); ?>">���ذ���</a>]
[<a href="javascript:history.go(-1)">���ٷ���</a>]
<?php
}
	$brdarr = array();
	if( isset( $_GET["bid"] ) ){
		$brdnum = $_GET["bid"] ;
		if( $brdnum == 0 ){
			html_init("gb2312","","",1);
			html_error_quit("�����������!");
		}
		$board = bbs_getbname($brdnum);
		if( !$board ){
			html_init("gb2312","","",1);
			html_error_quit("�����������");
		}
		if( $brdnum != bbs_getboard($board, $brdarr) ){
			html_init("gb2312","","",1);
			html_error_quit("�����������");
		}
	}
	elseif (isset($_GET["board"])){
		$board = $_GET["board"];
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0) {
			html_init("gb2312","","",1);
			html_error_quit("�����������");
		}
	}
	elseif (isset($_SERVER['argv'])){
		$board = $_SERVER['argv'][1];
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0) {
			html_init("gb2312","","",1);
			html_error_quit("�����������");
		}
	}
	else {
		html_init("gb2312","","",1);
		html_error_quit("�����������");
	}
               $isnormalboard=bbs_normalboard($board);
               if (($loginok != 1)&&!$isnormalboard) {
                   html_nologin();
                   return;
               }
        bbs_set_onboard($brdnum,1);

	if($loginok == 1)
		$usernum = $currentuser["index"];

	if (!$isnormalboard && bbs_checkreadperm($usernum, $brdnum) == 0) {
		html_init("gb2312","","",1);
		html_error_quit("�����������");
	}
	if (isset($_GET["id"]))
		$id = $_GET["id"];
	elseif (isset($_SERVER['argv'][2]))
		$id = $_SERVER['argv'][2];
	else {
		html_init("gb2312","","",1);
		html_error_quit("��������º�");
	}
	settype($id, "integer");
	// ��ȡ��һƪ����һƪ��ͬ������һƪ����һƪ��ָʾ
	@$ptr=$_GET["p"];
	// ͬ�����ָʾ�����ﴦ��
	if ($ptr == "tn")
	{
		$articles = bbs_get_threads_from_id($brdnum, $id, $dir_modes["NORMAL"],1);
		if ($articles == FALSE)
			$redirt_id = $id;
		else
			$redirt_id = $articles[0]["ID"];
		if (($loginok == 1) && $currentuser["userid"] != "guest")
			bbs_brcaddread($brdarr["NAME"], $redirt_id);
		header("Location: " . "/bbscon.php?bid=" . $brdnum . "&id=" . $redirt_id);
		exit;
	}
	elseif ($ptr == "tp")
	{
		$articles = bbs_get_threads_from_id($brdnum, $id, $dir_modes["NORMAL"],-1);
		if ($articles == FALSE)
			$redirt_id = $id;
		else
			$redirt_id = $articles[0]["ID"];
		if (($loginok == 1) && $currentuser["userid"] != "guest")
			bbs_brcaddread($brdarr["NAME"], $redirt_id);
		header("Location: " . "/bbscon.php?bid=" . $brdnum . "&id=" . $redirt_id);
		exit;
	}

	if (isset($_GET["ftype"])){
		$ftype = $_GET["ftype"];
		if($ftype != $dir_modes["ZHIDING"])
			$ftype = $dir_modes["NORMAL"];
	}
	else
		$ftype = $dir_modes["NORMAL"];
	$total = bbs_countarticles($brdnum, $ftype);
	if ($total <= 0) {
		html_init("gb2312","","",1);
		html_error_quit("��������Ŀǰû������,$brdnum,$board,$ftype,$total".$brdarr["NAME"]);
	}
	$articles = array ();
	$num = bbs_get_records_from_id($brdarr["NAME"], $id, 
			$ftype, $articles);
	if ($num == 0)
	{
		html_init("gb2312","","",1);
		html_error_quit("��������º�,ԭ�Ŀ����Ѿ���ɾ��");
	}
	else
	{
		$filename=bbs_get_board_filename($brdarr["NAME"], $articles[1]["FILENAME"]);
		if ($isnormalboard) {
       			if (cache_header("public",filemtime($filename),300))
               			return;
               	}
//		Header("Cache-control: nocache");
		@$attachpos=$_GET["ap"];//pointer to the size after ATTACHMENT PAD
		if ($attachpos!=0) {
			$file = fopen($filename, "rb");
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
			Header("Content-type: " . get_mimetype($attachname));
			Header("Accept-Ranges: bytes");
			Header("Accept-Length: " . $attachsize);
			Header("Content-Disposition: inline;filename=" . $attachname);
			echo fread($file,$attachsize);
			fclose($file);
			exit;
		} else
		{
			//$http_uri = "http" . ($_SERVER["HTTPS"] == "on" ? "s" : "") . "://";
			if ($ptr == 'p' && $articles[0]["ID"] != 0)
		{
				if (($loginok == 1) && $currentuser["userid"] != "guest")
					bbs_brcaddread($brdarr["NAME"], $articles[0]["ID"]);
				header("Location: " . "/bbscon.php?bid=" . $brdnum . "&id=" . $articles[0]["ID"]);
				exit;
			}
			elseif ($ptr == 'n' && $articles[2]["ID"] != 0)
			{
				if (($loginok == 1) && $currentuser["userid"] != "guest")
					bbs_brcaddread($brdarr["NAME"], $articles[2]["ID"]);
				header("Location: " ."/bbscon.php?bid=" . $brdnum . "&id=" . $articles[2]["ID"]);
				exit;
			}
			html_init("gb2312","","",1);
?>
<body>
<!-- ��ʱ�����ģ��Ƚ� ugly  -->
<script language="javascript">
<?php
                      $query_str = urlencode($_SERVER["PHP_SELF"] . "?" . $_SERVER["QUERY_STRING"]);
                      $home_url = "/guest-frames.html?mainurl=" . $query_str;
?>
var strHomeURL = '<?php echo $home_url; ?>';
var strBBSName = '<?php echo BBS_FULL_NAME; ?>';
var strDesc = '<?php echo htmlspecialchars($brdarr["DESC"]); ?>';
</script>
<script language="javascript" src="bbscon.js"></script>
<tr><td align="center" class="b4"><?php echo $brdarr["NAME"]; ?> ��</td></tr>
<tr><td class="b1">
<table width="100%" border="0" cellspacing="0" cellpadding="3" class="t1">
<tr><td class="t2">
<?php
			display_navigation_bar_in($brdarr, $articles, $num, $brdnum );
?>
</td></tr>
<tr><td class="t5">
<font class="content"><script language="Javascript" src="jscon.php?ftype=<?php echo $ftype; ?>&bid=<?php echo $brdarr["BID"]; ?>&id=<?php echo $articles[1]["ID"]; ?>">
</script>
</font>
</td></tr>
</td></tr>
<tr><td class="t2">
<?php
			display_navigation_bar_in($brdarr, $articles, $num, $brdnum );
?>
</td></tr>
</table></td></tr>
<tr><td height="20" class="b1"> </td></tr>
<tr><td align="center" class="b1">
<?php
			display_navigation_bar_out($brdarr, $articles, $num, $brdnum);
?>
</td></tr>
</table>
<?php
		}
	}
	if ($loginok==1&&($currentuser["userid"] != "guest"))
		bbs_brcaddread($brdarr["NAME"], $articles[1]["ID"]);
	html_normal_quit();
?>
